(* OCaml-Win32
 * win32_region.ml
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
(* Graphics and Multimedia Services: GDI: Regions *)

external combine_rgn : dst:hrgn -> src1:hrgn -> src2:hrgn -> mode:region_mode -> region_complexity = "combine_rgn"

external create_elliptic_rgn : rect:rect -> hrgn = "create_elliptic_rgn"

external create_polygon_rgn : pts:point array -> mode:poly_fill_mode -> hrgn = "create_polygon_rgn"

external create_poly_polygon_rgn : pts:point array array -> mode:poly_fill_mode -> hrgn = "create_poly_polygon_rgn"

external create_rect_rgn : rect:rect -> hrgn = "create_rect_rgn"

external create_round_rect_rgn : rect:rect -> corner_size:size -> hrgn = "create_round_rect_rgn"

external equal_rgn : hrgn -> hrgn -> bool = "equal_rgn"

external ext_create_region : xform:xform option -> data:rgndata -> hrgn = "ext_create_region"

external fill_rgn : dc:hdc -> rgn:hrgn -> brush:hbrush -> unit = "fill_rgn"

external frame_rgn : dc:hdc -> rgn:hrgn -> brush:hbrush -> size:size -> unit = "frame_rgn"

external get_poly_fill_mode : dc:hdc -> poly_fill_mode = "get_poly_fill_mode"

external get_region_data : rgn:hrgn -> rgndata = "get_region_data"

external get_rgn_box : rgn:hrgn -> rect = "get_rgn_box"

external invert_rgn : dc:hdc -> rgn:hrgn -> unit = "invert_rgn"

external offset_rgn : rgn:hrgn -> offset:point -> region_complexity = "offset_rgn"

external paint_rgn : dc:hdc -> rgn:hrgn -> unit = "paint_rgn"

external pt_in_region : rgn:hrgn -> pt:point -> bool = "pt_in_region"

external rect_in_region : rgn:hrgn -> rect:rect -> bool = "rect_in_region"

external set_poly_fill_mode : dc:hdc -> mode:poly_fill_mode -> poly_fill_mode = "set_poly_fill_mode"

external set_rect_rgn : rgn:hrgn -> rect:rect -> unit = "set_rect_rgn"
