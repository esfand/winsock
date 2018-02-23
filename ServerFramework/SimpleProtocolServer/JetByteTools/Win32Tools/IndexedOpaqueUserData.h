#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JETBYTE_TOOLS_INDEXED_OPAQUE_USER_DATA_INCLUDED__
#define JETBYTE_TOOLS_INDEXED_OPAQUE_USER_DATA_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File IndexedOpaqueUserData.h
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
// Private copy constructor
//lint -esym(1704, CIndexedOpaqueUserData::CIndexedOpaqueUserData)
//
// No default constructor
//lint -esym(1712, CIndexedOpaqueUserData) 
//
// Default constructor implicitly called
//lint -esym(1926, CIndexedOpaqueUserData::m_userData)
//
// Member not defined
//lint -esym(1526, CIndexedOpaqueUserData::CIndexedOpaqueUserData)
//lint -esym(1526, CIndexedOpaqueUserData::operator=)
// 
///////////////////////////////////////////////////////////////////////////////

#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CIndexedOpaqueUserData
///////////////////////////////////////////////////////////////////////////////

class CIndexedOpaqueUserData
{
   public:

      inline void *GetUserPtr(
         const size_t index) const
      {
         return m_userData[index];
      }
      
      inline void SetUserPtr(
         const size_t index,
         void *pData)
      {
         m_userData[index] = pData;
      }

      inline unsigned long GetUserData(
         const size_t index) const
      {
         return reinterpret_cast<unsigned long>(m_userData[index]);
      }

      inline void SetUserData(
         const size_t index,
         unsigned long data)
      {
         m_userData[index] = reinterpret_cast<void*>(data);
      }

   protected :

      explicit inline CIndexedOpaqueUserData(
         const size_t numberOfSlots)
      {
         m_userData.resize(numberOfSlots);

         ClearUserData();
      }

      inline void ClearUserData()
      {
         for (size_t i = 0; i < m_userData.capacity(); ++i)
         {
            m_userData[i] = 0;
         }
      }


   private :

      typedef std::vector<void*> IndexedUserData;

      IndexedUserData m_userData;

      // No copies do not implement
      CIndexedOpaqueUserData(const CIndexedOpaqueUserData &rhs);
      CIndexedOpaqueUserData &operator=(const CIndexedOpaqueUserData &rhs);
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

#endif // JETBYTE_TOOLS_INDEXED_OPAQUE_USER_DATA_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of File: IndexedOpaqueUserData.h
///////////////////////////////////////////////////////////////////////////////

