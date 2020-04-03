# Microsoft Developer Studio Project File - Name="PDragoon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=PDragoon - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PDragoon.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PDragoon.mak" CFG="PDragoon - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PDragoon - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "PDragoon - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PDragoon - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy
PostBuild_Cmds=copy Release\PDragoon.lib ..\DataEdit
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PDragoon - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "PDragoon - Win32 Release"
# Name "PDragoon - Win32 Debug"
# Begin Group "CPP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\button.cpp
# End Source File
# Begin Source File

SOURCE=.\combo.cpp
# End Source File
# Begin Source File

SOURCE=.\dbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\dmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\DString.cpp
# End Source File
# Begin Source File

SOURCE=.\edit.cpp
# End Source File
# Begin Source File

SOURCE=.\grid.cpp
# End Source File
# Begin Source File

SOURCE=.\label.cpp
# End Source File
# Begin Source File

SOURCE=.\list.cpp
# End Source File
# Begin Source File

SOURCE=.\plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\plugobj.cpp
# End Source File
# End Group
# Begin Group "H"

# PROP Default_Filter ""
# Begin Source File

SOURCE="C:\Program Files\Visual Studio\Vc98\Include\Basetsd.h"
# End Source File
# Begin Source File

SOURCE=.\DString.h
# End Source File
# Begin Source File

SOURCE=.\pluginclass.h
# End Source File
# End Group
# End Target
# End Project
