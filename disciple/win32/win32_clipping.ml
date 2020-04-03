(* OCaml-Win32
 * win32_clipping.ml
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
(* Graphics and Multimedia Services: GDI: Clipping *)

external exclude_clip_rect : dc:hdc -> rect:rect -> region_complexity = "exclude_clip_rect"

external ext_select_clip_rgn : dc:hdc -> rgn:hrgn -> mode:region_mode -> region_complexity = "ext_select_clip_rgn"

external get_clip_box : dc:hdc -> rect = "get_clip_box"

external get_clip_rgn : dc:hdc -> rgn:hrgn -> bool = "get_clip_rgn"

external get_meta_rgn : dc:hdc -> rgn:hrgn -> unit = "get_meta_rgn"

external intersect_clip_rect : dc:hdc -> rect:rect -> region_complexity = "intersect_clip_rect"

external offset_clip_rgn : dc:hdc -> offset:point -> region_complexity = "offset_clip_rgn"

external pt_visible : dc:hdc -> pt:point -> bool = "pt_visible"

external rect_visible : dc:hdc -> rect:rect -> bool = "rect_visible"

external select_clip_path : dc:hdc -> mode:region_mode -> unit = "select_clip_path"

external select_clip_rgn : dc:hdc -> rgn:hrgn -> region_complexity = "select_clip_rgn"

external set_meta_rgn : dc:hdc -> region_complexity = "set_meta_rgn"
