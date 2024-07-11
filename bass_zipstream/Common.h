#pragma once

#include "../7z/CPP/Common/MyException.h"

static void OK(HRESULT result) {
	if (result == S_OK) {
		return;
	}
	throw CSystemException(result);
}

//#include <stdio.h>
//
//static void WriteTrace(const wchar_t* format, ...)
//{
//	va_list args;
//	va_start(args, format);
//	wchar_t buffer[4096];
//	vswprintf(buffer, format, args);
//	va_end(args);
//	OutputDebugString(buffer);
//}