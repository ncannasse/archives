/* OCaml-Win32
 * win32_coord.c
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
/* Graphics and Multimedia Services: GDI: Coordinate Spaces and Transformations */

CAMLprim value client_to_screen(value hwnd, value point)
{
    POINT pt;

    pt = point_val(point);
    if (!ClientToScreen(Handle_val(hwnd), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value combine_transform(value xform1, value xform2)
{
    XFORM xf1, xf2, xf;

    xform_val(xform1, &xf1);
    xform_val(xform2, &xf2);
    if (!CombineTransform(&xf, &xf1, &xf2))
        raise_last_error();
    return alloc_xform(&xf);
}

CAMLprim value dp_to_lp(value hdc, value point)
{
    POINT pt;

    pt = point_val(point);
    if (!DPtoLP(Handle_val(hdc), &pt, 1))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value dp_to_lp_multi(value hdc, value point_array)
{
    CAMLparam2(hdc, point_array);
    int num_pts, i;
    POINT* pts;
    CAMLlocal2(val, v);

    num_pts = Wosize_val(point_array);
    if (num_pts != 0) {
        pts = malloc(num_pts * sizeof(POINT));
        for (i = 0; i < num_pts; ++i)
            pts[i] = point_val(Field(point_array, i));
        if (!DPtoLP(Handle_val(hdc), pts, num_pts)) {
            free(pts);
            raise_last_error();
        }
        val = alloc_tuple(num_pts);
        for (i = 0; i < num_pts; ++i) {
            v = alloc_point(pts[i]);
            Store_field(val, i, v);
        }
        free(pts);
    } else
        val = Atom(0);
    CAMLreturn(val);
}

CAMLprim value get_current_position(value hdc)
{
    POINT pt;

    if (!GetCurrentPositionEx(Handle_val(hdc), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value get_graphics_mode(value hdc)
{
    int result;

    result = GetGraphicsMode(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, graphics_modes);
}

CAMLprim value get_map_mode(value hdc)
{
    int result;

    result = GetMapMode(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, map_modes);
}

CAMLprim value get_viewport_ext(value hdc)
{
    SIZE s;

    if (!GetViewportExtEx(Handle_val(hdc), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value get_viewport_org(value hdc)
{
    POINT pt;

    if (!GetViewportOrgEx(Handle_val(hdc), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value get_window_ext(value hdc)
{
    SIZE s;

    if (!GetWindowExtEx(Handle_val(hdc), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value get_window_org(value hdc)
{
    POINT pt;

    if (!GetWindowOrgEx(Handle_val(hdc), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value get_world_transform(value hdc)
{
    XFORM xf;

    if (!GetWorldTransform(Handle_val(hdc), &xf))
        raise_last_error();
    return alloc_xform(&xf);
}

CAMLprim value lp_to_dp(value hdc, value point)
{
    POINT pt;

    pt = point_val(point);
    if (!LPtoDP(Handle_val(hdc), &pt, 1))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value lp_to_dp_multi(value hdc, value point_array)
{
    CAMLparam2(hdc, point_array);
    int num_pts, i;
    POINT* pts;
    CAMLlocal2(val, v);

    num_pts = Wosize_val(point_array);
    if (num_pts != 0) {
        pts = malloc(num_pts * sizeof(POINT));
        for (i = 0; i < num_pts; ++i)
            pts[i] = point_val(Field(point_array, i));
        if (!LPtoDP(Handle_val(hdc), pts, num_pts)) {
            free(pts);
            raise_last_error();
        }
        val = alloc_tuple(num_pts);
        for (i = 0; i < num_pts; ++i) {
            v = alloc_point(pts[i]);
            Store_field(val, i, v);
        }
        free(pts);
    } else
        val = Atom(0);
    CAMLreturn(val);
}

CAMLprim value map_window_point(value from, value to, value point)
{
    POINT pt;

    pt = point_val(point);
    if (!MapWindowPoints(Handle_val(from), Handle_val(to), &pt, 1))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value map_window_points(value from, value to, value point_array)
{
    CAMLparam3(from, to, point_array);
    int num_pts, i;
    POINT* pts;
    CAMLlocal2(val, v);

    num_pts = Wosize_val(point_array);
    if (num_pts != 0) {
        pts = malloc(num_pts * sizeof(POINT));
        for (i = 0; i < num_pts; ++i)
            pts[i] = point_val(Field(point_array, i));
        if (!MapWindowPoints(Handle_val(from), Handle_val(to), pts, num_pts)) {
            free(pts);
            raise_last_error();
        }
        val = alloc_tuple(num_pts);
        for (i = 0; i < num_pts; ++i) {
            v = alloc_point(pts[i]);
            Store_field(val, i, v);
        }
        free(pts);
    } else
        val = Atom(0);
    CAMLreturn(val);
}

CAMLprim value map_window_rect(value from, value to, value rect)
{
    RECT r;

    rect_val(rect, &r);
    if (!MapWindowPoints(Handle_val(from), Handle_val(to), (LPPOINT)&r, 2))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value modify_world_transform(value hdc, value modifier)
{
    XFORM xf;
    DWORD mode;

    if (Is_long(modifier))
        mode = MWT_IDENTITY;
    else {
        xform_val(Field(modifier, 0), &xf);
        mode = (Tag_val(modifier) == 0) ? MWT_LEFTMULTIPLY : MWT_RIGHTMULTIPLY;
    }
    if (!ModifyWorldTransform(Handle_val(hdc), &xf, mode))
        raise_last_error();
    return Val_unit;
}

CAMLprim value offset_viewport_org(value hdc, value offset)
{
    POINT pt;

    if (!OffsetViewportOrgEx(Handle_val(hdc), X(offset), Y(offset), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value offset_window_org(value hdc, value offset)
{
    POINT pt;

    if (!OffsetWindowOrgEx(Handle_val(hdc), X(offset), Y(offset), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value scale_viewport_ext(value hdc, value x_num, value x_denom, value y_num, value y_denom)
{
    SIZE s;
    BOOL result;

    result = ScaleViewportExtEx(
        Handle_val(hdc),
        Int_val(x_num),
        Int_val(x_denom),
        Int_val(y_num),
        Int_val(y_denom),
        &s);
    if (!result)
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value scale_window_ext(value hdc, value x_num, value x_denom, value y_num, value y_denom)
{
    SIZE s;
    BOOL result;

    result = ScaleWindowExtEx(
        Handle_val(hdc),
        Int_val(x_num),
        Int_val(x_denom),
        Int_val(y_num),
        Int_val(y_denom),
        &s);
    if (!result)
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value screen_to_client(value hwnd, value point)
{
    POINT pt;

    pt = point_val(point);
    if (!ScreenToClient(Handle_val(hwnd), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value set_graphics_mode(value hdc, value mode)
{
    int result;

    result = SetGraphicsMode(Handle_val(hdc), Constant_val(mode, graphics_modes));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, graphics_modes);
}

CAMLprim value set_map_mode(value hdc, value mode)
{
    int result;

    result = SetMapMode(Handle_val(hdc), Constant_val(mode, map_modes));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, map_modes);
}

CAMLprim value set_viewport_ext(value hdc, value size)
{
    SIZE s;

    if (!SetViewportExtEx(Handle_val(hdc), Cx(size), Cy(size), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value set_viewport_org(value hdc, value point)
{
    POINT pt;

    if (!SetViewportOrgEx(Handle_val(hdc), X(point), Y(point), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value set_window_ext(value hdc, value size)
{
    SIZE s;

    if (!SetWindowExtEx(Handle_val(hdc), Cx(size), Cy(size), &s))
        raise_last_error();
    return alloc_size(s);
}

CAMLprim value set_window_org(value hdc, value point)
{
    POINT pt;

    if (!SetWindowOrgEx(Handle_val(hdc), X(point), Y(point), &pt))
        raise_last_error();
    return alloc_point(pt);
}

CAMLprim value set_world_transform(value hdc, value xform)
{
    XFORM xf;

    xform_val(xform, &xf);
    if (!SetWorldTransform(Handle_val(hdc), &xf))
        raise_last_error();
    return Val_unit;
}
