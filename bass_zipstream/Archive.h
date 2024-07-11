#pragma once

#include "ArchiveEntry.h"
#include "Codecs.h"
#include "Interfaces.h"

#include "..\7z\CPP\7zip\UI\Common\LoadCodecs.h"

class ArchiveEntry;

class Archive
{
private:
	UString FileName;
	UString Password;
	int FormatIndex;

	CMyComPtr<CCodecs> Codecs;
	CMyComPtr<IInStream> InStream;
	CMyComPtr<IInArchive> InArchive;
	CObjectVector<CMyComPtr<ArchiveExtractTask>> Tasks;

	void LoadCodecs();

	void LoadFormat();

	void CreateInStream();

	void CreateInArchive();

	void OpenInArchive();

	bool ReadProperty(UString& value, PROPID propID, int index);

	bool ReadProperty(UInt64& value, PROPID propID, int index);

	void WaitForTasks();

public:
	Archive();

	int GetFormatCount();

	void GetFormat(UString& name, UString& extensions, int index);

	void Open(UString& fileName);

	bool IsOpen(UString& fileName);

	bool GetPassword(UString& password);

	void SetPassword(UString password);

	int GetEntryCount();

	void GetEntry(UString& path, UInt64& size, int index);

	void ExtractEntry(CMyComPtr<IInStream>& stream, CMyComPtr<ArchiveExtractTask>& task, int index);

	ArchiveEntry* OpenEntry(int index);

	void CloseEntry(ArchiveEntry* entry);

	void Close();

	static bool Cleanup();
};

