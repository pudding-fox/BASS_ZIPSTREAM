#pragma once

#include "Config.h"

#include "../bass/bass.h"
#include "../bass/bass_addon.h"

#ifndef BASSZIPSTREAMDEF
#define BASSZIPSTREAMDEF(f) WINAPI f
#endif

#if __cplusplus
extern "C" {
#endif

	BOOL BASSDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved);

	const VOID* BASSDEF(BASSplugin)(DWORD face);

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_SetConfig)(ZS_ATTRIBUTE attrib, DWORD value);

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_GetConfig)(ZS_ATTRIBUTE attrib, DWORD* value);

	HSTREAM BASSZIPSTREAMDEF(BASS_ZIPSTREAM_StreamCreateFile)(BOOL mem, const void* file, DWORD index, QWORD offset, QWORD length, DWORD flags);

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_Free)();

#if __cplusplus
}
#endif