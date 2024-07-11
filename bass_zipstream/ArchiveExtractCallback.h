#pragma once

#include "Interfaces.h"
#include "CFileStream.h"

class Archive;

class ArchiveExtractFile :
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP;

	UString Path;
	UInt64 Size;
	CMyComPtr<CFileStream> Stream;
	UInt32 Index;
};

class ArchiveExtractCallback :
	public IArchiveExtractCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
private:
	Archive* Parent;

	CObjectVector<CMyComPtr<ArchiveExtractFile>> Files;

	bool OpenFile(UInt32 index);

	bool OpenFile(ArchiveExtractFile* file, bool overwrite);

public:
	MY_QUERYINTERFACE_BEGIN2(IArchiveExtractCallback)
		MY_QUERYINTERFACE_ENTRY(ICryptoGetTextPassword)
		MY_QUERYINTERFACE_END
		MY_ADDREF_RELEASE
		INTERFACE_IArchiveExtractCallback(;)

public:
	ArchiveExtractCallback(Archive* parent);

	bool GetFileName(UString& path, UInt32 index);

	bool OpenFiles(const UInt32* indices, UInt32 count);

	void CloseReaders();

	void CloseWriters();

	void Close();

	bool GetInStream(CMyComPtr<IInStream>& stream, int index);

	bool GetOutStream(CMyComPtr<IOutStream>& stream, int index);

	STDMETHOD(CryptoGetTextPassword)(BSTR* password);

	static bool Cleanup();
};

