# Microsoft Developer Studio Project File - Name="viewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=viewer - Win32 Bytecode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "viewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "viewer.mak" CFG="viewer - Win32 Bytecode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "viewer - Win32 Bytecode" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake viewer.dsp"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "viewer.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "ocamake viewer.dsp win32.cma m3d.dll"
# PROP Rebuild_Opt "-all"
# PROP Target_File "viewer.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""
# Begin Target

# Name "viewer - Win32 Bytecode"

!IF  "$(CFG)" == "viewer - Win32 Bytecode"

!ENDIF 

# Begin Group "RES"

# PROP Default_Filter "ml;mly;mll"
# Begin Source File

SOURCE=..\m3d.ml
# End Source File
# Begin Source File

SOURCE=..\m3d.mli
# End Source File
# Begin Source File

SOURCE=..\mtlib.ml
# End Source File
# Begin Source File

SOURCE=..\mtlib.mli
# End Source File
# Begin Source File

SOURCE=..\osiris.ml
# End Source File
# Begin Source File

SOURCE=..\osiris.mli
# End Source File
# End Group
# Begin Source File

SOURCE=.\viewer.ml
# End Source File
# End Target
# End Project
