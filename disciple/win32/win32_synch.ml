(* OCaml-Win32
 * win32_synch.ml
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
(* Windows Base Services: Interprocess Communication: Synchronization *)

external cancel_waitable_timer : timer:handle -> unit = "cancel_waitable_timer"

external create_event :
    security:security_attributes option ->
    manual_reset:bool ->
    initial_state:bool ->
    name:string option ->
    (handle * bool) = "create_event"

external create_mutex :
    security:security_attributes option ->
    initial_owner:bool ->
    name:string option ->
    (handle * bool) = "create_mutex"

external create_semaphore :
    security:security_attributes option ->
    initial_count:int ->
    maximum_count:int ->
    name:string option ->
    (handle * bool) = "create_semaphore"

external create_waitable_timer :
    security:security_attributes option ->
    manual_reset:bool ->
    name:string option ->
    (handle * bool) = "create_waitable_timer"

(* ... delete_critical_section, enter_critical_section *)

type overlapped_result =
    Ovl_completed
  | Ovl_incomplete
external get_overlapped_result :
    file:handle ->
    overlapped:overlapped ->
    wait:bool ->
    (overlapped_result * int) = "get_overlapped_result"

external has_overlapped_io_completed : overlapped:overlapped -> bool = "has_overlapped_io_completed"

(* ... initialize_critical_section, initialize_critical_section_and_spin_count *)

(* ... interlocked_* *)

(* ... leave_critical_section *)

external msg_wait_for_multiple_objects :
    objects:handle array ->
    wait_all:bool ->
    timeout:delay ->
    input_events:queue_status_option list ->
    wait_result = "msg_wait_for_multiple_objects"

external msg_wait_for_multiple_objects_ex :
    objects:handle array ->
    timeout:delay ->
    input_events:queue_status_option list ->
    options:msg_wait_option list ->
    wait_result = "msg_wait_for_multiple_objects_ex"

external open_event :
    access:access_mask ->
    inherit_handle:bool ->
    name:string ->
    handle = "open_event"

external open_mutex :
    access:access_mask ->
    inherit_handle:bool ->
    name:string ->
    handle = "open_mutex"

external open_semaphore :
    access:access_mask ->
    inherit_handle:bool ->
    name:string ->
    handle = "open_semaphore"

external open_waitable_timer :
    access:access_mask ->
    inherit_handle:bool ->
    name:string ->
    handle = "open_waitable_timer"

external pulse_event : event:handle -> unit = "pulse_event"

(* ... queue_user_apc *)

external release_mutex : mutex:handle -> unit = "release_mutex"

external release_semaphore : semaphore:handle -> release_count:int -> int = "release_semaphore"

external reset_event : event:handle -> unit = "reset_event"

(* ... set_critical_section_spin_count *)

external set_event : event:handle -> unit = "set_event"

(* ... set_waitable_timer *)

external signal_object_and_wait :
    signal_object:handle ->
    wait_object:handle ->
    timeout:delay ->
    alertable:bool ->
    wait_result = "signal_object_and_wait"

(* ... try_enter_critical_section *)

external wait_for_multiple_objects :
    objects:handle array ->
    wait_all:bool ->
    timeout:delay ->
    wait_result = "wait_for_multiple_objects"

external wait_for_multiple_objects_ex :
    objects:handle array ->
    wait_all:bool ->
    timeout:delay ->
    alertable:bool ->
    wait_result = "wait_for_multiple_objects_ex"

external wait_for_single_object :
    obj:handle ->
    timeout:delay ->
    wait_result = "wait_for_single_object"

external wait_for_single_object_ex :
    obj:handle ->
    timeout:delay ->
    alertable:bool ->
    wait_result = "wait_for_single_object_ex"
