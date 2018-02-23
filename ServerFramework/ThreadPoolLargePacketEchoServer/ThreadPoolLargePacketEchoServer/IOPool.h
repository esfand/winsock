#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef ECHO_SERVER_IO_POOL_INCLUDED__
#define ECHO_SERVER_IO_POOL_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File: IOPool.h
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2002 JetByte Limited.
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
// Private constructor
//lint -esym(1704, CIOPool::CIOPool)
//
// No default constructor
//lint -esym(1712, CIOPool)
//
///////////////////////////////////////////////////////////////////////////////

#include "JetByteTools\Win32Tools\SocketServer.h"
#include "JetByteTools\Win32Tools\tstring.h"

///////////////////////////////////////////////////////////////////////////////
// CIOPool
///////////////////////////////////////////////////////////////////////////////

class CIOPool: public JetByteTools::Win32::CSocketServer::IOPool
{
   public :

      explicit CIOPool(
         size_t numThreads = 0);

   private :

      virtual void OnError(
         const JetByteTools::Win32::_tstring &message);

      virtual void OnThreadCreated();
      virtual void OnThreadBeginProcessing();
      virtual void OnThreadEndProcessing();
      virtual void OnThreadDestroyed();

      // No copies do not implement
      CIOPool(const CIOPool &rhs);            
      CIOPool &operator=(const CIOPool &rhs); 
};

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

#endif // ECHO_SERVER_IO_POOL_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file: IOPool.h
///////////////////////////////////////////////////////////////////////////////
