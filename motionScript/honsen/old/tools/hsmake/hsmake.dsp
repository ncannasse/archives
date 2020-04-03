# Microsoft Developer Studio Project File - Name="hsmake" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=hsmake - Win32 Bytecode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "hsmake.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hsmake.mak" CFG="hsmake - Win32 Bytecode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hsmake - Win32 Bytecode" (based on "Win32 (x86) External Target")
!MESSAGE "hsmake - Win32 Native code" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "hsmake - Win32 Bytecode"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake hsmake.dsp"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "hsmake.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "ocamake hsmake.dsp extLib.cma xml-light.cma -o ..\..\bin\hsmake.exe -lp "-dllib hslib.dll""
# PROP Rebuild_Opt "-all"
# PROP Target_File "..\bin\hsmake.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "hsmake - Win32 Native code"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake -opt hsmake.dsp -o hsmake_opt.exe"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "hsmake_opt.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "ocamake -opt hsmake.dsp extLib.cma xml-light.cmxa -o ..\..\bin\hsmake.exe ../../hslib/release/hslib.lib"
# PROP Rebuild_Opt "-all"
# PROP Target_File "..\bin\hsmake.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "hsmake - Win32 Bytecode"
# Name "hsmake - Win32 Native code"

!IF  "$(CFG)" == "hsmake - Win32 Bytecode"

!ELSEIF  "$(CFG)" == "hsmake - Win32 Native code"

!ENDIF 

# Begin Source File

SOURCE=.\hsmake.ml
# End Source File
# Begin Source File

SOURCE=..\..\common\hsx.ml
# End Source File
# Begin Source File

SOURCE=..\..\common\mtw.ml
# End Source File
# Begin Source File

SOURCE=..\..\common\mtw.mli
# End Source File
# Begin Source File

SOURCE=..\..\common\tree.ml
# End Source File
# Begin Source File

SOURCE=..\..\common\tree.mli
# End Source File
# End Target
# End Project
