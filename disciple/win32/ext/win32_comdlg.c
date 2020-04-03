/* OCaml-Win32
 * win32_comdlg.c
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

/********************************************************************/
/* Common Dialogs Box Library */

static DWORD colorflag[] = {
	CC_FULLOPEN
  , CC_PREVENTFULLOPEN
  , CC_SHOWHELP
/*	CC_RGBINIT : use Nome/Some as cc_init
	CC_ENABLEHOOK
	CC_ENABLETEMPLATE
	CC_ENABLETEMPLATEHANDLE
*/
};
Define_static_constants(colorflag);

static DWORD openfileflag[] = {
	OFN_ALLOWMULTISELECT
  , OFN_CREATEPROMPT
  , OFN_EXPLORER
  , OFN_FILEMUSTEXIST
  , OFN_HIDEREADONLY
  , OFN_LONGNAMES
  , OFN_NOCHANGEDIR
  , OFN_NODEREFERENCELINKS
  , OFN_NOLONGNAMES
  , OFN_NONETWORKBUTTON
  , OFN_NOREADONLYRETURN
  , OFN_NOTESTFILECREATE
  , OFN_NOVALIDATE
  , OFN_OVERWRITEPROMPT
  , OFN_PATHMUSTEXIST
  , OFN_READONLY
  , OFN_SHAREAWARE
  , OFN_SHOWHELP
/*	OFN_ENABLEHOOK	
	OFN_ENABLETEMPLATE	
	OFN_ENABLETEMPLATEHANDLE
*/
};
Define_static_constants(openfileflag);

static DWORD comdlgerrors[] = {
	CDERR_DIALOGFAILURE,
	CDERR_FINDRESFAILURE,
	CDERR_INITIALIZATION,
	CDERR_LOADRESFAILURE,
	CDERR_LOADSTRFAILURE,
	CDERR_LOCKRESFAILURE,
	CDERR_MEMALLOCFAILURE,
	CDERR_MEMLOCKFAILURE,
	CDERR_NOHINSTANCE,
	CDERR_NOHOOK,
	CDERR_NOTEMPLATE,
	CDERR_REGISTERMSGFAIL,
	CDERR_STRUCTSIZE,
	PDERR_CREATEICFAILURE,
	PDERR_DEFAULTDIFFERENT,
	PDERR_DNDMMISMATCH,
	PDERR_GETDEVMODEFAIL,
	PDERR_INITFAILURE,
	PDERR_LOADDRVFAILURE,
	PDERR_NODEFAULTPRN,
	PDERR_NODEVICES,
	PDERR_PARSEFAILURE,
	PDERR_PRINTERNOTFOUND,
	PDERR_RETDEFFAILURE,
	PDERR_SETUPFAILURE,
	CFERR_MAXLESSTHANMIN,
	CFERR_NOFONTS,
	FNERR_BUFFERTOOSMALL,
	FNERR_INVALIDFILENAME,
	FNERR_SUBCLASSFAILURE,
	FRERR_BUFFERLENGTHZERO,
};
Define_static_constants(comdlgerrors);

void choosecolor_val( value ccval, CHOOSECOLOR *cc ) {
	int n = 0;
	cc->lStructSize = sizeof(CHOOSECOLOR);
	cc->hwndOwner = Handle_val(Field(ccval,n++));
	cc->hInstance = NULL;
	cc->Flags = 0;
	if( Is_some(Field(ccval,n++)) ) {
		cc->Flags |= CC_RGBINIT;
		cc->rgbResult = color_val(Value_of(Field(ccval,n-1)));
	}
	cc->Flags |= flags_val(Field(ccval,n++),colorflag);
	cc->lCustData = 0;
	cc->lpCustColors = NULL;
	cc->lpfnHook = NULL;
	cc->lpTemplateName = NULL;
}

