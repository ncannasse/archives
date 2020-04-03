# Microsoft Developer Studio Project File - Name="Animator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Animator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Animator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Animator.mak" CFG="Animator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Animator - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Animator - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Animator - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANIMATOR_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANIMATOR_EXPORTS" /D "ANIMS_ATOR" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ddraw.lib dxguid.lib dinput.lib PDragoon.lib /nologo /dll /machine:I386 /nodefaultlib:"libc.lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy
PostBuild_Cmds=copy Release\animator.dll d:\nico\dragoon2
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Animator - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANIMATOR_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ANIMATOR_EXPORTS" /D "ANIMS_ATOR" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib ddraw.lib dinput.lib dxguid.lib PDragoon.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy
PostBuild_Cmds=copy Debug\animator.dll d:\nico\dragoon2
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Animator - Win32 Release"
# Name "Animator - Win32 Debug"
# Begin Group "CPP"

# PROP Default_Filter "*.cpp"
# Begin Group "Moteur."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\decor_fake.cpp
# End Source File
# Begin Source File

SOURCE=.\dlib.cpp
# End Source File
# Begin Source File

SOURCE=.\entree.cpp
# End Source File
# Begin Source File

SOURCE=.\erreurs.cpp
# End Source File
# Begin Source File

SOURCE=.\fonts.cpp
# End Source File
# Begin Source File

SOURCE=.\graphisme.cpp
# End Source File
# Begin Source File

SOURCE=.\sprites.cpp
# End Source File
# End Group
# Begin Group "Plugin."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\animain.cpp
# End Source File
# Begin Source File

SOURCE=.\aplugin.cpp
# End Source File
# Begin Source File

SOURCE=.\interface.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\animator.cpp
# End Source File
# Begin Source File

SOURCE=.\anims.cpp
# End Source File
# End Group
# Begin Group "H"

# PROP Default_Filter "*.h"
# Begin Group "Moteur"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\decor_fake.h
# End Source File
# Begin Source File

SOURCE=.\dlib.h
# End Source File
# Begin Source File

SOURCE=.\entree.h
# End Source File
# Begin Source File

SOURCE=.\erreurs.h
# End Source File
# Begin Source File

SOURCE=.\fonts.h
# End Source File
# Begin Source File

SOURCE=.\graphisme.h
# End Source File
# Begin Source File

SOURCE=.\sprites.h
# End Source File
# End Group
# Begin Group "Plugin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DString.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\interface.h
# End Source File
# Begin Source File

SOURCE=.\Plugin.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\anims.h
# End Source File
# End Group
# Begin Group "Res"

# PROP Default_Filter "*.rc;*.bmp;*.def"
# Begin Group "bmp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\anm.bmp
# End Source File
# Begin Source File

SOURCE=.\anmico.bmp
# End Source File
# Begin Source File

SOURCE=.\classicfont.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\animator.def
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Visual Studio\Vc98\Include\Basetsd.h"
# End Source File
# Begin Source File

SOURCE=.\bmp.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# End Target
# End Project
