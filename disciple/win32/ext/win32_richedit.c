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
#include <richedit.h>
#include "win32_pack.h"

#define FT_MATCHCASE 0x4
#define FT_WHOLEWORD 0x2

/********************************************************************/
/* User Interface Services: Controls: Edit Controls */

/* Styles */

static DWORD richedit_style_options[] = {
	ES_DISABLENOSCROLL,
	ES_EX_NOCALLOLEINIT,
	ES_NOIME,
	ES_SAVESEL,
	ES_SELFIME,
	ES_VERTICAL,
};
Define_static_constants(richedit_style_options);

static DWORD findtext_style_options[] = {
	FT_MATCHCASE,
	FT_WHOLEWORD,
};
Define_static_constants(findtext_style_options);

static DWORD findword_style_options[] = {
	WB_ISDELIMITER,
	WB_LEFT,
	WB_LEFTBREAK,
	WB_MOVEWORDLEFT,
	WB_MOVEWORDRIGHT,
	WB_RIGHT,
	WB_RIGHTBREAK,
	WB_CLASSIFY,
};
Define_static_constants(findword_style_options);

static DWORD charformat_style_options[] = {
	SCF_ALL,
	SCF_SELECTION,
	SCF_SELECTION | SCF_WORD // has to be combined !
};
Define_static_constants(charformat_style_options);

static DWORD selection_type[] = {
	SEL_EMPTY,
	SEL_TEXT,
	SEL_OBJECT,
	SEL_MULTICHAR,
	SEL_MULTIOBJECT,
};
Define_static_constants(selection_type);

static DWORD charformat_effect[] = {
	CFE_AUTOCOLOR,
	CFE_BOLD,
	CFE_ITALIC,
	CFE_STRIKEOUT,
	CFE_UNDERLINE,
	CFE_PROTECTED,
};
Define_static_constants(charformat_effect);

static DWORD richoption[] = {
	ECO_AUTOWORDSELECTION
  , ECO_AUTOVSCROLL
  , ECO_AUTOHSCROLL
  , ECO_NOHIDESEL
  , ECO_READONLY
  , ECO_WANTRETURN
  , ECO_SAVESEL
  , ECO_SELECTIONBAR
  , ECO_VERTICAL
};
Define_static_constants(richoption);

static DWORD rich_operation[] = {
	ECOOP_SET
  , ECOOP_OR
  , ECOOP_AND
  , ECOOP_XOR
};
Define_static_constants(rich_operation);

static DWORD paraformat_number[] = {
	0/*PFN_ZERO*/
  , PFN_BULLET
};
Define_static_constants(paraformat_number);

static DWORD paraformat_alignment[] = {
	PFA_LEFT
  , PFA_RIGHT
  , PFA_CENTER	
};
Define_static_constants(paraformat_alignment);

static DWORD undonameid[] = {
    UID_UNKNOWN
  , UID_TYPING
  , UID_DELETE
  , UID_DRAGDROP
  , UID_CUT
  , UID_PASTE
};
Define_static_constants(undonameid);


static DWORD textmode[] = {
	TM_PLAINTEXT
  , TM_RICHTEXT
  , TM_SINGLELEVELUNDO
  , TM_MULTILEVELUNDO
};
Define_static_constants(textmode);

CAMLprim value richedit_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, richedit_style_options));
}

/********************************************************************/
/* Allocs */

value list_queue(value v, value *l) {
	CAMLparam2(v,*l);
	CAMLlocal1(r);
	r = alloc(2,0);
	Store_field(r,0,v);
	Store_field(r,1,*l);
	*l = r;
	CAMLreturn(r);
}

value alloc_charrange( CHARRANGE *c ) {
    CAMLparam0();
    CAMLlocal1(val);

    val = alloc_tuple(2);
    Store_field(val, 0, Val_int(c->cpMin));
    Store_field(val, 1, Val_int(c->cpMax));
    CAMLreturn(val);
}

