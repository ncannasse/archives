(* OCaml-Win32
 * win32_accelerator.ml
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

open Win32_core

(********************************************************************)
(* User Interface Services: User Input: Keyboard Accelerators *)

(* ... copy_accelerator_table, create_accelerator_table, destroy_accelerator_table *)

external load_accelerators : inst:hinstance -> name:resource_name -> haccel = "load_accelerators"

external translate_accelerator : wnd:hwnd -> accel:haccel -> msg:msg -> bool = "translate_accelerator"

(* Messages *)

let wm_initmenu = 0x0116
type wm_initmenu_handler =
    wnd:hwnd ->
    msg:message_contents ->
    menu:hmenu ->
    unit message_handler_result
external deliver_wm_initmenu :
    handler:wm_initmenu_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_initmenu"
let on_wm_initmenu = on_message ~id:wm_initmenu ~deliver:deliver_wm_initmenu ~return:return_simple_message

let wm_initmenupopup = 0x0117
type wm_initmenupopup_handler =
    wnd:hwnd ->
    msg:message_contents ->
    menu:hmenu ->
    pos:int ->
    system_menu:bool ->
    unit message_handler_result
external deliver_wm_initmenupopup :
    handler:wm_initmenupopup_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_initmenupopup"
let on_wm_initmenupopup = on_message ~id:wm_initmenupopup ~deliver:deliver_wm_initmenupopup ~return:return_simple_message

let wm_menuchar = 0x0120
(* ... *)

let wm_menuselect = 0x011F
(* ... *)

let wm_syschar = 0x0106
let on_wm_syschar = on_char_message ~id:wm_syschar

type sys_command =
    SC_CLOSE
  | SC_CONTEXTHELP
  | SC_DEFAULT
  | SC_HOTKEY
  | SC_HSCROLL
  | SC_KEYMENU
  | SC_MAXIMIZE
  | SC_MINIMIZE
  | SC_MOUSEMENU
  | SC_MOVE
  | SC_NEXTWINDOW
  | SC_PREVWINDOW
  | SC_RESTORE
  | SC_SCREENSAVE
  | SC_SIZE
  | SC_TASKLIST
  | SC_VSCROLL
  | SC_MONITORPOWER_low
  | SC_MONITORPOWER_off
type sys_command_source =
    Scs_mouse of point
  | Scs_accelerator
  | Scs_mnemonic
  | Scs_system
let wm_syscommand = 0x0112
type wm_syscommand_handler =
    wnd:hwnd ->
    msg:message_contents ->
    command:sys_command ->
    source:sys_command_source ->
    unit message_handler_result
external deliver_wm_syscommand :
    handler:wm_syscommand_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_syscommand"
let on_wm_syscommand = on_message ~id:wm_syscommand ~deliver:deliver_wm_syscommand ~return:return_simple_message
