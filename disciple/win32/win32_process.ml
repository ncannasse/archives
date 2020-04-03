(* OCaml-Win32
 * win32_process.ml
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
(* Windows Base Services: Executables: Processes and Threads *)

external attach_thread_input : thread:int32 -> to_thread:int32 -> attach:bool -> unit = "attach_thread_input"

(* ... command_line_to_argv_w *)

external convert_thread_to_fiber : param:int32 -> pointer = "convert_thread_to_fiber"

external create_fiber : stack_size:int -> start_address:pointer -> param:int32 -> pointer = "create_fiber"

external create_process : params:create_process_params -> process_information = "create_process"

external create_process_as_user : token:handle -> params:create_process_params -> process_information = "create_process_as_user"

external create_remote_thread :
    process:handle ->
    security:security_attributes option ->
    stack_size:int ->
    start_address:pointer ->
    param:int32 ->
    create_suspended:bool ->
    (handle * int32) = "create_remote_thread_bytecode" "create_remote_thread_native"

external create_thread :
    security:security_attributes option ->
    stack_size:int ->
    start_address:pointer ->
    param:int32 ->
    create_suspended:bool ->
    (handle * int32) = "create_thread"

external delete_fiber : fiber:pointer -> unit = "delete_fiber"

external exit_process : exit_code:int32 -> unit = "exit_process"

external exit_thread : exit_code:int32 -> unit = "exit_thread"

external get_command_line : unit -> string = "get_command_line"

external get_current_fiber : unit -> pointer = "get_current_fiber"

external get_current_process : unit -> handle = "get_current_process"

external get_current_process_id : unit -> int32 = "get_current_process_id"

external get_current_thread : unit -> handle = "get_current_thread"

external get_current_thread_id : unit -> int32 = "get_current_thread_id"

external get_environment_strings : unit -> string array = "get_environment_strings"

external get_environment_variable : name:string -> string option = "get_environment_variable"

external get_exit_code_process : process:handle -> termination_status = "get_exit_code_process"

external get_exit_code_thread : thread:handle -> termination_status = "get_exit_code_thread"

external get_fiber_data : unit -> int32 = "get_fiber_data"

external get_priority_class : process:handle -> priority_class = "get_priority_class"

(* ... get_process_affinity_mask *)

external get_process_priority_boost : process:handle -> bool = "get_process_priority_boost"

external get_process_shutdown_parameters : unit -> (int * bool) = "get_process_shutdown_parameters"

external get_process_times : process:handle -> process_times = "get_process_times"

(* ... get_process_version *)

external get_process_working_set_size : process:handle -> (int * int) = "get_process_working_set_size"

external get_startup_info : unit -> startupinfo = "get_startup_info"

external get_thread_priority : thread:handle -> thread_priority = "get_thread_priority"

external get_thread_priority_boost : thread:handle -> bool = "get_thread_priority_boost"

external get_thread_times : thread:handle -> process_times = "get_thread_times"

external open_process : access:access_mask -> inherit_handle:bool -> id:int32 -> handle = "open_process"

external resume_thread : thread:handle -> int = "resume_thread"

external set_environment_variable : name:string -> value:string option -> unit = "set_environment_variable"

external set_priority_class : process:handle -> priority_class:priority_class -> unit = "set_priority_class"

(* ... set_process_affinity_mask *)

external set_process_priority_boost : process:handle -> disable:bool -> unit = "set_process_priority_boost"

external set_process_shutdown_parameters : level:int -> noretry:bool -> unit = "set_process_shutdown_parameters"

external set_process_working_set_size : process:handle -> size:(int * int) option -> unit = "set_process_working_set_size"

(* ... set_thread_affinity_mask, set_thread_ideal_processor *)

external set_thread_priority : thread:handle -> priority:thread_priority -> unit = "set_thread_priority"

external set_thread_priority_boost : thread:handle -> disable:bool -> unit = "set_thread_priority_boost"

external sleep : delay:delay -> unit = "sleep"

external sleep_ex : delay:delay -> alertable:bool -> wait_result = "sleep_ex"

external suspend_thread : thread:handle -> int = "suspend_thread"

external switch_to_fiber : fiber:pointer -> unit = "switch_to_fiber"

external switch_to_thread : unit -> bool = "switch_to_thread"

external terminate_process : process:handle -> exit_code:int32 -> unit = "terminate_process"

external terminate_thread : thread:handle -> exit_code:int32 -> unit = "terminate_thread"

external tls_alloc : unit -> int = "tls_alloc"

external tls_free : index:int -> unit = "tls_free"

external tls_get_value : index:int -> int32 = "tls_get_value"

external tls_set_value : index:int -> value:int32 -> unit = "tls_set_value"

external wait_for_input_idle : process:handle -> timeout:delay -> wait_result = "wait_for_input_idle"

external win_exec : command:string -> show:show_window_option -> unit = "win_exec"
