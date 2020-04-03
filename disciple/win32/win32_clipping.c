/* OCaml-Win32
 * win32_clipping.c
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
/* Graphics and Multimedia Services: GDI: Clipping */

CAMLprim value exclude_clip_rect(value hdc, value rect)
{
    int result;

    result = ExcludeClipRect(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect));
    return val_regioncomplexity(result);
}

CAMLprim value ext_select_clip_rgn(value hdc, value hrgn, value mode)
{
    int result;

    result = ExtSelectClipRgn(
        Handle_val(hdc),
        Handle_val(hrgn),
        Constant_val(mode, region_modes));
    return val_regioncomplexity(result);
}

CAMLprim value get_clip_box(value hdc)
{
    RECT r;
    int result;

    result = GetClipBox(Handle_val(hdc), &r);
    if (result == ERROR)
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value get_clip_rgn(value hdc, value hrgn)
{
    int result;

    result = GetClipRgn(Handle_val(hdc), Handle_val(hrgn));
    if (result == -1)
        raise_last_error();
    return Val_bool(result == 1);
}

CAMLprim value get_meta_rgn(value hdc, value hrgn)
{
    if (!GetMetaRgn(Handle_val(hdc), Handle_val(hrgn)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value intersect_clip_rect(value hdc, value rect)
{
    int result;

    result = IntersectClipRect(
        Handle_val(hdc),
        Left(rect), Top(rect), Right(rect), Bottom(rect));
    return val_regioncomplexity(result);
}

CAMLprim value offset_clip_rgn(value hdc, value point)
{
    return val_regioncomplexity(OffsetClipRgn(Handle_val(hdc), X(point), Y(point)));
}

CAMLprim value pt_visible(value hdc, value point)
{
    return Val_bool(PtVisible(Handle_val(hdc), X(point), Y(point)));
}

CAMLprim value rect_visible(value hdc, value rect)
{
    RECT r;

    rect_val(rect, &r);
    return Val_bool(RectVisible(Handle_val(hdc), &r));
}

CAMLprim value select_clip_path(value hdc, value mode)
{
    if (!SelectClipPath(Handle_val(hdc), Constant_val(mode, region_modes)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value select_clip_rgn(value hdc, value hrgn)
{
    return val_regioncomplexity(SelectClipRgn(Handle_val(hdc), Handle_val(hrgn)));
}

CAMLprim value set_meta_rgn(value hdc)
{
    return val_regioncomplexity(SetMetaRgn(Handle_val(hdc)));
}
