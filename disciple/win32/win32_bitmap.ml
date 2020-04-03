(* OCaml-Win32
 * win32_bitmap.ml
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
(* Graphics and Multimedia Services: GDI: Bitmaps *)

external bit_blt :
    dst_dc:hdc ->
    dst_pt:point ->
    size:size ->
    src_dc:hdc ->
    src_pt:point ->
    rop:rop3 ->
    unit = "bit_blt_bytecode" "bit_blt_native"

external create_bitmap : size:size -> planes:int -> bits_per_pel:int -> bits:raw -> hbitmap = "create_bitmap"

external create_bitmap_indirect : bitmap:bitmap -> hbitmap = "create_bitmap_indirect"

external create_compatible_bitmap : dc:hdc -> size:size -> hbitmap = "create_compatible_bitmap"

(* ... create_di_bitmap, create_dib_section *)

external flood_fill : dc:hdc -> pt:point -> color:color -> typ:flood_fill_type -> unit = "flood_fill"

external get_bitmap_dimension : bitmap:hbitmap -> size = "get_bitmap_dimension"

(* ... get_dib_color_table, get_di_bits *)

external get_pixel : dc:hdc -> pt:point -> rgb = "get_pixel"

external get_stretch_blt_mode : dc:hdc -> stretch_blt_mode = "get_stretch_blt_mode"

external load_bitmap : inst:hinstance -> name:resource_name -> hbitmap = "load_bitmap"

external mask_blt :
    dst_dc:hdc ->
    dst_pt:point ->
    size:size ->
    src_dc:hdc ->
    src_pt:point ->
    mask:hbitmap ->
    mask_pt:point ->
    fg_rop:rop3 ->
    bg_rop:rop3 ->
    unit = "mask_blt_bytecode" "mask_blt_native"

external plg_blt :
    dst_dc:hdc ->
    dst_pts:(int * int * int) ->
    src_dc:hdc ->
    src_pt:point ->
    size:size ->
    mask:(hbitmap * point) option ->
    unit = "plg_blt_bytecode" "plg_blt_native"

external set_bitmap_dimension : bitmap:hbitmap -> size:size -> size = "set_bitmap_dimension"

(* ... set_dib_color_table, set_di_bits, set_di_bits_to_device *)

external set_pixel : dc:hdc -> pt:point -> color:color -> rgb = "set_pixel"

external set_stretch_blt_mode : dc:hdc -> mode:stretch_blt_mode -> stretch_blt_mode = "set_stretch_blt_mode"

external stretch_blt :
    dst_dc:hdc ->
    dst_pt:point ->
    dst_size:size ->
    src_dc:hdc ->
    src_pt:point ->
    src_size:size ->
    rop:rop3 ->
    unit = "stretch_blt_bytecode" "stretch_blt_native"

(* ... stretch_di_bits *)
