/* OCaml-Win32
 * win32_richedit.c
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
#include <shlobj.h>

/********************************************************************/
/* Shell API */

static const char * shell_ops[] = {
	NULL,
	"open",
	"print",
	"explore"
};
Define_static_constants(shell_ops);

CAMLprim value shell_execute_native( value hwnd, value op, value file, value oparams, value odir, value show ) {
	
	HINSTANCE h = ShellExecute(
		Handle_val(hwnd),
		Constant_val(op,shell_ops),
		String_val(file),
		Is_some(oparams)?String_val(Value_of(oparams)):NULL,
		Is_some(odir)?String_val(Value_of(odir)):NULL,
		Constant_val(show,show_window_options)
		);
	if( h == NULL )
		raise_last_error();

	return alloc_handle(h);
}

CAMLprim value shell_execute_bytecode( value *args, int argn ) {

	HINSTANCE h = ShellExecute(
		Handle_val(args[0]),
		Constant_val(args[1],shell_ops),
		String_val(args[2]),
		Is_some(args[3])?String_val(Value_of(args[3])):NULL,
		Is_some(args[4])?String_val(Value_of(args[4])):NULL,
		Constant_val(args[5],show_window_options)
		);
	if( h == NULL )
		raise_last_error();

	return alloc_handle(h);
}


/********************************************************************/
/* Shell's Namespace API */

#define BIF_NEWDIALOGSTYLE 0x0040
#define BIF_BROWSEINCLUDEURLS 0x0080
#define BIF_UAHINT 0x0100
#define BIF_NONEWFOLDERBUTTON 0x0200
#define BIF_NOTRANSLATETARGETS 0x0400
#define BIF_SHAREABLE 0x8000

static DWORD browse_flags[] = {
	BIF_BROWSEFORCOMPUTER
	, BIF_BROWSEFORPRINTER
	, BIF_DONTGOBELOWDOMAIN
	, BIF_RETURNFSANCESTORS
	, BIF_RETURNONLYFSDIRS
	, BIF_STATUSTEXT
	, BIF_NEWDIALOGSTYLE
	, BIF_EDITBOX
	, BIF_VALIDATE
	, BIF_BROWSEINCLUDEURLS
	, BIF_UAHINT
	, BIF_NONEWFOLDERBUTTON
	, BIF_NOTRANSLATETARGETS
	, BIF_BROWSEINCLUDEFILES
	, BIF_SHAREABLE
};


Define_static_constants(browse_flags);

void browseinfos_val( value v, BROWSEINFO *b ) {
	int n = 0;
	ZeroMemory(b,sizeof(BROWSEINFO));
	b->hwndOwner = Handle_val(Field(v,n++));
	b->lpszTitle = String_val(Field(v,n++));
	b->ulFlags = flags_val(Field(v,n++),browse_flags);
}

CAMLprim value sh_browse_for_folder( value infos ) {
	CAMLparam1(infos);
	CAMLlocal1(result);
	BROWSEINFO binfos;
	LPITEMIDLIST found;
	char name[MAX_PATH];
	browseinfos_val(infos,&binfos);

	binfos.pszDisplayName = name;

	found = SHBrowseForFolder(&binfos);
	if( found == NULL )
		result = Val_none;
	else
	{
		SHGetPathFromIDList(found,name);
		if( !*name )
			result = Val_none;
		else {
			result = alloc_tuple(2);		
			Field(result,0) = copy_string(name);
			Field(result,1) = Val_int(binfos.iImage);
			result = alloc_some(result);
		}
	}
	CAMLreturn(result);
}
