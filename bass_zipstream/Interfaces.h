#pragma once

#include "../7z/CPP/7zip/Archive/IArchive.h"
#include "../7z/CPP/7zip/UI/Common/IFileExtractCallback.h"
#include "../7z/CPP/7zip/ICoder.h"
#include "../7z/CPP/7zip/IPassword.h"
#include "../7z/CPP/7zip/IStream.h"

#define MY_INTERFACE_SUB(i, base, x) DECL_INTERFACE_SUB(i, base, 0xaa, x)
#define MY_INTERFACE(i, x) MY_INTERFACE_SUB(i, IUnknown, x)