(* OCaml-Win32
 * win32_control.ml
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
(* User Interface Services: Controls: Controls *)

(* Messages *)

let wm_getfont = 0x0031
type wm_getfont_handler = hfont plain_message_handler
external return_wm_getfont : result:hfont -> message_result = "return_wm_getfont"
let on_wm_getfont = on_message ~id:wm_getfont ~deliver:deliver_plain_message ~return:return_wm_getfont
let pack_wm_getfont = pack_simple_message ~id:wm_getfont
external unpack_wm_getfont : message_result -> hfont = "unpack_wm_getfont"
let issue_wm_getfont ~sender = unpack_wm_getfont (sender ~msg:pack_wm_getfont)

let wm_setfont = 0x0030
type wm_setfont_handler =
    wnd:hwnd ->
    msg:message_contents ->
    font:hfont ->
    redraw:bool ->
    unit message_handler_result
external deliver_wm_setfont:
    handler:wm_setfont_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_setfont"
let on_wm_setfont = on_message ~id:wm_setfont ~deliver:deliver_wm_setfont ~return:return_simple_message
external pack_wm_setfont : font:hfont -> redraw:bool -> message_contents = "pack_wm_setfont"
let unpack_wm_setfont = ignore
let issue_wm_setfont ~sender ~font ~redraw = unpack_wm_setfont (sender ~msg:(pack_wm_setfont ~font ~redraw))

let wm_command = 0x0111
type wm_command_handler =
    wnd:hwnd ->
    msg:message_contents ->
    notify_code:int ->
    id:int ->
    ctrl:hwnd ->
    unit message_handler_result
external deliver_wm_command :
    handler:wm_command_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_command"
let on_wm_command = on_message ~id:wm_command ~deliver:deliver_wm_command ~return:return_simple_message
