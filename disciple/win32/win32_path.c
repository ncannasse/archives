/* OCaml-Win32
 * win32_path.c
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
/* Graphics and Multimedia Services: GDI: Paths */

CAMLprim value abort_path(value hdc)
{
    if (!AbortPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value begin_path(value hdc)
{
    if (!BeginPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value close_figure(value hdc)
{
    if (!CloseFigure(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value end_path(value hdc)
{
    if (!EndPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value fill_path(value hdc)
{
    if (!FillPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value flatten_path(value hdc)
{
    if (!FlattenPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value get_miter_limit(value hdc)
{
    FLOAT f;

    if (!GetMiterLimit(Handle_val(hdc), &f))
        raise_last_error();
    return copy_double(f);
}

CAMLprim value path_to_region(value hdc)
{
    HRGN result;

    result = PathToRegion(Handle_val(hdc));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value set_miter_limit(value hdc, value new_limit)
{
    FLOAT f;

    if (!SetMiterLimit(Handle_val(hdc), (FLOAT)Double_val(new_limit), &f))
        raise_last_error();
    return copy_double(f);
}

CAMLprim value stroke_and_fill_path(value hdc)
{
    if (!StrokeAndFillPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value stroke_path(value hdc)
{
    if (!StrokePath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value widen_path(value hdc)
{
    if (!WidenPath(Handle_val(hdc)))
        raise_last_error();
    return Val_unit;
}
