# Microsoft Developer Studio Project File - Name="mtypes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=mtypes - Win32 Bytecode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mtypes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mtypes.mak" CFG="mtypes - Win32 Bytecode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mtypes - Win32 Bytecode" (based on "Win32 (x86) External Target")
!MESSAGE "mtypes - Win32 Native code" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "mtypes - Win32 Bytecode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake -pp camlp4o mtypes.dsp"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "mtypes.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "nmake byte"
# PROP Rebuild_Opt "-all"
# PROP Target_File "mtypes.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "mtypes - Win32 Native code"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake -pp camlp4o -opt mtypes.dsp -o mtypes_opt.exe"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "mtypes_opt.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "nmake native"
# PROP Rebuild_Opt "-all"
# PROP Target_File "mtypes.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "mtypes - Win32 Bytecode"
# Name "mtypes - Win32 Native code"

!IF  "$(CFG)" == "mtypes - Win32 Bytecode"

!ELSEIF  "$(CFG)" == "mtypes - Win32 Native code"

!ENDIF 

# Begin Group "doc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\genDoc\docDataBuilder.ml
# End Source File
# Begin Source File

SOURCE=.\genDoc\docDataPrinter.ml
# End Source File
# Begin Source File

SOURCE=.\genDoc\genDoc.ml
# End Source File
# End Group
# Begin Group "swf"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mtcvs\swflib\actionScript.ml
# End Source File
# Begin Source File

SOURCE=..\..\mtcvs\swflib\swf.ml
# End Source File
# Begin Source File

SOURCE=..\..\mtcvs\swflib\swfParser.ml
# End Source File
# Begin Source File

SOURCE=..\..\mtcvs\swflib\swfZip.ml
# End Source File
# End Group
# Begin Group "generators"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\class.ml
# End Source File
# Begin Source File

SOURCE=.\genAs.ml
# End Source File
# Begin Source File

SOURCE=.\genStrings.ml
# End Source File
# Begin Source File

SOURCE=.\genSwf.ml
# End Source File
# End Group
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
# Begin Source File

SOURCE=.\plugin.ml
# End Source File
# Begin Source File

SOURCE=.\type.ml
# End Source File
# Begin Source File

SOURCE=.\typer.ml
# End Source File
# End Target
# End Project
