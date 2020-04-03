(* OCaml-Win32
 * win32.ml
 * Copyright (c) 2002 by Harry Chomsky
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *)

(*
    Stuff to deal with:
        Do functions returning COLORREF ever return PALETTERGB or PALETTEINDEX values? Try SetPixel
        Is there any difference between HINSTANCE and HMODULE?
        OEM resource ids for LoadImage
        WM_SETTEXT return value; GetWindowText return value; GetDlgItemText return value
        Rich edit controls share EM_CHARFROMPOS with edit controls
        Analyzing / building resource data structs
        MDICREATESTRUCT / CLIENTCREATESTRUCT
*)

include Win32_core

(* Graphics and Multimedia Services: GDI *)
include Win32_bitmap
include Win32_brush
include Win32_clipping
include Win32_color
include Win32_coord
include Win32_dc
include Win32_filledshape
include Win32_text
include Win32_line
(* Metafiles ... *)
include Win32_paint
include Win32_path
include Win32_pen
(* Printing and Print Spooler ... *)
include Win32_rect
include Win32_region

(* Networking and Distributed Services: Windows Sockets Version 2: Windows Sockets Version 2 API ... *)

(* COM and ActiveX Object Services: ... *)

(* Setup and Systems Management Services *)
(* File Installation Library ... *)
(* System Information ... *)
(* System Policies ... *)
(* System Shutdown ... *)
(* Setup API ... *)
(* Windows Installer ... *)

(* User Interface Services: Shell and Common Controls: ... *)

(* User Interface Services: Controls *)
include Win32_control
include Win32_button
include Win32_combobox
include Win32_editcontrol
include Win32_listbox
(* Rich Edit Controls ... *)
include Win32_scrollbar
include Win32_staticcontrol

(* User Interface Services: Resources *)
include Win32_caret
include Win32_cursor
include Win32_icon
include Win32_menu
include Win32_resource

(* User Interface Services: User Input *)
(* Common Dialog Box Library ... *)
include Win32_mouse
include Win32_keyboard
include Win32_accelerator

(* User Interface Services: Windowing *)
include Win32_dialog
include Win32_message
(* Multiple Document Interface ... *)
include Win32_windowclass
include Win32_windowproc
include Win32_windowprop
include Win32_window

(* Windows Base Services: Debugging and Error Handling *)
include Win32_error
(* Event Logging ... *)
(* Structured Exception Handling ... *)

(* Windows Base Services: Executables *)
(* Dynamic-Link Libraries ... *)
include Win32_process
(* Services ... *)

(* Windows Base Services: Files and I/O *)
include Win32_console
(* Data Decompression Library ... *)
include Win32_file

(* Windows Base Services: General Library *)
include Win32_handle
(* Memory Management ... *)
(* Registry ... *)
include Win32_string
include Win32_time
(* Timers ... *)

(* Windows Base Services: Interprocess Communication *)
include Win32_ipc
include Win32_atom
(* Clipboard ... *)
(* Dynamic Data Exchange ... *)
(* Dynamic Data Exchange Management Library ... *)
(* Hooks ... *)
(* File Mapping ... *)
(* Mailslots ... *)
include Win32_pipe
include Win32_synch


(* Nicolas Cannasse extensions *)
include Win32_comctrl
include Win32_comdlg
include Win32_richedit
include Win32_shell
include Win32_dynlink
include Win32_system