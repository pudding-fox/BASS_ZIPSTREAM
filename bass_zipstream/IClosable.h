#pragma once

#include "Interfaces.h"

MY_INTERFACE(IClosable, 0xaa)
{
	STDMETHOD(Close)() PURE;
};