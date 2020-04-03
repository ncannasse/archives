/* OCaml-Win32
 * win32_paint.c
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
/* Graphics and Multimedia Services: GDI: Painting and Drawing */

CAMLprim value begin_paint(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    PAINTSTRUCT ps;
    HDC result;
    CAMLlocal1(val);

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = BeginPaint(hwnd, &ps);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_paintstruct(&ps);
    CAMLreturn(val);
}

CAMLprim value draw_animated_rects(value wnd, value type, value from_rect, value to_rect)
{
    CAMLparam4(wnd, type, from_rect, to_rect);
    HWND hwnd;
    RECT from, to;
    BOOL result;

    hwnd = Handle_val(wnd);
    rect_val(from_rect, &from);
    rect_val(to_rect, &to);
    enter_blocking_section();
    result = DrawAnimatedRects(hwnd, Constant_val(type, animation_types), &from, &to);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value draw_caption(value wnd, value hdc, value rect, value options)
{
    RECT r;

    rect_val(rect, &r);
    if (!DrawCaption(Handle_val(wnd), Handle_val(hdc), &r, flags_val(options, draw_caption_options)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_edge(value hdc, value rect, value type, value options)
{
    RECT r;

    rect_val(rect, &r);
    if (!DrawEdge(Handle_val(hdc), &r, edgetype_val(type), flags_val(options, border_options)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_focus_rect(value hdc, value rect)
{
    RECT r;

    rect_val(rect, &r);
    if (!DrawFocusRect(Handle_val(hdc), &r))
        raise_last_error();
    return Val_unit;
}

CAMLprim value draw_frame_control(value hdc, value rect, value type, value options)
{
    RECT r;
    DWORD type_and_state;
    DWORD more_state;
    BOOL result;

    rect_val(rect, &r);
    type_and_state = Constant_val(type, draw_frame_control_types);
    more_state = flags_val(options, draw_frame_control_options);
    result = DrawFrameControl(Handle_val(hdc), &r,
        LOWORD(type_and_state), HIWORD(type_and_state) | more_state);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value end_paint(value wnd, value paintstruct)
{
    CAMLparam2(wnd, paintstruct);
    HWND hwnd;
    PAINTSTRUCT ps;

    hwnd = Handle_val(wnd);
    paintstruct_val(paintstruct, &ps);
    enter_blocking_section();
    EndPaint(hwnd, &ps);
    leave_blocking_section();
    CAMLreturn(Val_unit);
}

CAMLprim value exclude_update_rgn(value hdc, value wnd)
{
    return val_regioncomplexity(ExcludeUpdateRgn(Handle_val(hdc), Handle_val(wnd)));
}

CAMLprim value gdi_flush(value unit)
{
    if (!GdiFlush())
        raise_last_error();
    return Val_unit;
}

CAMLprim value gdi_get_batch_limit(value unit)
{
    DWORD result;

    result = GdiGetBatchLimit();
    if (result == 0)
        raise_last_error();
    return copy_int64((int64)result);
}

CAMLprim value gdi_set_batch_limit(value limit)
{
    DWORD result;

    result = GdiSetBatchLimit((DWORD)Int64_val(limit));
    if (result == 0)
        raise_last_error();
    return copy_int64((int64)result);
}

CAMLprim value get_bk_color(value hdc)
{
    COLORREF result;

    result = GetBkColor(Handle_val(hdc));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value get_bk_mode(value hdc)
{
    int result;

    result = GetBkMode(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, background_modes);
}

CAMLprim value get_rop2(value hdc)
{
    int result;

    result = GetROP2(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, rop2s);
}

CAMLprim value get_update_rect(value wnd, value erase)
{
    RECT r;
    BOOL result;

    result = GetUpdateRect(Handle_val(wnd), &r, Bool_val(erase));
    if (result)
        return alloc_some(alloc_rect(&r));
    else
        return Val_none;
}

CAMLprim value get_update_rgn(value wnd, value rgn, value erase)
{
    return val_regioncomplexity(GetUpdateRgn(Handle_val(wnd), Handle_val(rgn), Bool_val(erase)));
}

CAMLprim value get_window_dc(value wnd)
{
    HDC result;

    result = GetWindowDC(Handle_val(wnd));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value get_window_rgn(value wnd, value rgn)
{
    return val_regioncomplexity(GetWindowRgn(Handle_val(wnd), Handle_val(rgn)));
}

CAMLprim value invalidate_rect(value wnd, value rect_option, value erase)
{
    CAMLparam3(wnd, rect_option, erase);
    HWND hwnd;
    RECT r;
    RECT* rect_ptr;
    BOOL result;

    hwnd = Handle_val(wnd);
    if (Is_some(rect_option)) {
        rect_val(Value_of(rect_option), &r);
        rect_ptr = &r;
    } else
        rect_ptr = NULL;
    enter_blocking_section();
    result = InvalidateRect(hwnd, rect_ptr, Bool_val(erase));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value invalidate_rgn(value wnd, value rgn, value erase)
{
    CAMLparam3(wnd, rgn, erase);
    HWND hwnd;
    HRGN hrgn;
    BOOL result;

    hwnd = Handle_val(wnd);
    hrgn = Handle_val(rgn);
    enter_blocking_section();
    result = InvalidateRgn(hwnd, hrgn, Bool_val(erase));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value lock_window_update(value wnd)
{
    if (!LockWindowUpdate(Handle_val(wnd)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value paint_desktop(value hdc)
{
    if (!PaintDesktop(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value redraw_window(value wnd, value area, value options)
{
    CAMLparam3(wnd, area, options);
    HWND hwnd;
    RECT r;
    RECT* rect_ptr;
    HRGN hrgn;
    UINT opt;
    BOOL result;

    hwnd = Handle_val(wnd);
    if (Is_long(area)) {
        rect_ptr = NULL;
        hrgn = NULL;
    } else if (Tag_val(area) == 0) {
        rect_val(Field(area, 0), &r);
        rect_ptr = &r;
        hrgn = NULL;
    } else {
        rect_ptr = NULL;
        hrgn = Handle_val(Field(area, 0));
    }
    opt = flags_val(options, redraw_window_options);
    enter_blocking_section();
    result = RedrawWindow(hwnd, rect_ptr, hrgn, opt);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_bk_color(value hdc, value color)
{
    COLORREF result;

    result = SetBkColor(Handle_val(hdc), color_val(color));
    if (result == CLR_INVALID)
        raise_last_error();
    return alloc_rgb(result);
}

CAMLprim value set_bk_mode(value hdc, value mode)
{
    int result;

    result = SetBkMode(Handle_val(hdc), Constant_val(mode, background_modes));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, background_modes);
}

CAMLprim value set_rop2(value hdc, value rop2)
{
    int result;

    result = SetROP2(Handle_val(hdc), Constant_val(rop2, rop2s));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, rop2s);
}

CAMLprim value set_window_rgn(value wnd, value rgn, value redraw)
{
    CAMLparam3(wnd, rgn, redraw);
    HWND hwnd;
    HRGN hrgn;
    int result;

    hwnd = Handle_val(wnd);
    hrgn = Handle_val(rgn);
    enter_blocking_section();
    result = SetWindowRgn(hwnd, hrgn, Bool_val(redraw));
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value update_window(value wnd)
{
    CAMLparam1(wnd);
    HWND hwnd;
    BOOL result;

    hwnd = Handle_val(wnd);
    enter_blocking_section();
    result = UpdateWindow(hwnd);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value validate_rect(value wnd, value rect_option)
{
    CAMLparam2(wnd, rect_option);
    HWND hwnd;
    RECT r;
    RECT* rect_ptr;
    BOOL result;

    hwnd = Handle_val(wnd);
    if (Is_some(rect_option)) {
        rect_val(Value_of(rect_option), &r);
        rect_ptr = &r;
    } else
        rect_ptr = NULL;
    enter_blocking_section();
    result = ValidateRect(hwnd, rect_ptr);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value validate_rgn(value wnd, value rgn)
{
    CAMLparam2(wnd, rgn);
    HWND hwnd;
    HRGN hrgn;
    BOOL result;

    hwnd = Handle_val(wnd);
    hrgn = Handle_val(rgn);
    enter_blocking_section();
    result = ValidateRgn(hwnd, hrgn);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value window_from_dc(value hdc)
{
    return alloc_handle(WindowFromDC(Handle_val(hdc)));
}

/* Messages */

CAMLprim value deliver_wm_displaychange(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);
    SIZE s;

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_int(Wparam(msg));
    s.cx = LOWORD(Lparam(msg));
    s.cy = HIWORD(Lparam(msg));
    args[3] = alloc_size(s);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_ncpaint(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(rgn, result);

    rgn = alloc_handle((HRGN)Wparam(msg));
    result = callback3(handler, wnd, msg, rgn);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_paint(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal2(hdc, result);

    hdc = alloc_handle((HDC)Wparam(msg));
    result = callback3(handler, wnd, msg, hdc);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_print(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 4);
    CAMLlocal1(result);

    args[0] = wnd;
    args[1] = msg;
    args[2] = alloc_handle((HDC)Wparam(msg));
    args[3] = Alloc_flags(Lparam(msg), print_options);
    result = callbackN(handler, 4, args);
    CAMLreturn(result);
}

CAMLprim value deliver_wm_setredraw(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocal1(result);

    result = callback3(handler, wnd, msg, Val_bool((BOOL)Wparam(msg)));
    CAMLreturn(result);
}
