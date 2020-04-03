/* OCaml-Win32
 * win32_atom.c
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
/* Windows Base Services: Interprocess Communication: Atoms */

CAMLprim value add_atom(value name)
{
    ATOM result;

    result = AddAtom(String_val(name));
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value delete_atom(value atom)
{
    if (DeleteAtom((ATOM)Int_val(atom)) != 0)
        raise_last_error();
    return Val_unit;
}

CAMLprim value find_atom(value name)
{
    ATOM result;

    result = FindAtom(String_val(name));
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value get_atom_name(value atom)
{
    char buffer[256];

    if (GetAtomName((ATOM)Int_val(atom), buffer, sizeof(buffer)) == 0)
        raise_last_error();
    return copy_string(buffer);
}

CAMLprim value global_add_atom(value name)
{
    ATOM result;

    result = GlobalAddAtom(String_val(name));
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value global_delete_atom(value atom)
{
    if (GlobalDeleteAtom((ATOM)Int_val(atom)) != 0)
        raise_last_error();
    return Val_unit;
}

CAMLprim value global_find_atom(value name)
{
    ATOM result;

    result = GlobalFindAtom(String_val(name));
    if (result == 0)
        raise_last_error();
    return Val_int(result);
}

CAMLprim value global_get_atom_name(value atom)
{
    char buffer[256];

    if (GlobalGetAtomName((ATOM)Int_val(atom), buffer, sizeof(buffer)) == 0)
        raise_last_error();
    return copy_string(buffer);
}

CAMLprim value init_atom_table(value size)
{
    if (!InitAtomTable(Int_val(size)))
        raise_last_error();
    return Val_unit;
}
