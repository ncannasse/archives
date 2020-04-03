(* OCaml-Win32
 * win32_text.ml
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
(* Graphics and Multimedia Services: GDI: Fonts and Text *)

external add_font_resource : filename:string -> int = "add_font_resource"

external create_font : logfont:logfont -> hfont = "create_font"

external create_scalable_font_resource :
    hidden:bool ->
    font_res:string ->
    font_file:string ->
    current_path:string ->
    unit = "create_scalable_font_resource"

type draw_text_option =
    Dt_bottom
  | Dt_calcrect
  | Dt_center
  | Dt_editcontrol
  | Dt_end_ellipsis
  | Dt_expandtabs
  | Dt_externalleading
  | Dt_internal
  | Dt_left
  | Dt_margins of int * int
  | Dt_modifystring
  | Dt_noclip
  | Dt_noprefix
  | Dt_path_ellipsis
  | Dt_right
  | Dt_rtlreading
  | Dt_singleline
  | Dt_tabstop of int
  | Dt_top
  | Dt_vcenter
  | Dt_wordbreak
  | Dt_word_ellipsis
type draw_text_result = {
    dtr_height : int;
    dtr_string : string;
    dtr_rect : rect;
    dtr_length_drawn : int }
external draw_text : dc:hdc -> text:text -> rect:rect -> options:draw_text_option list -> draw_text_result = "draw_text"

type enum_font_info = {
    efi_full_name : string;
    efi_style : string;
    efi_script : string;
    efi_type : font_type list;
    efi_ntm : newtextmetric option }
external enum_font_families :
    dc:hdc ->
    logfont:logfont ->
    func:(logfont:logfont -> metric:textmetric -> info:enum_font_info -> bool) ->
    unit = "enum_font_families"

external ext_text_out :
    dc:hdc ->
    pt:point ->
    options:ext_text_out_option list ->
    rect:rect option ->
    text:text ->
    distances:int array option ->
    unit = "ext_text_out_bytecode" "ext_text_out_native"

external get_aspect_ratio_filter : dc:hdc -> size = "get_aspect_ratio_filter"

external get_char_abc_widths : dc:hdc -> first:char -> last:char -> abc array = "get_char_abc_widths"

external get_char_abc_widths_float : dc:hdc -> first:char -> last:char -> abcfloat array = "get_char_abc_widths_float"

(* ... get_character_placement *)

external get_char_width : dc:hdc -> first:char -> last:char -> int array = "get_char_width"

external get_char_width_float : dc:hdc -> first:char -> last:char -> float array = "get_char_width_float"

external get_font_data : dc:hdc -> table:int -> offset:int -> buffer = "get_font_data"

(* ... get_font_language_info *)

(* ... get_glyph_outline *)

external get_kerning_pairs : dc:hdc -> kerningpair array = "get_kerning_pairs"

(* ... get_outline_text_metrics *)

(* ... get_rasterizer_caps *)

external get_tabbed_text_extent : dc:hdc -> text:text -> tab_stops:int array -> size = "get_tabbed_text_extent"

external get_text_align : dc:hdc -> text_alignment = "get_text_align"

external get_text_character_extra : dc:hdc -> int = "get_text_character_extra"

external get_text_color : dc:hdc -> rgb = "get_text_color"

external get_text_extent_ex_point : dc:hdc -> text:text -> max_extent:int -> (size * int array) = "get_text_extent_ex_point"

external get_text_extent_point : dc:hdc -> text:text -> size = "get_text_extent_point"

external get_text_face : dc:hdc -> string = "get_text_face"

external get_text_metrics : dc:hdc -> textmetric = "get_text_metrics"

(* ... poly_text_out *)

external remove_font_resource : filename:string -> unit = "remove_font_resource"

external set_mapper_flags : dc:hdc -> match_aspect_ratio:bool -> bool = "set_mapper_flags"

external set_text_align : dc:hdc -> alignment:text_alignment -> text_alignment = "set_text_align"

external set_text_character_extra : dc:hdc -> extra:int -> int = "set_text_character_extra"

external set_text_color : dc:hdc -> color:color -> rgb = "set_text_color"

external set_text_justification : dc:hdc -> break_extra:int -> break_count:int -> unit = "set_text_justification"

external tabbed_text_out : dc:hdc -> pt:point -> text:text -> tab_stops:int array -> tab_origin:int -> size = "tabbed_text_out"

external text_out : dc:hdc -> pt:point -> text:text -> unit = "text_out"

(* Messages *)

let wm_fontchange = 0x001D
let on_wm_fontchange = on_simple_message ~id:wm_fontchange
let pack_wm_fontchange = pack_simple_message ~id:wm_fontchange
let unpack_wm_fontchange = ignore
let issue_wm_fontchange ~sender = unpack_wm_fontchange (sender ~msg:pack_wm_fontchange)
