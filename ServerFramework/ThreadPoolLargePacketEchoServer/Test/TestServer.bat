cd ..\Test
start ..\ThreadPoolLargePacketEchoServer\Output\%1%\ThreadPoolLargePacketEchoServer.exe
IF ERRORLEVEL 1 goto Failed
SocketServerTest.exe LargePacketEchoServerTest.xml
IF ERRORLEVEL 1 goto Failed
Echo Test Passed

goto End

:Failed

Echo Test Failed

:End

ServerShutdown.exe STOP

