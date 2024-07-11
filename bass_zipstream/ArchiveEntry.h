#pragma once

#include "ArchiveExtractTask.h"

#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/Windows/FileIO.h"

class Archive;

class ArchiveEntry
{
public:
	ArchiveEntry(Archive* parent, int index);

	void Open();

	UInt64 GetPosition();

	UInt64 GetSize();

	UInt64 GetAvailable();

	bool IsCompleted();

	HRESULT GetResult();

	UInt32 Read(void* buffer, UInt32 length);

	bool Buffer(UInt64 position, UInt32 timeout);

	bool Seek(UInt64 position);

	void Close();

private:
	Archive* Parent;
	int Index;
	UString Path;
	UInt64 Size;

	CMyComPtr<IInStream> InStream;
	CMyComPtr<ArchiveExtractTask> Task;

	void Extract();
};
