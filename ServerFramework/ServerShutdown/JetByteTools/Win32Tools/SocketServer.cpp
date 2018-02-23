///////////////////////////////////////////////////////////////////////////////
// File: SocketServer.cpp
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

#include "SocketServer.h"
#include "IOCompletionPort.h"
#include "Win32Exception.h"
#include "Utils.h"
#include "SystemInfo.h"
#include "Socket.h"
#include "ICriticalSectionFactory.h"
#include "Todo.h"

#include <vector>

#pragma comment(lib, "ws2_32.lib")

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Symbol did not appear in the constructor initialiser list 
//lint -esym(1928, CThread)
//lint -esym(1928, CUsesWinsock)  
//lint -esym(1928, Node)  
//lint -esym(1928, UserData)
//lint -esym(1928, ServerData)
//
// Symbol's default constructor implicitly called
//lint -esym(1926, CSocketServer::m_listManipulationSection)
//lint -esym(1926, CSocketServer::m_shutdownEvent)
//lint -esym(1926, CSocketServer::m_acceptConnectionsEvent)
//lint -esym(1926, CSocketServer::m_activeList)
//lint -esym(1926, CSocketServer::m_freeList)
//lint -esym(1926, Socket::m_crit)
//lint -esym(1926, IOPool::m_workerThreads)
//
// Member not defined
//lint -esym(1526, CSocketServer::CSocketServer)
//lint -esym(1526, CSocketServer::operator=)
//lint -esym(1526, Socket::Socket)
//lint -esym(1526, Socket::operator=)
//lint -esym(1526, WorkerThread::WorkerThread)
//lint -esym(1526, WorkerThread::operator=)
//lint -esym(1526, IOPool::IOPool)
//lint -esym(1526, IOPool::operator=)
//lint -esym(1526, IOPoolHolder::IOPoolHolder)
//lint -esym(1526, IOPoolHolder::operator=)
//
//lint -esym(534, InterlockedIncrement)   ignoring return value
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using std::vector;

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// Static helper methods
///////////////////////////////////////////////////////////////////////////////

static size_t CalculateNumberOfThreads(
   size_t numThreads);

///////////////////////////////////////////////////////////////////////////////
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

CSocketServer::CSocketServer(
   const ICriticalSectionFactory &lockFactory,
   IOPool &pool,
   unsigned long addressToListenOn,
   unsigned short portToListenOn,
   size_t maxFreeSockets,
   size_t maxFreeBuffers,
   size_t bufferSize,
   size_t numberOfUserDataSlots /* = 0 */,
   bool useSequenceNumbers /* = true */,
   bool postZeroByteReads /* = false */)
   :  CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
      m_pool(pool),
      m_listeningSocket(INVALID_SOCKET),
      m_address(addressToListenOn),
      m_port(portToListenOn),
      m_maxFreeSockets(maxFreeSockets),
      m_numberOfUserDataSlots(numberOfUserDataSlots),
      m_useSequenceNumbers(useSequenceNumbers),
      m_postZeroByteReads(postZeroByteReads),
      m_lockFactory(lockFactory),
      m_listManipulationSection(lockFactory.GetCriticalSection(this))
{
}

CSocketServer::CSocketServer(
   const ICriticalSectionFactory &lockFactory,
   unsigned long addressToListenOn,
   unsigned short portToListenOn,
   size_t maxFreeSockets,
   size_t maxFreeBuffers,
   size_t bufferSize,
   size_t numberOfUserDataSlots /* = 0 */,
   size_t numThreads /* = 0 */, 
   bool useSequenceNumbers /* = true */,
   bool postZeroByteReads /* = false */)
   :  CIOBuffer::Allocator(bufferSize, maxFreeBuffers),
      m_pool(numThreads),
      m_listeningSocket(INVALID_SOCKET),
      m_address(addressToListenOn),
      m_port(portToListenOn),
      m_maxFreeSockets(maxFreeSockets),
      m_numberOfUserDataSlots(numberOfUserDataSlots),
      m_useSequenceNumbers(useSequenceNumbers),
      m_postZeroByteReads(postZeroByteReads),
      m_lockFactory(lockFactory),
      m_listManipulationSection(lockFactory.GetCriticalSection(this))
{
}

CSocketServer::~CSocketServer()
{
}

void CSocketServer::ReleaseSockets()
{
   CCriticalSection::Owner lock(m_listManipulationSection);

   while (m_activeList.Head())
   {
      ReleaseSocket(m_activeList.Head());
   }

   while (m_freeList.Head())
   {
      DestroySocket(m_freeList.PopNode());
   }

   if (m_freeList.Count() + m_freeList.Count() != 0)
   {
      //lint -e{1933} call to unqualified virtual function
      OnError(_T("CSocketServer::ReleaseSockets() - Leaked sockets"));
   }
}

void CSocketServer::StartAcceptingConnections()
{
   if (m_listeningSocket == INVALID_SOCKET)
   {
      //lint -e{1933} call to unqualified virtual function
      OnStartAcceptingConnections();

      //lint -e{1933} call to unqualified virtual function
      m_listeningSocket = CreateListeningSocket(m_address, m_port);
   
      m_acceptConnectionsEvent.Set();
   }
}

void CSocketServer::StopAcceptingConnections()
{
   if (m_listeningSocket != INVALID_SOCKET)
   {
      m_acceptConnectionsEvent.Reset();

      if (0 != ::closesocket(m_listeningSocket))
      {
         //lint -e{1933} call to unqualified virtual function
         OnError(_T("CSocketServer::StopAcceptingConnections() - closesocket - ") + GetLastErrorMessage(::WSAGetLastError()));
      }

      m_listeningSocket = INVALID_SOCKET;

      //lint -e{1933} call to unqualified virtual function
      OnStopAcceptingConnections();
   }
}

