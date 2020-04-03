/* OCaml-Win32
 * win32_string.c
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
/* Windows Base Services: General Library: String Manipulation */

CAMLprim value format_message(value source, value width, value args)
{
    CAMLparam3(source, width, args);
    DWORD flags;
    LPCVOID src;
    DWORD message_id;
    char* buffer;
    DWORD* a;
    int n, i;
    DWORD result;
    CAMLlocal2(val, v);

    flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY;
    if (Tag_val(source) == 0) {
        flags |= FORMAT_MESSAGE_FROM_STRING;
        src = (LPCVOID)String_val(Field(source, 0));
        message_id = 0;
    } else if (Tag_val(source) == 1) {
        flags |= FORMAT_MESSAGE_FROM_HMODULE;
        src = (LPCVOID)Handle_val(Field(source, 0));
        message_id = Int_val(Field(source, 1));
    } else {
        flags |= FORMAT_MESSAGE_FROM_SYSTEM;
        src = NULL;
        message_id = Int_val(Field(source, 0));
    }
    if (Is_block(width))
        flags |= Int_val(Field(width, 0));
    else if (Int_val(width) == 1)
        flags |= Int_val(FORMAT_MESSAGE_MAX_WIDTH_MASK);
    if (Is_some(args)) {
        val = Value_of(args);
        n = Wosize_val(val);
        a = malloc(n);
        for (i = 0; i < n; ++i) {
            v = Field(val, i);
            if (Tag_val(v) == 0 || Tag_val(v) == 1)
                a[i] = Int_val(Field(v, 0));
            else
                a[i] = (DWORD)String_val(Field(v, 0));
        }
    } else {
        flags |= FORMAT_MESSAGE_IGNORE_INSERTS;
        a = NULL;
    }
    result = FormatMessage(
        flags,
        src,
        message_id,
        0,
        (LPTSTR)&buffer,
        0,
        (va_list*)a);
    if (a)
        free(a);
    val = copy_string(buffer);
    LocalFree(buffer);
    CAMLreturn(val);
}

CAMLprim value load_string(value hinstance, value id, value max_length)
{
    CAMLparam3(hinstance, id, max_length);
    int n;
    char* s;
    CAMLlocal1(result);

    n = Int_val(max_length) + 1;
    s = malloc(n);
    if (LoadString(Handle_val(hinstance), Int_val(id), s, n) == 0) {
        free(s);
        raise_last_error();
    }
    result = copy_string(s);
    free(s);
    CAMLreturn(result);
}
