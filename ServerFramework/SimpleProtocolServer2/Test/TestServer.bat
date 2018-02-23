cd ..\Test
start ..\SimpleProtocolServer2\Output\%1%\SimpleProtocolServer2.exe
IF ERRORLEVEL 1 goto Failed
SocketServerTest.exe SimpleProtocolServerTest.xml
IF ERRORLEVEL 1 goto Failed
SocketServerTest.exe SimpleProtocolServerTest2.xml
IF ERRORLEVEL 1 goto Failed
Echo Test Passed

goto End

:Failed

Echo Test Failed

:End

ServerShutdown.exe STOP

