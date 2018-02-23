#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JETBYTE_TOOLS_WIN32_SOCKET_SERVER_INCLUDED__
#define JETBYTE_TOOLS_WIN32_SOCKET_SERVER_INCLUDED__
///////////////////////////////////////////////////////////////////////////////
// File: SocketServer.h
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
// Class member is a reference
//lint -esym(1725, CSocketServer::m_allocator)
//lint -esym(1725, Socket::m_server)
//lint -esym(1725, WorkerThread::m_pool)
//lint -esym(1725, CSocketServer::m_pool)
//
// Private copy constructor
//lint -esym(1704, CSocketServer::CSocketServer)
//lint -esym(1704, Socket::Socket)
//lint -esym(1704, WorkerThread::WorkerThread)
//lint -esym(1704, IOPool::IOPool)
//lint -esym(1704, IOPoolHolder::IOPoolHolder)
//
// No default constructor
//lint -esym(1712, CSocketServer)
//lint -esym(1712, Socket)
//lint -esym(1712, WorkerThread)
//lint -esym(1712, SequenceData)
//lint -esym(1712, IOPoolHolder)
//
// Base class destructor isnt virtual
//lint -esym(1509, CUsesWinsock)
//lint -esym(1509, CIndexedOpaqueUserData)
//lint -esym(1509, ServerData)
//
// Data member hides inherited member
//lint -esym(1516, Allocator::m_activeList)
//lint -esym(1516, Allocator::m_freeList)
//
// Member hides non-virtual member
//lint -esym(1511, CIndexedOpaqueUserData::GetUserPtr)
//lint -esym(1511, CIndexedOpaqueUserData::SetUserPtr)
//lint -esym(1511, CIndexedOpaqueUserData::GetUserData)
//lint -esym(1511, CIndexedOpaqueUserData::SetUserData)
//
// Ignoring return value of function 
//lint -esym(534, Socket::Read)
//lint -esym(534, Socket::Write)
//
// 
///////////////////////////////////////////////////////////////////////////////

#include "UsesWinsock.h"
#include "Thread.h"
#include "IOCompletionPort.h"
#include "IOBuffer.h"
#include "ManualResetEvent.h"
#include "NodeList.h"
#include "IndexedOpaqueUserData.h"
#include "Todo.h"

#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// Classes defined in other files...
///////////////////////////////////////////////////////////////////////////////

class ICriticalSectionFactory;

///////////////////////////////////////////////////////////////////////////////
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

