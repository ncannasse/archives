# Microsoft Developer Studio Project File - Name="maxtwin2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=maxtwin2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "maxtwin2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "maxtwin2.mak" CFG="maxtwin2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "maxtwin2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "maxtwin2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "maxtwin2 - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "c:\3dsmax4\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "EXPORTER" /YX /FD /LD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib advapi32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib d3dx8.lib /nologo /base:"0x3ea50000" /subsystem:windows /dll /machine:I386 /out:".\Release\maxtwin2.dle" /libpath:"c:\3dsmax4\maxsdk\lib" /release
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\maxtwin2.dle "c:\program files\3dsmax5\plugins\" >NUL	copy Release\maxtwin2.dle ..\..\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "maxtwin2 - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "c:\3dsmax4\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "EXPORTER" /FR /YX /FD /LD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib advapi32.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib d3dx8.lib /nologo /base:"0x3ea50000" /subsystem:windows /dll /debug /machine:I386 /out:".\Debug\maxtwin2.dle" /pdbtype:sept /libpath:"c:\3dsmax4\maxsdk\lib"
# SUBTRACT LINK32 /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy
PostBuild_Cmds=copy Debug\maxtwin2.dle "c:\program files\3dsmax5\plugins\" >NUL
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "maxtwin2 - Win32 Release"
# Name "maxtwin2 - Win32 Debug"
# Begin Group "MaxTwin2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\maxtwin.cpp
# End Source File
# Begin Source File

SOURCE=.\maxtwin.h
# End Source File
# Begin Source File

SOURCE=.\maxtwin_obj.cpp
# End Source File
# Begin Source File

SOURCE=.\maxtwin_obj.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\maxtwin.def
# End Source File
# Begin Source File

SOURCE=.\res\maxtwin.rc
# End Source File
# End Group
# Begin Group "LIB"

# PROP Default_Filter ""
# Begin Group "MAXSDK"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\acolor.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\actiontable.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\animtbl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\appio.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\assert1.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\baseinterface.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\bitarray.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\box2.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\box3.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\buildver.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\captypes.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\channels.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\cmdmode.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\color.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\coreexp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\custcont.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\dbgprint.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\dpoint3.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\euler.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\evuser.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\excllist.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\export.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\gencam.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\genhier.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\genlight.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\genshape.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\geom.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\geomlib.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\gfloat.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\gfx.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\gfxlib.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\gutil.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\hitdata.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\hold.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\icolorman.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ifnpub.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\impapi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\impexp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\impexpintf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\imtl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\inode.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\INodeGIProperties.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\interval.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ioapi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\iparamb.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\iparamb2.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\iparamm.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ipipelineclient.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ipoint2.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ipoint3.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\istdplug.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\itreevw.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\lockid.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\log.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\matrix2.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\matrix3.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\max.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\maxapi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\maxcom.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\maxtess.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\maxtypes.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mesh.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\meshlib.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mnbigmat.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mncommon.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mnmath.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mnmesh.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\MNNormalSpec.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\modstack.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mouseman.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\mtl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\namesel.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\nametab.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\object.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\objmode.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\paramtype.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\patch.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\patchlib.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\patchobj.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\plugapi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\plugin.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\point2.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\point3.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\point4.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\polyobj.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\polyshp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ptrvec.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\quat.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\random.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\ref.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\render.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\renderelements.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\rtclick.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\sbmtlapi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\sceneapi.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\sfx.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\shape.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\shphier.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\shpsels.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\snap.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\soundobj.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\spline3d.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\stack.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\stack3.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\stdmat.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\strbasic.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\strclass.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\svcore.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\systemutilities.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\tab.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\templt.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\trig.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\triobj.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\udmia64.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\units.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\utilexp.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\utilintf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\utillib.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\vedge.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\Program Files\3dsmax5\maxsdk\include\winutil.h"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\common\mtw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\mtw.h
# End Source File
# Begin Source File

SOURCE=..\..\common\wlist.h
# End Source File
# Begin Source File

SOURCE=..\..\common\wstring.h
# End Source File
# End Group
# End Target
# End Project
