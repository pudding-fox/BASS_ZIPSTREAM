#include "Archive.h"
#include "ArchiveExtractPrompt.h"
#include "Common.h"

#include "../7z/CPP/7zip/Common/FileStreams.h"

ArchiveEntry::ArchiveEntry(Archive* parent, int index) {
	this->Parent = parent;
	this->Index = index;
	this->Size = 0;
}

void ArchiveEntry::Open() {
	this->Parent->GetEntry(this->Path, this->Size, this->Index);
	if (this->Size) {
		this->Extract();
	}
}

void ArchiveEntry::Extract() {
	this->Parent->ExtractEntry(this->InStream, this->Task, this->Index);
}

UInt64 ArchiveEntry::GetPosition() {
	UInt64 position;
	HRESULT result = this->InStream->Seek(0, STREAM_SEEK_CUR, &position);
	if (result != S_OK) {
		throw CSystemException(result);
	}
	return position;
}

UInt64 ArchiveEntry::GetSize() {
	return this->Size;
}

UInt64 ArchiveEntry::GetAvailable() {
	UInt64 available;
	if (this->Task->IsRunning(this->Index, available)) {
		return available;
	}
	if (this->Task->GetResult() != S_OK) {
		return 0;
	}
	return this->Size;
}

bool ArchiveEntry::IsCompleted() {
	return this->Task->IsCompleted();
}

HRESULT ArchiveEntry::GetResult() {
	return this->Task->GetResult();
}

UInt32 ArchiveEntry::Read(void* buffer, UInt32 length) {
	UInt32 count;
	HRESULT result = this->InStream->Read(buffer, length, &count);
	if (result != S_OK) {
		throw CSystemException(result);
	}
	return count;
}

bool ArchiveEntry::Buffer(UInt64 position, UInt32 timeout) {
	if (position > this->Size) {
		return false;
	}
	if (this->Task->IsCompleted()) {
		return this->GetAvailable() >= position;
	}
	//Extract task is in progress, loop until we either have enough data or the timeout elapses.
retry:
	for (unsigned a = 0; a < timeout; a++) {
		if (this->GetAvailable() >= position) {
			return true;
		}
		Sleep(1);
	}
	//If timed out then check if we're prompting for input (likely password).
	UString fileName;
	if (this->Parent->IsOpen(fileName)) {
		if (ArchiveExtractPrompt::Wait(fileName)) {
			//If a prompt was presented then try again.
			goto retry;
		}
	}
	return false;
}

bool ArchiveEntry::Seek(UInt64 position) {
	if (position > this->GetAvailable()) {
		return false;
	}
	if (this->InStream->Seek(position, STREAM_SEEK_SET, nullptr) == S_OK) {
		return true;
	}
	return false;
}

void ArchiveEntry::Close() {
	if (this->Task) {
		this->Task->Cancel();
	}
}