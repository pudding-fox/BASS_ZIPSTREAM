// CompressionMode.h

#ifndef __ZIP_COMPRESSION_MODE_H
#define __ZIP_COMPRESSION_MODE_H

#include "../../../Common/MyString.h"

#ifndef _7ZIP_ST
#include "../../../Windows/System.h"
#endif

#include "../Common/HandlerOut.h"

namespace NArchive {
namespace NZip {

const CMethodId kMethodId_ZipBase = 0x040100;
const CMethodId kMethodId_BZip2   = 0x040202;

#if EXTRACT_ONLY
struct CBaseProps : public CCommonMethodProps
#else
struct CBaseProps: public CMultiMethodProps
#endif
{
  bool IsAesMode;
  Byte AesKeyMode;

  void Init()
  {
#if EXTRACT_ONLY
      CCommonMethodProps::InitCommon();
#else
    CMultiMethodProps::Init();
#endif
    
    IsAesMode = false;
    AesKeyMode = 3;
  }
};

struct CCompressionMethodMode: public CBaseProps
{
  CRecordVector<Byte> MethodSequence;
  bool PasswordIsDefined;
  AString Password;

  UInt64 _dataSizeReduce;
  bool _dataSizeReduceDefined;
  
  bool IsRealAesMode() const { return PasswordIsDefined && IsAesMode; }

  CCompressionMethodMode(): PasswordIsDefined(false)
  {
    _dataSizeReduceDefined = false;
    _dataSizeReduce = 0;
  }
};

}}

#endif
