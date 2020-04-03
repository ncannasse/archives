(* OCaml-Win32
 * win32_brush.ml
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
(* Graphics and Multimedia Services: GDI: Brushes *)

external create_brush_indirect : logbrush:logbrush -> hbrush = "create_brush_indirect"

external create_dib_pattern_brush_pt : dib:raw -> color_typ:dib_color_type -> hbrush = "create_dib_pattern_brush_pt"

external create_hatch_brush : style:hatch_style -> color:color -> hbrush = "create_hatch_brush"

external create_pattern_brush : pattern:hbitmap -> hbrush = "create_pattern_brush"

external create_solid_brush : color:color -> hbrush = "create_solid_brush"

external get_brush_org_ex : dc:hdc -> point = "get_brush_org_ex"

external get_sys_color_brush : color:sys_color -> hbrush = "get_sys_color_brush"

external pat_blt : dc:hdc -> pt:point -> size:size -> rop:rop3 -> unit = "pat_blt"

external set_brush_org_ex : dc:hdc -> pt:point -> point = "set_brush_org_ex"
