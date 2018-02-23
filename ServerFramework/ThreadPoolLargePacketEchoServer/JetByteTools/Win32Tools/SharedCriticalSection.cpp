///////////////////////////////////////////////////////////////////////////////
// File: SharedCriticalSection.cpp
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

#include "SharedCriticalSection.h"
#include "CriticalSection.h"
#include "Exception.h"
#include "Utils.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using std::cout;
using std::endl;

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CSharedCriticalSection
///////////////////////////////////////////////////////////////////////////////

CSharedCriticalSection::CSharedCriticalSection(
   const size_t numLocks,
   const size_t multiplier /* = 37 */)                                              
   :  m_numLocks(numLocks),
      m_multiplier(multiplier),
      m_pCrits(new CCriticalSection[numLocks])
{
}
      
CSharedCriticalSection::~CSharedCriticalSection()
{
   delete [] m_pCrits;
}

CCriticalSection &CSharedCriticalSection::GetCriticalSection(
   void *pKey) const
{
   return GetCriticalSection(Hash(pKey));
}

CCriticalSection &CSharedCriticalSection::GetCriticalSection(
   const _tstring &key) const
{
   return GetCriticalSection(Hash(key));
}

CCriticalSection &CSharedCriticalSection::GetCriticalSection(
   const size_t key) const
{
   if (key > m_numLocks)
   {
      throw CException(_T("CSharedCriticalSection::GetCriticalSection()"), _T("Key too large"));
   }

   return m_pCrits[key];
}

size_t CSharedCriticalSection::Hash(
   void *pKey) const
{
   size_t hash = reinterpret_cast<size_t>(pKey);

   return hash % m_numLocks;
}

size_t CSharedCriticalSection::Hash(
   const _tstring &key) const
{
   size_t hash = 0;

   const unsigned char *p = reinterpret_cast<const unsigned char *>(key.c_str());

   while (*p)
   {
      hash = m_multiplier * hash + *p;

      p++;
   }

   return hash % m_numLocks;
}
   
///////////////////////////////////////////////////////////////////////////////
// CInstrumentedSharedCriticalSection
///////////////////////////////////////////////////////////////////////////////

CInstrumentedSharedCriticalSection::CInstrumentedSharedCriticalSection(
   const size_t numLocks,           
   const size_t multiplier)
   :  CSharedCriticalSection(numLocks, multiplier),
      m_pStats(new size_t[numLocks])
{
   for (size_t i = 0; i < m_numLocks; ++i)
   {
      m_pStats[i] = 0;
   }
}     

CInstrumentedSharedCriticalSection::~CInstrumentedSharedCriticalSection()
{
   size_t max = 0;
   size_t min = -1;
   size_t ave = 0;

   for (size_t i = 0; i < m_numLocks; ++i)
   {
      const size_t stat = m_pStats[i];
      
      if (stat > max)
      {
         max = stat;
      }

      if (stat < min)
      {
         min = stat;
      }

      ave += stat;

      cout << ToStringA(i) << "\t- " << ToStringA(stat) << endl;
   }

   ave /= m_numLocks;

   cout << "Min: " << ToStringA(min) << endl;
   cout << "Max: " << ToStringA(max) << endl;
   cout << "Ave: " << ToStringA(ave) << endl;

   delete [] m_pStats;
}

CCriticalSection &CInstrumentedSharedCriticalSection::GetCriticalSection(
   const size_t key) const
{
   CCriticalSection &crit = CSharedCriticalSection::GetCriticalSection(key);

   m_pStats[key]++;

   return crit;
}

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

///////////////////////////////////////////////////////////////////////////////
// End of file: SharedCriticalSection.cpp
///////////////////////////////////////////////////////////////////////////////
