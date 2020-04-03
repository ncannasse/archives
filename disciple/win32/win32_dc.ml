(* OCaml-Win32
 * win32_dc.ml
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
(* Graphics and Multimedia Services: GDI: Device Contexts *)

external cancel_dc : dc:hdc -> unit = "cancel_dc"

(* ... change_display_settings, change_display_settings_ex *)

external create_compatible_dc : dc:hdc -> hdc = "create_compatible_dc"

(* ... create_dc *)

external delete_dc : dc:hdc -> unit = "delete_dc"

external delete_object : obj:hgdiobj -> unit = "delete_object"

(* ... draw_escape *)

(* ... enum_display_devices, enum_display_settings *)

(* enum_objects *)
external enum_brushes : dc:hdc -> func:(logbrush:logbrush -> bool) -> unit = "enum_brushes"
external enum_pens : dc:hdc -> func:(logpen:logpen -> bool) -> unit = "enum_pens"

external get_current_object : dc:hdc -> typ:object_type -> hgdiobj = "get_current_object"

external get_dc : wnd:hwnd -> hdc = "get_dc"

external get_dc_ex : wnd:hwnd -> clip:hrgn -> options:get_dc_ex_option list -> hdc = "get_dc_ex"

external get_dc_org_ex : dc:hdc -> point = "get_dc_org_ex"

(* ... get_device_caps *)

(* ... get_object *)

external get_object_type : obj:hgdiobj -> object_type = "get_object_type"

external get_stock_object : stock_object:stock_object -> hgdiobj = "get_stock_object"

external release_dc : wnd:hwnd -> dc:hdc -> bool = "release_dc"

external restore_dc : dc:hdc -> inst:int -> unit = "restore_dc"

external save_dc : dc:hdc -> int = "save_dc"

external select_object : dc:hdc -> obj:hgdiobj -> hgdiobj = "select_object"

external select_region : dc:hdc -> rgn:hrgn -> region_complexity = "select_region"

(* Messages *)

let wm_devmodechange = 0x001B
type wm_devmodechange_handler =
    wnd:hwnd ->
    msg:message_contents ->
    device_name:string ->
    unit message_handler_result
external deliver_wm_devmodechange :
    handler:wm_devmodechange_handler ->
    wnd:hwnd ->
    msg:message_contents ->
    unit message_handler_result = "deliver_wm_devmodechange"
let on_wm_devmodechange = on_message ~id:wm_devmodechange ~deliver:deliver_wm_devmodechange ~return:return_simple_message
