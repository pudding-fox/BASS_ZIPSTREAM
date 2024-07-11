#pragma once

#include "../7z/CPP/Common/MyString.h"
#include "../7z/CPP/Common/MyVector.h"
#include "../7z/CPP/Windows/Synchronization.h"

typedef bool (*ArchiveExtractPromptHandler)(UString fileName, UString& password);

class ArchiveExtractPrompt
{
private:
	static CObjectVector<UString> FileNames;
	static NWindows::NSynchronization::CCriticalSection SyncRoot;

	static bool ContainsFile(UString fileName);

	static void AddFile(UString fileName);

	static void RemoveFile(UString fileName);

public:
	static ArchiveExtractPromptHandler Handler;

	static bool CanPrompt();

	static bool Prompt(UString fileName, UString& password);

	static bool Wait(UString fileName);
};

