/* OCaml-Win32
 * win32_pipe.c
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
/* Windows Base Services: Interprocess Communication: Pipes */

CAMLprim value create_pipe(value security, value size)
{
    CAMLparam2(security, size);
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    int s;
    HANDLE read, write;
    BOOL result;
    CAMLlocal2(val, v);

    if (Is_some(security)) {
        securityattributes_val(Value_of(security), &sa);
        sap = &sa;
    } else
        sap = NULL;
    if (Is_some(size))
        s = Int_val(Value_of(size));
    else
        s = 0;
    enter_blocking_section();
    result = CreatePipe(&read, &write, sap, s);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_tuple(2);
    v = alloc_handle(read);
    Store_field(val, 0, v);
    v = alloc_handle(write);
    Store_field(val, 1, v);
    CAMLreturn(val);
}
