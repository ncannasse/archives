/* OCaml-Win32
 * win32_brush.c
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
/* Graphics and Multimedia Services: GDI: Brushes */

CAMLprim value create_brush_indirect(value logbrush)
{
    LOGBRUSH lb;
    HBRUSH result;

    logbrush_val(logbrush, &lb);
    result = CreateBrushIndirect(&lb);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_dib_pattern_brush_pt(value dib, value color_type)
{
    HBRUSH result;

    result = CreateDIBPatternBrushPt(raw_val(dib), Constant_val(color_type, dib_color_types));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_hatch_brush(value hatch_style, value color)
{
    HBRUSH result;

    result = CreateHatchBrush(Constant_val(hatch_style, hatch_styles), color_val(color));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_pattern_brush(value hbitmap)
{
    HBRUSH result;

    result = CreatePatternBrush(Handle_val(hbitmap));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_solid_brush(value color)
{
    HBRUSH result;

    result = CreateSolidBrush(color_val(color));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value get_brush_org_ex(value hdc)
{
    POINT p;

    if (!GetBrushOrgEx(Handle_val(hdc), &p))
        raise_last_error();
    return alloc_point(p);
}

CAMLprim value get_sys_color_brush(value sys_color)
{
    return alloc_handle(GetSysColorBrush(Constant_val(sys_color, sys_colors)));
}

CAMLprim value pat_blt(value hdc, value point, value size, value rop3)
{
    BOOL result;

    result = PatBlt(
        Handle_val(hdc),
        X(point), Y(point),
        Cx(size), Cy(size),
        Int_val(rop3));
    if (!result)
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_brush_org_ex(value hdc, value point)
{
    POINT p;

    if (!SetBrushOrgEx(Handle_val(hdc), X(point), Y(point), &p))
        raise_last_error();
    return alloc_point(p);
}
