///////////////////////////////////////////////////////////////////////////////
// File: NamedIndex.cpp
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

#pragma warning(disable: 4786)   // identifier was truncated to '255' characters in the debug information

#include "NamedIndex.h"
#include "Exception.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CNamedIndex
///////////////////////////////////////////////////////////////////////////////

CNamedIndex::CNamedIndex()
   : m_locked(false)
{
}

size_t CNamedIndex::Add(
   const _tstring &name)
{
   if (m_locked)
   {
      throw CException(_T("CNamedIndex::Add()"), _T("Can not add new named index. Indices are locked"));
   }

   for (size_t i = 0 ; i < m_names.size(); ++i)
   {
      if (m_names[i] == name)
      {
         throw CException(_T("CNamedIndex::Add()"), _T("Named index \"") + name + _T("\" already exists"));
      }
   }

   m_names.push_back(name);

   return m_names.size() - 1;
}

size_t CNamedIndex::Find(
   const _tstring &name) const
{
   for (size_t i = 0 ; i < m_names.size(); ++i)
   {
      if (m_names[i] == name)
      {
         return i;
      }
   }

   throw CException(_T("CNamedIndex::Find()"), _T("Named index \"") + name + _T("\" does not exist"));
}

size_t CNamedIndex::FindOrAdd(
   const _tstring &name)
{
   if (m_locked)
   {
      throw CException(_T("CNamedIndex::Add()"), _T("Can not add new named index. Indices are locked"));
   }

   for (size_t i = 0 ; i < m_names.size(); ++i)
   {
      if (m_names[i] == name)
      {
         return i;
      }
   }

   m_names.push_back(name);

   return m_names.size() - 1;
}

size_t CNamedIndex::GetMaxIndexValue() const
{
   return m_names.size();
}

size_t CNamedIndex::Lock()
{
   m_locked = true;

   return m_names.size();
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
// End of file: NamedIndex.cpp
///////////////////////////////////////////////////////////////////////////////
