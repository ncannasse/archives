/* OCaml-Win32
 * win32_caret.c
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
/* User Interface Services: Resources: Carets */

CAMLprim value create_caret(value hwnd, value description)
{
    HBITMAP h;
    SIZE s;

    if (Tag_val(description) == 2) {
        h = Handle_val(Field(description, 0));
        s.cx = s.cy = 0;
    } else {
        h = (Tag_val(description) == 0 ? NULL : (HBITMAP)1);
        s = size_val(Field(description, 0));
    }
    if (!CreateCaret(Handle_val(hwnd), h, s.cx, s.cy))
        raise_last_error();
    return Val_unit;
}

CAMLprim value destroy_caret(value unit)
{
    if (!DestroyCaret())
        raise_last_error();
    return Val_unit;
}

CAMLprim value get_caret_blink_time(value unit)
{
    UINT result;

    result = GetCaretBlinkTime();
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_caret_pos(value unit)
{
    POINT p;

    if (!GetCaretPos(&p))
        raise_last_error();
    return Val_unit;
}

CAMLprim value hide_caret(value hwnd)
{
    if (!HideCaret(Handle_val(hwnd)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_caret_blink_time(value new_time)
{
    if (!SetCaretBlinkTime(Int_val(new_time)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_caret_pos(value point)
{
    if (!SetCaretPos(X(point), Y(point)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value show_caret(value hwnd)
{
    if (!ShowCaret(Handle_val(hwnd)))
        raise_last_error();
    return Val_unit;
}
