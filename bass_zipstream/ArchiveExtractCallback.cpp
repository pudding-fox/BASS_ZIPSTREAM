#include "ArchiveExtractCallback.h"
#include "ArchiveExtractPrompt.h"

#include "Archive.h"
#include "CFileStream.h"
#include "IClosable.h"

#include "../7z/CPP/Common/IntToString.h"
#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/7zip/Common/FileStreams.h"

#define TEMP_FILE_PREFIX L"bass_zipstream."
#define TEMP_FILE_SUFFIX L".tmp"

ArchiveExtractCallback::ArchiveExtractCallback(Archive* parent) {
	this->Parent = parent;
}

bool ArchiveExtractCallback::GetFileName(UString& path, UInt32 index) {
	if (!NWindows::NFile::NDir::MyGetTempPath(path)) {
		return false;
	}
	UString fileName;
	if (!this->Parent->IsOpen(fileName)) {
		return false;
	}
	UInt64 hashCode = 7;
	for (unsigned a = 0; a < fileName.Len(); a++) {
		hashCode = 31 * hashCode + fileName[a] + index;
	}
	char temp[32];
	ConvertUInt64ToString(hashCode, temp);
	path += TEMP_FILE_PREFIX;
	path += UString(temp).Left(8);
	path += TEMP_FILE_SUFFIX;
	return true;
}

bool ArchiveExtractCallback::OpenFile(ArchiveExtractFile* file, bool overwrite) {
	CFileStream* fileStream = new CFileStream();
	if (overwrite) {
		if (!fileStream->Create(file->Path, file->Size)) {
			return false;
		}
	}
	else {
		if (!fileStream->Open(file->Path, file->Size)) {
			return false;
		}
	}

	file->Stream = fileStream;

	return true;
}

bool ArchiveExtractCallback::OpenFile(UInt32 index) {
	UString path;
	if (!this->GetFileName(path, index)) {
		return false;
	}

	ArchiveExtractFile* file = new ArchiveExtractFile();
	file->Path = path;
	file->Index = index;

	this->Parent->GetEntry(path, file->Size, index);

	bool overwrite = true;
	NWindows::NFile::NFind::CFileInfo fileInfo;
	if (fileInfo.Find(file->Path)) {
		if (fileInfo.Size == file->Size) {
			overwrite = false;
		}
	}

	if (!this->OpenFile(file, overwrite)) {
		delete file;
		return false;
	}

	this->Files.Add(file);

	return true;
}

bool ArchiveExtractCallback::OpenFiles(const UInt32* indices, UInt32 count) {
	for (UInt32 a = 0; a < count; a++) {
		if (!this->OpenFile(indices[a])) {
			return false;
		}
	}
	return true;
}

void ArchiveExtractCallback::Close() {
	this->CloseReaders();
	this->CloseWriters();
}

void ArchiveExtractCallback::CloseReaders() {
	for (unsigned a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Stream) {
			CMyComPtr<IInStream> stream;
			if (file->Stream->GetReader(stream)) {
				CMyComPtr<IClosable> closable;
				if (stream->QueryInterface(IID_IClosable, (void**)&closable) == S_OK) {
					closable->Close();
				}
			}
		}
	}
}

void ArchiveExtractCallback::CloseWriters() {
	for (unsigned a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Stream) {
			CMyComPtr<IOutStream> stream;
			if (file->Stream->GetWriter(stream)) {
				CMyComPtr<IClosable> closable;
				if (stream->QueryInterface(IID_IClosable, (void**)&closable) == S_OK) {
					closable->Close();
				}
			}
		}
	}
}

bool ArchiveExtractCallback::GetInStream(CMyComPtr<IInStream>& stream, int index) {
	for (unsigned a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Index == index) {
			if (!file->Stream) {
				return false;
			}
			return file->Stream->GetReader(stream);
		}
	}
	return false;
}

bool ArchiveExtractCallback::GetOutStream(CMyComPtr<IOutStream>& stream, int index) {
	for (unsigned a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Index == index) {
			if (!file->Stream) {
				return false;
			}
			return file->Stream->GetWriter(stream);
		}
	}
	return false;
}

STDMETHODIMP ArchiveExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode) {

	//Empty outStream means skip.
	*outStream = nullptr;

	CMyComPtr<IOutStream> fileStream;
	if (this->GetOutStream(fileStream, index)) {
		*outStream = fileStream.Detach();
	}

	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::PrepareOperation(Int32 askExtractMode) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetOperationResult(Int32 opRes) {
	switch (opRes) {
	case NArchive::NExtract::NOperationResult::kOK:
		return S_OK;
	case NArchive::NExtract::NOperationResult::kWrongPassword:
		return E_ACCESSDENIED;
	}
	//Unknown failure?
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetTotal(UInt64 total) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetCompleted(const UInt64* completeValue) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::CryptoGetTextPassword(BSTR* result) {
	UString password;
	*result = nullptr;
	if (!this->Parent->GetPassword(password)) {
		if (!ArchiveExtractPrompt::CanPrompt()) {
			return E_ACCESSDENIED;
		}
		UString fileName;
		if (!this->Parent->IsOpen(fileName)) {
			return ERROR_HANDLES_CLOSED;
		}
		if (!ArchiveExtractPrompt::Prompt(fileName, password)) {
			return E_ACCESSDENIED;
		}
		this->Parent->SetPassword(password);
	}
	StringToBstr(password, result);
	return S_OK;
}

bool ArchiveExtractCallback::Cleanup() {
	FString path;
	if (!NWindows::NFile::NDir::MyGetTempPath(path)) {
		return false;
	}
	bool success = true;
	NWindows::NFile::NFind::CEnumerator enumerator;
	enumerator.SetDirPrefix(path);
	NWindows::NFile::NFind::CFileInfo fileInfo;
	while (enumerator.Next(fileInfo))
	{
		if (fileInfo.IsDir()) {
			continue;
		}
		if (fileInfo.Name.Find(TEMP_FILE_PREFIX, 0) != 0) {
			continue;
		}
		if (!::DeleteFileW(fs2us(path + fileInfo.Name))) {
			success = false;
		}
	}
	return success;
}