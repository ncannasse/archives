/* OCaml-Win32
 * win32_console.c
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
/* Windows Base Services: Files and I/O: Consoles and Character-Mode Support */

CAMLprim value alloc_console(value unit)
{
    CAMLparam1(unit);
    BOOL result;

    enter_blocking_section();
    result = AllocConsole();
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value free_console(value unit)
{
    CAMLparam1(unit);
    BOOL result;

    enter_blocking_section();
    result = FreeConsole();
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value generate_console_ctrl_event(value event, value process_group_id)
{
    CAMLparam2(event, process_group_id);
    DWORD id;
    BOOL result;

    id = Int32_val(process_group_id);
    enter_blocking_section();
    result = GenerateConsoleCtrlEvent(Constant_val(event, ctrl_events), id);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value get_console_title(value max_length)
{
    CAMLparam1(max_length);
    int n;
    char* s;
    CAMLlocal1(result);

    n = Int_val(max_length) + 1;
    s = malloc(n);
    if (GetConsoleTitle(s, n) == 0) {
        free(s);
        raise_last_error();
    }
    result = copy_string(s);
    free(s);
    CAMLreturn(result);
}

CAMLprim value get_std_handle(value std_handle)
{
    HANDLE h;

    h = GetStdHandle(Constant_val(std_handle, std_handles));
    if (h == INVALID_HANDLE_VALUE)
        raise_last_error();
    return alloc_handle(h);
}

CAMLprim value set_console_title(value title)
{
    if (!SetConsoleTitle(String_val(title)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_std_handle(value std_handle, value handle)
{
    if (!SetStdHandle(Constant_val(std_handle, std_handles), Handle_val(handle)))
        raise_last_error();
    return Val_unit;
}
