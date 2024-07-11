#pragma once

#include "../bass/bass.h"

#ifndef ARCHIVEDEF
#define ARCHIVEDEF(f) WINAPI f
#endif

#if __cplusplus
extern "C" {
#endif

	typedef struct ARCHIVE_ENTRY_HANDLE {
		void* archive;
		void* entry;
	} ARCHIVE_ENTRY_HANDLE;

	BOOL ARCHIVEDEF(ARCHIVE_OpenEntry)(const void* file, DWORD index, ARCHIVE_ENTRY_HANDLE** handle);

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryPosition)(void* user);

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryLength)(void* user);

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryAvailable)(void* user);

	BOOL ARCHIVEDEF(ARCHIVE_GetEntryResult)(HRESULT* result, void* user);

	BOOL ARCHIVEDEF(ARCHIVE_BufferEntry)(QWORD position, void* user);

	DWORD ARCHIVEDEF(ARCHIVE_ReadEntry)(void* buffer, DWORD length, void* user);

	DWORD ARCHIVEDEF(ARCHIVE_ReadEntryWithOffset)(void* buffer, DWORD offset, DWORD length, void* user);

	BOOL ARCHIVEDEF(ARCHIVE_SeekEntry)(QWORD offset, void* user);

	BOOL ARCHIVEDEF(ARCHIVE_IsEOF)(void* user);

	VOID ARCHIVEDEF(ARCHIVE_CloseEntry)(void* user);

#if __cplusplus
}
#endif