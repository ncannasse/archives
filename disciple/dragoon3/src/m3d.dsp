# Microsoft Developer Studio Project File - Name="m3d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=m3d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "m3d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "m3d.mak" CFG="m3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "m3d - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "m3d - Win32 3D Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "m3d - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "m3d___Win32_Debug"
# PROP BASE Intermediate_Dir "m3d___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3D_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3D_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3d8.lib d3dx8.lib advapi32.lib user32.lib ocamlrun.lib gdi32.lib dxguid.lib dinput8.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy
PostBuild_Cmds=copy                   Debug\m3d.dll                   .\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "m3d - Win32 3D Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "3D Release"
# PROP BASE Intermediate_Dir "3D Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "3D Release"
# PROP Intermediate_Dir "3D Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3D_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "M3D_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3d8.lib d3dx8.lib ocamlrun.lib gdi32.lib user32.lib dxguid.lib dinput8.lib advapi32.lib /nologo /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy
PostBuild_Cmds=copy        "3D Release\m3d.dll"         .\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "m3d - Win32 Debug"
# Name "m3d - Win32 3D Release"
# Begin Group "CPP"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\c++\m3d\m3d_camera.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_data.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_events.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_externals.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_fx.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_inexp.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_infos.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_init.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_input.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_render.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_res.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_shader.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_sprite.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_text.cpp"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\vfs.cpp"
# End Source File
# End Group
# Begin Group "H"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\c++\fxlib\anyfx.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_caml.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_keys.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_res.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\m3d_struct.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\mdx.h"
# End Source File
# Begin Source File

SOURCE="..\c++\m3d\vfs.h"
# End Source File
# End Group
# Begin Group "LIBS"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=C:\ocaml\lib\caml\alloc.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\BASETSD.H"
# End Source File
# Begin Source File

SOURCE=C:\ocaml\lib\caml\callback.h
# End Source File
# Begin Source File

SOURCE=C:\ocaml\lib\caml\config.h
# End Source File
# Begin Source File

SOURCE=C:\ocaml\lib\caml\custom.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3d8.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3d8caps.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3d8types.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8core.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8effect.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8math.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8math.inl"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8mesh.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8shape.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\d3dx8tex.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\dxfile.h"
# End Source File
# Begin Source File

SOURCE=C:\ocaml\lib\caml\fail.h
# End Source File
# Begin Source File

SOURCE=C:\ocaml\lib\caml\misc.h
# End Source File
# Begin Source File

SOURCE=C:\ocaml\lib\caml\mlvalues.h
# End Source File
# End Group
# Begin Group "WLIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wlibs\warray.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\wbtree.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\wiostream.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\wlist.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\wmem.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\wpairbtree.h
# End Source File
# Begin Source File

SOURCE=..\..\wlibs\wstring.h
# End Source File
# End Group
# End Target
# End Project
