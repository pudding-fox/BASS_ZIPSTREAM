#pragma once

#include "../7z/CPP/Windows/FileIO.h"

class CFile :
	public NWindows::NFile::NIO::CFileBase
{
public:
	bool Open(CFSTR fileName, bool overwrite);

	bool Read(void* data, UInt32 size, UInt32& processedSize);

	bool ReadPart(void* data, UInt32 size, UInt32& processedSize);

	bool Write(const void* data, UInt32 size, UInt32& processedSize);

	bool WritePart(const void* data, UInt32 size, UInt32& processedSize);

	bool SetLength(UInt64 length);
};

