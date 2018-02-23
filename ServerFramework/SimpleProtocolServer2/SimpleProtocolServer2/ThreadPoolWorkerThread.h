#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef SIMPLE_PROTOCOL_SERVER_2_THREAD_POOL_WORKER_THREAD_INCLUDED__
#define SIMPLE_PROTOCOL_SERVER_2_THREAD_POOL_WORKER_THREAD_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File: ThreadPoolWorkerThread.h
/////////////////////////////////////////////////////////////////////////////////
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
// Private copy constructor
//lint -esym(1704, CThreadPoolWorkerThread::CThreadPoolWorkerThread) 
//
// No default constructor   
//lint -esym(1712, CThreadPoolWorkerThread)
//
///////////////////////////////////////////////////////////////////////////////

#include "ThreadPool.h"
#include "JetByteTools\Win32Tools\SocketServer.h"

///////////////////////////////////////////////////////////////////////////////
// Classes defined in other files...
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools
{
   namespace Win32
   {
      class CIOCompletionPort;
   }
}

///////////////////////////////////////////////////////////////////////////////
// CThreadPoolWorkerThread
///////////////////////////////////////////////////////////////////////////////

class CThreadPoolWorkerThread : public JetByteTools::Win32::CThreadPool::WorkerThread
{
   public :

      CThreadPoolWorkerThread(
         JetByteTools::Win32::CIOCompletionPort &iocp,
         JetByteTools::Win32::CEvent &messageReceivedEvent,
         CThreadPool &pool);

   private :

      virtual bool Initialise();

      virtual void Process(
         ULONG_PTR completionKey,
         DWORD dwNumBytes,
         OVERLAPPED *pOverlapped);

      virtual void Shutdown();

      void ProcessMessage(
         JetByteTools::Win32::CSocketServer::Socket *pSocket,
         const JetByteTools::Win32::CIOBuffer *pBuffer) const;

      void OnConnectionEstablished(
         JetByteTools::Win32::CSocketServer::Socket *pSocket,
         JetByteTools::Win32::CIOBuffer *pAddress);

      void OnConnectionClosing(
         JetByteTools::Win32::CSocketServer::Socket *pSocket);

      const CThreadPool &m_pool;

      // No copies do not implement
      CThreadPoolWorkerThread(const CThreadPoolWorkerThread &rhs);            
      CThreadPoolWorkerThread &operator=(const CThreadPoolWorkerThread &rhs); 
};

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

#endif // SIMPLE_PROTOCOL_SERVER_2_THREAD_POOL_WORKER_THREAD_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file: ThreadPoolWorkerThread.h
///////////////////////////////////////////////////////////////////////////////