void CSocketServer::InitiateShutdown()
{
   // signal that the dispatch thread should shut down all worker threads and then exit

   StopAcceptingConnections();

   {
      CCriticalSection::Owner lock(m_listManipulationSection);

      Socket *pSocket = m_activeList.Head();

      while (pSocket)
      {
         Socket *pNext = SocketList::Next(pSocket);

         pSocket->AbortiveClose();
   
         pSocket = pNext;
      }
   }

   m_shutdownEvent.Set();

   //lint -e{1933} call to unqualified virtual function
   OnShutdownInitiated();
}

void CSocketServer::WaitForShutdownToComplete()
{
   // if we havent already started a shut down, do so...

   InitiateShutdown();

   Wait();

   ReleaseSockets();
   Flush();
}

SOCKET CSocketServer::CreateListeningSocket(
   unsigned long address,
   unsigned short port)
{
   SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 

   if (s == INVALID_SOCKET)
   {
      throw CWin32Exception(_T("CSocket::CreateListeningSocket()"), ::WSAGetLastError());
   }

   CSocket listeningSocket(s);

   CSocket::InternetAddress localAddress(address, port);

   listeningSocket.Bind(localAddress);

   listeningSocket.Listen(5);

   return listeningSocket.Detatch();
}

SOCKET CSocketServer::CreateOutboundSocket(
   unsigned long address,
   unsigned short port)
{
   SOCKET s = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED); 

   if (s == INVALID_SOCKET)
   {
      throw CWin32Exception(_T("CSocket::CreateOutboundSocket()"), ::WSAGetLastError());
   }

   CSocket outboundSocket(s);

   CSocket::InternetAddress localAddress(address, port);

   outboundSocket.Bind(localAddress);

   return outboundSocket.Detatch();
}

