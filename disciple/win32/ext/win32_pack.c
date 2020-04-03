/* OCaml-Win32
 * win32_pack.c
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
#include "win32_pack.h"

#define MAX_STRING_SIZE		0xffff // 65K

/********************************************************************/
/* Standard Pack & Unpack functions for core types */


/* Custom Msg */

static void
cmsg_finalize(value m)
{
	void *val = *(void**)Data_custom_val(m);
	free(val);
}

static struct custom_operations free_lparam_ops = {
	"CustomMSG",
	cmsg_finalize,
	custom_compare_default,
	custom_hash_default,
	custom_serialize_default,
	custom_deserialize_default
};

value pack_custom_message( UINT msg, WPARAM wparam, void *lparam ) {
    CAMLparam0();
    CAMLlocal3(val, v, m);

    val = alloc_tuple(4);
    Store_field(val, 0, Val_int(msg));
    v = copy_int32(wparam);
    Store_field(val, 1, v);
    v = copy_int32((LPARAM)lparam);
    Store_field(val, 2, v);
	m = alloc_custom(&free_lparam_ops,sizeof(void*),1,1);
	*(void**)Data_custom_val(m) = lparam;
	Store_field(val, 3, m);

    CAMLreturn(val);
}

/********************************************************************/
/* STD unpack */

UNPACK(int)
	result = Val_int(val);
ENDUNPACK

UNPACK(bool)
	result = Val_bool(val!=0);
ENDUNPACK

UNPACK(int32)
	result = copy_int32(val);
ENDUNPACK

UNPACK(pointlh)
	POINT p;
	p.x = GET_X_LPARAM(val);
	p.y = GET_Y_LPARAM(val);
	result = alloc_point(p);
ENDUNPACK

UNPACK(string)
	result = copy_string((char*)val);
ENDUNPACK

UNPACK(color)
	result = alloc_color(val);
ENDUNPACK

UNPACK(handle)
	result = alloc_handle((void*)val);
ENDUNPACK

/********************************************************************/
/* STD pack */

PACK(wint,val)
	WPACK( Int_val(val) );
ENDPACK

PACK(wbool,val)
	WPACK( Bool_val(val) );
ENDPACK

PACK(whandle,val)
	WPACK( (WPARAM)Handle_val(val) );
ENDPACK

PACK(lrect,vrect)	
	ALLOC(RECT,r);
	rect_val(vrect,r);
	LPACK_FREE(r);
ENDPACK

PACK(lint32,val)
	LPACK( Int32_val(val) );
ENDPACK

PACK(lint,val)
	LPACK( Int_val(val) );
ENDPACK

PACK(lbool,val)
	LPACK( Bool_val(val) );
ENDPACK

PACK(lcolor,col)
	LPACK( color_val(col) );
ENDPACK

PACK(lpointlh,vp)
	POINT p = point_val(vp);
	LPACK(  (((WORD)p.y) << 16) + (WORD)p.x );
ENDPACK

PACK(lhandle,h)
	LPACK( (LPARAM)Handle_val(h) );
ENDPACK

/********************************************************************/
/* Special */

CAMLprim value unpack_lparam( value sender, value msgcont, value unpack ) {
	CAMLparam3(sender,msgcont,unpack);
	CAMLlocal1(result);

	callback(sender,msgcont);
	result = callback(unpack,Field(msgcont,2));

	CAMLreturn(result);
}

CAMLprim pack2( value msg, value wparam, value lparam ) {
	CAMLparam3(msg,wparam,lparam);
	CAMLlocal2(result,wmsg);
	
	result = callback(lparam,msg);
	wmsg = callback(wparam,msg);
	Store_field(result,1,Field(wmsg,1));

	CAMLreturn(result);
}

CAMLprim pack_lemptystring( value msg ) {
	CAMLparam1(msg);
	CAMLlocal1(result);
	result = pack_custom_message(Int_val(msg),0,malloc(MAX_STRING_SIZE));
	CAMLreturn(result);
}

CAMLprim pack_loption( value msg, value val, value pack ) {
	CAMLparam3(msg,val,pack);
	CAMLlocal1(result);
	if( Is_some(val) )
		result = callback2(pack,msg,Value_of(val));
	else
		result = pack_message(Int_val(msg),0,0);
	CAMLreturn(result);
}

/********************************************************************/
