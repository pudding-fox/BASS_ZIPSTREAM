#include "ArchiveExtractTask.h"
#include "Common.h"

ArchiveExtractTask::ArchiveExtractTask(CMyComPtr<IInArchive> archive, CMyComPtr<ArchiveExtractCallback> callback) {
	this->Archive = archive;
	this->Callback = callback;
	this->Result = S_PENDING;
	this->Completed = false;
}

bool ArchiveExtractTask::Start(const UInt32* indices, UInt32 count) {
	this->Indices.Clear();
	for (unsigned a = 0; a < count; a++) {
		this->Indices.Add(indices[a]);
	}
	this->Result = S_PENDING;
	this->Completed = false;
	return this->Thread.Create(ExtractThread, this) == 0;
}

HRESULT ArchiveExtractTask::Run() {
	UInt32* indices = (UInt32*)malloc(sizeof(UInt32) * this->Indices.Size());
	if (!indices) {
		this->Result = E_OUTOFMEMORY;
	}
	else {
		for (unsigned a = 0; a < this->Indices.Size(); a++) {
			indices[a] = this->Indices[a];
		}
		this->Result = this->Archive->Extract(indices, this->Indices.Size(), false, this->Callback);
		free(indices);
	}
	this->Callback->CloseWriters();
	this->Completed = true;
	return this->Result;
}

bool ArchiveExtractTask::IsRunning(UInt32 index, UInt64& available) {
	if (this->Completed) {
		return false;
	}
	for (unsigned a = 0; a < this->Indices.Size(); a++) {
		if (this->Indices[a] == index) {
			CMyComPtr<IOutStream> stream;
			if (this->Callback->GetOutStream(stream, index) && stream->Seek(0, SEEK_CUR, &available) == S_OK) {
				return true;
			}
			break;
		}
	}
	return false;
}

void ArchiveExtractTask::Cancel() {
	this->Callback->Close();
}

void ArchiveExtractTask::Wait() {
	this->Thread.Wait();
}

bool ArchiveExtractTask::IsCompleted() {
	return this->Completed;
}

HRESULT ArchiveExtractTask::GetResult() {
	return this->Result;
}

static THREAD_FUNC_DECL ExtractThread(void* param) {
	ArchiveExtractTask* task = (ArchiveExtractTask*)param;
	task->Run();
	return 0;
}