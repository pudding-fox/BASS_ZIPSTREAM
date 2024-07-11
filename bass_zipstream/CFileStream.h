#pragma once

#include "Interfaces.h"
#include "CFile.h"

#include "../7z/C/Threads.h"

class CFileStream :
	public CMyUnknownImp,
	public IUnknown
{
private:
	CFile File;
	UInt64 Size;
	CMyComPtr<IInStream> Reader;
	CMyComPtr<IOutStream> Writer;
	CSemaphore Semaphore;
	bool IsConcurrent;

public:
	MY_UNKNOWN_IMP;
	bool ReadDirty;
	bool WriteDirty;

	bool GetReader(CMyComPtr<IInStream>& stream);

	bool GetWriter(CMyComPtr<IOutStream>& stream);

	STDMETHOD(Begin)();

	STDMETHOD(End)();

	STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);

	STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize);

	STDMETHOD(Write)(const void* data, UInt32 size, UInt32* processedSize);

	STDMETHOD(SetSize)(UInt64 newSize);

	bool Open(CFSTR fileName, UInt64 size);

	bool Create(CFSTR fileName, UInt64 size);

	void CloseReader();

	void CloseWriter();

	UInt64 GetSize();

	~CFileStream();
};