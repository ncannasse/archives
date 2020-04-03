(* OCaml-Win32
 * win32_file.ml
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
(* Windows Base Services: Files and I/O: File I/O *)

external are_file_apis_ansi : unit -> bool = "are_file_apis_ansi"

external cancel_io : file:handle -> unit = "cancel_io"

external copy_file : from_name:string -> to_name:string -> fail_if_exists:bool -> unit = "copy_file"

(* ... copy_file_ex *)

external create_directory : name:string -> security:security_attributes option -> unit = "create_directory"

external create_directory_ex : template:string -> name:string -> security:security_attributes option -> unit = "create_directory_ex"

external create_file :
    name:string ->
    access:access_mask ->
    share:file_share_option list ->
    security:security_attributes option ->
    creation:file_create_option ->
    attributes:file_attribute list ->
    flags:file_flag list ->
    sqos:sqos_option list ->
    template:handle ->
    handle = "create_file_bytecode" "create_file_native"

(* ... create_io_completion_port, define_dos_device *)

external delete_file : name:string -> unit = "delete_file"

external find_files : name:string -> func:(data:find_data -> bool) -> unit = "find_files"

external find_close_change_notification : handle:handle -> unit = "find_close_change_notification"

external find_first_change_notification :
    path:string ->
    watch_subtree:bool ->
    filter:file_change_type list ->
    handle = "find_first_change_notification"

external find_next_change_notification : handle:handle -> unit = "find_next_change_notification"

(* ... find_first_file_ex *)

external flush_file_buffers : file:handle -> unit = "flush_file_buffers"

external get_binary_type : name:string -> binary_type list = "get_binary_type"

external get_current_directory : unit -> string = "get_current_directory"

type disk_free_space = {
    dfs_free_bytes_available : int64;
    dfs_total_bytes : int64;
    dfs_free_bytes : int64 }
external get_disk_free_space : path:string option -> disk_free_space = "get_disk_free_space"

external get_drive_type : root:string option -> drive_type = "get_drive_type"

external get_file_attributes : name:string -> file_attribute list = "get_file_attributes"

(* ... get_file_attributes_ex *)

external get_file_information_by_handle : file:handle -> by_handle_file_information = "get_file_information_by_handle"

external get_file_size : file:handle -> int64 = "get_file_size"

external get_file_type : file:handle -> file_type = "get_file_type"

external get_full_path_name : name:string -> (string * int) = "get_full_path_name"

external get_logical_drives : unit -> bool array = "get_logical_drives"

external get_logical_drive_strings : unit -> string array = "get_logical_drive_strings"

(* ... get_queued_completion_status *)

external get_short_path_name : name:string -> string = "get_short_path_name"

external get_temp_file_name : path:string -> prefix:string -> unique:int32 -> (string * int32) = "get_temp_file_name"

external get_temp_path : unit -> string = "get_temp_path"

external lock_file : file:handle -> offset:int64 -> length:int64 -> unit = "lock_file"

(* ... lock_file_ex *)

external move_file : from_name:string -> to_name:string -> unit = "move_file"

external move_file_ex : from_name:string -> to_name:string option -> options:move_file_option list -> unit = "move_file_ex"

(* ... post_queued_completion_status, query_dos_device, read_directory_changes_w *)

external read_file :
    file:handle ->
    buffer:sized_raw ->
    (error_code * int) = "read_file"
external read_file_overlapped :
    file:handle ->
    ptr:pointer ->
    size:int ->
    overlapped:overlapped ->
    (error_code * int) = "read_file_overlapped"

(* ... read_file_ex, read_file_scatter *)

external remove_directory : name:string -> unit = "remove_directory"

external search_path :
    path:string option ->
    name:string ->
    extension:string option ->
    (string * int) = "search_path"

external set_current_directory : name:string -> unit = "set_current_directory"

external set_end_of_file : file:handle -> unit = "set_end_of_file"

external set_file_apis_to_ansi : unit -> unit = "set_file_apis_to_ansi"

external set_file_apis_to_oem : unit -> unit = "set_file_apis_to_oem"

external set_file_attributes : name:string -> attributes:file_attribute list -> unit = "set_file_attributes"

external set_file_pointer : file:handle -> distance:int64 -> seek_method:seek_method -> int64 = "set_file_pointer"

external set_handle_count : count:int -> int = "set_handle_count"

external set_volume_label : root:string option -> label:string option -> unit = "set_volume_label"

external unlock_file : file:handle -> offset:int64 -> length:int64 -> unit = "unlock_file"

(* ... unlock_file_ex *)

external write_file :
    file:handle ->
    buffer:sized_raw ->
    (error_code * int) = "write_file"
external write_file_overlapped :
    file:handle ->
    ptr:pointer ->
    size:int ->
    overlapped:overlapped ->
    (error_code * int) = "write_file_overlapped"

(* ... write_file_ex, write_file_gather *)

(* ... *)
