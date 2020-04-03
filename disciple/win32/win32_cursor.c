/* OCaml-Win32
 * win32_cursor.c
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
/* User Interface Services: Resources: Cursors */

CAMLprim value clip_cursor(value rect_option)
{
    RECT r;
    LPCRECT rp;

    if (Is_some(rect_option)) {
        rect_val(Value_of(rect_option), &r);
        rp = &r;
    } else
        rp = NULL;
    if (!ClipCursor(rp))
        raise_last_error();
    return Val_unit;
}

CAMLprim value copy_cursor(value hcursor)
{
    HCURSOR result;

    result = CopyCursor(Handle_val(hcursor));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value destroy_cursor(value hcursor)
{
    if (!DestroyCursor(Handle_val(hcursor)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value get_clip_cursor(value unit)
{
    RECT r;

    if (!GetClipCursor(&r))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value get_cursor(value unit)
{
    return alloc_handle(GetCursor());
}

CAMLprim value get_cursor_pos(value unit)
{
    POINT p;

    if (!GetCursorPos(&p))
        raise_last_error();
    return alloc_point(p);
}

CAMLprim value load_cursor(value hinstance, value name)
{
    HCURSOR result;

    result = LoadCursor(Handle_val(hinstance), resname_val(name));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value set_cursor(value hcursor)
{
    return alloc_handle(SetCursor(Handle_val(hcursor)));
}

CAMLprim value set_cursor_pos(value point)
{
    if (!SetCursorPos(X(point), Y(point)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value show_cursor(value show)
{
    return Val_int(ShowCursor(Bool_val(show)));
}

/* Messages */

CAMLprim value deliver_wm_setcursor(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HWND)Wparam(msg));
    args[3] = Val_constant(LOWORD(Lparam(msg)), hit_test_results);
    args[4] = Val_int(HIWORD(Lparam(msg)));
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}

CAMLprim value return_wm_setcursor(value result)
{
    return copy_int32(Bool_val(result));
}
