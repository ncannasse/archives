/* OCaml-Win32
 * win32_pen.c
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
/* Graphics and Multimedia Services: GDI: Pens */

CAMLprim value create_pen(value style, value width, value color)
{
    HPEN result;

    result = CreatePen(Constant_val(style, pen_styles), Int_val(width), color_val(color));
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value create_pen_indirect(value logpen)
{
    LOGPEN lp;
    HPEN result;

    logpen_val(logpen, &lp);
    result = CreatePenIndirect(&lp);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}

CAMLprim value ext_create_pen(value style, value width, value logbrush, value user_style)
{
    LOGBRUSH lb;
    int us_count;
    DWORD* us_entries;
    HPEN result;

    logbrush_val(logbrush, &lb);
    if (Is_nil(user_style)) {
        us_count = 0;
        us_entries = NULL;
    } else {
        us_count = list_length(user_style);
        us_entries = malloc(us_count * sizeof(DWORD));
        us_count = 0;
        for (; !Is_nil(user_style); user_style = Tail(user_style))
            us_entries[us_count++] = Int_val(Head(user_style));
    }
    result = ExtCreatePen(extpenstyle_val(style), Int_val(width), &lb, us_count, us_entries);
    if (us_entries)
        free(us_entries);
    if (!result)
        raise_last_error();
    return alloc_handle(result);
}
