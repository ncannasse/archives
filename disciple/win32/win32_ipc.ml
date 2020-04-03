(* OCaml-Win32
 * win32_ipc.ml
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

(* Messages *)

let wm_copydata = 0x004A
type wm_copydata_handler =
    wnd:hwnd ->
    msg:message_contents ->
    source_wnd:hwnd ->
    cds:copydatastruct ->
    bool message_handler_result
external deliver_wm_copydata :
    handler:wm_copydata_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    bool message_handler_result = "deliver_wm_copydata"
external return_wm_copydata : result:bool -> message_result = "return_wm_copydata"
let on_wm_copydata = on_message ~id:wm_copydata ~deliver:deliver_wm_copydata ~return:return_wm_copydata
external issue_wm_copydata : sender:message_sender -> source_wnd:hwnd -> cds:copydatastruct -> bool = "issue_wm_copydata"
