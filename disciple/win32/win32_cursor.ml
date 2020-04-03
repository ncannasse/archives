(* OCaml-Win32
 * win32_cursor.ml
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
(* User Interface Services: Resources: Cursors *)

external clip_cursor : rect:rect option -> unit = "clip_cursor"

external copy_cursor : cursor:hcursor -> hcursor = "copy_cursor"

(* ... create_cursor *)

external destroy_cursor : cursor:hcursor -> unit = "destroy_cursor"

external get_clip_cursor : unit -> rect = "get_clip_cursor"

external get_cursor : unit -> hcursor = "get_cursor"

external get_cursor_pos : unit -> point = "get_cursor_pos"

external load_cursor : inst:hinstance -> name:resource_name -> hcursor = "load_cursor"

(* ... load_cursor_from_file *)

external set_cursor : cursor:hcursor -> hcursor = "set_cursor"

external set_cursor_pos : pos:point -> unit = "set_cursor_pos"

(* ... set_system_cursor *)

external show_cursor : show:bool -> int = "show_cursor"

(* Messages *)

let wm_setcursor = 0x0020
type wm_setcursor_handler =
    wnd:hwnd ->
    msg:message_contents ->
    cursor_wnd:hwnd ->
    hit:hit_test_result ->
    id:message_id ->
    bool message_handler_result
external deliver_wm_setcursor :
    handler:wm_setcursor_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    bool message_handler_result = "deliver_wm_setcursor"
external return_wm_setcursor : result:bool -> message_result = "return_wm_setcursor"
let on_wm_setcursor = on_message ~id:wm_setcursor ~deliver:deliver_wm_setcursor ~return:return_wm_setcursor
