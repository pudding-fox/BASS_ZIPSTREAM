#include "CFile.h"

bool CFile::Open(CFSTR fileName, bool overwrite) {
	if (overwrite) {
		return this->Create(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	}
	else {
		return this->Create(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL);
	}
}

bool CFile::Read(void* data, UInt32 size, UInt32& processedSize) {
	return this->ReadPart(data, size, processedSize);
}

bool CFile::ReadPart(void* data, UInt32 size, UInt32& processedSize) {
	DWORD processedLoc = 0;
	bool res = BOOLToBool(::ReadFile(_handle, data, size, &processedLoc, NULL));
	processedSize = (UInt32)processedLoc;
	return res;
}

bool CFile::Write(const void* data, UInt32 size, UInt32& processedSize) throw()
{
	processedSize = 0;
	do
	{
		UInt32 processedLoc = 0;
		bool res = this->WritePart(data, size, processedLoc);
		processedSize += processedLoc;
		if (!res) {
			return false;
		}
		if (processedLoc == 0) {
			return true;
		}
		data = (const void*)((const unsigned char*)data + processedLoc);
		size -= processedLoc;
	} while (size > 0);
	return true;
}

bool CFile::WritePart(const void* data, UInt32 size, UInt32& processedSize) {
	DWORD processedLoc = 0;
	bool res = BOOLToBool(::WriteFile(_handle, data, size, &processedLoc, NULL));
	processedSize = (UInt32)processedLoc;
	return res;
}

bool CFile::SetLength(UInt64 length)
{
	UInt64 newPosition;
	if (!this->Seek(length, newPosition)) {
		return false;
	}
	if (newPosition != length) {
		return false;
	}
	return BOOLToBool(::SetEndOfFile(_handle));
}