value alloc_charformat( CHARFORMAT *c ) {
	CAMLparam0();
	CAMLlocal2(val,v);

	val = alloc_tuple(8);
	v = Val_nil;
	if( (c->dwEffects & CFE_AUTOCOLOR) && (c->dwMask & CFM_COLOR) )
		list_queue(Val_constant(CFE_AUTOCOLOR,charformat_effect),&v);
	if( (c->dwEffects & CFE_BOLD) && (c->dwMask & CFM_BOLD) )
		list_queue(Val_constant(CFE_BOLD,charformat_effect),&v);
	if( (c->dwEffects & CFE_ITALIC) && (c->dwMask & CFM_ITALIC) )
		list_queue(Val_constant(CFE_ITALIC,charformat_effect),&v);
	if( (c->dwEffects & CFE_UNDERLINE) && (c->dwMask & CFM_UNDERLINE) )
		list_queue(Val_constant(CFE_UNDERLINE,charformat_effect),&v);
	if( (c->dwEffects & CFE_STRIKEOUT) && (c->dwMask & CFM_STRIKEOUT) )
		list_queue(Val_constant(CFE_STRIKEOUT,charformat_effect),&v);
	if( (c->dwEffects & CFE_PROTECTED) && (c->dwMask & CFM_PROTECTED) )
		list_queue(Val_constant(CFE_PROTECTED,charformat_effect),&v);
	Store_field(val,0,v);

	if( c->dwMask & CFM_SIZE )
		v = alloc_some(Val_int(c->yHeight));
	else
		v = Val_none;
	Store_field(val,1,v);

	if( c->dwMask & CFM_OFFSET )
		v = alloc_some(Val_int(c->yOffset));
	else
		v = Val_none;
	Store_field(val,2,v);

	if( c->dwMask & CFM_COLOR )
		v = alloc_some(alloc_color(c->crTextColor));
	else
		v = Val_none;
	Store_field(val,3,v);
	Store_field(val,4,alloc_charset(c->bCharSet));
    Store_field(val,5,Val_constant(c->bPitchAndFamily & 0x03, font_pitches));
    Store_field(val,6,Val_constant(c->bPitchAndFamily & 0xF0, font_families));
	
	if( c->dwMask & CFM_FACE )
		v = alloc_some(copy_string(c->szFaceName));
	else
		v = Val_none;
	Store_field(val,7,v);

	CAMLreturn(val);
}

void charrange_val( value val, CHARRANGE *c ) {
	c->cpMin = Int_val(Field(val,0));
	c->cpMax = Int_val(Field(val,1));
}

void charformat_val( value val, CHARFORMAT *c ) {
	value v;
	DWORD flags = flags_val(Field(val,0),charformat_effect);
	ZeroMemory(c,sizeof(CHARFORMAT));
	c->cbSize = sizeof(CHARFORMAT);
	c->dwEffects = flags;
	if( flags & CFE_AUTOCOLOR )
		c->dwMask |= CFM_COLOR;
	if( flags & CFE_BOLD )
		c->dwMask |= CFM_BOLD;
	if( flags & CFE_ITALIC )
		c->dwMask |= CFM_ITALIC;
	if( flags & CFE_UNDERLINE )
		c->dwMask |= CFM_UNDERLINE;
	if( flags & CFE_STRIKEOUT )
		c->dwMask |= CFM_STRIKEOUT;
	if( flags & CFE_PROTECTED )
		c->dwMask |= CFM_PROTECTED;

	v = Field(val,1);
	if( Is_some(v) ) {
		c->yHeight = Int_val(Value_of(v));
		c->dwMask |= CFM_SIZE;
	}

	v = Field(val,2);
	if( Is_some(v) ) {
		c->yOffset = Int_val(Value_of(v));
		c->dwMask |= CFM_OFFSET;
	}

	v = Field(val,3);
	if( Is_some(v) ) {
		c->crTextColor = color_val(Value_of(v));
		c->dwMask |= CFM_COLOR;
	}
	
	c->bCharSet = charset_val(Field(val,4));
	c->bPitchAndFamily =
		(BYTE)Constant_val(Field(val, 5), font_pitches) |
        (BYTE)Constant_val(Field(val, 6), font_families);
	
	v = Field(val,7);
	if( Is_some(v) ) {
		strcpy(c->szFaceName,String_val(Value_of(v)));
		c->dwMask |= CFM_FACE;
	}
}

