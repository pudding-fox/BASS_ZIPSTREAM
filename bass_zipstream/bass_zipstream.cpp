#pragma once

#include "bass_zipstream.h"
#include "ArchiveEntryInterface.h"
#include "Common.h"
#include "ErrorInterface.h"

#include <excpt.h>
#include <ehdata.h>

//2.4.0.0
#define BASS_VERSION 0x02040000

extern "C" {

	BASS_FILEPROCS procs = {
		ARCHIVE_CloseEntry,
		ARCHIVE_GetEntryLength,
		ARCHIVE_ReadEntry,
		ARCHIVE_SeekEntry
	};


	//I have no idea how to prevent linking against this routine in msvcrt.
	//It doesn't exist on Windows XP.
	//Hopefully it doesn't do anything important.
	int _except_handler4_common() {
		return 0;
	}

	extern EXCEPTION_DISPOSITION  CxxFrameHandler(
		EHExceptionRecord* pExcept,
		EHRegistrationNode* pRN,
		void* pContext,
		DispatcherContext* pDC);

	EXCEPTION_DISPOSITION  CxxFrameHandler3(
		EHExceptionRecord* pExcept,
		EHRegistrationNode* pRN,
		void* pContext,
		DispatcherContext* pDC) {
		return CxxFrameHandler(pExcept, pRN, pContext, pDC);
	}

	static const BASS_PLUGININFO plugin_info = { BASS_VERSION, 0, NULL };

	BOOL BASSDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved) {
		switch (reason) {
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls((HMODULE)dll);
			if (HIWORD(BASS_GetVersion()) != BASSVERSION || !GetBassFunc()) {
				MessageBoxA(0, "Incorrect BASS.DLL version (" BASSVERSIONTEXT " is required)", "BASS", MB_ICONERROR | MB_OK);
				return FALSE;
			}
			InitConfig();
			break;
		}
		return TRUE;
	}

	const VOID* BASSDEF(BASSplugin)(DWORD face) {
		switch (face) {
		case BASSPLUGIN_INFO:
			return (void*)&plugin_info;
		}
		return NULL;
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_GetConfig)(ZS_ATTRIBUTE attrib, DWORD* value) {
		return GetConfig(attrib, value);
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_SetConfig)(ZS_ATTRIBUTE attrib, DWORD value) {
		return SetConfig(attrib, value);
	}

	HSTREAM BASSZIPSTREAMDEF(BASS_ZIPSTREAM_StreamCreateFile)(BOOL mem, const void* file, DWORD index, QWORD offset, QWORD length, DWORD flags) {
		try {
			//Open the entry.
			ARCHIVE_ENTRY_HANDLE* handle;
			if (!ARCHIVE_OpenEntry(file, index, &handle)) {
				return 0;
			}
			//Ensure the minimum buffer percent is available.
			DWORD min;
			if (!BASS_ZIPSTREAM_GetConfig(ZS_BUFFER_MIN, &min)) {
				min = DEFAULT_BUFFER_MIN;
			}
			FLOAT factor = (FLOAT)min / 100;
			if (factor > 0) {
				if (factor > 1) {
					factor = 1;
				}
				QWORD position = (QWORD)((FLOAT)ARCHIVE_GetEntryLength(handle) * factor);
				if (!ARCHIVE_BufferEntry(position, handle)) {
					//Failed to get required data.
					ARCHIVE_CloseEntry(handle);
					return 0;
				}
			}
			//Determine whether we should double buffer (using the BASS stream buffer).
			DWORD doubleBuffer;
			if (!BASS_ZIPSTREAM_GetConfig(ZS_DOUBLE_BUFFER, &doubleBuffer)) {
				doubleBuffer = DEFAULT_DOUBLE_BUFFER;
			}
			DWORD system;
			if (doubleBuffer) {
				system = STREAMFILE_BUFFER;
			}
			else {
				system = STREAMFILE_NOBUFFER;
			}
			//Attempt to create the stream, if this fails ARCHIVE_CloseEntry will be called immidiately.
			return BASS_StreamCreateFileUser(system, flags, &procs, handle);
		}
		catch (CSystemException e) {
			ARCHIVE_SetLastError(e.ErrorCode);
			return 0;
		}
	}
}