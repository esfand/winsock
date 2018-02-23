cd ..\Test
start ..\PacketEchoServer\Output\%1%\PacketEchoServer.exe
IF ERRORLEVEL 1 goto Failed
SocketServerTest.exe PacketEchoServerTest3.xml
IF ERRORLEVEL 1 goto Failed
Echo Test Passed
SocketServerTest.exe PacketEchoServerTest4.xml
IF ERRORLEVEL 1 goto Failed
Echo Test Passed

goto End

:Failed

Echo Test Failed

:End

ServerShutdown.exe STOP