void paraformat_val( value val, PARAFORMAT *p ) {
	value v;
	ZeroMemory(p,sizeof(PARAFORMAT));
	p->cbSize = sizeof(PARAFORMAT);

	v = Field(val,0);
	if( Is_some(v) ) {
		p->wNumbering = (WORD)Constant_val(Value_of(v),paraformat_number);
		p->dwMask |= PFM_NUMBERING;
	}

	v = Field(val,1);
	if( Is_some(v) ) {
		BOOL b = Bool_val(Field(Value_of(v),1));
		p->dxStartIndent = Int_val(Field(Value_of(v),0));		
		p->dwMask |= b?PFM_OFFSETINDENT:PFM_STARTINDENT;
	}

	v = Field(val,2);
	if( Is_some(v) ) {
		p->dxRightIndent = Int_val(Value_of(v));
		p->dwMask |= PFM_RIGHTINDENT;
	}

	v = Field(val,3);
	if( Is_some(v) ) {
		p->dxOffset = Int_val(Value_of(v));
		p->dwMask |= PFM_OFFSET;
	}

	v = Field(val,4);
	if( Is_some(v) ) {
		p->wAlignment = (WORD)Constant_val(Value_of(v),paraformat_alignment);
		p->dwMask |= PFM_ALIGNMENT;
	}
	
	v = Field(val,5);
	if( Is_some(v) ) {
		int i;
		v = Value_of(v);
		p->cTabCount = list_length(v);
		for (i=0; !Is_nil(v); i++,v = Tail(v))
			p->rgxTabs[i] = Int_val(Head(v));		
		p->dwMask |= PFM_TABSTOPS;
	}

}

void findtext_val( value val, FINDTEXT *f ) {
	charrange_val(Field(val,0),&f->chrg);
	f->lpstrText = String_val(Field(val,1));
}

/********************************************************************/
/* RICHEDIT unpack */

UNPACK(charrange)
	result = alloc_charrange((CHARRANGE*)val);
ENDUNPACK

UNPACK(special_findtextex_chrgText)
	FINDTEXTEX *t = (FINDTEXTEX*)val;
	result = alloc_charrange(&t->chrgText);
ENDUNPACK

UNPACK(selection_type)
	result = Val_constant(val,selection_type);
ENDUNPACK

UNPACK(charformat)
	result = alloc_charformat((CHARFORMAT*)val);
ENDUNPACK

UNPACK(richoptions)
	result = Alloc_flags(val,richoption);
ENDUNPACK

UNPACK(undonameid)
	result = Val_constant(val,undonameid);
ENDUNPACK

UNPACK(textmodes)
	result = Alloc_flags(val,textmode);
ENDUNPACK

/********************************************************************/
/* STD pack */

PACK(wfindtext_styles,ftstyles)
	DWORD val = flags_val(ftstyles,findtext_style_options);
	WPACK(val);
ENDPACK

PACK(wfindword_style,fwstyle)
	WPACK( Constant_val(fwstyle,findword_style_options) );
ENDPACK

PACK(wcharformat_style,cfstyle)
	WPACK( Constant_val(cfstyle,charformat_style_options) );
ENDPACK

PACK(wrich_operation,val)
	WPACK( Constant_val(val,rich_operation) );
ENDPACK

PACK(wtextmodes,modes)
	DWORD val = flags_val(modes,textmode);
	WPACK(val);
ENDPACK

// -------- L pack ------------------

PACK(lcharrange,vcrange)
	ALLOC(CHARRANGE,c);
	charrange_val(vcrange,c);
	LPACK_FREE(c);
ENDPACK

PACK(lfindtext,vftext)
	ALLOC(FINDTEXT,f);
	findtext_val(vftext,f);
	LPACK_FREE(f);
