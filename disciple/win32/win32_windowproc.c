/* OCaml-Win32
 * win32_windowproc.c
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
/* User Interface Services: Windowing: Window Procedures */

CAMLprim value call_window_proc(value proc, value wnd, value contents)
{
    CAMLparam3(proc, wnd, contents);
    WNDPROC prc;
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT result;
    CAMLlocal1(val);

    prc = Pointer_val(proc);
    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    result = CallWindowProc(prc, hwnd, Int_val(Field(contents, 0)), wparam, lparam);
    leave_blocking_section();
    val = copy_int32(result);
    CAMLreturn(val);
}

CAMLprim value def_window_proc(value wnd, value contents)
{
    CAMLparam2(wnd, contents);
    HWND hwnd;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    wparam = Int32_val(Field(contents, 1));
    lparam = Int32_val(Field(contents, 2));
    enter_blocking_section();
    result = DefWindowProc(hwnd, Int_val(Field(contents, 0)), wparam, lparam);
    leave_blocking_section();
    val = copy_int32(result);
    CAMLreturn(val);
}
