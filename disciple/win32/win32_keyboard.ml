(* OCaml-Win32
 * win32_keyboard.ml
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
(* User Interface Services: User Input: Keyboard Input *)

(* ... activate_keyboard_layout *)

external enable_window : wnd:hwnd -> enable:bool -> bool = "enable_window"

external get_active_window : unit -> hwnd = "get_active_window"

external get_async_key_state : key:virtual_key_code -> (bool * bool) = "get_async_key_state"

external get_focus : unit -> hwnd = "get_focus"

(* ... get_keyboard_layout, get_keyboard_layout_list, get_keyboard_layout_name *)

(* ... get_keyboard_state *)

external get_key_name_text : data:key_data -> dont_care:bool -> max_length:int -> string = "get_key_name_text"

external get_key_state : key:virtual_key_code -> (bool * bool) = "get_key_state"

external is_window_enabled : wnd:hwnd -> bool = "is_window_enabled"

(* ... load_keyboard_layout *)

(* ... map_virtual_key, map_virtual_key_ex *)

(* ... oem_key_scan *)

external register_hot_key : wnd:hwnd -> id:int -> modifiers:hot_key_modifier list -> key:virtual_key_code -> unit = "register_hot_key"

(* ... send_input *)

external set_active_window : wnd:hwnd -> hwnd = "set_active_window"

external set_focus : wnd:hwnd -> hwnd = "set_focus"

(* ... set_keyboard_state *)

(* ... to_ascii, to_ascii_ex, to_unicode, to_unicode_ex *)

(* ... unload_keyboard_layout *)

external unregister_hot_key : wnd:hwnd -> id:int -> unit = "unregister_hot_key"

(* ... vk_key_scan, vk_key_scan_ex *)

(* Messages *)

let wm_activate = 0x0006
type wm_activate_handler =
    wnd:hwnd ->
    msg:message_contents ->
    flag:activate_flag ->
    minimized:bool ->
    other:hwnd ->
    unit message_handler_result
external deliver_wm_activate :
    handler:wm_activate_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_activate"
let on_wm_activate = on_message ~id:wm_activate ~deliver:deliver_wm_activate ~return:return_simple_message

let wm_char = 0x0102
let on_wm_char = on_char_message ~id:wm_char

let wm_deadchar = 0x0103
let on_wm_deadchar = on_char_message ~id:wm_deadchar

let wm_gethotkey = 0x0033
(* ... *)

let wm_hotkey = 0x0312
let idhot_snapdesktop = -2
let idhot_snapwindow = -1
type wm_hotkey_handler =
    wnd:hwnd ->
    msg:message_contents ->
    id:int ->
    modifiers:hot_key_modifier list ->
    key:virtual_key_code ->
    unit message_handler_result
external deliver_wm_hotkey :
    handler:wm_hotkey_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_hotkey"
let on_wm_hotkey = on_message ~id:wm_hotkey ~deliver:deliver_wm_hotkey ~return:return_simple_message

let wm_keydown = 0x0100
let on_wm_keydown = on_key_message ~id:wm_keydown

let wm_keyup = 0x0101
let on_wm_keyup = on_key_message ~id:wm_keyup

type wm_focus_handler =
    wnd:hwnd ->
    msg:message_contents ->
    focus_wnd:hwnd ->
    unit message_handler_result
external deliver_wm_focus :
    handler:wm_focus_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_focus"

let wm_killfocus = 0x0008
let on_wm_killfocus = on_message ~id:wm_killfocus ~deliver:deliver_wm_focus ~return:return_simple_message

let wm_setfocus = 0x0007
let on_wm_setfocus = on_message ~id:wm_setfocus ~deliver:deliver_wm_focus ~return:return_simple_message

let wm_sethotkey = 0x0032
(* ... *)

let wm_sysdeadchar = 0x0107
let on_wm_sysdeadchar = on_char_message ~id:wm_sysdeadchar

let wm_syskeydown = 0x0104
let on_wm_syskeydown = on_key_message ~id:wm_syskeydown

let wm_syskeyup = 0x0105
let on_wm_syskeyup = on_key_message ~id:wm_syskeyup