class CSocketServer : 
   protected CThread, 
   private CUsesWinsock, 
   protected CIOBuffer::Allocator
{
   public:

      class Socket;
      friend class Socket;

      class FilteringHelper;
      friend class FilteringHelper;

      class IOPool;
      friend class IOPool;

      virtual ~CSocketServer();

      using CThread::Start;

      void StartAcceptingConnections();
      void StopAcceptingConnections();

      void InitiateShutdown();

      void WaitForShutdownToComplete();

      Socket *Connect(
         const sockaddr_in &address);

   protected :

      CSocketServer(
         const ICriticalSectionFactory &lockFactory,
         IOPool &pool,
         unsigned long addressToListenOn,
         unsigned short portToListenOn,
         size_t maxFreeSockets,
         size_t maxFreeBuffers,
         size_t bufferSize,
         size_t numberOfUserDataSlots = 0,
         bool useSequenceNumbers = true,
         bool postZeroByteReads = false);

      CSocketServer(
         const ICriticalSectionFactory &lockFactory,
         unsigned long addressToListenOn,
         unsigned short portToListenOn,
         size_t maxFreeSockets,
         size_t maxFreeBuffers,
         size_t bufferSize,
         size_t numberOfUserDataSlots = 0,
         size_t numThreads = 0,
         bool useSequenceNumbers = true,
         bool postZeroByteReads = false);

      void PostFilteredReadRequest(
         Socket *pSocket);

      void PostFilteredReadRequest(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      void PostFilteredReadCompleted(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      void PostFilteredWriteRequest(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      void PostFilteredWriteCompleted(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      static void WriteCompleted(
         Socket *pSocket);

      static CSocketServer &GetServer(
         Socket *pSocket);

      //lint -e{1768} Virtual function has different access specifier to base class
      virtual int Run();

      enum ConnectionErrorSource
      {
         ZeroByteReadError,
         ReadError,
         WriteError
      };

      const unsigned long m_address;
      const unsigned short m_port;

   private :

      // Override this to create the listening socket of your choice

      virtual SOCKET CreateListeningSocket(
         unsigned long address,
         unsigned short port);

      // Override this to create the outbound socket of your choice

      virtual SOCKET CreateOutboundSocket(
         unsigned long address,
         unsigned short port);

      virtual void HandleOperation(
         Socket *pSocket,
         CIOBuffer *pBuffer,
         DWORD dwIoSize,
         DWORD lastError); 

      void ZeroByteRead(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      void Read(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      void Write(
         Socket *pSocket,
         CIOBuffer *pBuffer) const;

      // Interface for derived classes to receive state change notifications...

      virtual void OnStartAcceptingConnections() {}
      virtual void OnStopAcceptingConnections() {}
      virtual void OnShutdownInitiated() {}
      virtual void OnShutdownComplete() {}

      virtual void OnConnectionCreated() {}

      virtual void OnConnectionEstablished(
         Socket *pSocket,
         CIOBuffer *pAddress) = 0;

      virtual void OnConnectionClientClose(
          Socket * /*pSocket*/) {}

      virtual void OnConnectionReset(
          Socket * /*pSocket*/,
          DWORD /*lastError*/) {}

      virtual bool OnConnectionClosing(
         Socket * /*pSocket*/) { return false; }
      
      virtual void OnConnectionClosed(
         Socket * /*pSocket*/) {}

      virtual void OnSocketReleased(
         Socket * /*pSocket*/) {}

      virtual void OnConnectionDestroyed() {}

      virtual void OnConnectionError(
         ConnectionErrorSource source,
         Socket *pSocket,
         CIOBuffer *pBuffer,
         DWORD lastError);

      virtual void OnError(
         const _tstring &message);

      virtual void OnBufferCreated() {}
      virtual void OnBufferAllocated() {}
      virtual void OnBufferReleased() {}
      virtual void OnBufferDestroyed() {}

      virtual bool FilterReadRequest(
         Socket *pSocket,
         CIOBuffer **ppBuffer);

      virtual bool FilterReadCompleted(
         Socket *pSocket,
         CIOBuffer **ppBuffer);

      virtual bool FilterWriteRequest(
         Socket *pSocket,
         CIOBuffer **ppBuffer);

      virtual bool FilterWriteCompleted(
         Socket *pSocket,
         CIOBuffer **ppBuffer);

      virtual bool FilterSocketShutdown(
         Socket *pSocket,
         int how);

      virtual bool FilterSocketClose(
         Socket *pSocket);

      virtual void ReadCompleted(
         Socket *pSocket,
         CIOBuffer *pBuffer) = 0;

      virtual void WriteCompleted(
         Socket *pSocket,
         CIOBuffer *pBuffer);

      void ReleaseSockets();

      Socket *AllocateSocket(
         SOCKET theSocket);

      void ReleaseSocket(
         Socket *pSocket);

      void DestroySocket(
         Socket *pSocket);

      enum IO_Operation 
      { 
         IO_Zero_Byte_Read_Request,
         IO_Zero_Byte_Read_Completed,
         IO_Read_Request, 
         IO_Read_Completed, 
         IO_Write_Request, 
         IO_Write_Completed,
         IO_Filtered_Read_Request,
         IO_Filtered_Read_Completed, 
         IO_Filtered_Write_Request, 
         IO_Filtered_Write_Completed
      };

      void PostIoOperation(
         Socket *pSocket,
         CIOBuffer *pBuffer,
         IO_Operation operation) const;

      class IOPoolHolder
      {
         public :

            explicit IOPoolHolder(
               IOPool &pool);

            explicit IOPoolHolder(
               size_t numThreads);

            ~IOPoolHolder();

            const IOPool * const operator->() const;

         private :
         
            bool m_ourPool;
            IOPool *m_pPool;

            // No copies
            IOPoolHolder(const IOPoolHolder &rhs);
            IOPoolHolder &operator=(const IOPoolHolder &rhs);
      };

      IOPoolHolder m_pool;

      typedef JetByteTools::TNodeList<Socket> SocketList;

      SocketList m_activeList;
      SocketList m_freeList;

      SOCKET m_listeningSocket;

      CManualResetEvent m_shutdownEvent;

      CManualResetEvent m_acceptConnectionsEvent;

      const size_t m_maxFreeSockets;

      const size_t m_numberOfUserDataSlots;

      const bool m_useSequenceNumbers;

      const bool m_postZeroByteReads;

      const ICriticalSectionFactory &m_lockFactory;

      CCriticalSection &m_listManipulationSection;

      // No copies do not implement
      CSocketServer(const CSocketServer &rhs);
      CSocketServer &operator=(const CSocketServer &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServerIO
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::IOPool
{
   public:

      friend CSocketServer;
      friend CSocketServer::Socket;

      explicit IOPool(
         size_t numThreads = 0);

      virtual ~IOPool();

      void Start();

      void InitiateShutdown();

      void WaitForShutdownToComplete();

      void Dispatch(
         ULONG_PTR completionKey, 
         DWORD dwNumBytes, 
         OVERLAPPED *pOverlapped) const;

      void AssociateDevice(
         HANDLE hDevice, 
         ULONG_PTR completionKey) const;

   private :

      bool GetNextWorkItem(
         Socket *&pSocket,
         DWORD &dwIoSize, 
         CIOBuffer *&pBuffer,
         DWORD &lastError) const;

      class WorkerThread;

      friend class WorkerThread;

      virtual void OnError(
         const _tstring &message);

      virtual void OnThreadCreated() {}
      virtual void OnThreadBeginProcessing() {}
      virtual void OnThreadEndProcessing() {}
      virtual void OnThreadDestroyed() {}

      const size_t m_numThreads;

      CIOCompletionPort m_iocp;

      std::vector<WorkerThread *> m_workerThreads;

      // No copies do not implement
      IOPool(const IOPool &rhs);
      IOPool &operator=(const IOPool &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::IOPool::WorkerThread
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::IOPool::WorkerThread : public CThread
{
   public :

      explicit WorkerThread(
         CSocketServer::IOPool &pool);

      virtual ~WorkerThread();

      void InitiateShutdown() const;

      void WaitForShutdownToComplete() const;

   private :

      virtual int Run();

      CSocketServer::IOPool &m_pool;

      // No copies do not implement
      WorkerThread(const WorkerThread &rhs);
      WorkerThread &operator=(const WorkerThread &rhs);
};

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::Socket : 
   public CNodeList::Node, 
   private CIndexedOpaqueUserData
{
   public :

      friend class CSocketServer;
      friend class CSocketServer::IOPool::WorkerThread;

      bool Read(
         CIOBuffer *pBuffer = 0,
         bool throwOnFailure = false);

      bool Write(
         const char *pData, 
         size_t dataLength,
         bool throwOnFailure = false);

      bool Write(
         const BYTE *pData, 
         size_t dataLength,
         bool throwOnFailubre = false);

      bool Write(
         CIOBuffer *pBuffer,
         bool throwOnFailure = false);

      void AddRef();
      void Release();

      void Shutdown(
         int how = SD_BOTH);

      void Close();

      void AbortiveClose();

      bool IsConnected(
         int how = SD_BOTH) const;

      void WriteCompleted();
      void FilteredWritePending();

      void *GetUserPtr(
         const size_t index) const;
      
      void SetUserPtr(
         const size_t index,
         void *pData);

      unsigned long GetUserData(
         const size_t index) const;

      void SetUserData(
         const size_t index,
         unsigned long data);

      Socket *Connect(
         const sockaddr_in &address);

   private :

      Socket(
         CSocketServer &server,                                 
         SOCKET socket,
         bool useSequenceNumbers,
         size_t numberOfUserDataSlots,
         const ICriticalSectionFactory &lockFactory);

      ~Socket();
   
      void Attach(
         SOCKET socket);

      bool WritePending();

      void OnClientClose();
      void OnConnectionReset(
         DWORD lastError);

      bool IsValid();

      void InternalClose();

      void OnConnectionError(
         CSocketServer::ConnectionErrorSource source,
         CIOBuffer *pBuffer,
         DWORD lastError);

      void HandleOperation(
         CIOBuffer *pBuffer,
         DWORD dwIoSize,
         DWORD lastError);

      CCriticalSection &m_crit;

      CSocketServer &m_server;
      SOCKET m_socket;

      long m_ref;

      long m_outstandingWrites;

      //#pragma TODO("we could store all of these 1 bit flags in with the outstanding write count...")

      bool m_readShutdown;       
      bool m_writeShutdown;
      bool m_closing;
      long m_clientClosed;       
      
      const bool m_useSequenceNumbers;

      enum SequenceType 
      {
         ReadSequenceNo,
         WriteSequenceNo,
         FilteredReadSequenceNo,
         FilteredWriteSequenceNo,
         MaxSequenceNo
      };

      long GetSequenceNumber(
         SequenceType type) const;

      CIOBuffer *GetNextBuffer(
         CIOBuffer *pBuffer = 0) const;

   protected :    // Compiler complains about GetSequenceData() if SequenceData is private (!)

      struct SequenceData
      {
         explicit SequenceData(
            CCriticalSection &section);

         void Reset();

         long m_numbers[4];

         CIOBuffer::InOrderBufferList m_outOfSequenceWrites;
      };

   private :

      SequenceData *GetSequenceData() const;

      // No copies do not implement
      Socket(const Socket &rhs);
      Socket &operator=(const Socket &rhs);
};


///////////////////////////////////////////////////////////////////////////////
// CSocketServer::FilteringHelper
///////////////////////////////////////////////////////////////////////////////

class CSocketServer::FilteringHelper
{
   public :

      static void PostFilteredReadRequest(
         Socket *pSocket,
         CIOBuffer *pBuffer = 0);

      static void PostFilteredReadCompleted(
         Socket *pSocket,
         CIOBuffer *pBuffer);

      static void PostFilteredWriteRequest(
         Socket *pSocket,
         CIOBuffer *pBuffer);

      static void PostFilteredWriteCompleted(
         Socket *pSocket,
         CIOBuffer *pBuffer);

      static void WriteCompleted(
         Socket *pSocket);
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

#endif // JETBYTE_TOOLS_WIN32_SOCKET_SERVER_INCLUDED__

///////////////////////////////////////////////////////////////////////////////
// End of file: SocketServer.h
///////////////////////////////////////////////////////////////////////////////

