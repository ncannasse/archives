/* OCaml-Win32
 * win32_keyboard.c
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
/* User Interface Services: User Input: Keyboard Input */

CAMLprim value enable_window(value wnd, value enable)
{
    CAMLparam2(wnd, enable);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = EnableWindow(hwnd, Bool_val(enable));
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value get_active_window(value unit)
{
    return alloc_handle(GetActiveWindow());
}

CAMLprim value get_async_key_state(value code)
{
    CAMLparam1(code);
    SHORT result;
    CAMLlocal1(val);

    result = GetAsyncKeyState(virtualkeycode_val(code));
    val = alloc_tuple(2);
    Store_field(val, 0, Val_bool((result & 0x8000) != 0));
    Store_field(val, 1, Val_bool((result & 0x0001) != 0));
    CAMLreturn(val);
}

CAMLprim value get_focus(value unit)
{
    return alloc_handle(GetFocus());
}

CAMLprim value get_key_name_text(value key_data, value dont_care, value max_length)
{
    CAMLparam3(key_data, dont_care, max_length);
    DWORD data;
    int n;
    char* s;
    CAMLlocal1(result);

    data = keydata_val(key_data);
    if (Bool_val(dont_care))
        data |= 0x02000000;
    n = Int_val(max_length) + 1;
    s = malloc(n);
    GetKeyNameText(data, s, n);
    result = copy_string(s);
    free(s);
    CAMLreturn(result);
}

CAMLprim value get_key_state(value code)
{
    CAMLparam1(code);
    SHORT result;
    CAMLlocal1(val);

    result = GetKeyState(virtualkeycode_val(code));
    val = alloc_tuple(2);
    Store_field(val, 0, Val_bool((result & 0x8000) != 0));
    Store_field(val, 1, Val_bool((result & 0x0001) != 0));
    CAMLreturn(val);
}

CAMLprim value is_window_enabled(value wnd)
{
    return Val_bool(IsWindowEnabled(Handle_val(wnd)));
}

CAMLprim value register_hot_key(value wnd, value id, value modifiers, value code)
{
    BOOL result;

    result = RegisterHotKey(
        Handle_val(wnd),
        Int_val(id),
        flags_val(modifiers, hot_key_modifiers),
        virtualkeycode_val(code));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_active_window(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = SetActiveWindow(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value set_focus(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd, result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = SetFocus(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value unregister_hot_key(value wnd, value id)
{
    if (!UnregisterHotKey(Handle_val(wnd), Int_val(id)))
        raise_last_error();
    return Val_unit;
}

/* Messages */

CAMLprim value deliver_wm_activate(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_constant(LOWORD(Wparam(msg)), activate_flags);
    args[3] = Val_bool(HIWORD(Wparam(msg)));
    args[4] = alloc_handle((HWND)Lparam(msg));
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_hotkey(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_int(Wparam(msg));
    args[3] = Alloc_flags(LOWORD(Lparam(msg)), hot_key_modifiers);
    args[4] = alloc_virtualkeycode((BYTE)HIWORD(Lparam(msg)));
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_focus(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HWND)Wparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}
