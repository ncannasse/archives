/* OCaml-Win32
 * win32_mouse.c
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
/* User Interface Services: User Input: Mouse Input */

CAMLprim value drag_detect(value wnd, value point)
{
    CAMLparam2(wnd, point);
    HWND hwnd;
    POINT pt;
    BOOL result;

    hwnd = Handle_val(wnd);
    pt = point_val(point);
    enter_blocking_section();
    result = DragDetect(hwnd, pt);
    leave_blocking_section();
    CAMLreturn(Val_bool(result));
}

CAMLprim value get_capture(value unit)
{
    return alloc_handle(GetCapture());
}

CAMLprim value get_double_click_time(value unit)
{
    return Val_int(GetDoubleClickTime());
}

CAMLprim value release_capture(value unit)
{
    ReleaseCapture();
    return Val_unit;
}

CAMLprim value set_capture(value wnd)
{
    return alloc_handle(SetCapture(Handle_val(wnd)));
}

CAMLprim value set_double_click_time(value time)
{
    if (!SetDoubleClickTime(Int_val(time)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value swap_mouse_button(value swap)
{
    return Val_bool(SwapMouseButton(Bool_val(swap)));
}

/* Messages */

CAMLprim value deliver_wm_capturechanged(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(h, result);

    h = alloc_handle((HWND)Lparam(msg));
    result = callback3(handler, wnd, msg, h);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_mouseactivate(value handler, value wnd, value msg)
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

CAMLprim value return_wm_mouseactivate(value result)
{
    return copy_int32(Constant_val(result, mouse_activate_options));
}

CAMLprim value deliver_wm_mousewheel(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    POINT p;
    CAMLlocalN(args, 5);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Alloc_flags(LOWORD(Wparam(msg)), mouse_modifiers);
    args[3] = Val_int(HIWORD(Wparam(msg)));
    p.x = GET_X_LPARAM(Lparam(msg));
    p.y = GET_Y_LPARAM(Lparam(msg));
    args[4] = alloc_point(p);
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_nchittest(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    POINT p;
    CAMLlocal2(point, result);

    p.x = GET_X_LPARAM(Lparam(msg));
    p.y = GET_Y_LPARAM(Lparam(msg));
    point = alloc_point(p);
    result = callback3(handler, wnd, msg, point);
    CAMLreturn(result);
}

CAMLprim value return_wm_nchittest(value result)
{
    return copy_int32(Constant_val(result, hit_test_results));
}
