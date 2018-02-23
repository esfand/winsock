cd ..\Test
start ..\EchoServer\Output\%1%\EchoServer.exe
IF ERRORLEVEL 1 goto Failed
SocketServerTest.exe EchoServerTest.xml
IF ERRORLEVEL 1 goto Failed
Echo Test Passed

goto End

:Failed

Echo Test Failed

:End

ServerShutdown.exe STOP

