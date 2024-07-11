#include "Archive.h"
#include "ArchiveEntry.h"

#include "Common.h"
#include "ArchiveExtractCallback.h"
#include "IOUtils.h"

#include "../7z/CPP/7zip/Common/FileStreams.h"
#include "../7z/CPP/Windows/PropVariant.h"

Archive::Archive() {
	this->Codecs = new CCodecs();
	this->FormatIndex = -1;
	this->LoadCodecs();
}

void Archive::LoadCodecs() {
	OK(this->Codecs->Load());
}

void Archive::LoadFormat() {
	this->FormatIndex = this->Codecs->FindFormatForArchiveName(this->FileName);
	if (this->FormatIndex < 0) {
		throw CSystemException(E_NOTIMPL);
	}
}

int Archive::GetFormatCount() {
	CObjectVector<CArcInfoEx> formats = this->Codecs->Formats;
	return formats.Size();
}

void Archive::GetFormat(UString& name, UString& extensions, int index) {
	CObjectVector<CArcInfoEx> formats = this->Codecs->Formats;
	CArcInfoEx& format = formats[index];
	name = format.Name;
	for (unsigned a = 0; a < format.Exts.Size(); a++) {
		CArcExtInfo& ext = format.Exts[a];
		if (a > 0) {
			extensions += ",";
		}
		extensions += ext.Ext;
	}
}

void Archive::Open(UString& fileName) {
	this->FileName = fileName;
	this->LoadFormat();
	this->CreateInStream();
	this->CreateInArchive();
	this->OpenInArchive();
}

void Archive::CreateInStream() {
	CInFileStream* fileStream = new CInFileStream;
	this->InStream = fileStream;
	if (!fileStream->Open(this->FileName)) {
		//Hopefully there's an error from IO.
		throw CSystemException(::GetLastError());
	}
}

void Archive::CreateInArchive() {
	HRESULT result = this->Codecs->CreateInArchive(this->FormatIndex, this->InArchive);
	if (result != S_OK) {
		throw CSystemException(result);
	}
}

void Archive::OpenInArchive() {
	UInt64 maxStartPosition = 1 << 23;
	HRESULT result = this->InArchive->Open(this->InStream, &maxStartPosition, nullptr);
	if (result != S_OK) {
		throw CSystemException(result);
	}
}

bool Archive::ReadProperty(UString& value, PROPID propID, int index) {
	NWindows::NCOM::CPropVariant property;
	if (this->InArchive->GetProperty(index, propID, &property) == S_OK) {
		if (property.vt == VT_BSTR && property.bstrVal) {
			value.SetFromBstr(property.bstrVal);
			return true;
		}
		else if (property.vt == VT_EMPTY) {
			value.Empty();
			return true;
		}
	}
	return false;
}

bool Archive::ReadProperty(UInt64& value, PROPID propID, int index) {
	NWindows::NCOM::CPropVariant property;
	if (this->InArchive->GetProperty(index, propID, &property) == S_OK) {
		if (property.vt == VT_UI4 && property.ulVal) {
			value = property.ulVal;
			return true;
		}
		else if (property.vt == VT_UI8 && property.uhVal.QuadPart) {
			value = property.uhVal.QuadPart;
			return true;
		}
	}
	return false;
}

bool Archive::IsOpen(UString& fileName) {
	fileName = this->FileName;
	return !fileName.IsEmpty();
}

bool Archive::GetPassword(UString& password) {
	password = this->Password;
	return !password.IsEmpty();
}

void Archive::SetPassword(UString password) {
	this->Password = password;
}

int Archive::GetEntryCount() {
	UInt32 count;
	HRESULT result = this->InArchive->GetNumberOfItems(&count);
	if (result != S_OK) {
		throw CSystemException(result);
	}
	return count;
}

void Archive::GetEntry(UString& path, UInt64& size, int index) {
	if (!this->ReadProperty(path, kpidPath, index)) {
		//Could not read entry name for some reason.
		throw CSystemException(E_NOTIMPL);
	}
	if (!this->ReadProperty(size, kpidSize, index)) {
		//Not a file.
		size = 0;
	}
}

void Archive::ExtractEntry(CMyComPtr<IInStream>& stream, CMyComPtr<ArchiveExtractTask>& task, int index) {
	const UInt32 indices[1] = {
		index
	};

	ArchiveExtractCallback* callback = new ArchiveExtractCallback(this);
	if (!callback->OpenFiles(indices, 1)) {
		//Hopefully there's an error from IO.
		throw CSystemException(::GetLastError());
	}

	if (!callback->GetInStream(stream, index)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}

	task = new ArchiveExtractTask(this->InArchive, callback);
	if (!task->Start(indices, 1)) {
		//Hopefully there's an error from thread start.
		throw CSystemException(::GetLastError());
	}

	this->Tasks.Add(task);
}

ArchiveEntry* Archive::OpenEntry(int index) {
	ArchiveEntry* entry = new ArchiveEntry(this, index);
	entry->Open();
	return entry;
}

void Archive::CloseEntry(ArchiveEntry* entry) {
	entry->Close();
	delete entry;
}

void Archive::WaitForTasks() {
	for (unsigned a = 0; a < this->Tasks.Size(); a++) {
		this->Tasks[a]->Wait();
	}
	this->Tasks.Clear();
}

void Archive::Close() {
	this->WaitForTasks();
	if (this->InArchive) {
		this->InArchive->Close();
	}
	this->FileName.Empty();
	this->Password.Empty();
	this->FormatIndex = -1;
}

bool Archive::Cleanup() {
	return ArchiveExtractCallback::Cleanup();
}