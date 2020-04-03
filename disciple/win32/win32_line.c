/* OCaml-Win32
 * win32_line.c
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
/* Graphics and Multimedia Services: GDI: Lines and Curves */

CAMLprim value angle_arc(value hdc, value point, value radius, value start_angle, value sweep_angle)
{
    BOOL result;

    result = AngleArc(
        Handle_val(hdc),
        X(point), Y(point),
        Int_val(radius),
        (FLOAT)Double_val(start_angle),
        (FLOAT)Double_val(sweep_angle));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value arc(value hdc, value rect, value start_pt, value end_pt)
{
    BOOL result;

    result = Arc(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect),
        X(start_pt), Y(start_pt),
        X(end_pt), Y(end_pt));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value arc_to(value hdc, value rect, value start_pt, value end_pt)
{
    BOOL result;

    result = ArcTo(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect),
        X(start_pt), Y(start_pt),
        X(end_pt), Y(end_pt));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value get_arc_direction(value hdc)
{
    return Val_constant(GetArcDirection(Handle_val(hdc)), arc_directions);
}

static VOID CALLBACK line_dda_proc(int x, int y, LPARAM lpData)
{
    CAMLparam0();
    CAMLlocal2(func, point);
    POINT p;

    func = *(value*)lpData;
    p.x = x;
    p.y = y;
    point = alloc_point(p);
    callback(func, point);
    CAMLreturn0;
}

CAMLprim value line_dda(value start_pt, value end_pt, value dda_func)
{
    CAMLparam3(start_pt, end_pt, dda_func);
    value* p;
    BOOL result;

    p = malloc(sizeof(value));
    register_global_root(p);
    *p = dda_func;
    result = LineDDA(X(start_pt), Y(start_pt), X(end_pt), Y(end_pt), line_dda_proc, (LPARAM)p);
    remove_global_root(p);
    free(p);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value line_to(value hdc, value point)
{
    if (!LineTo(Handle_val(hdc), X(point), Y(point)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value move_to(value hdc, value point)
{
    POINT p;

    if (!MoveToEx(Handle_val(hdc), X(point), Y(point), &p))
        raise_last_error();
    return alloc_point(p);
}

CAMLprim value poly_bezier(value hdc, value points)
{
    int num_pts;
    POINT* pts;
    int i;
    BOOL result;

    num_pts = Wosize_val(points);
    pts = malloc(num_pts * sizeof(POINT));
    for (i = 0; i < num_pts; ++i)
        pts[i] = point_val(Field(points, i));
    result = PolyBezier(Handle_val(hdc), pts, num_pts);
    free(pts);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value poly_bezier_to(value hdc, value points)
{
    int num_pts;
    POINT* pts;
    int i;
    BOOL result;

    num_pts = Wosize_val(points);
    pts = malloc(num_pts * sizeof(POINT));
    for (i = 0; i < num_pts; ++i)
        pts[i] = point_val(Field(points, i));
    result = PolyBezierTo(Handle_val(hdc), pts, num_pts);
    free(pts);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value polyline(value hdc, value points)
{
    int num_pts;
    POINT* pts;
    int i;
    BOOL result;

    num_pts = Wosize_val(points);
    pts = malloc(num_pts * sizeof(POINT));
    for (i = 0; i < num_pts; ++i)
        pts[i] = point_val(Field(points, i));
    result = Polyline(Handle_val(hdc), pts, num_pts);
    free(pts);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value polyline_to(value hdc, value points)
{
    int num_pts;
    POINT* pts;
    int i;
    BOOL result;

    num_pts = Wosize_val(points);
    pts = malloc(num_pts * sizeof(POINT));
    for (i = 0; i < num_pts; ++i)
        pts[i] = point_val(Field(points, i));
    result = PolylineTo(Handle_val(hdc), pts, num_pts);
    free(pts);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value poly_polyline(value hdc, value points)
{
    int num_groups;
    DWORD* group_sizes;
    int num_pts;
    POINT* pts;
    value group;
    int i, j, k;
    BOOL result;

    num_groups = Wosize_val(points);
    group_sizes = malloc(num_groups * sizeof(DWORD));
    num_pts = 0;
    for (i = 0; i < num_groups; ++i) {
        group_sizes[i] = Wosize_val(Field(points, i));
        num_pts += group_sizes[i];
    }
    pts = malloc(num_pts * sizeof(POINT));
    k = 0;
    for (i = 0; i < num_groups; ++i) {
        group = Field(points, i);
        for (j = 0; j < (int)group_sizes[i]; ++j)
            pts[k++] = point_val(Field(group, j));
    }
    result = PolyPolyline(Handle_val(hdc), pts, group_sizes, num_groups);
    free(pts);
    free(group_sizes);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_arc_direction(value hdc, value new_direction)
{
    int result;

    result = SetArcDirection(Handle_val(hdc), Constant_val(new_direction, arc_directions));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, arc_directions);
}
