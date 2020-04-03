/* OCaml-Win32
 * win32_filledshape.c
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
/* Graphics and Multimedia Services: GDI: Filled Shapes */

CAMLprim value chord(value hdc, value rect, value point1, value point2)
{
    BOOL result;

    result = Chord(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect),
        X(point1), Y(point1),
        X(point2), Y(point2));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value ellipse(value hdc, value rect)
{
    BOOL result;

    result = Ellipse(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value fill_rect(value hdc, value rect, value fill)
{
    RECT r;
    HBRUSH brush;

    rect_val(rect, &r);
    if (Tag_val(fill) == 0)
        brush = (HBRUSH)(1 + Constant_val(Field(fill, 0), sys_colors));
    else
        brush = Handle_val(Field(fill, 0));
    if (!FillRect(Handle_val(hdc), &r, brush))
        raise_last_error();
    return Val_unit;
}

CAMLprim value frame_rect(value hdc, value rect, value hbrush)
{
    RECT r;

    rect_val(rect, &r);
    if (!FrameRect(Handle_val(hdc), &r, Handle_val(hbrush)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value invert_rect(value hdc, value rect)
{
    RECT r;

    rect_val(rect, &r);
    if (!InvertRect(Handle_val(hdc), &r))
        raise_last_error();
    return Val_unit;
}

CAMLprim value pie(value hdc, value rect, value point1, value point2)
{
    BOOL result;

    result = Pie(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect),
        X(point1), Y(point1),
        X(point2), Y(point2));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value polygon(value hdc, value points)
{
    int num_pts;
    POINT* pts;
    int i;
    BOOL result;

    num_pts = Wosize_val(points);
    pts = malloc(num_pts * sizeof(POINT));
    for (i = 0; i < num_pts; ++i)
        pts[i] = point_val(Field(points, i));
    result = Polygon(Handle_val(hdc), pts, num_pts);
    free(pts);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value poly_polygon(value hdc, value points)
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
    result = PolyPolygon(Handle_val(hdc), pts, group_sizes, num_groups);
    free(pts);
    free(group_sizes);
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value rectangle(value hdc, value rect)
{
    BOOL result;

    result = Rectangle(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value round_rect(value hdc, value rect, value size)
{
    BOOL result;

    result = RoundRect(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect),
        Cx(size), Cy(size));
    if (!result)
        raise_last_error();
    return Val_unit;
}
