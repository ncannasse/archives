/* OCaml-Win32
 * win32_time.c
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
/* Windows Base Services: General Library: Time */

CAMLprim value compare_file_time(value time1, value time2)
{
    FILETIME ft1, ft2;

    filetime_val(time1, &ft1);
    filetime_val(time2, &ft2);
    return Val_int(CompareFileTime(&ft1, &ft2));
}

CAMLprim value dos_date_time_to_file_time(value date, value time)
{
    FILETIME ft;

    if (!DosDateTimeToFileTime((WORD)Int_val(date), (WORD)Int_val(time), &ft))
        raise_last_error();
    return alloc_filetime(&ft);
}

CAMLprim value file_time_to_dos_date_time(value time)
{
    CAMLparam1(time);
    FILETIME ft;
    WORD d, t;
    CAMLlocal1(val);

    filetime_val(time, &ft);
    if (!FileTimeToDosDateTime(&ft, &d, &t))
        raise_last_error();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(d));
    Store_field(val, 1, Val_int(t));
    CAMLreturn(val);
}

CAMLprim value file_time_to_local_file_time(value time)
{
    FILETIME ft1, ft2;

    filetime_val(time, &ft1);
    if (!FileTimeToLocalFileTime(&ft1, &ft2))
        raise_last_error();
    return alloc_filetime(&ft2);
}

CAMLprim value file_time_to_system_time(value time)
{
    FILETIME ft;
    SYSTEMTIME st;

    filetime_val(time, &ft);
    if (!FileTimeToSystemTime(&ft, &st))
        raise_last_error();
    return alloc_systemtime(&st);
}

CAMLprim value get_file_time(value file)
{
    CAMLparam1(file);
    FILETIME creation, last_access, last_write;
    CAMLlocal2(val, v);

    if (!GetFileTime(Handle_val(file), &creation, &last_access, &last_write))
        raise_last_error();
    val = alloc_tuple(3);
    v = alloc_filetime(&creation);
    Store_field(val, 0, v);
    v = alloc_filetime(&last_access);
    Store_field(val, 1, v);
    v = alloc_filetime(&last_write);
    Store_field(val, 2, v);
    CAMLreturn(val);
}

CAMLprim value get_local_time(value unit)
{
    SYSTEMTIME st;

    GetLocalTime(&st);
    return alloc_systemtime(&st);
}

CAMLprim value get_system_time(value unit)
{
    SYSTEMTIME st;

    GetSystemTime(&st);
    return alloc_systemtime(&st);
}

CAMLprim value get_system_time_adjustment(value unit)
{
    CAMLparam1(unit);
    DWORD adjustment, increment;
    BOOL disabled;
    CAMLlocal1(val);

    if (!GetSystemTimeAdjustment(&adjustment, &increment, &disabled))
        raise_last_error();
    val = alloc_tuple(3);
    Store_field(val, 0, Val_int((int)adjustment));
    Store_field(val, 1, Val_int((int)increment));
    Store_field(val, 2, Val_bool(disabled));
    CAMLreturn(val);
}

CAMLprim value get_system_time_as_file_time(value unit)
{
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);
    return alloc_filetime(&ft);
}

CAMLprim value get_tick_count(value unit)
{
    return Val_int(GetTickCount());
}

CAMLprim value local_file_time_to_file_time(value time)
{
    FILETIME ft1, ft2;

    filetime_val(time, &ft1);
    if (!LocalFileTimeToFileTime(&ft1, &ft2))
        raise_last_error();
    return alloc_filetime(&ft2);
}

CAMLprim value set_file_time(value file, value creation, value last_access, value last_write)
{
    FILETIME c, la, lw;
    FILETIME* cp;
    FILETIME* lap;
    FILETIME* lwp;

    if (Is_some(creation)) {
        filetime_val(Value_of(creation), &c);
        cp = &c;
    } else
        cp = NULL;
    if (Is_some(last_access)) {
        filetime_val(Value_of(last_access), &la);
        lap = &la;
    } else
        lap = NULL;
    if (Is_some(last_write)) {
        filetime_val(Value_of(last_write), &lw);
        lwp = &lw;
    } else
        lwp = NULL;
    if (!SetFileTime(Handle_val(file), cp, lap, lwp))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_local_time(value time)
{
    SYSTEMTIME st;

    systemtime_val(time, &st);
    if (!SetLocalTime(&st))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_system_time(value time)
{
    SYSTEMTIME st;

    systemtime_val(time, &st);
    if (!SetSystemTime(&st))
        raise_last_error();
    return Val_unit;
}

CAMLprim value set_system_time_adjustment(value adjustment, value disabled)
{
    if (!SetSystemTimeAdjustment(Int_val(adjustment), Bool_val(disabled)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value system_time_to_file_time(value time)
{
    SYSTEMTIME st;
    FILETIME ft;

    systemtime_val(time, &st);
    if (!SystemTimeToFileTime(&st, &ft))
        raise_last_error();
    return alloc_filetime(&ft);
}
