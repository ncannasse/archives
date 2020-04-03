(* OCaml-Win32
 * win32_paint.ml
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
(* Graphics and Multimedia Services: GDI: Painting and Drawing *)

external begin_paint : wnd:hwnd -> paintstruct = "begin_paint"

external draw_animated_rects : wnd:hwnd -> typ:animation_type -> from_rect:rect -> to_rect:rect -> unit = "draw_animated_rects"

external draw_caption : wnd:hwnd -> dc:hdc -> rect:rect -> options:draw_caption_option list -> unit = "draw_caption"

external draw_edge : dc:hdc -> rect:rect -> typ:edge_type -> options:border_option list -> unit = "draw_edge"

external draw_focus_rect : dc:hdc -> rect:rect -> unit = "draw_focus_rect"

external draw_frame_control : dc:hdc -> rect:rect -> typ:draw_frame_control_type -> options:draw_frame_control_option list -> unit = "draw_frame_control"

(* ... draw_state *)

external end_paint : wnd:hwnd -> ps:paintstruct -> unit = "end_paint"

external exclude_update_rgn : dc:hdc -> wnd:hwnd -> region_complexity = "exclude_update_rgn"

external gdi_flush : unit -> unit = "gdi_flush"

external gdi_get_batch_limit : unit -> int64 = "gdi_get_batch_limit"

external gdi_set_batch_limit : limit:int64 -> int64 = "gdi_set_batch_limit"

external get_bk_color : dc:hdc -> rgb = "get_bk_color"

external get_bk_mode : dc:hdc -> background_mode = "get_bk_mode"

(* ... get_bounds_rect, set_bounds_rect *)

external get_rop2 : dc:hdc -> rop2 = "get_rop2"

external get_update_rect : wnd:hwnd -> erase:bool -> rect option = "get_update_rect"

external get_update_rgn : wnd:hwnd -> rgn:hrgn -> erase:bool -> region_complexity = "get_update_rgn"

external get_window_dc : wnd:hwnd -> hdc = "get_window_dc"

external get_window_rgn : wnd:hwnd -> rgn:hrgn -> region_complexity = "get_window_rgn"

(* ... gray_string *)

external invalidate_rect : wnd:hwnd -> rect:rect option -> erase:bool -> unit = "invalidate_rect"

external invalidate_rgn : wnd:hwnd -> rgn:hrgn -> erase:bool -> unit = "invalidate_rgn"

external lock_window_update : wnd:hwnd -> unit = "lock_window_update"

external paint_desktop : dc:hdc -> unit = "paint_desktop"

type redraw_window_area =
    Rw_all
  | Rw_rect of rect
  | Rw_rgn of hrgn
external redraw_window : wnd:hwnd -> area:redraw_window_area -> options:redraw_window_option list -> unit = "redraw_window"

external set_bk_color : dc:hdc -> color:color -> rgb = "set_bk_color"

external set_bk_mode : dc:hdc -> mode:background_mode -> background_mode = "set_bk_mode"

external set_rop2 : dc:hdc -> rop:rop2 -> rop2 = "set_rop2"

external set_window_rgn : dc:hwnd -> rgn:hrgn -> redraw:bool -> unit = "set_window_rgn"

external update_window : wnd:hwnd -> unit = "update_window"

external validate_rect : wnd:hwnd -> rect:rect option -> unit = "validate_rect"

external validate_rgn : wnd:hwnd -> rgn:hrgn -> unit = "validate_rgn"

external window_from_dc : dc:hdc -> hwnd = "window_from_dc"

(* Messages *)

let wm_displaychange = 0x007E
type wm_displaychange_handler =
    wnd:hwnd ->
    msg:message_contents ->
    bits_per_pixel:int ->
    screen_size:size ->
    unit message_handler_result
external deliver_wm_displaychange :
    handler:wm_displaychange_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_displaychange"
let on_wm_displaychange = on_message ~id:wm_displaychange ~deliver:deliver_wm_displaychange ~return:return_simple_message

let wm_ncpaint = 0x0085
type wm_ncpaint_handler =
    wnd:hwnd ->
    msg:message_contents ->
    rgn:hrgn ->
    unit message_handler_result
external deliver_wm_ncpaint :
    handler:wm_ncpaint_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_ncpaint"
let on_wm_ncpaint = on_message ~id:wm_ncpaint ~deliver:deliver_wm_ncpaint ~return:return_simple_message

let wm_paint = 0x000F
type wm_paint_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dc:hdc ->
    unit message_handler_result
external deliver_wm_paint :
    handler:wm_paint_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_paint"
let on_wm_paint = on_message ~id:wm_paint ~deliver:deliver_wm_paint ~return:return_simple_message

type wm_print_handler =
    wnd:hwnd ->
    msg:message_contents ->
    dc:hdc ->
    options:print_option list ->
    unit message_handler_result
external deliver_wm_print :
    handler:wm_print_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_print"

let wm_print = 0x0317
let on_wm_print = on_message ~id:wm_print ~deliver:deliver_wm_print ~return:return_simple_message

let wm_printclient = 0x0318
let on_wm_printclient = on_message ~id:wm_printclient ~deliver:deliver_wm_print ~return:return_simple_message

let wm_setredraw = 0x000B
type wm_setredraw_handler =
    wnd:hwnd ->
    msg:message_contents ->
    redraw:bool ->
    unit message_handler_result
external deliver_wm_setredraw :
    handler:wm_setredraw_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_setredraw"
let on_wm_setredraw = on_message ~id:wm_setredraw ~deliver:deliver_wm_setredraw ~return:return_simple_message
