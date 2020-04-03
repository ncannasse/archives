(* OCaml-Win32
 * win32_coord.ml
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
(* Graphics and Multimedia Services: GDI: Coordinate Spaces and Transformations *)

external client_to_screen : wnd:hwnd -> pt:point -> point = "client_to_screen"

external combine_transform : xform -> xform -> xform = "combine_transform"

external dp_to_lp : dc:hdc -> pt:point -> point = "dp_to_lp"

external dp_to_lp_multi : dc:hdc -> pts:point array -> point array = "dp_to_lp_multi"

external get_current_position : dc:hdc -> point = "get_current_position"

external get_graphics_mode : dc:hdc -> graphics_mode = "get_graphics_mode"

external get_map_mode : dc:hdc -> map_mode = "get_map_mode"

external get_viewport_ext : dc:hdc -> size = "get_viewport_ext"

external get_viewport_org : dc:hdc -> point = "get_viewport_org"

external get_window_ext : dc:hdc -> size = "get_window_ext"

external get_window_org : dc:hdc -> point = "get_window_org"

external get_world_transform : dc:hdc -> xform = "get_world_transform"

external lp_to_dp : dc:hdc -> pt:point -> point = "lp_to_dp"

external lp_to_dp_multi : dc:hdc -> pts:point array -> point array = "lp_to_dp_multi"

external map_window_point : from_wnd:hwnd -> to_wnd:hwnd -> point -> point = "map_window_point"

external map_window_points : from_wnd:hwnd -> to_wnd:hwnd -> pts:point array -> point array = "map_window_points"

external map_window_rect : from_wnd:hwnd -> to_wnd:hwnd -> rect:rect -> rect = "map_window_rect"

type world_transform_modifier =
    Mwt_identity
  | Mwt_leftmultiply of xform
  | Mwt_rightmultiply of xform
external modify_world_transform : dc:hdc -> modifier:world_transform_modifier -> unit = "modify_world_transform"

external offset_viewport_org : dc:hdc -> offset:point -> point = "offset_viewport_org"

external offset_window_org : dc:hdc -> offset:point -> point = "offset_window_org"

external scale_viewport_ext : dc:hdc -> x_num:int -> x_denom:int -> y_num:int -> y_denom:int -> size = "scale_viewport_ext"

external scale_window_ext : dc:hdc -> x_num:int -> x_denom:int -> y_num:int -> y_denom:int -> size = "scale_window_ext"

external screen_to_client : wnd:hwnd -> pt:point -> point = "screen_to_client"

external set_graphics_mode : dc:hdc -> mode:graphics_mode -> graphics_mode = "set_graphics_mode"

external set_map_mode : dc:hdc -> mode:map_mode -> map_mode = "set_map_mode"

external set_viewport_ext : dc:hdc -> size:size -> size = "set_viewport_ext"

external set_viewport_org : dc:hdc -> pt:point -> point = "set_viewport_org"

external set_window_ext : dc:hdc -> size:size -> size = "set_window_ext"

external set_window_org : dc:hdc -> pt:point -> point = "set_window_org"

external set_world_transform : dc:hdc -> xform:xform -> unit = "set_world_transform"
