# Microsoft Developer Studio Project File - Name="Test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Generic Project" 0x010a

CFG=Test - Win32 Unicode Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Test.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Test.mak" CFG="Test - Win32 Unicode Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Test - Win32 Release" (based on "Win32 (x86) Generic Project")
!MESSAGE "Test - Win32 Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE "Test - Win32 Unicode Debug" (based on "Win32 (x86) Generic Project")
!MESSAGE "Test - Win32 Unicode Release" (based on "Win32 (x86) Generic Project")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
MTL=midl.exe

!IF  "$(CFG)" == "Test - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "."
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Test - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "."
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Test - Win32 Unicode Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "."
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Test - Win32 Unicode Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "."
# PROP BASE Intermediate_Dir "."
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Test - Win32 Release"
# Name "Test - Win32 Debug"
# Name "Test - Win32 Unicode Debug"
# Name "Test - Win32 Unicode Release"
# Begin Source File

SOURCE=.\EchoServerTest.xml
# End Source File
# Begin Source File

SOURCE=.\TestServer.bat

!IF  "$(CFG)" == "Test - Win32 Release"

# Begin Custom Build - Testing
InputPath=.\TestServer.bat

"ServerTest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /F /Q ServerTest 
	$(InputPath) "VC6\Release" 
	echo > ServerTest 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Test - Win32 Debug"

# Begin Custom Build - Testing
InputPath=.\TestServer.bat

"ServerTest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /F /Q ServerTest 
	$(InputPath) "VC6\Debug" 
	echo > ServerTest 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Test - Win32 Unicode Debug"

# Begin Custom Build - Testing
InputPath=.\TestServer.bat

"ServerTest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /F /Q ServerTest 
	$(InputPath) "VC6\UDebug" 
	echo > ServerTest 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Test - Win32 Unicode Release"

# Begin Custom Build - Testing
InputPath=.\TestServer.bat

"ServerTest" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /F /Q ServerTest 
	$(InputPath) "VC6\URelease" 
	echo > ServerTest 
	
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