ENDPACK

PACK(lfindtextex,vftext)
	ALLOC(FINDTEXTEX,f);
	findtext_val(vftext,(FINDTEXT*)f);
	LPACK_FREE(f);
ENDPACK

PACK(lcharformat,vcformat)
	ALLOC(CHARFORMAT,c);
	charformat_val(vcformat,c);
	LPACK_FREE(c);
ENDPACK

PACK(lparaformat,vp)
	ALLOC(PARAFORMAT,p);
	paraformat_val(vp,p);
	LPACK_FREE(p);
ENDPACK

PACK(lrichoptions,options)
	LPACK( flags_val(options,richoption) );
ENDPACK

/********************************************************************/

CAMLprim value issue_em_gettextrange( value sender, value range ) {
	CAMLparam2(sender,range);
	CAMLlocal1(result);
	TEXTRANGE r;
	charrange_val(range,&r.chrg);
	r.lpstrText = malloc(1024);
	result = pack_message(EM_GETTEXTRANGE,0,(LPARAM)&r);
	callback(sender,result);
	result = copy_string(r.lpstrText);
	free(r.lpstrText);
	CAMLreturn(result);
}

int is_alphanum( char c ) {
	return( ( c >= 'a' && c <= 'z' ) ||
			( c >= '0' && c <= '9' ) ||
			( c >= 'A' && c <= 'Z' ) ||
			c == '_' );
}

int is_space( char c ) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

CAMLprim value richedit_colorize( value func, value hwnd, value range ) {
	CAMLparam3(hwnd,func,range);
	CAMLlocal2(result,strval);
	HANDLE h = Handle_val(hwnd);
	char *str;
	char *_str;	
	TEXTRANGE tr;
	CHARRANGE csav;
	CHARFORMAT format;
	int pos;
	int accu = -1;
	int type,ntype = 0;
	char last;

	charrange_val(range,&tr.chrg);
	if( tr.chrg.cpMin < 0 )
		tr.chrg.cpMin = 0;
	if( tr.chrg.cpMax < 0 )
		tr.chrg.cpMax = GetWindowTextLength(h);
	strval = alloc_string((tr.chrg.cpMax-tr.chrg.cpMin)*2);
	str = String_val(strval);
	_str = str;
	tr.lpstrText = str;
	pos = tr.chrg.cpMin;
	SendMessage(h,EM_EXGETSEL,0,(LPARAM)&csav);
	SendMessage(h,WM_SETREDRAW,FALSE,0);
 	SendMessage(h,EM_GETTEXTRANGE,0,(LPARAM)&tr);
	memset(&format,0,sizeof(CHARFORMAT));
	format.cbSize = sizeof(CHARFORMAT);
	format.dwMask = CFM_COLOR;
	result = Val_none;

	type = is_alphanum(*str)?0:(is_space(*str)?1:2);
	
	last = *str;
	while(last) {
		if( *str == '\n' ) pos--;
		ntype = is_alphanum(*str)?0:(is_space(*str)?1:2);
		accu++;		
		if( type != ntype || !*str) {
			char s = *str;
			*str = 0;
			result = callback3(func,Val_int(type),copy_string(str-accu),Val_int(pos));
			if( Is_some(result) ) {
				CHARRANGE sel;
				sel.cpMin = pos-accu;
				sel.cpMax = pos;
				*((COLORREF*)&format.crTextColor) = color_val(Value_of(result));
				SendMessage(h,EM_EXSETSEL,0,(LPARAM)&sel);
				SendMessage(h,EM_SETCHARFORMAT,SCF_SELECTION,(LPARAM)&format);
			}
			*str = s;
			accu = 0;
			type = ntype;
		}
		last = *str;
		str++;
		pos++;
	}
	

	SendMessage(h,EM_EXSETSEL,0,(LPARAM)&csav);
	SendMessage(h,WM_SETREDRAW,TRUE,0);	
	InvalidateRect(h,NULL,TRUE);
	String_val(strval) = _str;

	CAMLreturn(Val_unit);
}