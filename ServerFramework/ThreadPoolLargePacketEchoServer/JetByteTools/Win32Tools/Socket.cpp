///////////////////////////////////////////////////////////////////////////////
// File: Socket.cpp
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

#include "Socket.h"

#include <atlbase.h>

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
// Member not defined
//lint -esym(1526, CSocket::CSocket)
//lint -esym(1526, CSocket::operator=)
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Namespace: JetByteTools::Win32
///////////////////////////////////////////////////////////////////////////////

namespace JetByteTools {
namespace Win32 {

///////////////////////////////////////////////////////////////////////////////
// CSocket
///////////////////////////////////////////////////////////////////////////////

CSocket::CSocket()
   :  m_socket(INVALID_SOCKET)
{
}

CSocket::CSocket(
   SOCKET theSocket)
   :  m_socket(theSocket)
{
   if (INVALID_SOCKET == m_socket)
   {
      throw Exception(_T("CSocket::CSocket()"),  WSAENOTSOCK);
   }
}
      
CSocket::~CSocket()
{
   if (INVALID_SOCKET != m_socket)
   {
      AbortiveClose();
   }
}

void CSocket::Attach(
   SOCKET theSocket)
{
   AbortiveClose();

   m_socket = theSocket;
}

SOCKET CSocket::Detatch()
{
   SOCKET theSocket = m_socket;

   m_socket = INVALID_SOCKET;

   return theSocket;
}

void CSocket::Close()
{
   if (0 != ::closesocket(m_socket))
   {
      throw Exception(_T("CSocket::Close()"), ::WSAGetLastError());
   }
}

void CSocket::AbortiveClose()
{
   LINGER lingerStruct;

   lingerStruct.l_onoff = 1;
   lingerStruct.l_linger = 0;

   if (SOCKET_ERROR == ::setsockopt(m_socket, SOL_SOCKET, SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct)))
   {
      throw Exception(_T("CSocket::AbortiveClose()"), ::WSAGetLastError());
   }
   
   Close();
}

void CSocket::Shutdown(
   int how)
{
   if (0 != ::shutdown(m_socket, how))
   {
      throw Exception(_T(" CSocket::Shutdown()"), ::WSAGetLastError());
   }
}

void CSocket::Listen(
   int backlog)
{
   if (SOCKET_ERROR == ::listen(m_socket, backlog))
   {
      throw Exception(_T("CSocket::Listen()"), ::WSAGetLastError());
   }
}

void CSocket::Bind(
   const SOCKADDR_IN &address)
{
   if (SOCKET_ERROR == ::bind(m_socket, reinterpret_cast<struct sockaddr *>(const_cast<SOCKADDR_IN*>(&address)), sizeof(SOCKADDR_IN)))
   {
      throw Exception(_T("CSocket::Bind()"), ::WSAGetLastError());
   }
}

void CSocket::Bind(
   const struct sockaddr &address,   
   size_t addressLength)
{
   //lint -e{713} Loss of precision (arg. no. 3) (unsigned int to int)
   if (SOCKET_ERROR == ::bind(m_socket, const_cast<struct sockaddr *>(&address), addressLength))
   {
      throw Exception(_T("CSocket::Bind()"), ::WSAGetLastError());
   }
}

void CSocket::Connect(
   const SOCKADDR_IN &address)
{
   if (SOCKET_ERROR == ::connect(m_socket, reinterpret_cast<struct sockaddr *>(const_cast<SOCKADDR_IN*>(&address)), sizeof(SOCKADDR_IN)))
   {
      throw Exception(_T("CSocket::Connect()"), ::WSAGetLastError());
   }
}

void CSocket::Connect(
   const struct sockaddr &address,   
   size_t addressLength)
{
   //lint -e{713} Loss of precision (arg. no. 3) (unsigned int to int)
   if (SOCKET_ERROR == ::connect(m_socket, const_cast<struct sockaddr *>(&address), addressLength))
   {
      throw Exception(_T("CSocket::Conect()"), ::WSAGetLastError());
   }
}



///////////////////////////////////////////////////////////////////////////////
// CSocket::InternetAddress
///////////////////////////////////////////////////////////////////////////////

CSocket::InternetAddress::InternetAddress(
   const unsigned long address,
   const unsigned short port)
{
   sin_family = AF_INET;
   sin_port = htons(port);
   sin_addr.s_addr = address;
}

CSocket::InternetAddress::InternetAddress(
   const _tstring &address,
   const unsigned short port)
{
   USES_CONVERSION;

   const char *pAddress = T2A(const_cast<TCHAR*>(address.c_str()));
   
   sin_family = AF_INET;
   sin_port = htons(port);
   sin_addr.s_addr = inet_addr(pAddress);

   if (sin_addr.s_addr == INADDR_NONE)
   {
      // address wasnt dotted ip, try to resolve name...

      HOSTENT *pHostEnt = ::gethostbyname(pAddress);

      if (!pHostEnt)
      {
         throw Exception(_T("InternetAddress::InternetAddress() - gethostbyname"), ::WSAGetLastError());
      }
      
      CopyMemory(&sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);
   }
}

///////////////////////////////////////////////////////////////////////////////
// CSocket::Exception
///////////////////////////////////////////////////////////////////////////////

CSocket::Exception::Exception(
   const _tstring &where, 
   DWORD error)
   : CWin32Exception(where, error)
{
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
// End of file: Socket.cpp
///////////////////////////////////////////////////////////////////////////////

