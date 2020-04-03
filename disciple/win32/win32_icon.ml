(* OCaml-Win32
 * win32_icon.ml
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
(* User Interface Services: Resources: Icons *)

external copy_icon : icon:hicon -> hicon = "copy_icon"

external create_icon :
    inst:hinstance ->
    size:size ->
    planes:int ->
    bits_per_pel:int ->
    and_bits:raw ->
    xor_bits:raw ->
    hicon = "create_icon_bytecode" "create_icon_native"

(* ... create_icon_from_resource, create_icon_from_resource_ex *)

external create_icon_indirect : info:iconinfo -> hicon = "create_icon_indirect"

external destroy_icon : icon:hicon -> unit = "destroy_icon"

external draw_icon : dc:hdc -> pt:point -> icon:hicon -> unit = "draw_icon"

external draw_icon_ex :
    dc:hdc ->
    pt:point ->
    icon:hicon ->
    size:size ->
    step:int ->
    flicker_free:hbrush ->
    options:draw_icon_option list ->
    unit = "draw_icon_ex_bytecode" "draw_icon_ex_native"

external extract_associated_icon :
    inst:hinstance ->
    filename:string ->
    index:int ->
    (hicon * string * int) = "extract_associated_icon"

(* ... extract_icon, extract_icon_ex *)

external get_icon_info : icon:hicon -> iconinfo = "get_icon_info"

external get_icon_info_by_name : name:resource_name -> iconinfo = "get_icon_info_by_name"

external load_icon : inst:hinstance -> name:resource_name -> hicon = "load_icon"

(* ... lookup_icon_id_from_directory, lookup_icon_id_from_directory_ex *)

(* Messages *)

let wm_erasebkgnd = 0x0014
type wm_erasebkgnd_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dc:hdc ->
    bool message_handler_result
external deliver_wm_erasebkgnd :
    handler:wm_erasebkgnd_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    bool message_handler_result = "deliver_wm_erasebkgnd"
external return_wm_erasebkgnd : result:bool -> message_result = "return_wm_erasebkgnd"
let on_wm_erasebkgnd = on_message ~id:wm_erasebkgnd ~deliver:deliver_wm_erasebkgnd ~return:return_wm_erasebkgnd

let wm_iconerasebkgnd = 0x0027
type wm_iconerasebkgnd_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dc:hdc ->
    unit message_handler_result
external deliver_wm_iconerasebkgnd :
    handler:wm_iconerasebkgnd_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_iconerasebkgnd"
let on_wm_iconerasebkgnd = on_message ~id:wm_iconerasebkgnd ~deliver:deliver_wm_iconerasebkgnd ~return:return_simple_message
