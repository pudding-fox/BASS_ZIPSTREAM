#include "ArchiveExtractPrompt.h"

CObjectVector<UString> ArchiveExtractPrompt::FileNames;
NWindows::NSynchronization::CCriticalSection ArchiveExtractPrompt::SyncRoot;

bool ArchiveExtractPrompt::ContainsFile(UString fileName) {
	NWindows::NSynchronization::CCriticalSectionLock lock(ArchiveExtractPrompt::SyncRoot);
	int index = ArchiveExtractPrompt::FileNames.FindInSorted(fileName);
	return index >= 0;
}

void ArchiveExtractPrompt::AddFile(UString fileName) {
	NWindows::NSynchronization::CCriticalSectionLock lock(ArchiveExtractPrompt::SyncRoot);
	ArchiveExtractPrompt::FileNames.Add(fileName);
}

void ArchiveExtractPrompt::RemoveFile(UString fileName) {
	NWindows::NSynchronization::CCriticalSectionLock lock(ArchiveExtractPrompt::SyncRoot);
	int index = ArchiveExtractPrompt::FileNames.FindInSorted(fileName);
	if (index >= 0) {
		ArchiveExtractPrompt::FileNames.Delete(index);
	}
}

ArchiveExtractPromptHandler ArchiveExtractPrompt::Handler = nullptr;

bool ArchiveExtractPrompt::CanPrompt() {
	return ArchiveExtractPrompt::Handler != nullptr;
}

bool ArchiveExtractPrompt::Prompt(UString fileName, UString& password) {
	if (!ArchiveExtractPrompt::CanPrompt()) {
		return false;
	}
	ArchiveExtractPrompt::AddFile(fileName);
	bool result = ArchiveExtractPrompt::Handler(fileName, password);
	ArchiveExtractPrompt::RemoveFile(fileName);
	return result;
}

bool ArchiveExtractPrompt::Wait(UString fileName) {
	if (!ArchiveExtractPrompt::CanPrompt()) {
		return false;
	}
	bool result = false;
	while (ArchiveExtractPrompt::ContainsFile(fileName)) {
		result = true;
		Sleep(100);
	}
	return result;
}