int CSocketServer::Run()
{
   try
   {
      HANDLE handlesToWaitFor[2];

      handlesToWaitFor[0] = m_shutdownEvent.GetEvent();
      handlesToWaitFor[1] = m_acceptConnectionsEvent.GetEvent();

      while (!m_shutdownEvent.Wait(0))
      {
         DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

         if (waitResult == WAIT_OBJECT_0)
         {
            // Time to shutdown
            break;
         }
         else if (waitResult == WAIT_OBJECT_0 + 1)
         {
            // accept connections

            while (!m_shutdownEvent.Wait(0) && m_acceptConnectionsEvent.Wait(0))
            {
               CIOBuffer *pAddress = Allocate();

               int addressSize = (int)pAddress->GetSize();

               //lint -e{826} suspicious pointer conversion
               SOCKET acceptedSocket = ::WSAAccept(
                  m_listeningSocket, 
                  reinterpret_cast<sockaddr*>(const_cast<BYTE*>(pAddress->GetBuffer())), 
                  &addressSize, 
                  0, 
                  0);

               pAddress->Use(addressSize);

               if (acceptedSocket != INVALID_SOCKET)
               {
                  Socket *pSocket = AllocateSocket(acceptedSocket);
               
                  //lint -e{1933} call to unqualified virtual function
                  OnConnectionEstablished(pSocket, pAddress);

                  pSocket->Release();
               }
               else if (m_acceptConnectionsEvent.Wait(0))
               {
                  //lint -e{1933} call to unqualified virtual function
                  OnError(_T("CSocketServer::Run() - WSAAccept:") + GetLastErrorMessage(::WSAGetLastError()));
               }

               pAddress->Release();
            }
         }
         else
         {
            //lint -e{1933} call to unqualified virtual function
            OnError(_T("CSocketServer::Run() - WaitForMultipleObjects: ") + GetLastErrorMessage(::GetLastError()));
         }
      }
   }
   catch(const CException &e)
   {
      //lint -e{1933} call to unqualified virtual function
      OnError(_T("CSocketServer::Run() - Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
   }
   catch(...)
   {
      //lint -e{1933} call to unqualified virtual function
      OnError(_T("CSocketServer::Run() - Unexpected exception"));
   }

   //lint -e{1933} call to unqualified virtual function
   OnShutdownComplete();

   return 0;
}

CSocketServer::Socket *CSocketServer::Connect(
   const sockaddr_in &address)
{
   CSocket soc(CreateOutboundSocket(m_address, 0));

   soc.Connect(address); 

   Socket *pSocket = AllocateSocket(soc.Detatch());

   return pSocket;
}

CSocketServer::Socket *CSocketServer::AllocateSocket(
   SOCKET theSocket)
{
   CCriticalSection::Owner lock(m_listManipulationSection);

   Socket *pSocket = 0;

   if (!m_freeList.Empty())
   {
      pSocket = m_freeList.PopNode();

      pSocket->Attach(theSocket);
   }
   else
   {
      pSocket = new Socket(*this, theSocket, m_useSequenceNumbers, m_numberOfUserDataSlots, m_lockFactory);

      //lint -e{1933} call to unqualified virtual function
      OnConnectionCreated();
   }

   m_activeList.PushNode(pSocket);

   //lint -e{611} suspicious cast
   m_pool->AssociateDevice(reinterpret_cast<HANDLE>(theSocket), (ULONG_PTR)pSocket);

   return pSocket;
}

void CSocketServer::ReleaseSocket(Socket *pSocket)
{
   if (!pSocket)
   {
      throw CException(_T("CSocketServer::ReleaseSocket()"), _T("pSocket is null"));
   }

   //lint -e{1933} call to unqualified virtual function
   OnSocketReleased(pSocket);

   CCriticalSection::Owner lock(m_listManipulationSection);

   pSocket->RemoveFromList();

   if (m_maxFreeSockets == 0 || 
       m_freeList.Count() < m_maxFreeSockets)
   {
      m_freeList.PushNode(pSocket);
   }
   else
   {
      DestroySocket(pSocket);
   }
}

void CSocketServer::DestroySocket(
   Socket *pSocket)
{
   delete pSocket;

   //lint -e{1933} call to unqualified virtual function
   OnConnectionDestroyed();
}

void CSocketServer::OnError(
   const _tstring &message)
{
   DEBUG_ONLY(Output(message));
}
  
void CSocketServer::WriteCompleted(
   Socket * /*pSocket*/,
   CIOBuffer *pBuffer)
{
   if (pBuffer->GetUsed() != pBuffer->GetWSABUF()->len)
   {
      //lint -e{1933} call to unqualified virtual function
      OnError(_T("CSocketServer::WriteCompleted - Socket write where not all data was written"));
   }

   //lint -e{818} pointer pBuffer could be declared const (but not in derived classes...)
}

void CSocketServer::WriteCompleted(
   Socket *pSocket)
{
   pSocket->WriteCompleted();
}

bool CSocketServer::FilterReadRequest(
   Socket * /*pSocket*/,
   CIOBuffer ** /*ppBuffer*/)
{
   // Normal processing here is to return a filtered buffer if we can filter in place or false if 
   // the filtered data will be returned via a call to PostFilteredReadRequest

   return true;
}

bool CSocketServer::FilterReadCompleted(
   Socket * /*pSocket*/,
   CIOBuffer ** /*ppBuffer*/)
{
   // Normal processing here is to return a filtered buffer if we can filter in place or false if 
   // the filtered data will be returned via a call to PostFilteredReadCompleted

   return true;
}

bool CSocketServer::FilterWriteRequest(
   Socket * /*pSocket*/,
   CIOBuffer ** /*ppBuffer*/)
{
   // Normal processing here is to return a filtered buffer if we can filter in place or false if 
   // the filtered data will be returned via a call to PostFilteredWriteRequest

   return true;
}

bool CSocketServer::FilterWriteCompleted(
   Socket * /*pSocket*/,
   CIOBuffer ** /*ppBuffer*/)
{
   // Normal processing here is to return a filtered buffer if we can filter in place or false if 
   // the filtered data will be returned via a call to PostFilteredWriteCompleted

   return true;
}

bool CSocketServer::FilterSocketShutdown(
   Socket * /*pSocket*/,
   int /*how*/)
{
   return true;
}

bool CSocketServer::FilterSocketClose(
   Socket * /*pSocket*/)
{
   return true;
}

void CSocketServer::PostFilteredReadRequest(
   Socket *pSocket)
{
   CIOBuffer *pBuffer = Allocate();

   PostIoOperation(pSocket, pBuffer, IO_Filtered_Read_Request);

   pBuffer->Release();
}

void CSocketServer::PostFilteredReadRequest(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   PostIoOperation(pSocket, pBuffer, IO_Filtered_Read_Request);
}

void CSocketServer::PostFilteredReadCompleted(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   pBuffer->SetSequenceNumber(pSocket->GetSequenceNumber(Socket::FilteredReadSequenceNo));
   
   PostIoOperation(pSocket, pBuffer, IO_Filtered_Read_Completed);
}

void CSocketServer::PostFilteredWriteRequest(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   pSocket->FilteredWritePending();

   pBuffer->SetSequenceNumber(pSocket->GetSequenceNumber(Socket::FilteredWriteSequenceNo));

   PostIoOperation(pSocket, pBuffer, IO_Filtered_Write_Request);
}

void CSocketServer::PostFilteredWriteCompleted(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   PostIoOperation(pSocket, pBuffer, IO_Filtered_Write_Completed);
}

void CSocketServer::PostIoOperation(
   Socket *pSocket,
   CIOBuffer *pBuffer,
   IO_Operation operation) const
{
   pBuffer->SetOperation(operation);
   pBuffer->AddRef();

   pSocket->AddRef();

   m_pool->Dispatch((ULONG_PTR)pSocket, 0, pBuffer);
}

CSocketServer &CSocketServer::GetServer(
   Socket *pSocket)
{
   return pSocket->m_server;
}

void CSocketServer::OnConnectionError(
   ConnectionErrorSource /*source*/,
   Socket * /*pSocket*/,
   CIOBuffer * /*pBuffer*/,
   DWORD lastError)
{
   DEBUG_ONLY(Output(_T("CSocketServer::OnConnectionError() - : ") + GetLastErrorMessage(lastError)));
}

void CSocketServer::HandleOperation(
   Socket *pSocket,
   CIOBuffer *pBuffer,
   DWORD dwIoSize,
   DWORD lastError) 
{
   if (pBuffer)
   {
      const IO_Operation operation = static_cast<IO_Operation>(pBuffer->GetOperation());

      switch (operation)
      {
         case IO_Zero_Byte_Read_Request :

//            DEBUG_ONLY(Output(_T("ZeroByteRead:") + ToString(pBuffer)));

            ZeroByteRead(pSocket, pBuffer);

            pSocket->Release();
            pBuffer->Release();

         break ;

         case IO_Zero_Byte_Read_Completed :
         case IO_Read_Request :

//            DEBUG_ONLY(Output(_T("IO_Zero_Byte_Read_Completed|IO_Read_Request:") + ToString(pBuffer)));

            //lint -e{1933} call to unqualified virtual function
            if (FilterReadRequest(pSocket, &pBuffer))
            {
               Read(pSocket, pBuffer);
            }
            
            pSocket->Release();
            pBuffer->Release();

         break;
   
         case IO_Filtered_Read_Request :

//            DEBUG_ONLY(Output(_T("IO_Filtered_Read_Request:") + ToString(pBuffer)));

            Read(pSocket, pBuffer);

            pSocket->Release();
            pBuffer->Release();

         break;

         case IO_Read_Completed :

//            DEBUG_ONLY(Output(_T("IO_Read_Completed:") + ToString(pBuffer)));

            pBuffer->Use(dwIoSize);
            
//            DEBUG_ONLY(Output(_T("  RX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n")));
            //DEBUG_ONLY(Output(_T("  RX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetWSABUF()->buf), dwIoSize, 40)));
            
            //lint -e{1933} call to unqualified virtual function
            if (FilterReadCompleted(pSocket, &pBuffer))
            {
               if (0 != dwIoSize)
               {
                  //lint -e{1933} call to unqualified virtual function
                  ReadCompleted(pSocket, pBuffer);
               }
            }

            if (0 == dwIoSize && 0 == lastError)
            {
               // client connection dropped?

//               DEBUG_ONLY(Output(_T("ReadCompleted - 0 bytes - client connection dropped")));

               pSocket->OnClientClose();
            }

            pSocket->Release();
            pBuffer->Release();

         break;

         case IO_Filtered_Read_Completed :
            
//            DEBUG_ONLY(Output(_T("IO_Filtered_Read_Completed:") + ToString(pBuffer)));

            //DEBUG_ONLY(Output(_T("F RX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

            #pragma TODO("post the correct io size")

            if (0 != pBuffer->GetUsed())   
            {
               //lint -e{1933} call to unqualified virtual function
               ReadCompleted(pSocket, pBuffer);
            }
            
            pSocket->Release();
            pBuffer->Release();

         break;

         case IO_Write_Request :

//            DEBUG_ONLY(Output(_T("IO_Write_Request:") + ToString(pBuffer)));

//            DEBUG_ONLY(Output(_T("  TX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n")));
            //DEBUG_ONLY(Output(_T("  TX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

            //lint -e{1933} call to unqualified virtual function
            if (FilterWriteRequest(pSocket, &pBuffer))
            {
               Write(pSocket, pBuffer);
            }

            pSocket->Release();
            pBuffer->Release();

         break;

         case IO_Filtered_Write_Request :

//            DEBUG_ONLY(Output(_T("IO_Filtered_Write_Request:") + ToString(pBuffer)));

            //DEBUG_ONLY(Output(_T("F TX: ") + ToString(pBuffer->GetUsed()) + _T(" bytes") + _T(" Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" : ") + ToString(pBuffer) + _T("\n") + DumpData(reinterpret_cast<const BYTE*>(pBuffer->GetBuffer()), pBuffer->GetUsed(), 40)));

            Write(pSocket, pBuffer);

            pSocket->Release();
            pBuffer->Release();

         break;

         case IO_Write_Completed :

//            DEBUG_ONLY(Output(_T("IO_Write_Completed:") + ToString(pBuffer)));

            pBuffer->Use(dwIoSize);

            //lint -e{1933} call to unqualified virtual function
            if (FilterWriteCompleted(pSocket, &pBuffer))
            {
               //lint -e{1933} call to unqualified virtual function
               WriteCompleted(pSocket, pBuffer);

               pSocket->WriteCompleted();
            }

            pSocket->Release();
            pBuffer->Release();

         break;

         case IO_Filtered_Write_Completed :

            //lint -e{1933} call to unqualified virtual function
            WriteCompleted(pSocket, pBuffer);

            pSocket->WriteCompleted();

            pSocket->Release();
            pBuffer->Release();

         break;

         default :
            //lint -e{1933} call to unqualified virtual function
            OnError(_T("CSocketServer::HandleOperation() - Unexpected operation"));
         break;
      } 
   }
   else
   {
      //lint -e{1933} call to unqualified virtual function
      OnError(_T("CSocketServer::HandleOperation() - Unexpected - pBuffer is 0"));
   }
}

void CSocketServer::ZeroByteRead(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   pSocket->AddRef();

   pBuffer->SetOperation(IO_Zero_Byte_Read_Completed);
   pBuffer->SetupZeroByteRead();
   pBuffer->AddRef();

   DWORD dwNumBytes = 0;
   DWORD dwFlags = 0;

   if (SOCKET_ERROR == ::WSARecv(
      pSocket->m_socket, 
      pBuffer->GetWSABUF(), 
      1, 
      &dwNumBytes,
      &dwFlags,
      pBuffer, 
      NULL))
   {
      DWORD lastError = ::WSAGetLastError();

      if (ERROR_IO_PENDING != lastError)
      {
         pSocket->OnConnectionError(ZeroByteReadError, pBuffer, lastError);
         
         pSocket->Release();
         pBuffer->Release();
      }
   }
   // Note: even if WSARecv returns SUCCESS an IO Completion Packet is 
   // queued to the IOCP the same as if ERROR_IO_PENDING was returned.
   // Thus we need no special handling for the non error return case.
   // See http://support.microsoft.com/default.aspx?scid=kb;en-us;Q192800
   // for details.
}

void CSocketServer::Read(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   pSocket->AddRef();

   pBuffer->SetOperation(IO_Read_Completed);
   pBuffer->SetupRead();
   pBuffer->AddRef();

   CCriticalSection::Owner lock(pSocket->m_crit); 

   pBuffer->SetSequenceNumber(pSocket->GetSequenceNumber(Socket::ReadSequenceNo));

   DWORD dwNumBytes = 0;
   DWORD dwFlags = 0;

   if (SOCKET_ERROR == ::WSARecv(
      pSocket->m_socket, 
      pBuffer->GetWSABUF(), 
      1, 
      &dwNumBytes,
      &dwFlags,
      pBuffer, 
      NULL))
   {
      DWORD lastError = ::WSAGetLastError();

      if (ERROR_IO_PENDING != lastError)
      {
         pSocket->OnConnectionError(ReadError, pBuffer, lastError);
                  
         pSocket->Release();
         pBuffer->Release();
      }
   }
   // Note: even if WSARecv returns SUCCESS an IO Completion Packet is 
   // queued to the IOCP the same as if ERROR_IO_PENDING was returned.
   // Thus we need no special handling for the non error return case.
   // See http://support.microsoft.com/default.aspx?scid=kb;en-us;Q192800
   // for details.
}

void CSocketServer::Write(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   pSocket->AddRef();

   pBuffer->SetOperation(IO_Write_Completed);
   pBuffer->SetupWrite();
   pBuffer->AddRef();

   pBuffer = pSocket->GetNextBuffer(pBuffer);

   while(pBuffer)
   {
//      DEBUG_ONLY(Output(ToString(pSocket) + _T(" X TX: Seq: ") + ToString(pBuffer->GetSequenceNumber()) + _T(" bytes = ") + ToString(pBuffer->GetWSABUF()->len)));

      DWORD dwFlags = 0;
      DWORD dwSendNumBytes = 0;

      if (SOCKET_ERROR == ::WSASend(
         pSocket->m_socket,
         pBuffer->GetWSABUF(), 
         1, 
         &dwSendNumBytes,
         dwFlags,
         pBuffer, 
         NULL))
      {
         DWORD lastError = ::WSAGetLastError();

         if (ERROR_IO_PENDING != lastError)
         {
            pSocket->OnConnectionError(WriteError, pBuffer, lastError);
            
            pSocket->WriteCompleted();  // this pending write will never complete...

            pSocket->Release();
            pBuffer->Release();
         }
      }
      // Note: even if WSASend returns SUCCESS an IO Completion Packet is 
      // queued to the IOCP the same as if ERROR_IO_PENDING was returned.
      // Thus we need no special handling for the non error return case.
      // See http://support.microsoft.com/default.aspx?scid=kb;en-us;Q192800
      // for details.

      pBuffer = pSocket->GetNextBuffer();
   }
}

//////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket
///////////////////////////////////////////////////////////////////////////////

CSocketServer::Socket::Socket(
   CSocketServer &server,                                 
   SOCKET theSocket,
   bool useSequenceNumbers,
   size_t numberOfUserDataSlots,
   const ICriticalSectionFactory &lockFactory)
   //lint -e{514} Unusual use of a Boolean
   :  CIndexedOpaqueUserData(numberOfUserDataSlots + useSequenceNumbers),
      m_server(server),
      m_socket(theSocket),
      m_ref(1),
      m_outstandingWrites(0),
      m_readShutdown(false),
      m_writeShutdown(false),
      m_closing(false),
      m_clientClosed(false),
      m_useSequenceNumbers(useSequenceNumbers),
      m_crit(lockFactory.GetCriticalSection(this))
{
   if (!IsValid())
   {
      throw CException(_T("CSocketServer::Socket::Socket()"), _T("Invalid socket"));
   }

   if (m_useSequenceNumbers)
   {
      CIndexedOpaqueUserData::SetUserPtr(0, new SequenceData(m_crit));
   }
}

CSocketServer::Socket::~Socket()
{
   delete GetSequenceData();
}

CSocketServer::Socket::SequenceData *CSocketServer::Socket::GetSequenceData() const
{
   if (m_useSequenceNumbers)
   {
      return static_cast<SequenceData*>(CIndexedOpaqueUserData::GetUserPtr(0));
   }

   return 0;
}


void CSocketServer::Socket::Attach(
   SOCKET theSocket)
{
   if (IsValid())
   {
      throw CException(_T("CSocketServer::Socket::Attach()"), _T("Socket already attached"));
   }

   m_socket = theSocket;

   SequenceData *pSequenceData = GetSequenceData();

   ClearUserData();

   m_readShutdown = false;
   m_writeShutdown = false;
   m_outstandingWrites = 0;
   m_closing = false;
   m_clientClosed = false;

   if (pSequenceData)
   {
      pSequenceData->Reset();

      CIndexedOpaqueUserData::SetUserPtr(0, pSequenceData);
   }

   ::InterlockedIncrement(&m_ref);
}

void CSocketServer::Socket::AddRef()
{
   if (1 == ::InterlockedIncrement(&m_ref))
   {
      throw CException(
         _T("CSocketServer::Socket::AddRef()"), 
         _T("Reference count incremented from 0 - usually because you're calling AddRef() on the socket passed to you in OnSocketReleased() and that's not allowed."));
   }
}

void CSocketServer::Socket::Release()
{
   if (0 == ::InterlockedDecrement(&m_ref))
   {
      if (IsValid())
      {
         ::InterlockedIncrement(&m_ref);
      
         if (!m_closing)
         {
            m_closing = true;

            if (!m_server.OnConnectionClosing(this))
            {
               AbortiveClose();
            }
         }
         else
         {
            AbortiveClose();
         }

         Release();
         return;
      }

      m_server.ReleaseSocket(this);
   }
}

void *CSocketServer::Socket::GetUserPtr(
   const size_t index) const
{
   return CIndexedOpaqueUserData::GetUserPtr(index + m_useSequenceNumbers);
}

void CSocketServer::Socket::SetUserPtr(
   const size_t index,
   void *pData)
{
   CIndexedOpaqueUserData::SetUserPtr(index + m_useSequenceNumbers, pData);
}

unsigned long CSocketServer::Socket::GetUserData(
   const size_t index) const
{
   return CIndexedOpaqueUserData::GetUserData(index + m_useSequenceNumbers);
}

void CSocketServer::Socket::SetUserData(
   const size_t index,
   unsigned long data)
{
   CIndexedOpaqueUserData::SetUserData(index + m_useSequenceNumbers, data);
}

void CSocketServer::Socket::Shutdown(
   int how /* = SD_BOTH */)
{
//   DEBUG_ONLY(Output(_T("CSocketServer::Socket::Shutdown() ") + ToString(how)));

   if (m_server.FilterSocketShutdown(this, how))
   {
      if (how == SD_RECEIVE || how == SD_BOTH)
      {
         m_readShutdown = true;
      }

      if (how == SD_SEND || how == SD_BOTH)
      {
         m_writeShutdown = true;
      }

      if (::InterlockedExchangeAdd(&m_outstandingWrites, 0) > 0)
      {
         // Send side will be shut down when last pending write completes...

         if (how == SD_BOTH)
         {
            how = SD_RECEIVE;      
         }
         else if (how == SD_SEND)
         {
            return;
         }
      }

      if (IsValid())
      {
         if (0 != ::shutdown(m_socket, how))
         {
            m_server.OnError(_T("CSocketServer::Server::Shutdown() - ") + GetLastErrorMessage(::WSAGetLastError()));
         }

//         DEBUG_ONLY(Output(_T("shutdown initiated")));
      }
   }
}

bool CSocketServer::Socket::IsConnected(
   int how /*= SD_BOTH*/) const
{
   if (how == SD_RECEIVE)
   {
      return !m_readShutdown;
   }

   if (how == SD_SEND)
   {
      return !m_writeShutdown;
   }

   if (how == SD_BOTH)
   {
      return (!m_writeShutdown && !m_readShutdown);
   }

   return false;
}

void CSocketServer::Socket::Close()
{
   CCriticalSection::Owner lock(m_crit);

   if (IsValid())
   {
      if (m_server.FilterSocketClose(this))
      {
         InternalClose();
      }
   }
}

bool CSocketServer::Socket::WritePending()
{
   if (m_writeShutdown)
   {
//      DEBUG_ONLY(Output(_T("CSocketServer::Socket::WritePending() - Attempt to write after write shutdown")));
      
      return false;
   }
   
   FilteredWritePending();

   return true;
}

void CSocketServer::Socket::FilteredWritePending()
{
   ::InterlockedIncrement(&m_outstandingWrites);
}

void CSocketServer::Socket::WriteCompleted()
{
   if (::InterlockedDecrement(&m_outstandingWrites) == 0)
   {
      if (m_writeShutdown)
      {
         // The final pending write has been completed so we can now shutdown the send side of the
         // connection.

         Shutdown(SD_SEND);
      }
   }
}

void CSocketServer::Socket::AbortiveClose()
{
   // Force an abortive close.

   LINGER lingerStruct;

   lingerStruct.l_onoff = 1;
   lingerStruct.l_linger = 0;

   if (SOCKET_ERROR == ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct)))
   {
      m_server.OnError(_T("CSocketServer::Socket::AbortiveClose() - setsockopt(SO_LINGER) - ")  + GetLastErrorMessage(::WSAGetLastError()));
   }

   InternalClose();
}

