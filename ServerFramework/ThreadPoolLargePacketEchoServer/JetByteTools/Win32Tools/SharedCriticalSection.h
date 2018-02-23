#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JETBYTE_TOOLS_WIN32_SHARED_CRITICAL_SECTION_INCLUDED__
#define JETBYTE_TOOLS_WIN32_SHARED_CRITICAL_SECTION_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File SharedCriticalSection.h
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
//
///////////////////////////////////////////////////////////////////////////////

#include "ICriticalSectionFactory.h"

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CSharedCriticalSection
///////////////////////////////////////////////////////////////////////////////

class CSharedCriticalSection : public ICriticalSectionFactory
{
   public :
   
      CSharedCriticalSection(
         const size_t numLocks,           // better if it's a prime number
         const size_t multiplier = 37);
      
      virtual ~CSharedCriticalSection();

      virtual CCriticalSection &GetCriticalSection(
         void *pKey) const;

      virtual CCriticalSection &GetCriticalSection(
         const _tstring &key) const;

      virtual CCriticalSection &GetCriticalSection(
         const size_t key) const;

   protected :

      size_t Hash(
         void *pKey) const;

      size_t Hash(
         const _tstring &key) const;

      const size_t m_numLocks;

      const size_t m_multiplier;

   private :

      CCriticalSection *m_pCrits;

      // No copies do not implement
      CSharedCriticalSection(const CSharedCriticalSection &rhs);
      CSharedCriticalSection &operator=(const CSharedCriticalSection &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CInstrumentedSharedCriticalSection
//
// You can use this to help tune the hash functions, it tracks how the hash
// distributes across the locks and reports on the distribution during 
// destruction.
//
///////////////////////////////////////////////////////////////////////////////

class CInstrumentedSharedCriticalSection : public CSharedCriticalSection 
{
   public :
   
      CInstrumentedSharedCriticalSection(
         const size_t numLocks,           // better if it's a prime number
         const size_t multiplier = 37);
      
      virtual ~CInstrumentedSharedCriticalSection();

      virtual CCriticalSection &GetCriticalSection(
         const size_t key) const;

   private :

      size_t *m_pStats;

      // No copies do not implement
      CInstrumentedSharedCriticalSection(const CInstrumentedSharedCriticalSection &rhs);
      CInstrumentedSharedCriticalSection &operator=(const CInstrumentedSharedCriticalSection &rhs);
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

#endif // JETBYTE_TOOLS_WIN32_SHARED_CRITICAL_SECTION_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file SharedCriticalSection.h
///////////////////////////////////////////////////////////////////////////////
