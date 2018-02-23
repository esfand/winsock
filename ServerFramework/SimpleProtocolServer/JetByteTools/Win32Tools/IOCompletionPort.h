#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JETBYTE_TOOLS_WIN32_IO_COMPLETION_PORT_INCLUDED__
#define JETBYTE_TOOLS_WIN32_IO_COMPLETION_PORT_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File IOCompletionPort.h
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1997 - 2003 JetByte Limited.
//
// JetByte Limited grants you ("Licensee") a non-exclusive, royalty free, 
// licence to use, modify and redistribute this software in source and binary 
// code form, provided that i) this copyright notice and licence appear on all 
// copies of the software; and ii) Licensee does not utilize the software in a 
// manner which is disparaging to JetByte Limited.
//
// This software is provided "as is" without a warranty of any kind. All 
// express or implied conditions, representations and warranties, including
// any implied warranty of merchantability, fitness for a particular purpose
// or non-infringement, are hereby excluded. JetByte Limited and its licensors 
// shall not be liable for any damages suffered by licensee as a result of 
// using, modifying or distributing the software or its derivatives. In no
// event will JetByte Limited be liable for any lost revenue, profit or data,
// or for direct, indirect, special, consequential, incidental or punitive
// damages, however caused and regardless of the theory of liability, arising 
// out of the use of or inability to use software, even if JetByte Limited 
// has been advised of the possibility of such damages.
//
// This software is not designed or intended for use in on-line control of 
// aircraft, air traffic, aircraft navigation or aircraft communications; or in 
// the design, construction, operation or maintenance of any nuclear 
// facility. Licensee represents and warrants that it will not use or 
// redistribute the Software for such purposes. 
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//lint -esym(1704, CIOCompletionPort::CIOCompletionPort) private copy ctor
//lint -esym(1712, CIOCompletionPort)                    no default ctor
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#include "NoPlatformSDK.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CIOCompletionPort
///////////////////////////////////////////////////////////////////////////////

class CIOCompletionPort
{
   public:
   
      explicit CIOCompletionPort(
         size_t maxConcurrency);

      ~CIOCompletionPort();

   void AssociateDevice(
      HANDLE hDevice, 
      ULONG_PTR completionKey) const;

   void PostStatus(
      ULONG_PTR completionKey, 
      DWORD dwNumBytes = 0, 
      OVERLAPPED *pOverlapped = 0) const;

   void GetStatus(
      ULONG_PTR *pCompletionKey, 
      PDWORD pdwNumBytes,
      OVERLAPPED **ppOverlapped) const;

   bool GetStatus(
      ULONG_PTR *pCompletionKey, 
      PDWORD pdwNumBytes,
      OVERLAPPED **ppOverlapped, 
      DWORD dwMilliseconds) const;

   bool GetStatus(
      ULONG_PTR *pCompletionKey, 
      PDWORD pdwNumBytes,
      OVERLAPPED **ppOverlapped,
      DWORD *pLastError) const;

   private:
      
      HANDLE m_iocp;

      // No copies do not implement
      CIOCompletionPort(const CIOCompletionPort &rhs);
      CIOCompletionPort &operator=(const CIOCompletionPort &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Win32
} // End of namespace JetByteTools 

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

#endif // JETBYTE_TOOLS_WIN32_IO_COMPLETION_PORT_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file IOCompletionPort.h
///////////////////////////////////////////////////////////////////////////////