void openfilename_val( value ofval, OPENFILENAME *ofn ) {
	int n = 0;	
	char *f;
	value list;
	ofn->lStructSize = sizeof(OPENFILENAME);
	ofn->hwndOwner = Handle_val(Field(ofval,n++));
	ofn->hInstance = NULL;
	ofn->lpstrFilter = malloc(1024);
	f = (char*)ofn->lpstrFilter;
	list = Field(ofval,n++);
	while( !Is_nil(list) ) {
		value s = Head(list);
		list = Tail(list);
		strcpy(f,String_val(Field(s,0)));
		f+=strlen(f)+1;
		strcpy(f,String_val(Field(s,1)));
		f+=strlen(f)+1;	
	}
	*f = 0;
	*(f+1) = 0;
	*(f+2) = 0;
	ofn->lpstrCustomFilter = NULL;
	ofn->nMaxCustFilter = 0;
	ofn->nFilterIndex = Int_val(Field(ofval,n++));
	ofn->lpstrFile = malloc(1024);
	if( Is_some(Field(ofval,n++)) )
		strcpy(ofn->lpstrFile,String_val(Value_of(Field(ofval,n-1))));
	else
		*ofn->lpstrFile = 0;
	ofn->nMaxFile = 1024;
	ofn->lpstrFileTitle = NULL;
	ofn->nMaxFileTitle = 0;
	if( Is_some(Field(ofval,n++)) )
		ofn->lpstrInitialDir = String_val(Value_of(Field(ofval,n-1)));
	else
		ofn->lpstrInitialDir = NULL;
	if( Is_some(Field(ofval,n++)) )
		ofn->lpstrTitle = String_val(Value_of(Field(ofval,n-1)));
	else
		ofn->lpstrTitle = NULL;
	ofn->Flags = flags_val(Field(ofval,n++),openfileflag);
	ofn->nFileOffset = 0;
	ofn->nFileExtension = 0;
	if( Is_some(Field(ofval,n++)) )
		ofn->lpstrDefExt = String_val(Value_of(Field(ofval,n-1)));
	else
		ofn->lpstrDefExt = NULL;
	ofn->lCustData = 0;
	ofn->lpfnHook = NULL;
	ofn->lpTemplateName = NULL;
}

void free_openfilename( OPENFILENAME *ofn ) {
	free((char*)ofn->lpstrFilter);
	free(ofn->lpstrFile);
}

void check_comdlgresult() {
	DWORD code = CommDlgExtendedError();
	if( code != 0 ) {
        value *e = caml_named_value("win32 common dialog error");
		raise_with_arg(*e, Val_constant(code,comdlgerrors));
	}
}


CAMLprim value choose_color( value ccval ) {
	CAMLparam1(ccval);
	CAMLlocal1(result);
	CHOOSECOLOR cc;
	BOOL b;
	choosecolor_val(ccval,&cc);
	b = ChooseColor(&cc);
	if( !b ) {
		check_comdlgresult();
		result = Val_none;
	}
	else
		result = alloc_some( alloc_color(cc.rgbResult) );
	CAMLreturn(result);
}

CAMLprim value get_open_file_name( value ofval ) {
	CAMLparam1(ofval);
	CAMLlocal1(result);
	OPENFILENAME ofn;
	BOOL b;
	openfilename_val(ofval,&ofn);
	b = GetOpenFileName(&ofn);
	if( !b ) {
		check_comdlgresult();
		result = Val_none;
	}
	else
		result = alloc_some( copy_string(ofn.lpstrFile) );
	free_openfilename(&ofn);
	CAMLreturn(result);
}

CAMLprim value get_save_file_name( value ofval ) {
	CAMLparam1(ofval);
	CAMLlocal1(result);
	OPENFILENAME ofn;
	BOOL b;
	openfilename_val(ofval,&ofn);
	b = GetSaveFileName(&ofn);
	if( !b ) {
		check_comdlgresult();
		result = Val_none;
	}
	else
		result = alloc_some( copy_string(ofn.lpstrFile) );
	free_openfilename(&ofn);
	CAMLreturn(result);
}
