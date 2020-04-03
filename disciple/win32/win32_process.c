/* OCaml-Win32
 * win32_process.c
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
/* Windows Base Services: Executables: Processes and Threads */

CAMLprim value attach_thread_input(value thread, value to_thread, value attach)
{
    if (!AttachThreadInput(Int32_val(thread), Int32_val(to_thread), Bool_val(attach)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value convert_thread_to_fiber(value parameter)
{
    LPVOID result;

    result = ConvertThreadToFiber((LPVOID)Int32_val(parameter));
    if (!result)
        raise_last_error();
    return alloc_pointer(result);
}

CAMLprim value create_fiber(value stack_size, value start_address, value parameter)
{
    LPVOID result;

    result = CreateFiber(
        Int_val(stack_size),
        (LPFIBER_START_ROUTINE)Pointer_val(start_address),
        (LPVOID)Int32_val(parameter));
    if (!result)
        raise_last_error();
    return alloc_pointer(result);
}

CAMLprim value create_process(value params)
{
    CAMLparam1(params);
    create_process_params* cpp;
    PROCESS_INFORMATION pi;
    BOOL result;
    CAMLlocal1(val);

    cpp = new_create_process_params(params);
    enter_blocking_section();
    result = CreateProcess(
        cpp->application_name,
        cpp->command_line,
        cpp->process_attributes,
        cpp->thread_attributes,
        cpp->inherit_handles,
        cpp->creation_flags,
        cpp->environment,
        cpp->current_directory,
        cpp->startup_info,
        &pi);
    leave_blocking_section();
    free_create_process_params(cpp);
    if (!result)
        raise_last_error();
    val = alloc_processinformation(&pi);
    CAMLreturn(val);
}

CAMLprim value create_process_as_user(value token, value params)
{
    CAMLparam2(token, params);
    HANDLE htoken;
    create_process_params* cpp;
    PROCESS_INFORMATION pi;
    BOOL result;
    CAMLlocal1(val);

    htoken = Handle_val(token);
    cpp = new_create_process_params(params);
    enter_blocking_section();
    result = CreateProcessAsUser(
        htoken,
        cpp->application_name,
        cpp->command_line,
        cpp->process_attributes,
        cpp->thread_attributes,
        cpp->inherit_handles,
        cpp->creation_flags,
        cpp->environment,
        cpp->current_directory,
        cpp->startup_info,
        &pi);
    leave_blocking_section();
    free_create_process_params(cpp);
    if (!result)
        raise_last_error();
    val = alloc_processinformation(&pi);
    CAMLreturn(val);
}

CAMLprim value create_remote_thread_native(value process, value security_attributes, value stack_size, value start_address, value param, value create_suspended)
{
    CAMLparam5(process, security_attributes, stack_size, start_address, param);
    CAMLxparam1(create_suspended);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    DWORD thread_id;
    HANDLE result;
    CAMLlocal2(val, v);

    if (Is_some(security_attributes)) {
        securityattributes_val(Value_of(security_attributes), &sa);
        sap = &sa;
    } else
        sap = NULL;
    result = CreateRemoteThread(
        Handle_val(process),
        sap,
        Int_val(stack_size),
        (LPTHREAD_START_ROUTINE)Pointer_val(start_address),
        (LPVOID)Int32_val(param),
        (Bool_val(create_suspended) ? CREATE_SUSPENDED : 0),
        &thread_id);
    if (!result)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(result);
    Store_field(val, 0, v);
    v = copy_int32(thread_id);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value create_remote_thread_bytecode(value* argv, int argn)
{
    return create_remote_thread_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value create_thread(value security_attributes, value stack_size, value start_address, value param, value create_suspended)
{
    CAMLparam5(security_attributes, stack_size, start_address, param, create_suspended);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    DWORD thread_id;
    HANDLE result;
    CAMLlocal2(val, v);

    if (Is_some(security_attributes)) {
        securityattributes_val(Value_of(security_attributes), &sa);
        sap = &sa;
    } else
        sap = NULL;
    result = CreateThread(
        sap,
        Int_val(stack_size),
        (LPTHREAD_START_ROUTINE)Pointer_val(start_address),
        (LPVOID)Int32_val(param),
        (Bool_val(create_suspended) ? CREATE_SUSPENDED : 0),
        &thread_id);
    if (!result)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(result);
    Store_field(val, 0, v);
    v = copy_int32(thread_id);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value delete_fiber(value fiber)
{
    DeleteFiber(Pointer_val(fiber));
    return Val_unit;
}

CAMLprim value exit_process(value exit_code)
{
    ExitProcess(Int32_val(exit_code));
    return Val_unit;
}

CAMLprim value exit_thread(value exit_code)
{
    ExitThread(Int32_val(exit_code));
    return Val_unit;
}

CAMLprim value get_command_line(value unit)
{
    return copy_string(GetCommandLine());
}

CAMLprim value get_current_fiber(value unit)
{
    return alloc_pointer(GetCurrentFiber());
}

CAMLprim value get_current_process(value unit)
{
    return alloc_handle(GetCurrentProcess());
}

CAMLprim value get_current_process_id(value unit)
{
    return copy_int32(GetCurrentProcessId());
}

CAMLprim value get_current_thread(value unit)
{
    return alloc_handle(GetCurrentThread());
}

CAMLprim value get_current_thread_id(value unit)
{
    return copy_int32(GetCurrentThreadId());
}

CAMLprim value get_environment_strings(value unit)
{
    CAMLparam1(unit);
    LPSTR env;
    char* p;
    int n, i;
    CAMLlocal2(val, v);

    env = GetEnvironmentStrings();
    n = 0;
    for (p = env; *p; p += strlen(p) + 1)
        ++n;
    val = alloc_tuple(n);
    i = 0;
    for (p = env; *p; p += strlen(p) + 1) {
        v = copy_string(p);
        Store_field(val, i, v);
        ++i;
    }
    FreeEnvironmentStrings(env);
    CAMLreturn(val);
}

CAMLprim value get_environment_variable(value name)
{
    CAMLparam1(name);
    int len;
    char* buf;
    CAMLlocal1(val);

    len = GetEnvironmentVariable(String_val(name), NULL, 0);
    if (len == 0)
        val = Val_none;
    else {
        buf = malloc(len);
        GetEnvironmentVariable(String_val(name), buf, len);
        val = alloc_some(copy_string(buf));
        free(buf);
    }
    CAMLreturn(val);
}

CAMLprim value get_exit_code_process(value process)
{
    DWORD exit_code;

    if (!GetExitCodeProcess(Handle_val(process), &exit_code))
        raise_last_error();
    return alloc_terminationstatus(exit_code);
}

CAMLprim value get_exit_code_thread(value thread)
{
    DWORD exit_code;

    if (!GetExitCodeThread(Handle_val(thread), &exit_code))
        raise_last_error();
    return alloc_terminationstatus(exit_code);
}

CAMLprim value get_fiber_data(value unit)
{
    return copy_int32((DWORD)GetFiberData());
}

CAMLprim value get_priority_class(value process)
{
    DWORD result;

    result = GetPriorityClass(Handle_val(process));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, priority_classes);
}

CAMLprim value get_process_priority_boost(value process)
{
    BOOL disabled;

    if (!GetProcessPriorityBoost(Handle_val(process), &disabled))
        raise_last_error();
    return Val_bool(disabled);
}

CAMLprim value get_process_shutdown_parameters(value unit)
{
    CAMLparam1(unit);
    DWORD level, flags;
    CAMLlocal1(val);

    if (!GetProcessShutdownParameters(&level, &flags))
        raise_last_error();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(level));
    Store_field(val, 1, Val_bool((flags & SHUTDOWN_NORETRY) != 0));
    CAMLreturn(val);
}

CAMLprim value get_process_times(value process)
{
    CAMLparam1(process);
    FILETIME creation_time, exit_time, kernel_time, user_time;
    CAMLlocal2(val, v);

    if (!GetProcessTimes(Handle_val(process), &creation_time, &exit_time, &kernel_time, &user_time))
        raise_last_error();
    val = alloc_tuple(4);
    v = alloc_filetime(&creation_time);
    Store_field(val, 0, v);
    v = alloc_filetime(&exit_time);
    Store_field(val, 1, v);
    v = alloc_filetime(&kernel_time);
    Store_field(val, 2, v);
    v = alloc_filetime(&user_time);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

CAMLprim value get_process_working_set_size(value process)
{
    CAMLparam1(process);
    DWORD minimum, maximum;
    CAMLlocal1(val);

    if (!GetProcessWorkingSetSize(Handle_val(process), &minimum, &maximum))
        raise_last_error();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(minimum));
    Store_field(val, 1, Val_int(maximum));
    CAMLreturn(val);
}

CAMLprim value get_startup_info(value unit)
{
    STARTUPINFO si;

    GetStartupInfo(&si);
    return alloc_startupinfo(&si);
}

CAMLprim value get_thread_priority(value thread)
{
    int result;

    result = GetThreadPriority(Handle_val(thread));
    if (result == THREAD_PRIORITY_ERROR_RETURN)
        raise_last_error();
    return Val_constant(result, thread_priorities);
}

CAMLprim value get_thread_priority_boost(value thread)
{
    BOOL disabled;

    if (!GetThreadPriorityBoost(Handle_val(thread), &disabled))
        raise_last_error();
    return Val_bool(disabled);
}

CAMLprim value get_thread_times(value thread)
{
    CAMLparam1(thread);
    FILETIME creation_time, exit_time, kernel_time, user_time;
    CAMLlocal2(val, v);

    if (!GetThreadTimes(Handle_val(thread), &creation_time, &exit_time, &kernel_time, &user_time))
        raise_last_error();
    val = alloc_tuple(4);
    v = alloc_filetime(&creation_time);
    Store_field(val, 0, v);
    v = alloc_filetime(&exit_time);
    Store_field(val, 1, v);
    v = alloc_filetime(&kernel_time);
    Store_field(val, 2, v);
    v = alloc_filetime(&user_time);
    Store_field(val, 3, v);
    CAMLreturn(val);
}

CAMLprim value open_process(value access_mask, value inherit_handle, value process_id)
{
    HANDLE result;

    result = OpenProcess((DWORD)Int64_val(access_mask), Bool_val(inherit_handle), Int32_val(process_id));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value resume_thread(value thread)
{
    DWORD result;

    result = ResumeThread(Handle_val(thread));
    if (result == 0xFFFFFFFF)
        raise_last_error();
    return Val_int((int)result);
}

CAMLprim value set_environment_variable(value name, value val)
{
    if (!SetEnvironmentVariable(String_val(name), stringoption_val(val)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_priority_class(value process, value priority_class)
{
    if (!SetPriorityClass(Handle_val(process), Constant_val(priority_class, priority_classes)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_process_priority_boost(value process, value disable)
{
    if (!SetProcessPriorityBoost(Handle_val(process), Bool_val(disable)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_process_shutdown_parameters(value level, value noretry)
{
    if (!SetProcessShutdownParameters(Int_val(level), (Bool_val(noretry) ? SHUTDOWN_NORETRY : 0)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_process_working_set_size(value process, value size)
{
    DWORD minimum, maximum;

    if (Is_some(size)) {
        minimum = Int_val(Field(Value_of(size), 0));
        maximum = Int_val(Field(Value_of(size), 1));
    } else
        minimum = maximum = 0xFFFFFFFF;
    if (!SetProcessWorkingSetSize(Handle_val(process), minimum, maximum))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_thread_priority(value thread, value priority)
{
    if (!SetThreadPriority(Handle_val(thread), Constant_val(priority, thread_priorities)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_thread_priority_boost(value thread, value disable)
{
    if (!SetThreadPriorityBoost(Handle_val(thread), Bool_val(disable)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value sleep(value delay)
{
    CAMLparam1(delay);
    DWORD d;

    d = delay_val(delay);
    enter_blocking_section();
    Sleep(d);
    leave_blocking_section();
    CAMLreturn(Val_unit);
}

CAMLprim value sleep_ex(value delay, value alertable)
{
    CAMLparam2(delay, alertable);
    DWORD d;
    DWORD result;
    CAMLlocal1(val);

    d = delay_val(delay);
    enter_blocking_section();
    result = SleepEx(d, Bool_val(alertable));
    leave_blocking_section();
    if (result == 0)
        result = WAIT_TIMEOUT;
    val = alloc_waitresult(result, 0);
    CAMLreturn(val);
}

CAMLprim value suspend_thread(value thread)
{
    DWORD result;

    result = SuspendThread(Handle_val(thread));
    if (result == 0xFFFFFFFF)
        raise_last_error();
    return Val_int((int)result);
}

CAMLprim value switch_to_fiber(value fiber)
{
    SwitchToFiber(Pointer_val(fiber));
    return Val_unit;
}

CAMLprim value switch_to_thread(value unit)
{
    CAMLparam1(unit);
    BOOL result;

    enter_blocking_section();
    result = SwitchToThread();
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value terminate_process(value process, value exit_code)
{
    if (!TerminateProcess(Handle_val(process), Int32_val(exit_code)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value terminate_thread(value thread, value exit_code)
{
    if (!TerminateThread(Handle_val(thread), Int32_val(exit_code)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value tls_alloc(value unit)
{
    DWORD result;

    result = TlsAlloc();
    if (result == 0xFFFFFFFF)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value tls_free(value index)
{
    if (!TlsFree(Int_val(index)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value tls_get_value(value index)
{
    DWORD result;
    DWORD error;

    result = (DWORD)TlsGetValue(Int_val(index));
    if (result == 0) {
        error = GetLastError();
        if (error != NO_ERROR)
            raise_win32_error(error);
    }
    return copy_int32(result);
}

CAMLprim value tls_set_value(value index, value val)
{
    if (!TlsSetValue(Int_val(index), (LPVOID)Int32_val(val)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value wait_for_input_idle(value process, value timeout)
{
    CAMLparam2(process, timeout);
    HANDLE proc;
    DWORD time;
    DWORD result;
    CAMLlocal1(val);

    proc = Handle_val(process);
    time = delay_val(timeout);
    enter_blocking_section();
    result = WaitForInputIdle(proc, time);
    leave_blocking_section();
    if (result == WAIT_FAILED)
        raise_last_error();
    if (result == 0)
        result = WAIT_OBJECT_0;
    val = alloc_waitresult(result, 1);
    CAMLreturn(val);
}

CAMLprim value win_exec(value command, value show_option)
{
    CAMLparam2(command, show_option);
    char* cmd;
    UINT result;

    cmd = new_string(command);
    enter_blocking_section();
    result = WinExec(cmd, Constant_val(show_option, show_window_options));
    leave_blocking_section();
    free_string(cmd);
    if (result == 0)
        result = ERROR_NOT_ENOUGH_MEMORY;
    if (result <= 31)
        raise_win32_error(result);
    CAMLreturn(Val_unit);
}
