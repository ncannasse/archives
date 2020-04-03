/* OCaml-Win32
 * win32_region.c
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
/* Graphics and Multimedia Services: GDI: Regions */

CAMLprim value combine_rgn(value dest, value src1, value src2, value mode)
{
    int result;

    result = CombineRgn(
        Handle_val(dest),
        Handle_val(src1),
        Handle_val(src2),
        Constant_val(mode, region_modes));
    return val_regioncomplexity(result);
}

CAMLprim value create_elliptic_rgn(value rect)
{
    HRGN result;

    result = CreateEllipticRgn(Left(rect), Top(rect), Right(rect), Bottom(rect));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_polygon_rgn(value points, value mode)
{
    int num_pts;
    POINT* pts;
    int i;
    HRGN result;

    num_pts = Wosize_val(points);
    pts = malloc(num_pts * sizeof(POINT));
    for (i = 0; i < num_pts; ++i)
        pts[i] = point_val(Field(points, i));
    result = CreatePolygonRgn(pts, num_pts, Constant_val(mode, poly_fill_modes));
    free(pts);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_poly_polygon_rgn(value points, value mode)
{
    int num_groups;
    INT* group_sizes;
    int num_pts;
    POINT* pts;
    value group;
    int i, j, k;
    HRGN result;

    num_groups = Wosize_val(points);
    group_sizes = malloc(num_groups * sizeof(INT));
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
    result = CreatePolyPolygonRgn(
        pts,
        group_sizes,
        num_groups,
        Constant_val(mode, poly_fill_modes));
    free(pts);
    free(group_sizes);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_rect_rgn(value rect)
{
    HRGN result;

    result = CreateRectRgn(Left(rect), Top(rect), Right(rect), Bottom(rect));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_round_rect_rgn(value rect, value size)
{
    HRGN result;

    result = CreateRoundRectRgn(
        Left(rect), Top(rect), Right(rect), Bottom(rect),
        Cx(size), Cy(size));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value equal_rgn(value rgn1, value rgn2)
{
    BOOL result;

    result = EqualRgn(Handle_val(rgn1), Handle_val(rgn2));
    if (result == ERROR)
        raise_last_error();
    return Val_bool(result);
}

CAMLprim value ext_create_region(value xform, value rgndata)
{
    XFORM xf;
    int size;
    RGNDATA* data;
    HRGN result;

    xform_val(xform, &xf);
    size = rgndata_size(rgndata);
    data = malloc(size);
    rgndata_val(rgndata, data);
    result = ExtCreateRegion(&xf, size, data);
    free(data);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value fill_rgn(value hdc, value hrgn, value hbrush)
{
    if (!FillRgn(Handle_val(hdc), Handle_val(hrgn), Handle_val(hbrush)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value frame_rgn(value hdc, value hrgn, value hbrush, value size)
{
    BOOL result;

    result = FrameRgn(
        Handle_val(hdc),
        Handle_val(hrgn),
        Handle_val(hbrush),
        Cx(size), Cy(size));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value get_poly_fill_mode(value hdc)
{
    int result;

    result = GetPolyFillMode(Handle_val(hdc));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, poly_fill_modes);
}

CAMLprim value get_region_data(value hrgn)
{
    CAMLparam1(hrgn);
    DWORD size;
    RGNDATA* data;
    CAMLlocal1(val);

    size = GetRegionData(Handle_val(hrgn), 0, NULL);
    if (size == 0)
        raise_last_error();
    data = malloc(size);
    GetRegionData(Handle_val(hrgn), size, data);
    val = alloc_rgndata(data);
    free(data);
    CAMLreturn(val);
}

CAMLprim value get_rgn_box(value hrgn)
{
    RECT r;
    int result;

    result = GetRgnBox(Handle_val(hrgn), &r);
    if (result == ERROR)
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value invert_rgn(value hdc, value hrgn)
{
    if (!InvertRgn(Handle_val(hdc), Handle_val(hrgn)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value offset_rgn(value hrgn, value point)
{
    return val_regioncomplexity(OffsetRgn(Handle_val(hrgn), X(point), Y(point)));
}

CAMLprim value paint_rgn(value hdc, value hrgn)
{
    if (!PaintRgn(Handle_val(hdc), Handle_val(hrgn)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value pt_in_region(value hrgn, value point)
{
    return Val_bool(PtInRegion(Handle_val(hrgn), X(point), Y(point)));
}

CAMLprim value rect_in_region(value hrgn, value rect)
{
    RECT r;

    rect_val(rect, &r);
    return Val_bool(RectInRegion(Handle_val(hrgn), &r));
}

CAMLprim value set_poly_fill_mode(value hdc, value mode)
{
    int result;

    result = SetPolyFillMode(Handle_val(hdc), Constant_val(mode, poly_fill_modes));
    if (result == 0)
        raise_last_error();
    return Val_constant(result, poly_fill_modes);
}

CAMLprim value set_rect_rgn(value hrgn, value rect)
{
    if (!SetRectRgn(Handle_val(hrgn), Left(rect), Top(rect), Right(rect), Bottom(rect)))
        raise_last_error();
    return Val_unit;
}
