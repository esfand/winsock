///////////////////////////////////////////////////////////////////////////////
// File: IOPool.cpp
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

#include "IOPool.h"

#include "JetByteTools\Win32Tools\Utils.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Member not defined
//lint -esym(1526, CIOPool::CIOPool)
//lint -esym(1526, CIOPool::operator=)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using JetByteTools::Win32::Output;
using JetByteTools::Win32::_tstring;

///////////////////////////////////////////////////////////////////////////////
// CIOPool
///////////////////////////////////////////////////////////////////////////////

CIOPool::CIOPool(
   size_t numThreads /* = 0 */)
   :  JetByteTools::Win32::CSocketServer::IOPool(numThreads)
{

}

void CIOPool::OnError(
   const _tstring &message)
{
   Output(_T("OnError - ") + message);
}

void CIOPool::OnThreadCreated()
{
   Output(_T("OnThreadCreated"));
}

void CIOPool::OnThreadBeginProcessing()
{
//   Output(_T("OnThreadBeginProcessing"));
}

void CIOPool::OnThreadEndProcessing()
{
//   Output(_T("OnThreadEndProcessing"));
}

void CIOPool::OnThreadDestroyed()
{
   Output(_T("OnThreadDestroyed"));
}

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// End of file: IOPool.cpp
///////////////////////////////////////////////////////////////////////////////
