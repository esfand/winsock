///////////////////////////////////////////////////////////////////////////////
// File: ServerMain.cpp
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

#include "JetByteTools\Win32Tools\WinsockWrapper.h"

#include "JetByteTools\Win32Tools\Exception.h"
#include "JetByteTools\Win32Tools\Utils.h"
#include "JetByteTools\Win32Tools\NamedIndex.h"
#include "JetByteTools\Win32Tools\SharedCriticalSection.h"

#include "SocketServer.h"
#include "IOPool.h"
#include "ThreadPool.h"

///////////////////////////////////////////////////////////////////////////////
// Lint options
//
//lint -save
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Using directives
///////////////////////////////////////////////////////////////////////////////

using JetByteTools::Win32::_tstring;
using JetByteTools::Win32::CException;
using JetByteTools::Win32::Output;
using JetByteTools::Win32::CManualResetEvent;
using JetByteTools::Win32::CNamedIndex;
using JetByteTools::Win32::CInstrumentedSharedCriticalSection;
using JetByteTools::Win32::CSharedCriticalSection;

///////////////////////////////////////////////////////////////////////////////
// Program entry point
///////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char * /*argv[ ]*/)
{
   try
   {
      CNamedIndex userDataSlots;

      CThreadPool businessLogicPool(
         userDataSlots,
         5,                            // initial number of threads to create
         5,                            // minimum number of threads to keep in the pool
         10,                           // maximum number of threads in the pool
         5,                            // maximum number of "dormant" threads
         5000,                         // pool maintenance period (millis)
         100,                          // dispatch timeout (millis)
         10000);                       // dispatch timeout for when pool is at max threads

      businessLogicPool.Start();

      const size_t numberOfUserDataSlots = userDataSlots.Lock();

      CIOPool ioPool(
         0);                           // number of threads (0 = 2 x processors)

      ioPool.Start();

      //CInstrumentedSharedCriticalSection lockFactory(47);

      CSharedCriticalSection lockFactory(47);

      CSocketServer server(
         lockFactory,
         ioPool,
         INADDR_ANY,                   // address to listen on
         5001,                         // port to listen on
         10,                           // max number of sockets to keep in the pool
         10,                           // max number of buffers to keep in the pool
         1024,                         // buffer size 
         numberOfUserDataSlots,
         businessLogicPool);

      server.Start();

      server.StartAcceptingConnections();

      CManualResetEvent shutdownEvent(_T("JetByteToolsServerShutdown"), false);
      CManualResetEvent pauseResumeEvent(_T("JetByteToolsServerPauseResume"), false);

      HANDLE handlesToWaitFor[2];

      handlesToWaitFor[0] = shutdownEvent.GetEvent();
      handlesToWaitFor[1] = pauseResumeEvent.GetEvent();

      bool accepting = true;
      bool done = false;

      while (!done)
      {
         DWORD waitResult = ::WaitForMultipleObjects(2, handlesToWaitFor, false, INFINITE);

         if (waitResult == WAIT_OBJECT_0)
         {
            done = true;
         }
         else if (waitResult == WAIT_OBJECT_0 + 1)
         {
            if (accepting)
            {
               server.StopAcceptingConnections();
            }
            else
            {
               server.StartAcceptingConnections();
            }

            accepting = !accepting;
         }
         else
         {
            Output(_T("Unexpected result from WaitForMultipleObjects - exiting"));
            done = true;
         }
      }

      businessLogicPool.WaitForShutdownToComplete(15000, true);

      ioPool.WaitForShutdownToComplete();

      server.WaitForShutdownToComplete();  
   }
   catch(const CException &e)
   {
      Output(_T("Exception: ") + e.GetWhere() + _T(" - ") + e.GetMessage());
   }
   catch(...)
   {
      Output(_T("Unexpected exception"));
   }


   return 0;
}

///////////////////////////////////////////////////////////////////////////////
// End of file: ServerMain.cpp
///////////////////////////////////////////////////////////////////////////////
