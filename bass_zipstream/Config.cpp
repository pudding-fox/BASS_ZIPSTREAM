#include "Config.h"

static DWORD config[MAX_CONFIGS] = { 0 };

VOID InitConfig() {
	SetConfig(ZS_BUFFER_MIN, DEFAULT_BUFFER_MIN);
	SetConfig(ZS_BUFFER_TIMEOUT, DEFAULT_BUFFER_TIMEOUT);
	SetConfig(ZS_DOUBLE_BUFFER, DEFAULT_DOUBLE_BUFFER);
}

BOOL GetConfig(ZS_ATTRIBUTE attrib, DWORD* value) {
	if (*value = config[attrib]) {
		return TRUE;
	}
	return FALSE;
}

BOOL SetConfig(ZS_ATTRIBUTE attrib, DWORD value) {
	config[attrib] = value;
	return TRUE;
}