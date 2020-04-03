# Microsoft Developer Studio Project File - Name="disciple" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=disciple - Win32 Bytecode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "disciple.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "disciple.mak" CFG="disciple - Win32 Bytecode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "disciple - Win32 Bytecode" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ""
# PROP BASE Intermediate_Dir ""
# PROP BASE Cmd_Line "ocamake -a disciple.dsp"
# PROP BASE Rebuild_Opt "-all"
# PROP BASE Target_File "disciple.cma"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Cmd_Line "ocamake -a disciple.dsp"
# PROP Rebuild_Opt "-all"
# PROP Target_File "disciple.dll"
# PROP Bsc_Name ""
# PROP Target_Dir ""
# Begin Target

# Name "disciple - Win32 Bytecode"

!IF  "$(CFG)" == "disciple - Win32 Bytecode"

!ENDIF 

# Begin Group "DISCLIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\input.ml
# End Source File
# Begin Source File

SOURCE=.\input.mli
# End Source File
# Begin Source File

SOURCE=.\m3d.dll
# End Source File
# Begin Source File

SOURCE=.\m3d.ml
# End Source File
# Begin Source File

SOURCE=.\m3d.mli
# End Source File
# Begin Source File

SOURCE=.\mtlib.ml
# End Source File
# Begin Source File

SOURCE=.\mtlib.mli
# End Source File
# Begin Source File

SOURCE=.\osiris.ml
# End Source File
# Begin Source File

SOURCE=.\osiris.mli
# End Source File
# Begin Source File

SOURCE=.\protocol.ml
# End Source File
# Begin Source File

SOURCE=.\protocol.mli
# End Source File
# Begin Source File

SOURCE=.\run.mli
# End Source File
# Begin Source File

SOURCE=.\vfs.ml
# End Source File
# Begin Source File

SOURCE=.\vfs.mli
# End Source File
# End Group
# Begin Group "CLIENT"

# PROP Default_Filter ""
# Begin Group "INGAME."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\disciple\pBoard.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\pCard.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\pCreature.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\pEvent.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\pGame.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\pGame.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\pHand.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\pPlayer.ml
# End Source File
# End Group
# Begin Group "OUTGAME."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\disciple\cGames.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\cGames.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\data.ml
# End Source File
# End Group
# Begin Group "GENERAL."

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\constfiles.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\mouse.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\selection.ml
# End Source File
# End Group
# Begin Source File

SOURCE=.\disciple\client.ml
# End Source File
# End Group
# Begin Group "SERVER"

# PROP Default_Filter ""
# Begin Group "INGAME"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\disciple\game.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\game.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\player.ml
# End Source File
# End Group
# Begin Group "OUTGAME"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\disciple\games.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\games.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\players.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\players.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\sData.ml
# End Source File
# End Group
# Begin Source File

SOURCE=.\disciple\deck.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\server.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\server.mli
# End Source File
# End Group
# Begin Group "COMMON"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\disciple\board.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\card.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\card.mli
# End Source File
# Begin Source File

SOURCE=.\disciple\constants.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\creature.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\eQueue.ml
# End Source File
# Begin Source File

SOURCE=.\disciple\hand.ml
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
# End Target
# End Project
