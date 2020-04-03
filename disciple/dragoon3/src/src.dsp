# Microsoft Developer Studio Project File - Name="src" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=src - Win32 Boot
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "src.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "src.mak" CFG="src - Win32 Boot"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "src - Win32 Boot" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "nmake /f "src.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "boot.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "nmake /nologo /f "src.mak" all"
# PROP Rebuild_Opt "/a"
# PROP Target_File "boot.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""
# Begin Target

# Name "src - Win32 Boot"

!IF  "$(CFG)" == "src - Win32 Boot"

!ENDIF 

# Begin Group "INTERFACE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\console.ml
# End Source File
# Begin Source File

SOURCE=.\document.ml
# End Source File
# Begin Source File

SOURCE=.\document.mli
# End Source File
# Begin Source File

SOURCE=.\main.ml
# End Source File
# Begin Source File

SOURCE=.\main.mli
# End Source File
# Begin Source File

SOURCE=.\workspace.ml
# End Source File
# Begin Source File

SOURCE=.\workspace.mli
# End Source File
# End Group
# Begin Group "VFS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dir.ml
# End Source File
# Begin Source File

SOURCE=.\dir.mli
# End Source File
# Begin Source File

SOURCE=.\protocol.ml
# End Source File
# Begin Source File

SOURCE=.\protocol.mli
# End Source File
# Begin Source File

SOURCE=.\vfs.ml
# End Source File
# Begin Source File

SOURCE=.\vfs.mli
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dialogs.ml
# End Source File
# Begin Source File

SOURCE=.\dialogs.mli
# End Source File
# Begin Source File

SOURCE=.\gfxres.ml
# End Source File
# Begin Source File

SOURCE=.\gfxres.mli
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\osiris\src\osiris.ml
# End Source File
# Begin Source File

SOURCE=..\..\OCaml\osiris\src\osiris.mli
# End Source File
# Begin Source File

SOURCE=.\xgui.ml
# End Source File
# Begin Source File

SOURCE=.\xgui.mli
# End Source File
# End Group
# Begin Group "PLUGINS"

# PROP Default_Filter ""
# Begin Group "DISC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\disciple\card.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\card.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\iCard.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\iCard.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\maths.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\message.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\message.mli
# End Source File
# End Group
# Begin Source File

SOURCE=.\dcards.ml
# End Source File
# Begin Source File

SOURCE=.\dparser.mly
# End Source File
# Begin Source File

SOURCE=.\dparser_lex.mll
# End Source File
# Begin Source File

SOURCE=.\symbols.ml
# End Source File
# End Group
# Begin Group "CORE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\boot.ml
# End Source File
# Begin Source File

SOURCE=.\core.ml
# End Source File
# Begin Source File

SOURCE=.\log.ml
# End Source File
# Begin Source File

SOURCE=.\log.mli
# End Source File
# Begin Source File

SOURCE=.\run.ml
# End Source File
# Begin Source File

SOURCE=.\run.mli
# End Source File
# End Group
# Begin Group "3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\m3d.ml
# End Source File
# Begin Source File

SOURCE=.\m3d.mli
# End Source File
# Begin Source File

SOURCE=.\m3dview.ml
# End Source File
# End Group
# Begin Source File

SOURCE=.\mtlib.ml
# End Source File
# Begin Source File

SOURCE=.\mtlib.mli
# End Source File
# End Target
# End Project
