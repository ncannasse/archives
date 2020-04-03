# Microsoft Developer Studio Project File - Name="mysql" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mysql - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mysql.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mysql.mak" CFG="mysql - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mysql - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Mysql"
# PROP Intermediate_Dir "Mysql"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MYSQL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MYSQL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ocamlrund.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy mysql\mysql.dll c:\ocaml\lib\mysql.dll
# End Special Build Tool
# Begin Target

# Name "mysql - Win32 Debug"
# Begin Group "H"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wlibs\res\caml\alloc.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\res\caml\callback.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\res\caml\config.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\res\caml\custom.h
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\mysql\src\inc\errmsg.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\res\caml\fail.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\res\caml\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\res\caml\mlvalues.h
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\mysql\src\inc\mysql.h
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\mysql\src\inc\mysql_com.h
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\mysql\src\inc\mysql_version.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\OCaml\mysql\src\mysql_stubs.c
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\mysql\src\lib\libmySQL.lib
# End Source File
# End Target
# End Project