void CSocketServer::Socket::OnClientClose()
{
   if (0 == ::InterlockedExchange(&m_clientClosed, 1))
   {
      Shutdown(SD_RECEIVE);

      m_server.OnConnectionClientClose(this);
   }
}

void CSocketServer::Socket::OnConnectionReset(
   DWORD lastError)
{
   CCriticalSection::Owner lock(m_crit);

   if (IsValid())
   {
      m_server.OnConnectionReset(this, lastError);

      InternalClose();
   }
}

void CSocketServer::Socket::InternalClose()
{
   CCriticalSection::Owner lock(m_crit);

   if (IsValid())
   {
#pragma TODO("Look at using lingering close even though async?")
      if (0 != ::closesocket(m_socket))
      {
         m_server.OnError(_T("CSocketServer::Socket::InternalClose() - closesocket - ") + GetLastErrorMessage(::WSAGetLastError()));
      }

      m_socket = INVALID_SOCKET;

      m_readShutdown = true;
      m_writeShutdown = true;

      m_server.OnConnectionClosed(this);
   }
}

bool CSocketServer::Socket::Read(
   CIOBuffer *pBuffer /* = 0 */,
   bool throwOnFailure /* = false*/)
{
   if (!IsValid())
   {
      if (throwOnFailure)
      {
         #pragma TODO("throw SocketClosedException()")
         
         throw CException(_T("CSocketServer::Socket::Read()"), _T("Socket is closed"));
      }
      else
      {
         return false;
      }
   }
   // Post a read request to the iocp so that the actual socket read gets performed by
   // one of the server's IO threads...

   if (!pBuffer)
   {
      pBuffer = m_server.Allocate();
   }
   else
   {
      pBuffer->AddRef();
   }

   m_server.PostIoOperation(this, pBuffer, m_server.m_postZeroByteReads ? IO_Zero_Byte_Read_Request : IO_Read_Request);

   pBuffer->Release();

   return true;
}

