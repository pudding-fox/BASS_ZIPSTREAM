#pragma once

#include "../7z/CPP/Common/MyString.h"

static bool GetExtension(UString& fileName, UString& extension) {
	int position = fileName.ReverseFind(L'.');
	if (position < 0) {
		return false;
	}
	extension = fileName.Mid(position, fileName.Len() - position);
	return true;
}