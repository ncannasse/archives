(* OCaml-Win32
 * win32_filledshape.ml
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
(* Graphics and Multimedia Services: GDI: Filled Shapes *)

external chord : dc:hdc -> rect:rect -> point -> point -> unit = "chord"

external ellipse : dc:hdc -> rect:rect -> unit = "ellipse"

type rect_fill =
    Rf_sys_color of sys_color
  | Rf_brush of hbrush
external fill_rect : dc:hdc -> rect:rect -> filler:rect_fill -> unit = "fill_rect"

external frame_rect : dc:hdc -> rect:rect -> brush:hbrush -> unit = "frame_rect"

external invert_rect : dc:hdc -> rect:rect -> unit = "invert_rect"

external pie : dc:hdc -> rect:rect -> point -> point -> unit = "pie"

external polygon : dc:hdc -> pts:point array -> unit = "polygon"

external poly_polygon : dc:hdc -> pts:point array array -> unit = "poly_polygon"

external rectangle : dc:hdc -> rect:rect -> unit = "rectangle"

external round_rect : dc:hdc -> rect:rect -> corner_size:size -> unit = "round_rect"