bool CSocketServer::Socket::Write(
   const char *pData, 
   size_t dataLength,
   bool throwOnFailure /* = false*/)
{
   return Write(reinterpret_cast<const BYTE*>(pData), dataLength, throwOnFailure);
}

bool CSocketServer::Socket::Write(
   const BYTE *pData, 
   size_t dataLength,
   bool throwOnFailure /* = false*/)
{
   if (!IsValid())
   {
      if (throwOnFailure)
      {
         #pragma TODO("throw SocketClosedException()")
         
         throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is closed"));
      }
      else
      {
         return false;
      }
   }

   if (!WritePending())
   {
      if (throwOnFailure)
      {
         #pragma TODO("throw SocketClosedException()")

         throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is shutdown"));
      }
      else
      {
         return false;
      }
   }

   CIOBuffer *pBuffer = m_server.Allocate();

   pBuffer->AddData(pData, dataLength);

   pBuffer->SetSequenceNumber(GetSequenceNumber(WriteSequenceNo));

   m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

   pBuffer->Release();

   return true;
}

bool CSocketServer::Socket::Write(
   CIOBuffer *pBuffer,
   bool throwOnFailure /* = false*/)
{
   if (!IsValid())
   {
      if (throwOnFailure)
      {
         #pragma TODO("throw SocketClosedException()")

         throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is closed"));
      }
      else
      {
         return false;
      }
   }

   if (!WritePending())
   {
      if (throwOnFailure)
      {
         #pragma TODO("throw SocketClosedException()")

         throw CException(_T("CSocketServer::Socket::Write()"), _T("Socket is shutdown"));
      }
      else
      {
         return false;
      }
   }

   pBuffer->SetSequenceNumber(GetSequenceNumber(WriteSequenceNo));

   m_server.PostIoOperation(this, pBuffer, IO_Write_Request);

   return true;
}

