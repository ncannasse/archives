/* OCaml-Win32
 * win32_synch.c
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
 */

#include "win32.h"

/********************************************************************/
/* Windows Base Services: Interprocess Communication: Synchronization */

CAMLprim value cancel_waitable_timer(value timer)
{
    if (!CancelWaitableTimer(Handle_val(timer)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value create_event(value security, value manual_reset, value initial_state, value name)
{
    CAMLparam4(security, manual_reset, initial_state, name);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    HANDLE event;
    CAMLlocal2(val, v);

    if (Is_some(security)) {
        securityattributes_val(security, &sa);
        sap = &sa;
    } else
        sap = NULL;
    event = CreateEvent(sap, Bool_val(manual_reset), Bool_val(initial_state), stringoption_val(name));
    if (event == NULL)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(event);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(GetLastError() == ERROR_ALREADY_EXISTS));
    CAMLreturn(val);
}

CAMLprim value create_mutex(value security, value initial_owner, value name)
{
    CAMLparam3(security, initial_owner, name);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    HANDLE mutex;
    CAMLlocal2(val, v);

    if (Is_some(security)) {
        securityattributes_val(security, &sa);
        sap = &sa;
    } else
        sap = NULL;
    mutex = CreateMutex(sap, Bool_val(initial_owner), stringoption_val(name));
    if (mutex == NULL)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(mutex);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(GetLastError() == ERROR_ALREADY_EXISTS));
    CAMLreturn(val);
}

CAMLprim value create_semaphore(value security, value initial_count, value maximum_count, value name)
{
    CAMLparam4(security, initial_count, maximum_count, name);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    HANDLE sem;
    CAMLlocal2(val, v);

    if (Is_some(security)) {
        securityattributes_val(security, &sa);
        sap = &sa;
    } else
        sap = NULL;
    sem = CreateSemaphore(sap, Int_val(initial_count), Int_val(maximum_count), stringoption_val(name));
    if (sem == NULL)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(sem);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(GetLastError() == ERROR_ALREADY_EXISTS));
    CAMLreturn(val);
}

CAMLprim value create_waitable_timer(value security, value manual_reset, value name)
{
    CAMLparam3(security, manual_reset, name);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    HANDLE timer;
    CAMLlocal2(val, v);

    if (Is_some(security)) {
        securityattributes_val(security, &sa);
        sap = &sa;
    } else
        sap = NULL;
    timer = CreateWaitableTimer(sap, Bool_val(manual_reset), stringoption_val(name));
    if (timer == NULL)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(timer);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_bool(GetLastError() == ERROR_ALREADY_EXISTS));
    CAMLreturn(val);
}

CAMLprim value get_overlapped_result(value file, value overlapped, value wait)
{
    CAMLparam3(file, overlapped, wait);
    HANDLE f;
    LPOVERLAPPED ovl;
    DWORD num;
    BOOL result;
    int code;
    DWORD err;
    CAMLlocal1(val);

    f = Handle_val(file);
    ovl = Overlappedptr_val(overlapped);
    enter_blocking_section();
    result = GetOverlappedResult(f, ovl, &num, Bool_val(wait));
    leave_blocking_section();
    code = 0;
    if (!result) {
        err = GetLastError();
        if (err == ERROR_IO_INCOMPLETE)
            code = 1;
        else
            raise_win32_error(err);
    }
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(code));
    Store_field(val, 1, Val_int(num));
    CAMLreturn(val);
}

CAMLprim value has_overlapped_io_completed(value overlapped)
{
    return Val_bool(HasOverlappedIoCompleted(Overlappedptr_val(overlapped)));
}

CAMLprim value msg_wait_for_multiple_objects(value objects, value wait_all, value timeout, value input_events)
{
    CAMLparam4(objects, wait_all, timeout, input_events);
    HANDLE* handles;
    int num, i;
    DWORD time, events;
    DWORD result;
    CAMLlocal1(val);

    num = Wosize_val(objects);
    if (num != 0) {
        handles = malloc(num * sizeof(HANDLE));
        for (i = 0; i < num; ++i)
            handles[i] = Handle_val(Field(objects, i));
    } else
        handles = NULL;
    time = delay_val(timeout);
    events = flags_val(input_events, queue_status_options);
    enter_blocking_section();
    result = MsgWaitForMultipleObjects(num, handles, Bool_val(wait_all), time, events);
    leave_blocking_section();
    if (handles)
        free(handles);
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, num);
    CAMLreturn(val);
}

CAMLprim value msg_wait_for_multiple_objects_ex(value objects, value timeout, value input_events, value options)
{
    CAMLparam4(objects, timeout, input_events, options);
    HANDLE* handles;
    int num, i;
    DWORD time, events, opt;
    DWORD result;
    CAMLlocal1(val);

    num = Wosize_val(objects);
    if (num != 0) {
        handles = malloc(num * sizeof(HANDLE));
        for (i = 0; i < num; ++i)
            handles[i] = Handle_val(Field(objects, i));
    } else
        handles = NULL;
    time = delay_val(timeout);
    events = flags_val(input_events, queue_status_options);
    opt = flags_val(options, msg_wait_options);
    enter_blocking_section();
    result = MsgWaitForMultipleObjectsEx(num, handles, time, events, opt);
    leave_blocking_section();
    if (handles)
        free(handles);
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, num);
    CAMLreturn(val);
}

