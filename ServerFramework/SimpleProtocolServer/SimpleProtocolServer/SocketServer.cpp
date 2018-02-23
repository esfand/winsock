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

#include "JetByteTools\Win32Tools\Utils.h"
#include "JetByteTools\Win32Tools\Exception.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Member not defined
//lint -esym(1526, CSocketServer::CSocketServer)
//lint -esym(1526, CSocketServer::operator=)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using JetByteTools::Win32::CIOCompletionPort;
using JetByteTools::Win32::CIOBuffer;
using JetByteTools::Win32::Output;
using JetByteTools::Win32::ToString;
using JetByteTools::Win32::_tstring;
using JetByteTools::Win32::CException;
using JetByteTools::Win32::DumpData;
using JetByteTools::Win32::GetLastErrorMessage;
using JetByteTools::Win32::ICriticalSectionFactory;

///////////////////////////////////////////////////////////////////////////////
// CSocketServer
///////////////////////////////////////////////////////////////////////////////

CSocketServer::CSocketServer(
   const ICriticalSectionFactory &lockFactory,
   JetByteTools::Win32::CSocketServer::IOPool &pool,
   const std::string &welcomeMessage,
   unsigned long addressToListenOn,
   unsigned short portToListenOn,
   size_t maxFreeSockets,
   size_t maxFreeBuffers,
   size_t bufferSize)
   :  JetByteTools::Win32::CSocketServer(lockFactory, pool, addressToListenOn, portToListenOn, maxFreeSockets, maxFreeBuffers, bufferSize),
      m_welcomeMessage(welcomeMessage)
{

}

void CSocketServer::OnStartAcceptingConnections()
{
   Output(_T("OnStartAcceptingConnections"));
}

void CSocketServer::OnStopAcceptingConnections()
{
   Output(_T("OnStopAcceptingConnections"));
}
      
void CSocketServer::OnShutdownInitiated()
{
   Output(_T("OnShutdownInitiated"));
}
      
void CSocketServer::OnShutdownComplete()
{
   Output(_T("OnShutdownComplete"));
}

void CSocketServer::OnConnectionEstablished(
   Socket *pSocket,
   CIOBuffer * /*pAddress*/)
{
   Output(_T("OnConnectionEstablished"));

   pSocket->Write(m_welcomeMessage.c_str(), m_welcomeMessage.length());

   pSocket->Read();
}

void CSocketServer::OnConnectionClientClose(
   Socket * /*pSocket*/)
{
   Output(_T("OnConnectionClientClose"));
}

void CSocketServer::OnConnectionReset(
   Socket * /*pSocket*/,
   DWORD lastError)
{
   Output(_T("OnConnectionReset: ") + GetLastErrorMessage(lastError));
}

bool CSocketServer::OnConnectionClosing(
   Socket *pSocket)
{
   Output(_T("OnConnectionClosing"));

   // we'll handle socket closure so that we can do a lingering close
   // note that this is not ideal as this code is executed on the an
   // IO thread. If we had a thread pool we could fire this off to the
   // thread pool to handle.

   pSocket->Close();

   return true;
}

void CSocketServer::OnConnectionClosed(
   Socket * /*pSocket*/)
{
   Output(_T("OnConnectionClosed"));
}

void CSocketServer::OnConnectionCreated()
{
   Output(_T("OnConnectionCreated"));
}

void CSocketServer::OnConnectionDestroyed()
{
   Output(_T("OnConnectionDestroyed"));
}

void CSocketServer::OnConnectionError(
   ConnectionErrorSource source,
   Socket *pSocket,
   CIOBuffer *pBuffer,
   DWORD lastError)
{
   const LPCTSTR errorSource = (source == ZeroByteReadError ? _T(" Zero Byte Read Error:") : (source == ReadError ? _T(" Read Error:") : _T(" Write Error:")));

   Output(_T("OnConnectionError - Socket = ") + ToString(pSocket) + _T(" Buffer = ") + ToString(pBuffer) + errorSource + GetLastErrorMessage(lastError));
}

void CSocketServer::OnError(
   const JetByteTools::Win32::_tstring &message)
{
   Output(_T("OnError - ") + message);
}

void CSocketServer::OnBufferCreated()
{
   Output(_T("OnBufferCreated"));
}

void CSocketServer::OnBufferAllocated()
{
   Output(_T("OnBufferAllocated"));
}