long CSocketServer::Socket::GetSequenceNumber(
   SequenceType type) const 
{
   SequenceData *pSequenceData = GetSequenceData();

   if (pSequenceData)
   {
		return ::InterlockedIncrement(&pSequenceData->m_numbers[type]);
   }

   return 0;
}

CIOBuffer *CSocketServer::Socket::GetNextBuffer(
   CIOBuffer *pBuffer /* = 0 */) const
{
   SequenceData *pSequenceData = GetSequenceData();
   
   if (pSequenceData)
   {
      if (pBuffer)
      {
         return pSequenceData->m_outOfSequenceWrites.GetNext(pBuffer);
      }
      else
      {
         return pSequenceData->m_outOfSequenceWrites.ProcessAndGetNext();
      }
   }

   return pBuffer;
}

bool CSocketServer::Socket::IsValid()
{
   return (INVALID_SOCKET != m_socket);
}

void CSocketServer::Socket::OnConnectionError(
   CSocketServer::ConnectionErrorSource source,
   CIOBuffer *pBuffer,
   DWORD lastError)
{
   if (WSAESHUTDOWN == lastError)
   {
      OnClientClose();
   }
   else if (WSAECONNRESET == lastError || WSAECONNABORTED == lastError)
   {
      OnConnectionReset(lastError);
   }
   else if (!IsValid() && WSAENOTSOCK == lastError)
   {
      // Swallow this error as we expect it...
   }
   else
   {
      m_server.OnConnectionError(source, this, pBuffer, lastError);
   }
}