CAMLprim value open_event(value access, value inherit_handle, value name)
{
    HANDLE h;

    h = OpenEvent((DWORD)Int64_val(access), Bool_val(inherit_handle), String_val(name));
    if (h == NULL)
        raise_last_error();
    return alloc_handle(h);
}

CAMLprim value open_mutex(value access, value inherit_handle, value name)
{
    HANDLE h;

    h = OpenMutex((DWORD)Int64_val(access), Bool_val(inherit_handle), String_val(name));
    if (h == NULL)
        raise_last_error();
    return alloc_handle(h);
}

CAMLprim value open_semaphore(value access, value inherit_handle, value name)
{
    HANDLE h;

    h = OpenSemaphore((DWORD)Int64_val(access), Bool_val(inherit_handle), String_val(name));
    if (h == NULL)
        raise_last_error();
    return alloc_handle(h);
}

CAMLprim value open_waitable_timer(value access, value inherit_handle, value name)
{
    HANDLE h;

    h = OpenWaitableTimer((DWORD)Int64_val(access), Bool_val(inherit_handle), String_val(name));
    if (h == NULL)
        raise_last_error();
    return alloc_handle(h);
}

CAMLprim value pulse_event(value event)
{
    if (!PulseEvent(Handle_val(event)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value release_mutex(value mutex)
{
    if (!ReleaseMutex(Handle_val(mutex)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value release_semaphore(value semaphore, value release_count)
{
    LONG prev_count;

    if (!ReleaseSemaphore(Handle_val(semaphore), Int_val(release_count), &prev_count))
        raise_last_error();
    return Val_int(prev_count);
}

CAMLprim value reset_event(value event)
{
    if (!ResetEvent(Handle_val(event)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_event(value event)
{
    if (!SetEvent(Handle_val(event)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value signal_object_and_wait(value signal_object, value wait_object, value timeout, value alertable)
{
    CAMLparam4(signal_object, wait_object, timeout, alertable);
    HANDLE sig, wait;
    DWORD time;
    DWORD result;
    CAMLlocal1(val);

    sig = Handle_val(signal_object);
    wait = Handle_val(wait_object);
    time = delay_val(timeout);
    enter_blocking_section();
    result = SignalObjectAndWait(sig, wait, time, Bool_val(alertable));
    leave_blocking_section();
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, 1);
    CAMLreturn(val);
}

CAMLprim value wait_for_multiple_objects(value objects, value wait_all, value timeout)
{
    CAMLparam3(objects, wait_all, timeout);
    HANDLE* handles;
    int num, i;
    DWORD time;
    DWORD result;
    CAMLlocal1(val);

    num = Wosize_val(objects);
    if (num != 0) {
        handles = malloc(num * sizeof(HANDLE));
        for (i = 0; i < num; ++i)
            handles[i] = Handle_val(Field(objects, i));
    } else
        handles = NULL;
    time = delay_val(timeout);
    enter_blocking_section();
    result = WaitForMultipleObjects(
        num,
        handles,
        Bool_val(wait_all),
        time);
    leave_blocking_section();
    if (handles)
        free(handles);
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, num);
    CAMLreturn(val);
}

CAMLprim value wait_for_multiple_objects_ex(value objects, value wait_all, value timeout, value alertable)
{
    CAMLparam4(objects, wait_all, timeout, alertable);
    HANDLE* handles;
    int num, i;
    DWORD time;
    DWORD result;
    CAMLlocal1(val);

    num = Wosize_val(objects);
    if (num != 0) {
        handles = malloc(num * sizeof(HANDLE));
        for (i = 0; i < num; ++i)
            handles[i] = Handle_val(Field(objects, i));
    } else
        handles = NULL;
    time = delay_val(timeout);
    enter_blocking_section();
    result = WaitForMultipleObjectsEx(
        num,
        handles,
        Bool_val(wait_all),
        time,
        Bool_val(alertable));
    leave_blocking_section();
    if (handles)
        free(handles);
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, num);
    CAMLreturn(val);
}

CAMLprim value wait_for_single_object(value obj, value timeout)
{
    CAMLparam2(obj, timeout);
    HANDLE o;
    DWORD time;
    DWORD result;
    CAMLlocal1(val);

    o = Handle_val(obj);
    time = delay_val(timeout);
    enter_blocking_section();
    result = WaitForSingleObject(o, time);
    leave_blocking_section();
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, 1);
    CAMLreturn(val);
}

CAMLprim value wait_for_single_object_ex(value obj, value timeout, value alertable)
{
    CAMLparam3(obj, timeout, alertable);
    HANDLE o;
    DWORD time;
    DWORD result;
    CAMLlocal1(val);

    o = Handle_val(obj);
    time = delay_val(timeout);
    enter_blocking_section();
    result = WaitForSingleObjectEx(o, time, Bool_val(alertable));
    leave_blocking_section();
    if (result == WAIT_FAILED)
        raise_last_error();
    val = alloc_waitresult(result, 1);
    CAMLreturn(val);
}
