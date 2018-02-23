cd ..\Test
start ..\SimpleProtocolServer\Output\%1%\SimpleProtocolServer.exe
IF ERRORLEVEL 1 goto Failed
SocketServerTest.exe SimpleProtocolServerTest.xml
IF ERRORLEVEL 1 goto Failed
Echo Test Passed

goto End

:Failed

Echo Test Failed

:End

ServerShutdown.exe STOP

