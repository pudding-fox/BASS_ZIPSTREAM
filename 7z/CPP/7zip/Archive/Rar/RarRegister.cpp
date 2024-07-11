#include "StdAfx.h"

#include "../../Common/RegisterArc.h"

#include "RarHandler.h"

namespace NArchive {
	namespace NRar {

#define SIGNATURE { 0x52 , 0x61, 0x72, 0x21, 0x1a, 0x07, 0x00 }

		static const Byte kMarker[NHeader::kMarkerSize] = SIGNATURE;

		REGISTER_ARC_I(
			"Rar", "rar r00", 0, 3,
			kMarker,
			0,
			NArcInfoFlags::kFindSignature,
			NULL)
	}
}