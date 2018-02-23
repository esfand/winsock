#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef SIMPLE_PROTOCOL_SERVER_2_THREAD_POOL_INCLUDED__
#define SIMPLE_PROTOCOL_SERVER_2_THREAD_POOL_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File: ThreadPool.h
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
//lint -esym(1704, CThreadPool::CThreadPool) 
//
///////////////////////////////////////////////////////////////////////////////

#include "JetByteTools\Win32Tools\SocketServer.h"
#include "JetByteTools\Win32Tools\ThreadPool.h"

///////////////////////////////////////////////////////////////////////////////
// Classes defined in other files...
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools
{
   namespace Win32
   {
      class CEvent;
      class CNamedIndex;
   }
}

///////////////////////////////////////////////////////////////////////////////
// CThreadPool
///////////////////////////////////////////////////////////////////////////////

class CThreadPool : public JetByteTools::Win32::CThreadPool
{
   public:

      CThreadPool(
         JetByteTools::Win32::CNamedIndex &userDataSlots,
         size_t initialThreads,
         size_t minThreads,
         size_t maxThreads,
         size_t maxDormantThreads,
         size_t poolMaintPeriod,
         size_t dispatchTimeout,
         size_t maxThreadsDispatchTimeout);

      void DispatchConnectionEstablished(
         JetByteTools::Win32::CSocketServer::Socket *pSocket,
         JetByteTools::Win32::CIOBuffer *pAddress);

      void DispatchConnectionClosing(
         JetByteTools::Win32::CSocketServer::Socket *pSocket);

      void DispatchReadCompleted(
         JetByteTools::Win32::CSocketServer::Socket *pSocket,
         JetByteTools::Win32::CIOBuffer *pBuffer);

      void OnSocketReleased(
         JetByteTools::Win32::CSocketServer::Socket *pSocket);

      enum DispatchEvents
      {
         ConnectionEstablished,
         ConnectionClosing,
         ReadCompleted
      };

      class CPerConnectionData
      {
         public :
      
            CPerConnectionData();

            void SetAddress(
               const JetByteTools::Win32::_tstring &address);

            void UseConnection();

            JetByteTools::Win32::_tstring GetConnectionDetails() const;

         private :

            long m_numUses;

            JetByteTools::Win32::_tstring m_address;

            // No copies, do not implement
            CPerConnectionData(const CPerConnectionData &rhs);
            CPerConnectionData &operator=(const CPerConnectionData &rhs);
      };

      CPerConnectionData *GetPerConnectionData(
         JetByteTools::Win32::CSocketServer::Socket *pSocket) const;

   private :

      void DoDispatch(
         JetByteTools::Win32::CSocketServer::Socket *pSocket,
         JetByteTools::Win32::CIOBuffer *pBuffer,
         DispatchEvents event);

      virtual void OnThreadStarted();
      virtual void OnThreadInitialised();
      virtual void OnProcessingStarted();
      virtual void OnReceivedMessage();
      virtual void OnProcessingStopped();
      virtual void OnThreadStopped();

      virtual JetByteTools::Win32::CThreadPool::WorkerThread *CreateWorkerThread(
         JetByteTools::Win32::CIOCompletionPort &iocp,
         JetByteTools::Win32::CEvent &messageReceivedEvent,
         JetByteTools::Win32::CThreadPool &pool);

      const size_t m_userDataIndex;

      // No copies do not implement
      CThreadPool(const CThreadPool &rhs);
      CThreadPool &operator=(const CThreadPool &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

#endif // SIMPLE_PROTOCOL_SERVER_2_THREAD_POOL_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file: ThreadPool.h
///////////////////////////////////////////////////////////////////////////////
