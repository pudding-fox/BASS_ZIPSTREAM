#include "CFileStream.h"
#include "IClosable.h"

static inline HRESULT ConvertBoolToHRESULT(bool result)
{
	if (result) {
		return S_OK;
	}
	DWORD lastError = ::GetLastError();
	if (lastError == 0) {
		return E_FAIL;
	}
	return HRESULT_FROM_WIN32(lastError);
}

class CFileStreamBase :
	public CMyUnknownImp,
	public IClosable
{

protected:
	CFileStream* Stream;
	UInt64 Position;
	UInt64 Processed;
	bool IsClosed;

	STDMETHODIMP Begin() {
		if (this->IsClosed) {
			return ERROR_HANDLES_CLOSED;
		}
		return  this->Stream->Begin();
	}

	STDMETHODIMP End() {
		return this->Stream->End();
	}

public:
	CFileStreamBase(CFileStream* stream) {
		this->Stream = stream;
		this->Position = 0;
		this->Processed = 0;
		this->IsClosed = false;
	}
};

class CFileStreamReader :
	public CFileStreamBase,
	public IInStream
{

public:
	MY_QUERYINTERFACE_BEGIN2(IClosable)
		MY_QUERYINTERFACE_END
		MY_ADDREF_RELEASE;

	CFileStreamReader(CFileStream* stream) : CFileStreamBase(stream) {

	}

	STDMETHODIMP Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition) {
		HRESULT result = this->Begin();
		if (result != S_OK) {
			return result;
		}
		switch (seekOrigin) {
		case SEEK_CUR:
			offset += this->Position;
			break;
		case SEEK_END:
			offset += this->Stream->GetSize();
			break;
		}
		if (offset < 0 || (UInt64)offset > this->Stream->GetSize()) {
			result = E_FAIL;
		}
		else {
			if (this->Position != offset) {
				this->Position = offset;
				this->Stream->ReadDirty = true;
			}
			if (newPosition) {
				*newPosition = offset;
			}
		}
		this->End();
		return result;
	}

	STDMETHODIMP Read(void* data, UInt32 size, UInt32* processedSize) {
		HRESULT result = this->Begin();
		if (result != S_OK) {
			return result;
		}
		if (this->Stream->ReadDirty) {
			result = this->Stream->Seek(this->Position, SEEK_SET, nullptr);
			if (result != S_OK) {
				goto done;
			}
			this->Stream->ReadDirty = false;
		}
		UInt32 realProcessedSize;
		result = this->Stream->Read(data, size, &realProcessedSize);
		if (processedSize) {
			*processedSize = realProcessedSize;
		}
		if (realProcessedSize) {
			this->Position += realProcessedSize;
			this->Processed += realProcessedSize;
			this->Stream->WriteDirty = true;
		}
	done:
		this->End();
		return result;
	}

	STDMETHODIMP Close() override {
		this->Stream->CloseReader();
		this->IsClosed = true;
		return S_OK;
	}

	~CFileStreamReader() {
		this->Close();
	}
};

class CFileStreamWriter :
	public CFileStreamBase,
	public IOutStream
{

public:
	MY_QUERYINTERFACE_BEGIN2(IClosable)
		MY_QUERYINTERFACE_END
		MY_ADDREF_RELEASE;

	CFileStreamWriter(CFileStream* stream) : CFileStreamBase(stream) {

	}

	STDMETHODIMP Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition) {
		HRESULT result = this->Begin();
		if (result != S_OK) {
			return result;
		}
		switch (seekOrigin) {
		case SEEK_CUR:
			offset += this->Position;
			break;
		case SEEK_END:
			offset += this->Stream->GetSize();
			break;
		}
		if (offset < 0 || (UInt64)offset > this->Stream->GetSize()) {
			result = E_FAIL;
		}
		else {
			if (this->Position != offset) {
				this->Position = offset;
				this->Stream->WriteDirty = true;
			}
			if (newPosition) {
				*newPosition = offset;
			}
		}
		this->End();
		return result;
	}

	STDMETHODIMP Write(const void* data, UInt32 size, UInt32* processedSize) {
		HRESULT result = this->Begin();
		if (result != S_OK) {
			return result;
		}
		if (this->Stream->WriteDirty) {
			result = this->Stream->Seek(this->Position, SEEK_SET, nullptr);
			if (result != S_OK) {
				goto done;
			}
			this->Stream->WriteDirty = false;
		}
		UInt32 realProcessedSize;
		result = this->Stream->Write(data, size, &realProcessedSize);
		if (processedSize) {
			*processedSize = realProcessedSize;
		}
		if (realProcessedSize) {
			this->Position += realProcessedSize;
			this->Processed += realProcessedSize;
			this->Stream->ReadDirty = true;
		}
	done:
		this->End();
		return result;
	}

	STDMETHODIMP SetSize(UInt64 newSize) {
		HRESULT result = this->Begin();
		if (result != S_OK) {
			return result;
		}
		if (this->Stream->WriteDirty) {
			result = this->Stream->Seek(this->Position, SEEK_SET, nullptr);
			if (result != S_OK) {
				goto done;
			}
			this->Stream->WriteDirty = false;
		}
		result = this->Stream->SetSize(newSize);
		this->Stream->ReadDirty = true;
	done:
		this->End();
		return result;
	}

	STDMETHODIMP Close() override {
		this->Stream->CloseWriter();
		this->IsClosed = true;
		return S_OK;
	}

	~CFileStreamWriter() {
		this->Close();
	}
};

