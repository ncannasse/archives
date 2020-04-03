/* OCaml-Win32
 * win32_rect.c
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
/* Graphics and Multimedia Services: GDI: Rectangles */

CAMLprim value inflate_rect(value rect, value amount)
{
    RECT r;

    rect_val(rect, &r);
    if (!InflateRect(&r, Cx(amount), Cy(amount)))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value intersect_rect(value rect1, value rect2)
{
    RECT src1, src2, dst;

    rect_val(rect1, &src1);
    rect_val(rect2, &src2);
    if (IntersectRect(&dst, &src1, &src2))
        return alloc_some(alloc_rect(&dst));
    else
        return Val_none;
}

CAMLprim value is_rect_empty(value rect)
{
    RECT r;

    rect_val(rect, &r);
    return Val_bool(IsRectEmpty(&r));
}

CAMLprim value offset_rect(value rect, value amount)
{
    RECT r;

    rect_val(rect, &r);
    if (!OffsetRect(&r, X(amount), Y(amount)))
        raise_last_error();
    return alloc_rect(&r);
}

CAMLprim value pt_in_rect(value rect, value point)
{
    RECT r;

    rect_val(rect, &r);
    return Val_bool(PtInRect(&r, point_val(point)));
}

CAMLprim value subtract_rect(value rect1, value rect2)
{
    RECT src1, src2, dst;

    rect_val(rect1, &src1);
    rect_val(rect2, &src2);
    if (!SubtractRect(&dst, &src1, &src2))
        raise_last_error();
    return alloc_rect(&dst);
}

CAMLprim value union_rect(value rect1, value rect2)
{
    RECT src1, src2, dst;

    rect_val(rect1, &src1);
    rect_val(rect2, &src2);
    UnionRect(&dst, &src1, &src2);
    return alloc_rect(&dst);
}
