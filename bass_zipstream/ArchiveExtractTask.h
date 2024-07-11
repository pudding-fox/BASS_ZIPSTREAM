#include "ArchiveExtractCallback.h"

#include "..\7z\CPP\Windows\Thread.h"

#define S_PENDING ((HRESULT)1L)

static THREAD_FUNC_DECL ExtractThread(void* param);

class ArchiveExtractTask :
	public CMyUnknownImp,
	public IUnknown {

private:
	CMyComPtr<IInArchive> Archive;
	CMyComPtr<ArchiveExtractCallback> Callback;
	CRecordVector<UInt32> Indices;
	NWindows::CThread Thread;
	HRESULT Result;
	bool Completed;

public:
	MY_UNKNOWN_IMP;

	ArchiveExtractTask(CMyComPtr<IInArchive> archive, CMyComPtr<ArchiveExtractCallback> callback);

	bool Start(const UInt32* indices, UInt32 count);

	bool IsRunning(UInt32 index, UInt64& available);

	HRESULT Run();

	void Cancel();

	void Wait();

	bool IsCompleted();

	HRESULT GetResult();
};

