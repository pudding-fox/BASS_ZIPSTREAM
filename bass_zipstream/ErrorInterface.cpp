#include "ErrorInterface.h"

static HRESULT LastError = S_OK;

HRESULT ARCHIVEDEF(ARCHIVE_GetLastError)() {
	HRESULT lastError = LastError;
	LastError = S_OK;
	return lastError;
}

VOID ARCHIVEDEF(ARCHIVE_SetLastError)(HRESULT result) {
	LastError = result;
}