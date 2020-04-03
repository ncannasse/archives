(* OCaml-Win32
 * win32_time.ml
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
(* Windows Base Services: General Library: Time *)

external compare_file_time : filetime -> filetime -> int = "compare_file_time"

external dos_date_time_to_file_time : date:int -> time:int -> filetime = "dos_date_time_to_file_time"

external file_time_to_dos_date_time : filetime -> (int * int) = "file_time_to_dos_date_time"

external file_time_to_local_file_time : filetime -> filetime = "file_time_to_local_file_time"

external file_time_to_system_time : filetime -> systemtime = "file_time_to_system_time"

external get_file_time : file:handle -> file_times = "get_file_time"

external get_local_time : unit -> systemtime = "get_local_time"

external get_system_time : unit -> systemtime = "get_system_time"

external get_system_time_adjustment : unit -> (int * int * bool) = "get_system_time_adjustment"

external get_system_time_as_file_time : unit -> filetime = "get_system_time_as_file_time"

external get_tick_count : unit -> int = "get_tick_count"

(* ... get_time_zone_information *)

external local_file_time_to_file_time : filetime -> filetime = "local_file_time_to_file_time"

external set_file_time :
    file:handle ->
    creation:filetime option ->
    last_access:filetime option ->
    last_write:filetime option ->
    unit = "set_file_time"

external set_local_time : time:systemtime -> unit = "set_local_time"

external set_system_time : time:systemtime -> unit = "set_system_time"

external set_system_time_adjustment : adjustment:int -> disabled:bool -> unit = "set_system_time_adjustment"

(* ... set_time_zone_information *)

external system_time_to_file_time : systemtime -> filetime = "system_time_to_file_time"

(* ... system_time_to_tz_specific_local_time *)
