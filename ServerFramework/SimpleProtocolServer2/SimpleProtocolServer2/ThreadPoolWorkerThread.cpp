///////////////////////////////////////////////////////////////////////////////
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
using JetByteTools::Win32::CEvent;
using JetByteTools::Win32::CSocketServer;
using JetByteTools::Win32::CException;
using JetByteTools::Win32::Output;
using JetByteTools::Win32::_tstring;
using JetByteTools::Win32::CSocketAddress;

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

   std::string welcomeMessage("+OK POP3 server ready - ");

   USES_CONVERSION;

   welcomeMessage = welcomeMessage + T2A(const_cast<TCHAR*>(pData->GetConnectionDetails().c_str())) + "\r\n";

   pSocket->Write(welcomeMessage.c_str(), welcomeMessage.length());

   pSocket->Read();

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
   const CIOBuffer *pBuffer) const
{
   std::string command = reinterpret_cast<const char*>(pBuffer->GetBuffer());

   for (size_t i = 0; i < command.length(); ++i)
   {
	   command[i] = (char)toupper(command[i]);
   }

   bool ok = false;

   if (command.length() >= 5)
   {
      // cant be a valid command, unless it's more than 5 chars      
   
      if (command.substr(0, 3) == "TOP")
      {
         ok = true;
      }
      else 
      {
         const std::string cmd = command.substr(0, 4);

         if (cmd == "APOP" ||
             cmd == "DELE" ||
             cmd == "LIST" ||
             cmd == "NOOP" ||
             cmd == "PASS" ||
             cmd == "QUIT" ||
             cmd == "RETR" ||
             cmd == "RSET" ||
             cmd == "STAT" ||
             cmd == "UIDL" ||
             cmd == "USER")
         {
            ok = true;
         }
      }
   }

   CThreadPool::CPerConnectionData *pData = m_pool.GetPerConnectionData(pSocket);

   pData->UseConnection();

   USES_CONVERSION;

   if (ok)
   {
      // We understand, but we aren't really a POP3 server...

      std::string response("-ERR sorry, we understand what you want, but, we're just a fake POP3 server... ");

      response = response + T2A(const_cast<TCHAR*>(pData->GetConnectionDetails().c_str())) + "\r\n";

      // We take a while to reply because we do lots of processing...

      //::Sleep(5000);

      pSocket->Write(response.c_str(), response.length());
   }
   else
   {
      std::string response("-ERR sorry, we dont know what you mean, bye... ");

      response = response + T2A(const_cast<TCHAR*>(pData->GetConnectionDetails().c_str())) + "\r\n";

      pSocket->Write(response.c_str(), response.length());
      pSocket->Shutdown();
   }
}

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -restore
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// End of file: ThreadPoolWorkerThread.cpp
///////////////////////////////////////////////////////////////////////////////

