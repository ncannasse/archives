(* OCaml-Win32
 * win32_scrollbar.ml
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
(* User Interface Services: Controls: Scroll Bars *)

external enable_scroll_bar : wnd:hwnd -> typ:scroll_bar_type -> option:enable_scroll_bar_option -> bool = "enable_scroll_bar"

external get_scroll_info : wnd:hwnd -> typ:scroll_bar_type -> get_scroll_info = "get_scroll_info"

external get_scroll_pos : wnd:hwnd -> typ:scroll_bar_type -> int = "get_scroll_pos"

external get_scroll_range : wnd:hwnd -> typ:scroll_bar_type -> (int * int) = "get_scroll_range"

external scroll_dc : dc:hdc -> amount:point -> scroll:rect -> clip:rect -> update:hrgn -> rect = "scroll_dc"

external scroll_window :
    wnd:hwnd ->
    amount:point ->
    scroll:rect ->
    clip:rect ->
    update:hrgn ->
    options:scroll_window_option list ->
    rect = "scroll_window_bytecode" "scroll_window_native"

external set_scroll_info : wnd:hwnd -> typ:scroll_bar_type -> info:set_scroll_info -> redraw:bool -> int = "set_scroll_info"

external set_scroll_pos : wnd:hwnd -> typ:scroll_bar_type -> pos:int -> redraw:bool -> int = "set_scroll_pos"

external set_scroll_range : wnd:hwnd -> typ:scroll_bar_type -> (int * int) -> redraw:bool -> unit = "set_scroll_range"

external show_scroll_bar : wnd:hwnd -> typ:scroll_bar_type -> show:bool -> unit = "show_scroll_bar"

(* Styles *)

type scrollbar_style_option =
    SBS_BOTTOMALIGN
  | SBS_HORZ
  | SBS_LEFTALIGN
  | SBS_RIGHTALIGN
  | SBS_SIZEBOX
  | SBS_SIZEBOXBOTTOMRIGHTALIGN
  | SBS_SIZEBOXTOPLEFTALIGN
  | SBS_SIZEGRIP
  | SBS_TOPALIGN
  | SBS_VERT

external scrollbar_style_of_option : scrollbar_style_option -> style = "scrollbar_style_of_option"

let scrollbar_style_of_options = special_style_of_options ~special_func:scrollbar_style_of_option

(* Messages *)

let sbm_enable_arrows = 0x00E4
external pack_sbm_enable_arrows : option:enable_scroll_bar_option -> message_contents = "pack_sbm_enable_arrows"
external unpack_sbm_enable_arrows : message_result -> unit = "unpack_sbm_enable_arrows"
let issue_sbm_enable_arrows ~sender ~option = unpack_sbm_enable_arrows (sender ~msg:(pack_sbm_enable_arrows ~option))

let sbm_getpos = 0x00E1
let pack_sbm_getpos = pack_simple_message ~id:sbm_getpos
external unpack_sbm_getpos : message_result -> int = "unpack_sbm_getpos"
let issue_sbm_getpos ~sender = unpack_sbm_getpos (sender ~msg:pack_sbm_getpos)

let sbm_getrange = 0x00E3
external issue_sbm_getrange : sender:message_sender -> (int * int) = "issue_sbm_getrange"

let sbm_getscrollinfo = 0x00EA
external issue_sbm_getscrollinfo : sender:message_sender -> get_scroll_info = "issue_sbm_getscrollinfo"

let sbm_setpos = 0x00E0
external pack_sbm_setpos : pos:int -> redraw:bool -> message_contents = "pack_sbm_setpos"
external unpack_sbm_setpos : message_result -> int = "unpack_sbm_setpos"
let issue_sbm_setpos ~sender ~pos ~redraw = unpack_sbm_setpos (sender ~msg:(pack_sbm_setpos ~pos ~redraw))

let sbm_setrange = 0x00E2
external pack_sbm_setrange : range:(int * int) -> message_contents = "pack_sbm_setrange"
external unpack_sbm_setrange : message_result -> int = "unpack_sbm_setrange"
let issue_sbm_setrange ~sender ~range = unpack_sbm_setrange (sender ~msg:(pack_sbm_setrange ~range))

let sbm_setrangeredraw = 0x00E6
external pack_sbm_setrangeredraw : range:(int * int) -> message_contents = "pack_sbm_setrangeredraw"
external unpack_sbm_setrangeredraw : message_result -> int = "unpack_sbm_setrangeredraw"
let issue_sbm_setrangeredraw ~sender ~range = unpack_sbm_setrangeredraw (sender ~msg:(pack_sbm_setrangeredraw ~range))

let sbm_setscrollinfo = 0x00E9
external issue_sbm_setscrollinfo : sender:message_sender -> info:set_scroll_info -> redraw:bool -> int = "issue_sbm_setscrollinfo"

let wm_ctlcolorscrollbar = 0x0137
let on_wm_ctlcolorscrollbar = on_ctlcolor_message ~id:wm_ctlcolorscrollbar
let _ = add_explicit_dialog_return wm_ctlcolorscrollbar

type wm_scroll_handler =
    wnd:hwnd ->
    msg:message_contents ->
    action:scroll_action ->
    bar:hwnd ->
    unit message_handler_result
external deliver_wm_scroll :
    handler:wm_scroll_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_scroll"

let wm_hscroll = 0x0114
let on_wm_hscroll = on_message ~id:wm_hscroll ~deliver:deliver_wm_scroll ~return:return_simple_message

let wm_vscroll = 0x0115
let on_wm_vscroll = on_message ~id:wm_vscroll ~deliver:deliver_wm_scroll ~return:return_simple_message
