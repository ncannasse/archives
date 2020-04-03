(* OCaml-Win32
 * win32_color.ml
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
(* Graphics and Multimedia Services: GDI: Colors *)

(* ... animate_palette *)

external create_halftone_palette : dc:hdc -> hpalette = "create_halftone_palette"

(* ... create_palette *)

(* ... get_color_adjustment *)

external get_nearest_color : dc:hdc -> color:color -> rgb = "get_nearest_color"

external get_nearest_palette_index : palette:hpalette -> color:color -> int = "get_nearest_palette_index"

(* ... get_palette_entries, get_system_palette_entries *)

external get_system_palette_use : dc:hdc -> system_palette_use = "get_system_palette_use"

external realize_palette : dc:hdc -> int = "realize_palette"

external resize_palette : palette:hpalette -> num_entries:int -> unit = "resize_palette"

external select_palette : dc:hdc -> palette:hpalette -> force_background:bool -> hpalette = "select_palette"

(* ... set_color_adjustment *)

(* ... set_palette_entries *)

external set_system_palette_use : dc:hdc -> usage:system_palette_use -> system_palette_use = "set_system_palette_use"

external unrealize_object : obj:hgdiobj -> unit = "unrealize_object"

external update_colors : dc:hdc -> unit = "update_colors"

(* Messages *)

type wm_palettechange_handler =
    wnd:hwnd ->
    msg:message_contents ->
    source_wnd:hwnd ->
    unit message_handler_result
external deliver_wm_palettechange :
    handler:wm_palettechange_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_palettechange"

let wm_palettechanged = 0x0311
let on_wm_palettechanged = on_message ~id:wm_palettechanged ~deliver:deliver_wm_palettechange ~return:return_simple_message

let wm_paletteischanging = 0x0310
let on_wm_paletteischanging = on_message ~id:wm_paletteischanging ~deliver:deliver_wm_palettechange ~return:return_simple_message

let wm_querynewpalette = 0x030F
type wm_querynewpalette_handler = bool plain_message_handler
external return_wm_querynewpalette : result:bool -> message_result = "return_wm_querynewpalette"
let on_wm_querynewpalette = on_message ~id:wm_querynewpalette ~deliver:deliver_plain_message ~return:return_wm_querynewpalette

let wm_syscolorchange = 0x0015
let on_wm_syscolorchange = on_simple_message ~id:wm_syscolorchange