void CSocketServer::OnBufferReleased()
{
   Output(_T("OnBufferReleased"));
}

void CSocketServer::OnBufferDestroyed()
{
   Output(_T("OnBufferDestroyed"));
}

void CSocketServer::ReadCompleted(
   Socket *pSocket,
   CIOBuffer *pBuffer)
{
   try
   {
      pBuffer = ProcessDataStream(pSocket, pBuffer);

      pSocket->Read(pBuffer);
   }
   catch(const CException &e)
   {
      Output(_T("ReadCompleted - Exception - ") + e.GetWhere() + _T(" - ") + e.GetMessage());
      pSocket->Shutdown();
   }
   catch(...)
   {
      Output(_T("ReadCompleted - Unexpected exception"));
      pSocket->Shutdown();
   }
}

CIOBuffer *CSocketServer::ProcessDataStream(
   Socket *pSocket,
   CIOBuffer *pBuffer) const
{
   bool done;

   DEBUG_ONLY(Output(_T("ProcessDataStream:\n") + DumpData(pBuffer->GetBuffer(), pBuffer->GetUsed(), 40)));

   do
   {
      done = true;

      const size_t used = pBuffer->GetUsed();

      if (used >= GetMinimumMessageSize())
      {
         const size_t messageSize = GetMessageSize(pBuffer);

         if (messageSize == 0)
         {
            // havent got a complete message yet.

            // we null terminate our messages in the buffer, so we need to reserve
            // a byte of the buffer for this purpose...

            if (used == (pBuffer->GetSize() - 1))
            {
               Output(_T("Too much data!"));

               const std::string response("-ERR too much data! Go away!\r\n");

               // Write this message and then shutdown the sending side of the socket.
               pSocket->Write(response.c_str(), response.length());
               pSocket->Shutdown();

               // throw the rubbish away
               pBuffer->Empty();
               
               done = true;
            }
         }
         else if (used == messageSize)
         {
            Output(_T("Got complete, distinct, message"));
            // we have a whole, distinct, message

            pBuffer->AddData(0);   // null terminate the command string;

            ProcessCommand(pSocket, pBuffer);

            pBuffer->Empty();

            done = true;
         }
         else if (used > messageSize)
         {
            Output(_T("Got message plus extra data"));
            // we have a message, plus some more data

            // allocate a new buffer, copy the extra data into it and try again...

            CIOBuffer *pMessage = pBuffer->SplitBuffer(messageSize);
         
            pMessage->AddData(0);   // null terminate the command string;

            ProcessCommand(pSocket, pMessage);

            pMessage->Release();

            // loop again, we may have another complete message in there...
   
            done = false;
         }
      }
   }
   while (!done);

   // not enough data in the buffer, reissue a read into the same buffer to collect more data
   return pBuffer;
}

size_t CSocketServer::GetMinimumMessageSize() const
{
   // The smallest possible command we accept is TOP (plus the crlf terminator,
   // once we have this many bytes we can start with try and work out
   // what we have...
   return 5;   
}

size_t CSocketServer::GetMessageSize(const CIOBuffer *pBuffer) const
{
   const BYTE *pData = pBuffer->GetBuffer();
   
   const size_t used = pBuffer->GetUsed();

   for (size_t i = 0; i < used; ++i)
   {
      if (pData[i] == '\r')
      {
         if (i + 1 < used && pData[i + 1] == '\n')
         {
            // the end of the message is i+1
            // we actually want a count of characters, not a zero based
            // index, so we have to add 1...
            return i + 1 + 1; 
         }
      }
   }

   return 0;
}

void CSocketServer::ProcessCommand(
   Socket *pSocket,
   const CIOBuffer *pBuffer) const
{
   std::string command = reinterpret_cast<const char*>(pBuffer->GetBuffer());

   std::string echoCommand = command.substr(0, command.length() - 2); // strip the \r\n

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

   if (ok)
   {
      // We understand, but we aren't really a POP3 server...

      std::string response("-ERR sorry, we understand \"" +  echoCommand + "\", but, we're just a fake POP3 server...\r\n");

      pSocket->Write(response.c_str(), response.length());
   }
   else
   {
      std::string response("-ERR sorry, we dont know what you mean, bye...\r\n");

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
// End of file: SocketServer.cpp
///////////////////////////////////////////////////////////////////////////////
