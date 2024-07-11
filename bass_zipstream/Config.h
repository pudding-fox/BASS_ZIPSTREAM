#pragma once

#include "../bass/bass.h"

#define MAX_CONFIGS 10
//Buffer 20% before creating stream.
#define DEFAULT_BUFFER_MIN 20
//Buffer timeout is 1 second.
#define DEFAULT_BUFFER_TIMEOUT 1000
//Double buffer is disabled.
#define DEFAULT_DOUBLE_BUFFER FALSE

typedef enum {
	ZS_BUFFER_MIN = 1,
	ZS_BUFFER_TIMEOUT = 2,
	ZS_DOUBLE_BUFFER = 3
} ZS_ATTRIBUTE;

VOID InitConfig();

BOOL GetConfig(ZS_ATTRIBUTE attrib, DWORD* value);

BOOL SetConfig(ZS_ATTRIBUTE attrib, DWORD value);