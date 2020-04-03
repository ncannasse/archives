/* OCaml-Win32
 * win32_error.c
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
/* Windows Base Services: Debugging and Error Handling: Errors */

CAMLprim value beep(value frequency, value duration)
{
    CAMLparam2(frequency, duration);
    BOOL result;

    enter_blocking_section();
    result = Beep(Int_val(frequency), Int_val(duration));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value fatal_app_exit(value message)
{
    FatalAppExit(0, String_val(message));
    return Val_unit;
}

CAMLprim value flash_window(value wnd, value invert)
{
    CAMLparam2(wnd, invert);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = FlashWindow(hwnd, Bool_val(invert));
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value get_last_error(value unit)
{
    return Val_int(GetLastError());
}

CAMLprim value message_beep(value beep_type)
{
    CAMLparam1(beep_type);
    DWORD type;
    BOOL result;

    if (Is_some(beep_type))
        type = Constant_val(Value_of(beep_type), message_box_options);
    else
        type = 0xFFFFFFFF;
    enter_blocking_section();
    result = MessageBeep(type);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_error_mode(value options)
{
    DWORD result;

    result = SetErrorMode(flags_val(options, error_mode_options));
    return Alloc_flags(result, error_mode_options);
}

CAMLprim value set_last_error(value code)
{
    SetLastError(Int_val(code));
    return Val_unit;
}
