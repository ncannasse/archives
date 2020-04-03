(* OCaml-Win32
 * win32_window.ml
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
(* User Interface Services: Windowing: Windows *)

external adjust_window_rect : rect:rect -> style:style -> has_menu:bool -> rect = "adjust_window_rect"

external arrange_iconic_windows : wnd:hwnd -> int = "arrange_iconic_windows"

external bring_window_to_top : wnd:hwnd -> unit = "bring_window_to_top"

external cascade_windows : parent:hwnd -> options:tile_option list -> rect:rect option -> children:hwnd list -> int = "cascade_windows"

external child_window_from_point : wnd:hwnd -> pt:point -> hwnd = "child_window_from_point"

external child_window_from_point_ex : wnd:hwnd -> pt:point -> options:child_window_option list -> hwnd = "child_window_from_point_ex"

external close_window : wnd:hwnd -> unit = "close_window"

external create_window : cs:createstruct -> hwnd = "create_window"

external destroy_window : wnd:hwnd -> unit = "destroy_window"

external enum_child_windows : parent:hwnd -> func:(wnd:hwnd -> bool) -> unit = "enum_child_windows"

external enum_thread_windows : thread:int32 -> func:(wnd:hwnd -> bool) -> unit = "enum_thread_windows"

external enum_windows : func:(wnd:hwnd -> bool) -> unit = "enum_windows"

external find_window : class_name:atom_name -> window_name:string option -> hwnd = "find_window"

external find_window_ex : parent:hwnd -> child_after:hwnd -> class_name:atom_name -> window_name:string option -> hwnd = "find_window_ex"

external get_client_rect : wnd:hwnd -> rect = "get_client_rect"

external get_desktop_window : unit -> hwnd = "get_desktop_window"

external get_foreground_window : unit -> hwnd = "get_foreground_window"

external get_last_active_popup : wnd:hwnd -> hwnd = "get_last_active_popup"

external get_parent : wnd:hwnd -> hwnd = "get_parent"

external get_window_placement : wnd:hwnd -> windowplacement = "get_window_placement"

external get_window_rect : wnd:hwnd -> rect = "get_window_rect"

external get_window_text : wnd:hwnd -> string = "get_window_text"

external get_window_thread_process_id : wnd:hwnd -> (int32 * int32) = "get_window_thread_process_id"

external is_child : parent:hwnd -> child:hwnd -> bool = "is_child"

external is_iconic : wnd:hwnd -> bool = "is_iconic"

external is_window : wnd:hwnd -> bool = "is_window"

external is_window_unicode : wnd:hwnd -> bool = "is_window_unicode"

external is_window_visible : wnd:hwnd -> bool = "is_window_visible"

external is_zoomed : wnd:hwnd -> bool = "is_zoomed"

external move_window : wnd:hwnd -> pt:point -> size:size -> repaint:bool -> unit = "move_window"

external open_icon : wnd:hwnd -> unit = "open_icon"

external set_foreground_window : wnd:hwnd -> unit = "set_foreground_window"

external set_parent : child:hwnd -> parent:hwnd -> hwnd = "set_parent"

external set_window_placement : wnd:hwnd -> placement:windowplacement -> options:set_window_placement_option list -> unit = "set_window_placement"

external set_window_pos : pos:windowpos -> unit = "set_window_pos"

external set_window_pos_multi : poses:windowpos list -> unit = "set_window_pos_multi"

external set_window_text : wnd:hwnd -> text:string -> unit = "set_window_text"

external show_owned_popups : wnd:hwnd -> show:bool -> unit = "show_owned_popups"

external show_window : wnd:hwnd -> show:show_window_option -> bool = "show_window"

external show_window_async : wnd:hwnd -> show:show_window_option -> bool = "show_window_async"

external tile_windows : parent:hwnd -> options:tile_option list -> rect:rect option -> children:hwnd list -> int = "tile_windows"

external window_from_point : pt:point -> hwnd = "window_from_point"

(* Messages *)

type wm_moving_sizing_handler =
    wnd:hwnd ->
    msg:message_contents ->
    side:sizing_side ->
    rect:rect ->
    rect message_handler_result
external on_wm_moving_sizing :
    id:message_id ->
    wm_moving_sizing_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    message_result option = "on_wm_moving_sizing"

let wm_activateapp = 0x001C
type wm_activateapp_handler =
    wnd:hwnd ->
    msg:message_contents ->
    active:bool ->
    thread:int32 ->
    unit message_handler_result
external deliver_wm_activateapp :
    handler:wm_activateapp_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_activateapp"
let on_wm_activateapp = on_message ~id:wm_activateapp ~deliver:deliver_wm_activateapp ~return:return_simple_message

let wm_cancelmode = 0x001F
let on_wm_cancelmode = on_simple_message ~id:wm_cancelmode

let wm_childactivate = 0x0022
let on_wm_childactivate = on_simple_message ~id:wm_childactivate

let wm_close = 0x0010
let on_wm_close = on_simple_message ~id:wm_close

let wm_compacting = 0x0041
type wm_compacting_handler =
    wnd:hwnd ->
    msg:message_contents ->
    ratio:int ->
    unit message_handler_result
external deliver_wm_compacting :
    handler:wm_compacting_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_compacting"
let on_wm_compacting = on_message ~id:wm_compacting ~deliver:deliver_wm_compacting ~return:return_simple_message

let wm_create = 0x0001
type wm_create_handler =
    wnd:hwnd ->
    msg:message_contents ->
    cs:createstruct ->
    bool message_handler_result
external deliver_wm_create :
    handler:wm_create_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    bool message_handler_result = "deliver_wm_create"
external return_wm_create : result:bool -> message_result = "return_wm_create"
let on_wm_create = on_message ~id:wm_create ~deliver:deliver_wm_create ~return:return_wm_create

let wm_destroy = 0x0002
let on_wm_destroy = on_simple_message ~id:wm_destroy

let wm_enable = 0x000A
type wm_enable_handler =
    wnd:hwnd ->
    msg:message_contents ->
    enabled:bool ->
    unit message_handler_result
external deliver_wm_enable :
    handler:wm_enable_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_enable"
let on_wm_enable = on_message ~id:wm_enable ~deliver:deliver_wm_enable ~return:return_simple_message

let wm_entersizemove = 0x0231
let on_wm_entersizemove = on_simple_message ~id:wm_entersizemove

let wm_exitsizemove = 0x0232
let on_wm_exitsizemove = on_simple_message ~id:wm_exitsizemove

let wm_geticon = 0x007F
type wm_geticon_handler =
    wnd:hwnd ->
    msg:message_contents ->
    option:icon_option->
    hicon message_handler_result
external deliver_wm_geticon :
    handler:wm_geticon_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    hicon message_handler_result = "deliver_wm_geticon"
external return_wm_geticon : result:hicon -> message_result = "return_wm_geticon"
let on_wm_geticon = on_message ~id:wm_geticon ~deliver:deliver_wm_geticon ~return:return_wm_geticon

let wm_getminmaxinfo = 0x0024
type wm_getminmaxinfo_handler =
    wnd:hwnd ->
    msg:message_contents ->
    info:minmaxinfo ->
    minmaxinfo message_handler_result
external on_wm_getminmaxinfo :
    wm_getminmaxinfo_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    message_result option = "on_wm_getminmaxinfo"

let wm_gettext = 0x000D
type wm_gettext_handler =
    wnd:hwnd ->
    msg:message_contents ->
    string message_handler_result
external on_wm_gettext :
    wm_gettext_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    message_result option = "on_wm_gettext"

let wm_gettextlength = 0x000E
type wm_gettextlength_handler = int plain_message_handler
external return_wm_gettextlength : result:int -> message_result = "return_wm_gettextlength"
let on_wm_gettextlength = on_message ~id:wm_gettextlength ~deliver:deliver_plain_message ~return:return_wm_gettextlength

let wm_inputlangchange = 0x0051
(* ... *)

let wm_inputlangchangerequest = 0x0050
(* ... *)

let wm_move = 0x0003
type wm_move_handler =
    wnd:hwnd ->
    msg:message_contents ->
    pt:point ->
    unit message_handler_result
external deliver_wm_move :
    handler:wm_move_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_move"
let on_wm_move = on_message ~id:wm_move ~deliver:deliver_wm_move ~return:return_simple_message

let wm_moving = 0x0216
let on_wm_moving = on_wm_moving_sizing ~id:wm_moving

let wm_ncactivate = 0x0086
type wm_ncactivate_handler =
    wnd:hwnd ->
    msg:message_contents ->
    active:bool ->
    unit message_handler_result
external deliver_wm_ncactivate :
    handler:wm_ncactivate_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_ncactivate"
let on_wm_ncactivate = on_message ~id:wm_ncactivate ~deliver:deliver_wm_ncactivate ~return:return_simple_message

let wm_nccalcsize = 0x0083
(* ... *)

let wm_nccreate = 0x0081
type wm_nccreate_handler =
    wnd:hwnd ->
    msg:message_contents ->
    cs:createstruct ->
    bool message_handler_result
external deliver_wm_nccreate :
    handler:wm_nccreate_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    bool message_handler_result = "deliver_wm_nccreate"
external return_wm_nccreate : result:bool -> message_result = "return_wm_nccreate"
let on_wm_nccreate = on_message ~id:wm_nccreate ~deliver:deliver_wm_nccreate ~return:return_wm_nccreate

let wm_ncdestroy = 0x0082
let on_wm_ncdestroy = on_simple_message ~id:wm_ncdestroy

type parent_notify =
    Pn_create of int * hwnd
  | Pn_destroy of int * hwnd
  | Pn_lbutton of point
  | Pn_mbutton of point
  | Pn_rbutton of point
let wm_parentnotify = 0x0210
type wm_parentnotify_handler =
    wnd:hwnd ->
    msg:message_contents ->
    note:parent_notify ->
    unit message_handler_result
external deliver_wm_parentnotify :
    handler:wm_parentnotify_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_parentnotify"
let on_wm_parentnotify = on_message ~id:wm_parentnotify ~deliver:deliver_wm_parentnotify ~return:return_simple_message

let wm_querydragicon = 0x0037
type wm_querydragicon_handler = hicon plain_message_handler
external return_wm_querydragicon : result:hicon -> message_result = "return_wm_querydragicon"
let on_wm_querydragicon = on_message ~id:wm_querydragicon ~deliver:deliver_plain_message ~return:return_wm_querydragicon
let _ = add_explicit_dialog_return wm_querydragicon

let wm_queryopen = 0x0013
type wm_queryopen_handler = bool plain_message_handler
external return_wm_queryopen : result:bool -> message_result = "return_wm_queryopen"
let on_wm_queryopen = on_message ~id:wm_queryopen ~deliver:deliver_plain_message ~return:return_wm_queryopen

let wm_quit = 0x0012

let wm_seticon = 0x0080
type wm_seticon_handler =
    wnd:hwnd ->
    msg:message_contents ->
    option:icon_option ->
    icon:hicon ->
    hicon message_handler_result
external deliver_wm_seticon :
    handler:wm_seticon_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    hicon message_handler_result = "deliver_wm_seticon"
external return_wm_seticon : result:hicon -> message_result = "return_wm_seticon"
let on_wm_seticon = on_message ~id:wm_seticon ~deliver:deliver_wm_seticon ~return:return_wm_seticon

let wm_settext = 0x000C
type wm_settext_handler =
    wnd:hwnd ->
    msg:message_contents ->
    text:string ->
    int32 message_handler_result
external deliver_wm_settext :
    handler:wm_settext_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    int32 message_handler_result = "deliver_wm_settext"
let on_wm_settext = on_message ~id:wm_settext ~deliver:deliver_wm_settext ~return:return_general_message

let wm_settingchange = 0x001A
(* ... *)

let wm_showwindow = 0x0018
type wm_showwindow_handler =
    wnd:hwnd ->
    msg:message_contents ->
    show:bool ->
    reason:show_window_reason ->
    unit message_handler_result
external deliver_wm_showwindow :
    handler:wm_showwindow_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_showwindow"
let on_wm_showwindow = on_message ~id:wm_showwindow ~deliver:deliver_wm_showwindow ~return:return_simple_message

let wm_size = 0x0005
type wm_size_handler =
    wnd:hwnd ->
    msg:message_contents ->
    typ:size_type ->
    size:size ->
    unit message_handler_result
external deliver_wm_size :
    handler:wm_size_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_size"
let on_wm_size = on_message ~id:wm_size ~deliver:deliver_wm_size ~return:return_simple_message

let wm_sizing = 0x0214
let on_wm_sizing = on_wm_moving_sizing ~id:wm_sizing

type wm_stylechange_handler =
    wnd:hwnd ->
    msg:message_contents ->
    is_ex:bool ->
    old_style:style ->
    new_style:style ->
    unit message_handler_result
external deliver_wm_stylechange :
    handler:wm_stylechange_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_stylechange"

let wm_stylechanged = 0x007D
let on_wm_stylechanged = on_message ~id:wm_stylechanged ~deliver:deliver_wm_stylechange ~return:return_simple_message

let wm_stylechanging = 0x007C
let on_wm_stylechanging = on_message ~id:wm_stylechanging ~deliver:deliver_wm_stylechange ~return:return_simple_message

let wm_userchanged = 0x0054
let on_wm_userchanged = on_simple_message ~id:wm_userchanged

let wm_windowposchanged = 0x0047
type wm_windowposchanged_handler =
    wnd:hwnd ->
    msg:message_contents ->
    pos:windowpos ->
    unit message_handler_result
external deliver_wm_windowposchanged :
    handler:wm_windowposchanged_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_windowposchanged"
let on_wm_windowposchanged = on_message ~id:wm_windowposchanged ~deliver:deliver_wm_windowposchanged ~return:return_simple_message

let wm_windowposchanging = 0x0046
type wm_windowposchanging_handler =
    wnd:hwnd ->
    msg:message_contents ->
    pos:windowpos ->
    windowpos message_handler_result
external on_wm_windowposchanging :
    wm_windowposchanging_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    message_result option = "on_wm_windowposchanging"
