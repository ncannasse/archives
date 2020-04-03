(* OCaml-Win32
 * win32_string.ml
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
(* Windows Base Services: General Library: String Manipulation *)

(* ... char_*, compare_string, fold_string, get_string_type_*, is_char_*, oem_* *)

(* ... language parameter *)
type format_message_source =
    Fms_string of string
  | Fms_hmodule of hinstance * int
  | Fms_system of int
type format_message_width =
    Fmw_none
  | Fmw_width of int
  | Fmw_max_width
type format_message_arg =
    Fma_int of int
  | Fma_char of char
  | Fma_string of string
external format_message :
    source:format_message_source ->
    width:format_message_width ->
    args:format_message_arg array option ->
    string = "format_message"

external load_string : inst:hinstance -> id:int -> max_length:int -> string = "load_string"
