/////////////////////////////////////////////////////////////////////////////////
// File: ThreadPoolWorkerThread.cpp
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

#include "ThreadPool.h"
#include "ThreadPoolWorkerThread.h"

#include "JetByteTools\Win32Tools\IOBuffer.h"
#include "JetByteTools\Win32Tools\SocketServer.h"
#include "JetByteTools\Win32Tools\Exception.h"
#include "JetByteTools\Win32Tools\Utils.h"
#include "JetByteTools\Win32Tools\tstring.h"
#include "JetByteTools\Win32Tools\SocketAddress.h"
#include "JetByteTools\Win32Tools\CriticalSection.h"

#include <atlbase.h>       // USES_CONVERSION

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Private copy constructor
//lint -esym(1704, CPerConnectionData::CPerConnectionData) 
//
// No default constructor
//lint -esym(1712, CPerConnectionData)
//
// Local structure member not referenced
//lint -esym(754, CPerConnectionData::CPerConnectionData)
//lint -esym(754, CPerConnectionData::operator=)
//
// Member not defined
//lint -esym(1526, CPerConnectionData::CPerConnectionData)
//lint -esym(1526, CPerConnectionData::operator=)
//lint -esym(1526, CThreadPoolWorkerThread::CThreadPoolWorkerThread)
//lint -esym(1526, CThreadPoolWorkerThread::operator=)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using JetByteTools::Win32::CIOBuffer;
using JetByteTools::Win32::CIOCompletionPort;
using JetByteTools::Win32::CCriticalSection;
using JetByteTools::Win32::CEvent;
using JetByteTools::Win32::CSocketServer;
using JetByteTools::Win32::CException;
using JetByteTools::Win32::Output;
using JetByteTools::Win32::_tstring;
using JetByteTools::Win32::CSocketAddress;
using JetByteTools::Win32::ToString;
using JetByteTools::Win32::DumpData;

typedef JetByteTools::Win32::CSocketServer::Socket Socket;

///////////////////////////////////////////////////////////////////////////////
// CThreadPoolWorkerThread
///////////////////////////////////////////////////////////////////////////////

CThreadPoolWorkerThread::CThreadPoolWorkerThread(
   CIOCompletionPort &iocp,
   CEvent &messageReceivedEvent,
   CThreadPool &pool)
   :  JetByteTools::Win32::CThreadPool::WorkerThread(iocp, messageReceivedEvent, pool),
      m_pool(pool)
{
}
      
bool CThreadPoolWorkerThread::Initialise()
{
   Output(_T("CThreadPoolWorkerThread::Initialise"));

   return true;
}

void CThreadPoolWorkerThread::Process(
   ULONG_PTR completionKey,
   DWORD operation,
   OVERLAPPED *pOverlapped)
{
   Socket *pSocket = reinterpret_cast<Socket *>(completionKey);
   CIOBuffer *pBuffer = static_cast<CIOBuffer *>(pOverlapped);
   
   try
   {
      switch(operation)
      {
         case CThreadPool::ConnectionEstablished :    
      
            OnConnectionEstablished(pSocket, pBuffer);

         break;

         case CThreadPool::ReadCompleted :    

            ProcessMessage(pSocket, pBuffer);

         break;

         case CThreadPool::ConnectionClosing :    

            OnConnectionClosing(pSocket);

         break;

         default :

            // do nothing

         break;
      }
   }
   catch(const CException &e)
   {
      Output(_T("Process - Exception - ") + e.GetWhere() + _T(" - ") + e.GetMessage());
      pSocket->Shutdown();
   }
   catch(...)
   {
      Output(_T("Process - Unexpected exception"));
      pSocket->Shutdown();
   }

   pSocket->Release();

   if (pBuffer)
   {
      pBuffer->Release();
   }
}

void CThreadPoolWorkerThread::Shutdown()
{
   Output(_T("CThreadPoolWorkerThread::Shutdown"));
}

void CThreadPoolWorkerThread::OnConnectionEstablished(
   Socket *pSocket,
   CIOBuffer *pAddress)
{
   CThreadPool::CPerConnectionData *pData = m_pool.GetPerConnectionData(pSocket);

   //lint -e{826} Suspicious pointer-to-pointer conversion (area too small)
   CSocketAddress address(reinterpret_cast<const sockaddr*>(pAddress->GetBuffer()));

   pData->SetAddress(address.AsString());

   USES_CONVERSION;

   std::string welcomeMessage("Welcome to large packet echo server ");

   welcomeMessage = welcomeMessage + T2A(const_cast<TCHAR*>(pData->GetConnectionDetails().c_str())) + "\r\n";

   pSocket->Write(welcomeMessage.c_str(), welcomeMessage.length());

   for (size_t i = 0; i < m_pool.m_numReads; ++i)
   {
      pSocket->Read();
   }

   //lint -e{1762} Member function could be made const

   //lint -e{818} Pointer parameter 'pAddress' (line 215) could be declared as pointing to const
}

void CThreadPoolWorkerThread::OnConnectionClosing(
   Socket *pSocket)
{
   // We'll perform a lingering close on this thread

   pSocket->Close();

   //lint -e{1762} Member function could be made const
}

void CThreadPoolWorkerThread::ProcessMessage(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   if (!pSocket->IsConnected(SD_SEND))
   {
      return;
   }

   CThreadPool::CPerConnectionData *pData = m_pool.GetPerConnectionData(pSocket);

   pData->UseConnection();

   pBuffer->AddRef();

   if (m_pool.m_usingSequenceNumbers)
   {
      pBuffer = pData->m_outOfSequenceReads.GetNext(pBuffer);
   }

   while (pSocket->IsConnected(SD_SEND) && pBuffer)
   {
      Output(_T("EchoMessage - ") + ToString(pBuffer->GetSequenceNumber()));

      //DEBUG_ONLY(Output(_T("EchoMessage\n") + DumpData(pBuffer->GetBuffer(), pBuffer->GetUsed(), 40)));
   

      if (m_pool.m_useMultipleWrites)
      {
         // To test write sequencing, we break this packet up and echo it with multiple writes.
         size_t bytesSent = 0;
      
         const BYTE *pMessageData = pBuffer->GetBuffer();
         const size_t messageDataLength = pBuffer->GetUsed();

         // Stop straight away if the client closes the connection on us...

         while (pSocket->IsConnected(SD_SEND) && bytesSent < messageDataLength)
         {
            const size_t responsePacketSize = 666;

            const size_t bytesToSend = min(responsePacketSize, messageDataLength - bytesSent);

            pSocket->Write(pMessageData + bytesSent, bytesToSend);

            bytesSent += bytesToSend;
         }
      }
      else
      {
         // Echo packet with a single write. 
         pSocket->Write(pBuffer);
      }

      pBuffer->Release();

      pBuffer = 0;

      if (m_pool.m_usingSequenceNumbers)
      {
         pBuffer = pData->m_outOfSequenceReads.ProcessAndGetNext();
      }
   }

   if (pBuffer)
   {
      pBuffer->Release();
   }
}

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// End of file: ThreadPoolWorkerThread.h
///////////////////////////////////////////////////////////////////////////////

