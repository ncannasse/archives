# Microsoft Developer Studio Project File - Name="asml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=asml - Win32 Bytecode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "asml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "asml.mak" CFG="asml - Win32 Bytecode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "asml - Win32 Bytecode" (based on "Win32 (x86) External Target")
!MESSAGE "asml - Win32 Native code" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "asml - Win32 Bytecode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake -pp camlp4o asml.dsp"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "asml.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "ocamake -pp camlp4o asml.dsp -g extlib.cma win32.cma"
# PROP Rebuild_Opt "-all"
# PROP Target_File "asml.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "asml - Win32 Native code"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake -pp camlp4o -opt asml.dsp -o asml_opt.exe"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "asml_opt.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "ocamake -pp camlp4o -opt asml.dsp -o asml.exe extlib.cmxa win32.cmxa"
# PROP Rebuild_Opt "-all"
# PROP Target_File "asml.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "asml - Win32 Bytecode"
# Name "asml - Win32 Native code"

!IF  "$(CFG)" == "asml - Win32 Bytecode"

!ELSEIF  "$(CFG)" == "asml - Win32 Native code"

!ENDIF 

# Begin Group "_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\checker.ml
# End Source File
# Begin Source File

SOURCE=.\expr.ml
# End Source File
# Begin Source File

SOURCE=.\lexer.mll
# End Source File
# Begin Source File

SOURCE=.\main.ml
# End Source File
# Begin Source File

SOURCE=.\parser.ml
# End Source File
# End Group
# Begin Source File

SOURCE=.\typer.ml
# End Source File
# Begin Source File

SOURCE=.\types.ml
# End Source File
# Begin Source File

SOURCE=.\oSDep.ml
# End Source File
# End Target
# End Project
