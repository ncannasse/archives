# Microsoft Developer Studio Project File - Name="m3dse" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=m3dse - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "m3dse.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "m3dse.mak" CFG="m3dse - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "m3dse - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m3dse - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "m3dse - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3DSE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3DSE_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3d8.lib d3dx8.lib advapi32.lib gdi32.lib user32.lib ../std/Release/std.lib /nologo /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\m3dse.dll ..\bin >NUL
# End Special Build Tool

!ELSEIF  "$(CFG)" == "m3dse - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3DSE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3DSE_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3d8.lib d3dx8.lib advapi32.lib gdi32.lib user32.lib ../std/Debug/std.lib /nologo /dll /profile /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\m3dse.dll ..\bin >NUL
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "m3dse - Win32 Release"
# Name "m3dse - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\m3d.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_2d.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_core.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_font.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_lib.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_pipeline.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_res.cpp
# End Source File
# Begin Source File

SOURCE=.\m3d_texture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\m3d.h
# End Source File
# Begin Source File

SOURCE=.\m3d_2d.h
# End Source File
# Begin Source File

SOURCE=.\m3d_api.h
# End Source File
# Begin Source File

SOURCE=.\m3d_cache.h
# End Source File
# Begin Source File

SOURCE=.\m3d_context.h
# End Source File
# Begin Source File

SOURCE=.\m3d_pipeline.h
# End Source File
# Begin Source File

SOURCE=.\m3d_res.h
# End Source File
# Begin Source File

SOURCE=.\m3d_sdk.h
# End Source File
# Begin Source File

SOURCE=.\mdx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3d8.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3d8caps.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3d8types.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8core.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8effect.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8math.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8math.inl"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8mesh.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8shape.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8tex.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\Program Files\Microsoft Visual Studio\VC98\Include\dxfile.h"
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\context.c
# End Source File
# Begin Source File

SOURCE=..\common\context.h
# End Source File
# Begin Source File

SOURCE=..\common\mtw.cpp
# End Source File
# Begin Source File

SOURCE=..\common\mtw.h
# End Source File
# Begin Source File

SOURCE=..\common\timer.c
# End Source File
# Begin Source File

SOURCE=..\common\timer.h
# End Source File
# Begin Source File

SOURCE=..\common\wbtree.h
# End Source File
# Begin Source File

SOURCE=..\common\wlist.h
# End Source File
# Begin Source File

SOURCE=..\common\wmem.h
# End Source File
# Begin Source File

SOURCE=..\common\wpairbtree.h
# End Source File
# Begin Source File

SOURCE=..\common\wstring.h
# End Source File
# End Group
# Begin Group "Script"

# PROP Default_Filter ""
# Begin Group "2D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\script\element.cpp
# End Source File
# Begin Source File

SOURCE=.\script\element.h
# End Source File
# Begin Source File

SOURCE=.\script\group.cpp
# End Source File
# Begin Source File

SOURCE=.\script\sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\script\text.cpp
# End Source File
# End Group
# Begin Group "3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\script\anim.cpp
# End Source File
# Begin Source File

SOURCE=.\script\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\script\mesh.cpp
# End Source File
# End Group
# Begin Group "Maths"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\script\matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\script\quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\script\script.h
# End Source File
# Begin Source File

SOURCE=.\script\trs.cpp
# End Source File
# Begin Source File

SOURCE=.\script\vector.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\script\others.cpp
# End Source File
# Begin Source File

SOURCE=..\std\value.h
# End Source File
# End Group
# End Target
# End Project
