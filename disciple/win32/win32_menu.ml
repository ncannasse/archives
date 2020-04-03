(* OCaml-Win32
 * win32_menu.ml
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
(* User Interface Services: Resources: Menus *)

external check_menu_item : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> check:bool -> bool = "check_menu_item"

external check_menu_radio_item :
    menu:hmenu ->
    first:int ->
    last:int ->
    check:int ->
    typ:menu_item_specifier_type ->
    unit = "check_menu_radio_item"

external create_menu : unit -> hmenu = "create_menu"

external create_popup_menu : unit -> hmenu = "create_popup_menu"

external delete_menu : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> unit = "delete_menu"

external destroy_menu : menu:hmenu -> unit = "destroy_menu"

external draw_menu_bar : wnd:hwnd -> unit = "draw_menu_bar"

external enable_menu_item : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> state:menu_item_enable_state -> menu_item_enable_state = "enable_menu_item"

external get_menu : wnd:hwnd -> hmenu = "get_menu"

external get_menu_default_item : menu:hmenu -> typ:menu_item_specifier_type -> options:get_menu_default_item_option list -> int = "get_menu_default_item"

external get_menu_item_count : menu:hmenu -> int = "get_menu_item_count"

external get_menu_item_id : menu:hmenu -> pos:int -> int = "get_menu_item_id"

external get_menu_item_info : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> max_length:int -> get_menu_item_info = "get_menu_item_info"

external get_menu_item_rect : wnd:hwnd -> menu:hmenu -> pos:int -> rect = "get_menu_item_rect"

external get_sub_menu : menu:hmenu -> pos:int -> hmenu = "get_sub_menu"

external get_system_menu : wnd:hwnd -> revert:bool -> hmenu = "get_system_menu"

external hilite_menu_item : wnd:hwnd -> menu:hmenu -> item:int -> typ:menu_item_specifier_type -> hilite:bool -> unit = "hilite_menu_item"

external insert_menu_item : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> info:set_menu_item_info -> unit = "insert_menu_item"

external is_menu : menu:hmenu -> bool = "is_menu"

external load_menu : inst:hinstance -> name:resource_name -> hmenu = "load_menu"

(* ... load_menu_indirect *)

external menu_item_from_point : wnd:hwnd -> menu:hmenu -> pt:point -> int option = "menu_item_from_point"

external remove_menu : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> unit = "remove_menu"

external set_menu : wnd:hwnd -> menu:hmenu -> unit = "set_menu"

external set_menu_default_item : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> unit = "set_menu_default_item"

external set_menu_item_bitmaps :
    menu:hmenu ->
    item:int ->
    typ:menu_item_specifier_type ->
    unchecked:hbitmap ->
    checked:hbitmap ->
    unit = "set_menu_item_bitmaps"

external set_menu_item_info : menu:hmenu -> item:int -> typ:menu_item_specifier_type -> info:set_menu_item_info -> unit = "set_menu_item_info"

external track_popup_menu : menu:hmenu -> options:track_popup_menu_option list -> pt:point -> wnd:hwnd -> rect:rect option -> int option = "track_popup_menu"

(* Messages *)

let wm_contextmenu = 0x007B
type wm_contextmenu_handler =
    wnd:hwnd ->
    msg:message_contents ->
    mouse_wnd:hwnd ->
    pt:point ->
    unit message_handler_result
external deliver_wm_contextmenu :
    handler:wm_contextmenu_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_contextmenu"
let on_wm_contextmenu = on_message ~id:wm_contextmenu ~deliver:deliver_wm_contextmenu ~return:return_simple_message

type wm_menuloop_handler =
    wnd:hwnd ->
    msg:message_contents ->
    is_track_popup_menu:bool ->
    unit message_handler_result
external deliver_wm_menuloop :
    handler:wm_menuloop_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_menuloop"

let wm_entermenuloop = 0x0211
let on_wm_entermenuloop = on_message ~id:wm_entermenuloop ~deliver:deliver_wm_menuloop ~return:return_simple_message

let wm_exitmenuloop = 0x0212
let on_wm_exitmenuloop = on_message ~id:wm_exitmenuloop ~deliver:deliver_wm_menuloop ~return:return_simple_message

let wm_nextmenu = 0x0213
type wm_nextmenu_handler =
    wnd:hwnd ->
    msg:message_contents ->
    key:virtual_key_code ->
    in_menu:hmenu ->
    next_menu:hmenu ->
    next_window:hwnd ->
    unit message_handler_result
external deliver_wm_nextmenu :
    handler:wm_nextmenu_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_nextmenu"
let on_wm_nextmenu = on_message ~id:wm_nextmenu ~deliver:deliver_wm_nextmenu ~return:return_simple_message
