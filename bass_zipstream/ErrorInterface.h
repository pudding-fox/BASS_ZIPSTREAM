#pragma once

#include "../bass/bass.h"

#ifndef ARCHIVEDEF
#define ARCHIVEDEF(f) WINAPI f
#endif

#define E_NOT_A_FILE			 _HRESULT_TYPEDEF_(0x9000100EL)
#define E_TIMEOUT					 _HRESULT_TYPEDEF_(0x9000200EL)

#if __cplusplus
extern "C" {
#endif

	HRESULT ARCHIVEDEF(ARCHIVE_GetLastError)();

	VOID ARCHIVEDEF(ARCHIVE_SetLastError)(HRESULT result);

#if __cplusplus
}
#endif