void CSocketServer::Socket::HandleOperation(
   CIOBuffer *pBuffer,
   DWORD dwIoSize,
   DWORD lastError) 
{
   m_server.HandleOperation(this, pBuffer, dwIoSize, lastError);
}

CSocketServer::Socket *CSocketServer::Socket::Connect(
   const sockaddr_in &address)
{
   return m_server.Connect(address);
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::Socket::SequenceData
///////////////////////////////////////////////////////////////////////////////

CSocketServer::Socket::SequenceData::SequenceData(
   CCriticalSection &section)
   :  m_outOfSequenceWrites(section)
{
   memset(m_numbers, 0, sizeof(m_numbers));
}

void CSocketServer::Socket::SequenceData::Reset()
{
   memset(m_numbers, 0, sizeof(m_numbers));

   m_outOfSequenceWrites.Reset();
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::IOPool
///////////////////////////////////////////////////////////////////////////////

CSocketServer::IOPool::IOPool(
   size_t numThreads /* = 0 */)
   :  m_numThreads(CalculateNumberOfThreads(numThreads)),
      m_iocp(0)
{
   m_workerThreads.resize(m_numThreads);

   for (size_t i = 0; i < m_numThreads; ++i)
   {
      m_workerThreads[i] = 0;      
   }
}

void CSocketServer::IOPool::Start()
{
   if (m_workerThreads[0])
   {
      throw CException(_T(""), _T("Already started"));
   }

   for (size_t i = 0; i < m_numThreads; ++i)
   {
      WorkerThread *pThread = new WorkerThread(*this); 

      m_workerThreads[i] = pThread;

      pThread->Start();
   }
}

void CSocketServer::IOPool::InitiateShutdown()
{
   for (size_t i = 0; i < m_numThreads; ++i)
   {
      if (m_workerThreads[i])
      {
         m_workerThreads[i]->InitiateShutdown();
      }
   }  
}

void CSocketServer::IOPool::WaitForShutdownToComplete()
{
   InitiateShutdown();

   for (size_t i = 0; i < m_numThreads; ++i)
   {
      if (m_workerThreads[i])
      {
         m_workerThreads[i]->WaitForShutdownToComplete();

         delete m_workerThreads[i];

         m_workerThreads[i] = 0;
      }
   }
}

CSocketServer::IOPool::~IOPool()
{
   WaitForShutdownToComplete();
}

void CSocketServer::IOPool::OnError(
   const _tstring &message)
{
   DEBUG_ONLY(Output(message));
}

void CSocketServer::IOPool::Dispatch(
   ULONG_PTR completionKey, 
   DWORD dwNumBytes, 
   OVERLAPPED *pOverlapped) const
{
   m_iocp.PostStatus(completionKey, dwNumBytes, pOverlapped);
}

void CSocketServer::IOPool::AssociateDevice(
   HANDLE hDevice, 
   ULONG_PTR completionKey) const
{
   m_iocp.AssociateDevice(hDevice, completionKey);
}

bool CSocketServer::IOPool::GetNextWorkItem(            
   Socket *&pSocket,
   DWORD &dwIoSize, 
   CIOBuffer *&pBuffer,
   DWORD &lastError) const
{
   return m_iocp.GetStatus((PDWORD_PTR)&pSocket, &dwIoSize, (OVERLAPPED**)&pBuffer, &lastError);
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::IOPool::WorkerThread
///////////////////////////////////////////////////////////////////////////////

CSocketServer::IOPoolHolder::IOPoolHolder(
   IOPool &pool)
   :  m_ourPool(false),
      m_pPool(&pool)
{
}

CSocketServer::IOPoolHolder::IOPoolHolder(
   size_t numThreads)
   :  m_ourPool(true),
      m_pPool(new IOPool(numThreads))
{
   m_pPool->Start();
}

CSocketServer::IOPoolHolder::~IOPoolHolder()
{
   if (m_ourPool)
   {
      m_pPool->WaitForShutdownToComplete();

      delete m_pPool;
   }

   //lint -e{1740} m_pPool not directly freed of zeroed in dtor
}

const CSocketServer::IOPool * const CSocketServer::IOPoolHolder::operator->() const
{
   return m_pPool;
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::IOPool::WorkerThread
///////////////////////////////////////////////////////////////////////////////

CSocketServer::IOPool::WorkerThread::WorkerThread(
   CSocketServer::IOPool &pool)
   :  m_pool(pool)
{
   m_pool.OnThreadCreated();
}

CSocketServer::IOPool::WorkerThread::~WorkerThread()
{
   m_pool.OnThreadDestroyed();
}

int CSocketServer::IOPool::WorkerThread::Run()
{
   try
   {
      //lint -e{716} while(1)
      while (true)   
      {
         // continually loop to service io completion packets

         DWORD dwIoSize = 0;
         Socket *pSocket = 0;
         CIOBuffer *pBuffer = 0;
         DWORD lastError = 0;
         
         if (!m_pool.GetNextWorkItem(pSocket, dwIoSize, pBuffer, lastError))
         {
            if (pSocket)
            {
               pSocket->OnConnectionReset(lastError);
            }
         }

         if (!pSocket)
         {
            // A completion key of 0 is posted to the iocp to request us to shut down...

            break;
         }

         m_pool.OnThreadBeginProcessing();
   
         //lint -e{1933} call to unqualified virtual function
         pSocket->HandleOperation(pBuffer, dwIoSize, lastError);

         m_pool.OnThreadEndProcessing();
      } 
   }
   catch(const CException &e)
   {
      m_pool.OnError(_T("CSocketServer::IOPool::WorkerThread::Run() - Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
   }
   catch(...)
   {
      m_pool.OnError(_T("CSocketServer::IOPool::WorkerThread::Run() - Unexpected exception"));
   }

   return 0;
}

void CSocketServer::IOPool::WorkerThread::InitiateShutdown() const
{
   m_pool.Dispatch(0,0,0);
}

void CSocketServer::IOPool::WorkerThread::WaitForShutdownToComplete() const
{
   // if we havent already started a shut down, do so...

   InitiateShutdown();

   Wait();
}

///////////////////////////////////////////////////////////////////////////////
// CSocketServer::FilteringHelper
///////////////////////////////////////////////////////////////////////////////

void CSocketServer::FilteringHelper::PostFilteredReadRequest(
   Socket *pSocket,
   CIOBuffer *pBuffer /* = 0 */)
{
   if (pBuffer)
   {
      CSocketServer::GetServer(pSocket).PostFilteredReadRequest(pSocket, pBuffer);
   }
   else
   {
      CSocketServer::GetServer(pSocket).PostFilteredReadRequest(pSocket);
   }
}


void CSocketServer::FilteringHelper::PostFilteredReadCompleted(
   Socket *pSocket,
   CIOBuffer *pBuffer)
{
   CSocketServer::GetServer(pSocket).PostFilteredReadCompleted(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::PostFilteredWriteRequest(
   Socket *pSocket,
   CIOBuffer *pBuffer)
{
   CSocketServer::GetServer(pSocket).PostFilteredWriteRequest(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::PostFilteredWriteCompleted(
   Socket *pSocket,
   CIOBuffer *pBuffer)
{
   CSocketServer::GetServer(pSocket).PostFilteredWriteCompleted(pSocket, pBuffer);
}

void CSocketServer::FilteringHelper::WriteCompleted(
   Socket *pSocket)
{
   CSocketServer::WriteCompleted(pSocket);
}

///////////////////////////////////////////////////////////////////////////////
// Static helper methods
///////////////////////////////////////////////////////////////////////////////

static size_t CalculateNumberOfThreads(size_t numThreads)
{
   if (numThreads == 0)
   {
      CSystemInfo systemInfo;
   
      numThreads = systemInfo.dwNumberOfProcessors * 2;
   }

   return numThreads;
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
// End of file: SocketServer.cpp
///////////////////////////////////////////////////////////////////////////////
