/* OCaml-Win32
 * win32_dynlink.c
 * Copyright (c) 2002 by Harry Chomsky & Nicolas Cannasse
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

#include "..\win32.h"

static DWORD dynamic_flags[] = {
	DONT_RESOLVE_DLL_REFERENCES,
	LOAD_LIBRARY_AS_DATAFILE,
	LOAD_WITH_ALTERED_SEARCH_PATH
};
Define_static_constants(dynamic_flags);


CAMLprim value disable_thread_library_calls( value mod ) {
    if( !DisableThreadLibraryCalls(Handle_val(mod)) )
        raise_last_error();
	return Val_unit;
}

CAMLprim value free_library( value mod ) {
	if( !FreeLibrary(Handle_val(mod)) )
		raise_last_error();
	return Val_unit;
}

CAMLprim value free_library_and_exit_thread( value mod, value ecode ) {
	FreeLibraryAndExitThread( Handle_val(mod), Int_val(ecode) );
	return Val_unit;
}

CAMLprim value get_module_file_name( value mod ) {
	CAMLparam1(mod);
	CAMLlocal1(result);
	char *buf = (char*)malloc(1024);
	if( !GetModuleFileName( Is_some(mod)?Handle_val(Value_of(mod)):NULL, buf, 1024) ) {
		free(buf);
		raise_last_error();
	}
	result = copy_string(buf);
	free(buf);
	CAMLreturn(result);
}

CAMLprim value get_module_handle( value str ) {
	HMODULE h = GetModuleHandle( Is_some(str)?String_val(Value_of(str)):NULL );
	if( !h )
		raise_last_error();
	return alloc_handle(h);
}

CAMLprim value get_proc_address( value mod, value str ) {
	PVOID p = GetProcAddress( Handle_val(mod), String_val(str) );
	if( !p )
		raise_last_error();
	return alloc_pointer(p);
}

CAMLprim value load_library( value str ) {
	HMODULE h = LoadLibrary( String_val(str) );
	if( !h )
		raise_last_error();
	return alloc_handle(h);
}

CAMLprim value load_library_ex( value str, value flag ) {
	HMODULE h = LoadLibraryEx( String_val(str), NULL, Constant_val(flag,dynamic_flags) );
	if( !h )
		raise_last_error();
	return alloc_handle(h);
}
