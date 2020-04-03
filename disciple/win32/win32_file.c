/* OCaml-Win32
 * win32_file.c
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
/* Windows Base Services: Files and I/O: File I/O */

CAMLprim value are_file_apis_ansi(value unit)
{
    return Val_bool(AreFileApisANSI());
}

CAMLprim value cancel_io(value file)
{
    if (!CancelIo(Handle_val(file)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value copy_file(value from_name, value to_name, value fail_if_exists)
{
    CAMLparam3(from_name, to_name, fail_if_exists);
    char* from;
    char* to;
    BOOL result;

    from = new_string(from_name);
    to = new_string(to_name);
    enter_blocking_section();
    result = CopyFile(from, to, Bool_val(fail_if_exists));
    leave_blocking_section();
    free_string(from);
    free_string(to);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value create_directory(value name, value security)
{
    CAMLparam2(name, security);
    char* n;
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    BOOL result;

    n = new_string(name);
    if (Is_some(security)) {
        securityattributes_val(Value_of(security), &sa);
        sap = &sa;
    } else
        sap = NULL;
    enter_blocking_section();
    result = CreateDirectory(n, sap);
    leave_blocking_section();
    free_string(n);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value create_directory_ex(value templ, value name, value security)
{
    CAMLparam3(templ, name, security);
    char* tmpl;
    char* n;
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    BOOL result;

    tmpl = new_string(templ);
    n = new_string(name);
    if (Is_some(security)) {
        securityattributes_val(Value_of(security), &sa);
        sap = &sa;
    } else
        sap = NULL;
    enter_blocking_section();
    result = CreateDirectoryEx(tmpl, n, sap);
    leave_blocking_section();
    free_string(tmpl);
    free_string(n);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value create_file_native(value name, value access, value share, value security, value creation, value attributes, value flags, value sqos, value templ)
{
    CAMLparam5(name, access, share, security, creation);
    CAMLxparam4(attributes, flags, sqos, templ);
    char* n;
    DWORD acc, shr;
    SECURITY_ATTRIBUTES sa;
    SECURITY_ATTRIBUTES* sap;
    DWORD fl;
    HANDLE tmpl;
    HANDLE result;
    CAMLlocal1(val);

    n = new_string(name);
    acc = (DWORD)Int64_val(access);
    shr = flags_val(share, file_share_options);
    if (Is_some(security)) {
        securityattributes_val(Value_of(security), &sa);
        sap = &sa;
    } else
        sap = NULL;
    fl = flags_val(attributes, file_attributes) |
        flags_val(flags, file_flags) |
        flags_val(sqos, sqos_options);
    tmpl = Handle_val(templ);
    enter_blocking_section();
    result = CreateFile(n, acc, shr, sap, Constant_val(creation, file_create_options), fl, tmpl);
    leave_blocking_section();
    free_string(n);
    if (result == INVALID_HANDLE_VALUE)
        raise_last_error();
    val = alloc_handle(result);
    CAMLreturn(val);
}

CAMLprim value create_file_bytecode(value* argv, int argn)
{
    return create_file_native(argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
}

CAMLprim value delete_file(value name)
{
    CAMLparam1(name);
    char* n;
    BOOL result;

    n = new_string(name);
    enter_blocking_section();
    result = DeleteFile(n);
    leave_blocking_section();
    free_string(n);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value find_files(value name, value func)
{
    CAMLparam2(name, func);
    char* n;
    WIN32_FIND_DATA data;
    HANDLE h;
    DWORD err;
    CAMLlocal2(val, result);
    BOOL found;

    n = new_string(name);
    enter_blocking_section();
    h = FindFirstFile(n, &data);
    leave_blocking_section();
    free_string(n);
    if (h == INVALID_HANDLE_VALUE) {
        err = GetLastError();
        if (err != ERROR_FILE_NOT_FOUND)
            raise_win32_error(err);
    } else {
        for (;;) {
            val = alloc_finddata(&data);
            result = callback(func, val);
            if (!Bool_val(result))
                break;
            enter_blocking_section();
            found = FindNextFile(h, &data);
            leave_blocking_section();
            if (!found)
                break;
        }
        enter_blocking_section();
        FindClose(h);
        leave_blocking_section();
    }
    CAMLreturn(Val_unit);
}

CAMLprim value find_close_change_notification(value handle)
{
    if (!FindCloseChangeNotification(Handle_val(handle)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value find_first_change_notification(value path, value watch_subtree, value filter)
{
    HANDLE h;

    h = FindFirstChangeNotification(String_val(path), Bool_val(watch_subtree), flags_val(filter, file_change_types));
    if (h == INVALID_HANDLE_VALUE)
        raise_last_error();
    return alloc_handle(h);
}

CAMLprim value find_next_change_notification(value handle)
{
    if (!FindNextChangeNotification(Handle_val(handle)))
        raise_last_error();
    return Val_unit;
}

CAMLprim value flush_file_buffers(value file)
{
    CAMLparam1(file);
    HANDLE f;
    BOOL result;

    f = Handle_val(file);
    enter_blocking_section();
    result = FlushFileBuffers(f);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value get_binary_type(value name)
{
    CAMLparam1(name);
    char* n;
    DWORD bt;
    BOOL result;
    CAMLlocal1(val);

    n = new_string(name);
    enter_blocking_section();
    result = GetBinaryType(n, &bt);
    leave_blocking_section();
    free_string(n);
    if (!result)
        raise_last_error();
    val = Alloc_flags(bt, binary_types);
    CAMLreturn(val);
}

CAMLprim value get_current_directory(value unit)
{
    CAMLparam1(unit);
    int len;
    char* buf;
    CAMLlocal1(val);

    enter_blocking_section();
    len = GetCurrentDirectory(0, NULL);
    leave_blocking_section();
    if (len == 0)
        raise_last_error();
    buf = malloc(len);
    enter_blocking_section();
    len = GetCurrentDirectory(len, buf);
    leave_blocking_section();
    if (len == 0) {
        free(buf);
        raise_last_error();
    }
    val = copy_string(buf);
    free(buf);
    CAMLreturn(val);
}

CAMLprim value get_disk_free_space(value path)
{
    CAMLparam1(path);
    char* p;
    ULARGE_INTEGER avail, total, free;
    BOOL result;
    CAMLlocal2(val, v);

    p = new_stringoption(path);
    enter_blocking_section();
    result = GetDiskFreeSpaceEx(p, &avail, &total, &free);
    leave_blocking_section();
    free_stringoption(p);
    if (!result)
        raise_last_error();
    val = alloc_tuple(3);
    v = copy_int64(avail.QuadPart);
    Store_field(val, 0, v);
    v = copy_int64(total.QuadPart);
    Store_field(val, 1, v);
    v = copy_int64(free.QuadPart);
    Store_field(val, 2, v);
    CAMLreturn(val);
}

CAMLprim value get_drive_type(value root)
{
    CAMLparam1(root);
    char* r;
    UINT result;

    r = new_stringoption(root);
    enter_blocking_section();
    result = GetDriveType(r);
    leave_blocking_section();
    free_stringoption(r);
    CAMLreturn(Val_constant(result, drive_types));
}

CAMLprim value get_file_attributes(value name)
{
    CAMLparam1(name);
    char* n;
    DWORD result;
    CAMLlocal1(val);

    n = new_string(name);
    enter_blocking_section();
    result = GetFileAttributes(n);
    leave_blocking_section();
    free_string(n);
    if (result == 0xFFFFFFFF)
        raise_last_error();
    val = Alloc_flags(result, file_attributes);
    CAMLreturn(val);
}

CAMLprim value get_file_information_by_handle(value file)
{
    CAMLparam1(file);
    HANDLE f;
    BY_HANDLE_FILE_INFORMATION bhfi;
    BOOL result;
    CAMLlocal1(val);

    f = Handle_val(file);
    enter_blocking_section();
    result = GetFileInformationByHandle(f, &bhfi);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    val = alloc_byhandlefileinformation(&bhfi);
    CAMLreturn(val);
}

CAMLprim value get_file_size(value file)
{
    CAMLparam1(file);
    HANDLE f;
    LARGE_INTEGER li;
    DWORD err;
    CAMLlocal1(val);

    f = Handle_val(file);
    enter_blocking_section();
    li.LowPart = GetFileSize(f, &li.HighPart);
    leave_blocking_section();
    if (li.LowPart == 0xFFFFFFFF) {
        err = GetLastError();
        if (err != 0)
            raise_win32_error(err);
    }
    val = copy_int64(li.QuadPart);
    CAMLreturn(val);
}

CAMLprim value get_file_type(value file)
{
    CAMLparam1(file);
    HANDLE f;
    DWORD result;

    f = Handle_val(file);
    enter_blocking_section();
    result = GetFileType(f);
    leave_blocking_section();
    CAMLreturn(Val_constant(result, file_types));
}

CAMLprim value get_full_path_name(value name)
{
    CAMLparam1(name);
    char* n;
    int len;
    char* buf;
    char* file_part;
    CAMLlocal2(val, v);

    n = new_string(name);
    enter_blocking_section();
    len = GetFullPathName(n, 0, NULL, NULL);
    leave_blocking_section();
    if (len == 0) {
        free_string(n);
        raise_last_error();
    }
    buf = malloc(len);
    enter_blocking_section();
    len = GetFullPathName(n, len, buf, &file_part);
    leave_blocking_section();
    free_string(n);
    if (len == 0) {
        free(buf);
        raise_last_error();
    }
    val = alloc_tuple(2);
    v = copy_string(buf);
    Store_field(val, 0, v);
    Store_field(val, 1, Val_int(file_part - buf));
    free(buf);
    CAMLreturn(val);
}

CAMLprim value get_logical_drives(value unit)
{
    CAMLparam1(unit);
    DWORD mask, bit;
    CAMLlocal1(val);
    int i;

    enter_blocking_section();
    mask = GetLogicalDrives();
    leave_blocking_section();
    if (mask == 0)
        raise_last_error();
    val = alloc_tuple(32);
    for (i = 0, bit = 1; i < 32; ++i, bit <<= 1)
        Store_field(val, i, Val_bool((mask & bit) != 0));
    CAMLreturn(val);
}

CAMLprim value get_logical_drive_strings(value unit)
{
    CAMLparam1(unit);
    int len;
    char* buf;
    int n;
    char* p;
    CAMLlocal2(val, v);

    enter_blocking_section();
    len = GetLogicalDriveStrings(0, NULL);
    leave_blocking_section();
    if (len == 0)
        raise_last_error();
    buf = malloc(len);
    enter_blocking_section();
    len = GetLogicalDriveStrings(len, buf);
    leave_blocking_section();
    if (len == 0) {
        free(buf);
        raise_last_error();
    }
    n = 0;
    for (p = buf; *p; p += strlen(p) + 1)
        ++n;
    val = alloc_tuple(n);
    n = 0;
    for (p = buf; *p; p += strlen(p) + 1) {
        v = copy_string(p);
        Store_field(val, n, v);
        ++n;
    }
    free(buf);
    CAMLreturn(val);
}

CAMLprim value get_short_path_name(value name)
{
    CAMLparam1(name);
    char* n;
    int len;
    char* buf;
    CAMLlocal1(val);

    n = new_string(name);
    enter_blocking_section();
    len = GetShortPathName(n, NULL, 0);
    leave_blocking_section();
    if (len == 0) {
        free_string(n);
        raise_last_error();
    }
    buf = malloc(len);
    enter_blocking_section();
    len = GetShortPathName(n, buf, len);
    leave_blocking_section();
    free_string(n);
    if (len == 0) {
        free(buf);
        raise_last_error();
    }
    val = copy_string(buf);
    free(buf);
    CAMLreturn(val);
}

CAMLprim value get_temp_file_name(value path, value prefix, value unique)
{
    CAMLparam3(path, prefix, unique);
    char* pth;
    char* pref;
    UINT u;
    char buf[_MAX_PATH+1];
    UINT result;
    CAMLlocal2(val, v);

    pth = new_string(path);
    pref = new_string(prefix);
    u = Int32_val(unique);
    enter_blocking_section();
    result = GetTempFileName(pth, pref, u, buf);
    leave_blocking_section();
    free_string(pth);
    free_string(pref);
    if (result == 0)
        raise_last_error();
    val = alloc_tuple(2);
    v = copy_string(buf);
    Store_field(val, 0, v);
    v = copy_int32(result);
    Store_field(val, 1, v);
    CAMLreturn(val);
}

CAMLprim value get_temp_path(value unit)
{
    CAMLparam1(unit);
    int len;
    char* buf;
    CAMLlocal1(val);

    enter_blocking_section();
    len = GetTempPath(0, NULL);
    leave_blocking_section();
    if (len == 0)
        raise_last_error();
    buf = malloc(len);
    enter_blocking_section();
    len = GetTempPath(len, buf);
    leave_blocking_section();
    if (len == 0) {
        free(buf);
        raise_last_error();
    }
    val = copy_string(buf);
    free(buf);
    CAMLreturn(val);
}

CAMLprim value lock_file(value file, value offset, value length)
{
    CAMLparam3(file, offset, length);
    HANDLE f;
    LARGE_INTEGER off, len;
    BOOL result;

    f = Handle_val(file);
    off.QuadPart = Int64_val(offset);
    len.QuadPart = Int64_val(length);
    enter_blocking_section();
    result = LockFile(f, off.LowPart, off.HighPart, len.LowPart, len.HighPart);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value move_file(value from_name, value to_name)
{
    CAMLparam2(from_name, to_name);
    char* from;
    char* to;
    BOOL result;

    from = new_string(from_name);
    to = new_string(to_name);
    enter_blocking_section();
    result = MoveFile(from, to);
    leave_blocking_section();
    free_string(from);
    free_string(to);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value move_file_ex(value from_name, value to_name, value options)
{
    CAMLparam3(from_name, to_name, options);
    char* from;
    char* to;
    DWORD fl;
    BOOL result;

    from = new_string(from_name);
    to = new_stringoption(to_name);
    fl = flags_val(options, move_file_options);
    enter_blocking_section();
    result = MoveFileEx(from, to, fl);
    leave_blocking_section();
    free_string(from);
    free_stringoption(to);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value read_file(value file, value buffer)
{
    CAMLparam2(file, buffer);
    HANDLE f;
    PVOID buf, p;
    int len;
    DWORD num_read;
    BOOL result;
    CAMLlocal1(val);

    f = Handle_val(file);
    sizedraw_val(buffer, &buf, &len);
    if (is_sizedraw_fixed(buffer))
        p = buf;
    else
        p = malloc(len);
    enter_blocking_section();
    result = ReadFile(f, p, len, &num_read, NULL);
    leave_blocking_section();
    if (!is_sizedraw_fixed(buffer)) {
        sizedraw_val(buffer, &buf, &len);
        memcpy(buf, p, num_read);
        free(p);
    }
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(result ? 0 : GetLastError()));
    Store_field(val, 1, Val_int(num_read));
    CAMLreturn(val);
}

CAMLprim value read_file_overlapped(value file, value ptr, value size, value overlapped)
{
    CAMLparam4(file, ptr, size, overlapped);
    HANDLE f;
    PVOID p;
    OVERLAPPED* ovl;
    DWORD num_read;
    BOOL result;
    CAMLlocal1(val);

    f = Handle_val(file);
    p = Pointer_val(ptr);
    ovl = Overlappedptr_val(overlapped);
    enter_blocking_section();
    result = ReadFile(f, p, Int_val(size), &num_read, ovl);
    leave_blocking_section();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(result ? 0 : GetLastError()));
    Store_field(val, 1, Val_int(num_read));
    CAMLreturn(val);
}

CAMLprim value remove_directory(value name)
{
    CAMLparam1(name);
    char* n;
    BOOL result;

    n = new_string(name);
    enter_blocking_section();
    result = RemoveDirectory(n);
    leave_blocking_section();
    free_string(n);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value search_path(value path, value name, value extension)
{
    CAMLparam3(path, name, extension);
    char* pth;
    char* n;
    char* ext;
    char buf[_MAX_PATH+1];
    char* file_part;
    DWORD result;
    CAMLlocal2(val, v);

    pth = new_stringoption(path);
    n = new_string(name);
    ext = new_stringoption(extension);
    enter_blocking_section();
    result = SearchPath(pth, n, ext, sizeof(buf), buf, &file_part);
    leave_blocking_section();
    free_stringoption(pth);
    free_string(n);
    free_stringoption(ext);
    if (result == 0)
        raise_last_error();
    val = alloc_tuple(2);
    v = copy_string(buf);
    Store_field(val, 0, v);
    Store_field(val, 1, file_part - buf);
    CAMLreturn(val);
}

CAMLprim value set_current_directory(value name)
{
    CAMLparam1(name);
    char* n;
    BOOL result;

    n = new_string(name);
    enter_blocking_section();
    result = SetCurrentDirectory(n);
    leave_blocking_section();
    free_string(n);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_end_of_file(value file)
{
    CAMLparam1(file);
    HANDLE f;
    BOOL result;

    f = Handle_val(file);
    enter_blocking_section();
    result = SetEndOfFile(f);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_file_apis_to_ansi(value unit)
{
    SetFileApisToANSI();
    return Val_unit;
}

CAMLprim value set_file_apis_to_oem(value unit)
{
    SetFileApisToOEM();
    return Val_unit;
}

CAMLprim value set_file_attributes(value name, value attributes)
{
    CAMLparam2(name, attributes);
    char* n;
    DWORD attr;
    BOOL result;

    n = new_string(name);
    attr = flags_val(attributes, file_attributes);
    enter_blocking_section();
    result = SetFileAttributes(n, attr);
    leave_blocking_section();
    free_string(n);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value set_file_pointer(value file, value distance, value method)
{
    CAMLparam3(file, distance, method);
    HANDLE f;
    LARGE_INTEGER li;
    DWORD err;
    CAMLlocal1(val);

    f = Handle_val(file);
    li.QuadPart = Int64_val(distance);
    enter_blocking_section();
    li.LowPart = SetFilePointer(f, li.LowPart, &li.HighPart, Constant_val(method, seek_methods));
    leave_blocking_section();
    err = GetLastError();
    if (err != 0)
        raise_win32_error(err);
    val = copy_int64(li.QuadPart);
    CAMLreturn(val);
}

CAMLprim value set_handle_count(value count)
{
    UINT result;

    result = SetHandleCount(Int_val(count));
    return Val_int(result);
}

CAMLprim value set_volume_label(value root, value label)
{
    CAMLparam2(root, label);
    char* r;
    char* l;
    BOOL result;

    r = new_stringoption(root);
    l = new_stringoption(label);
    enter_blocking_section();
    result = SetVolumeLabel(r, l);
    leave_blocking_section();
    free_stringoption(r);
    free_stringoption(l);
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value unlock_file(value file, value offset, value length)
{
    CAMLparam3(file, offset, length);
    HANDLE f;
    LARGE_INTEGER off, len;
    BOOL result;

    f = Handle_val(file);
    off.QuadPart = Int64_val(offset);
    len.QuadPart = Int64_val(length);
    enter_blocking_section();
    result = UnlockFile(f, off.LowPart, off.HighPart, len.LowPart, len.HighPart);
    leave_blocking_section();
    if (!result)
        raise_last_error();
    CAMLreturn(Val_unit);
}

CAMLprim value write_file(value file, value buffer)
{
    CAMLparam2(file, buffer);
    HANDLE f;
    PVOID buf, p;
    int len;
    DWORD num_written;
    BOOL result;
    CAMLlocal1(val);

    f = Handle_val(file);
    sizedraw_val(buffer, &buf, &len);
    if (is_sizedraw_fixed(buffer))
        p = buf;
    else {
        p = malloc(len);
        memcpy(p, buf, len);
    }
    enter_blocking_section();
    result = WriteFile(f, p, len, &num_written, NULL);
    leave_blocking_section();
    if (!is_sizedraw_fixed(buffer))
        free(p);
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(result ? 0 : GetLastError()));
    Store_field(val, 1, Val_int(num_written));
    CAMLreturn(val);
}

CAMLprim value write_file_overlapped(value file, value ptr, value size, value overlapped)
{
    CAMLparam4(file, ptr, size, overlapped);
    HANDLE f;
    PVOID p;
    OVERLAPPED* ovl;
    DWORD num_written;
    BOOL result;
    CAMLlocal1(val);

    f = Handle_val(file);
    p = Pointer_val(ptr);
    ovl = Overlappedptr_val(overlapped);
    enter_blocking_section();
    result = WriteFile(f, p, Int_val(size), &num_written, ovl);
    leave_blocking_section();
    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(result ? 0 : GetLastError()));
    Store_field(val, 1, Val_int(num_written));
    CAMLreturn(val);
}