bool CFileStream::GetReader(CMyComPtr<IInStream>& stream) {
	if (!this->Reader) {
		return false;
	}
	stream = this->Reader;
	return true;
}

bool CFileStream::GetWriter(CMyComPtr<IOutStream>& stream) {
	if (!this->Writer) {
		return false;
	}
	stream = this->Writer;
	return true;
}

STDMETHODIMP CFileStream::Begin() {
	if (this->IsConcurrent) {
		if (Semaphore_Wait(&this->Semaphore) != 0) {
			return E_FAIL;
		}
	}
	return S_OK;
}

STDMETHODIMP CFileStream::End() {
	if (this->IsConcurrent) {
		Semaphore_Release1(&this->Semaphore);
	}
	return S_OK;
}

STDMETHODIMP CFileStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition) {
	UInt64 realNewPosition;
	bool result = this->File.Seek(offset, seekOrigin, realNewPosition);
	if (newPosition) {
		*newPosition = realNewPosition;
	}
	return ConvertBoolToHRESULT(result);
}

STDMETHODIMP CFileStream::Read(void* data, UInt32 size, UInt32* processedSize) {
	UInt32 realProcessedSize;
	bool result = this->File.Read(data, size, realProcessedSize);
	if (processedSize) {
		*processedSize = realProcessedSize;
	}
	return ConvertBoolToHRESULT(result);
}

STDMETHODIMP CFileStream::Write(const void* data, UInt32 size, UInt32* processedSize) {
	UInt32 realProcessedSize;
	bool result = this->File.Write(data, size, realProcessedSize);
	if (processedSize) {
		*processedSize = realProcessedSize;
	}
	return ConvertBoolToHRESULT(result);
}

STDMETHODIMP CFileStream::SetSize(UInt64 newSize) {
	UInt64 currentPos;
	if (!this->File.Seek(0, FILE_CURRENT, currentPos)) {
		return E_FAIL;
	}
	bool result = this->File.SetLength(newSize);
	UInt64 currentPos2;
	result = result && this->File.Seek(currentPos, currentPos2);
	return result ? S_OK : E_FAIL;
}

bool CFileStream::Create(CFSTR fileName, UInt64 size) {
	if (!this->File.Open(fileName, true)) {
		return false;
	}
	this->Reader = new CFileStreamReader(this);
	this->ReadDirty = false;
	this->Writer = new CFileStreamWriter(this);
	this->WriteDirty = false;
	Semaphore_Construct(&this->Semaphore);
	Semaphore_Create(&this->Semaphore, 1, 1);
	this->IsConcurrent = true;
	this->Size = size;
	return true;
}

bool CFileStream::Open(CFSTR fileName, UInt64 size) {
	if (!this->File.Open(fileName, false)) {
		return false;
	}
	this->Reader = new CFileStreamReader(this);
	this->ReadDirty = false;
	this->IsConcurrent = false;
	this->Size = size;
	return true;
}

void CFileStream::CloseReader() {
	this->IsConcurrent = false;
}

void CFileStream::CloseWriter() {
	this->IsConcurrent = false;
}

UInt64 CFileStream::GetSize() {
	return this->Size;
}

CFileStream::~CFileStream() {
	this->File.Close();
	if (this->IsConcurrent) {
		Semaphore_Close(&this->Semaphore);
	}
}