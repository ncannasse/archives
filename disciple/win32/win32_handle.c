/* OCaml-Win32
 * win32_handle.c
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
/* Windows Base Services: General Library: Handles and Objects */

CAMLprim value close_handle(value handle)
{
    if (!CloseHandle(Handle_val(handle)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value duplicate_handle_native(value source_process, value source, value target_process, value access_mask, value inherit_handle, value close_source)
{
    DWORD mask;
    DWORD options;
    HANDLE target;
    BOOL result;

    options = 0;
    if (Is_some(access_mask))
        mask = (DWORD)Int64_val(Value_of(access_mask));
    else {
        mask = 0;
        options |= DUPLICATE_SAME_ACCESS;
    }
    if (Bool_val(close_source))
        options |= DUPLICATE_CLOSE_SOURCE;
    result = DuplicateHandle(
        Handle_val(source_process),
        Handle_val(source),
        Handle_val(target_process),
        &target,
        mask,
        Bool_val(inherit_handle),
        options);
    if (!result)
        raise_last_error();
    return alloc_handle(target);
}

CAMLprim value duplicate_handle_bytecode(value* argv, int argn)
{
    return duplicate_handle_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
}

CAMLprim value get_handle_information(value handle)
{
    DWORD flags;

    if (!GetHandleInformation(Handle_val(handle), &flags))
        raise_last_error();
    return Alloc_flags(flags, handle_flags);
}

CAMLprim value set_handle_information(value handle, value mask, value flags)
{
    if (!SetHandleInformation(Handle_val(handle), flags_val(mask, handle_flags), flags_val(flags, handle_flags)))
        raise_last_error();
    return Val_unit;
}
