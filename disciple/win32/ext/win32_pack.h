/* OCaml-Win32
 * win32_pack.h
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

/********************************************************************/
/* Standard Pack & Unpack functions for core types */
/* Header file */

value pack_custom_message( UINT msg, WPARAM wparam, void *lparam );

#define UNPACK(x)	CAMLprim value unpack_##x( value _val ) { CAMLparam1(_val);CAMLlocal1(result); DWORD val = Int32_val(_val);
#define ENDUNPACK	CAMLreturn(result); }

#define PACK(x,param) CAMLprim value pack_##x( value msg, value param ) { CAMLparam2(msg,param); CAMLlocal1(result);
#define ENDPACK			CAMLreturn(result); }
#define ALLOC(type,var) type *var = (type*)malloc(sizeof(type))
#define WPACK(v) result = pack_message( Int_val(msg), v, 0 )
#define LPACK(v) result = pack_message( Int_val(msg), 0, v )
#define LPACK_FREE(v) result = pack_custom_message( Int_val(msg), 0, v )

/********************************************************************/
