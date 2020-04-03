(* OCaml-Win32
 * win32_line.ml
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
(* Graphics and Multimedia Services: GDI: Lines and Curves *)

external angle_arc : dc:hdc -> center:point -> radius:int -> start_angle:float -> sweep_angle:float -> unit = "angle_arc"

external arc : dc:hdc -> rect:rect -> start_pt:point -> end_pt:point -> unit = "arc"

external arc_to : dc:hdc -> rect:rect -> start_pt:point -> end_pt:point -> unit = "arc_to"

external get_arc_direction : dc:hdc -> arc_direction = "get_arc_direction"

external line_dda : start_pt:point -> end_pt:point -> func:(point -> unit) -> unit = "line_dda"

external line_to : dc:hdc -> pt:point -> unit = "line_to"

external move_to : dc:hdc -> pt:point -> point = "move_to"

external poly_bezier : dc:hdc -> pts:point array -> unit = "poly_bezier"

external poly_bezier_to : dc:hdc -> pts:point array -> unit = "poly_bezier_to"

(* ... poly_draw *)

external polyline : dc:hdc -> pts:point array -> unit = "polyline"

external polyline_to : dc:hdc -> pts:point array -> unit = "polyline_to"

external poly_polyline : dc:hdc -> pts:point array array -> unit = "poly_polyline"

external set_arc_direction : dc:hdc -> direction:arc_direction -> arc_direction = "set_arc_direction"
