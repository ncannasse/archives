(* OCaml-Win32
 * win32_rect.ml
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
(* Graphics and Multimedia Services: GDI: Rectangles *)

let empty_rect = { left = 0; top = 0; right = 0; bottom = 0 }

external inflate_rect : rect:rect -> amount:size -> rect = "inflate_rect"

external intersect_rect : rect -> rect -> rect option = "intersect_rect"

external is_rect_empty : rect:rect -> bool = "is_rect_empty"

external offset_rect : rect:rect -> offset:point -> rect = "offset_rect"

external pt_in_rect : rect:rect -> pt:point -> bool = "pt_in_rect"

external subtract_rect : rect -> rect -> rect = "subtract_rect"

external union_rect : rect -> rect -> rect = "union_rect"
