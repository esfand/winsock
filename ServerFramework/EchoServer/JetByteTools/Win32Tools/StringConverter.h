#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JETBYTE_TOOLS_WIN32_STRING_CONVERTER__
#define JETBYTE_TOOLS_WIN32_STRING_CONVERTER__
///////////////////////////////////////////////////////////////////////////////
// File: StringConverter.h
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2003 JetByte Limited.
//
// This software is provided "as is" without a warranty of any kind. All 
// express or implied conditions, representations and warranties, including
// any implied warranty of merchantability, fitness for a particular purpose
// or non-infringement, are hereby excluded. JetByte Limited and its licensors 
// shall not be liable for any damages suffered by licensee as a result of 
// using the software. In no event will JetByte Limited be liable for any 
// lost revenue, profit or data, or for direct, indirect, special, 
// consequential, incidental or punitive damages, however caused and regardless 
// of the theory of liability, arising out of the use of or inability to use 
// software, even if JetByte Limited has been advised of the possibility of 
// such damages.
//
///////////////////////////////////////////////////////////////////////////////

#include "tstring.h"

#include <objbase.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CStringConverter
///////////////////////////////////////////////////////////////////////////////

class CStringConverter
{
   public :
      
      // Ato

      static _tstring AtoT(
         const std::string &input);

      static _tstring AtoT(
         const char *pInput);

      static _tstring AtoT(
         const char *pInput,
         const size_t inputLength);

      static std::wstring AtoW(
         const std::string &input);

      static std::wstring AtoW(
         const char *pInput);

      static std::wstring AtoW(
         const char *pInput,
         const size_t inputLength);

      static BSTR AtoBSTR(
         const std::string &input);

      static BSTR AtoBSTR(
         const char *pInput,
         const size_t inputLength);

      // Wto

      static _tstring WtoT(
         const std::wstring &input);

      static _tstring WtoT(
         const wchar_t *pInput);

      static _tstring WtoT(
         const wchar_t *pInput,
         const size_t inputLength);

      static std::string WtoA(
         const std::wstring &input);

      static std::string WtoA(
         const wchar_t *pInput);

      static std::string WtoA(
         const wchar_t *pInput,
         const size_t inputLength);

      static BSTR WtoBSTR(
         const std::wstring &input);

      // Tto

      static std::string TtoA(
         const _tstring &input);

      static std::wstring TtoW(
         const _tstring &input);

      static BSTR TtoBSTR(
         const _tstring &input);

      // BSTRto

      static _tstring BSTRtoT(
	      const BSTR bstr);

      static std::string BSTRtoA(
	      const BSTR bstr);

      static std::wstring BSTRtoW(
	      const BSTR bstr);

      // UTF8to

      static std::wstring UTF8toW(
         const std::string &input);
};

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

} // End of namespace Win32
} // End of namespace JetByteTools 

#endif // JETBYTE_TOOLS_WIN32_STRING_CONVERTER__

///////////////////////////////////////////////////////////////////////////////
// End of file: StringConverter.h
///////////////////////////////////////////////////////////////////////////////

