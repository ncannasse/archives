/* OCaml-Win32
 * win32_comctrl.c
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
#include <commctrl.h>
/********************************************************************/
/* Common Controls Library */

#define CCS_NOHILITE 0x10

static DWORD comctrl_style_options[] = {
	CCS_ADJUSTABLE
  , CCS_BOTTOM
  , CCS_NODIVIDER
  , CCS_NOHILITE
  , CCS_NOMOVEY
  , CCS_NOPARENTALIGN
  , CCS_NORESIZE
  , CCS_TOP
};
Define_static_constants(comctrl_style_options);

CAMLprim value init_common_controls( value _ ) {
	InitCommonControls();
	return Val_unit;
}

CAMLprim value comctrl_style_of_option( value option ) {
	return copy_int64((int64)Constant_val(option, comctrl_style_options));
}

void raise_comctrl_error( const char *str ) {
    value *e = caml_named_value("win32 common control error");
	raise_with_arg(*e, copy_string(str));	
}

CAMLprim value deliver_wm_notify(value handler, value wnd, value msg)
{
    CAMLparam3(handler, wnd, msg);
    CAMLlocalN(args, 5);
    CAMLlocal1(result);
	NMHDR *hdr = (NMHDR*)Lparam(msg);

    args[0] = wnd;
    args[1] = msg;
    args[2] = Val_int(hdr->code);
    args[3] = Val_int(hdr->idFrom);
    args[4] = alloc_handle(hdr->hwndFrom);
    result = callbackN(handler, 5, args);
    CAMLreturn(result);
}


/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
// IMAGE LIST

static DWORD img_color_flag [] = {
	ILC_COLOR
  , ILC_COLOR4
  , ILC_COLOR8
  , ILC_COLOR16
  , ILC_COLOR24
  , ILC_COLOR32
  , ILC_COLORDDB
};
Define_static_constants(img_color_flag);


static DWORD img_draw_flag [] = {
	ILD_BLEND25
  , ILD_FOCUS
  , ILD_BLEND50
  , ILD_SELECTED
  , ILD_BLEND
  , ILD_MASK
  , ILD_NORMAL
  , ILD_TRANSPARENT
};
Define_static_constants(img_draw_flag);

CAMLprim value imagelist_create( value init ) {
	CAMLparam1(init);
	CAMLlocal1(result);

	SIZE s = size_val(Field(init,0));
	UINT color = Constant_val(Field(init,1),img_color_flag);
	BOOL masked = Bool_val(Field(init,2));
	int initial = Int_val(Field(init,3));
	int grow = Int_val(Field(init,4));

	HIMAGELIST img = ImageList_Create(s.cx,s.cy,color | (masked?ILC_MASK:0),initial,grow);
	if( img == NULL )
		raise_comctrl_error("imagelist_create");
	result = alloc_handle(img);

	CAMLreturn(result);
};

CAMLprim value imagelist_destroy( value img ) {
	if( !ImageList_Destroy(Handle_val(img)) )
		raise_comctrl_error("imagelist_destroy");
	return Val_unit;
};

CAMLprim value imagelist_add( value imglist, value image, value mask ) {
	int i = ImageList_Add(Handle_val(imglist),Handle_val(image),Is_some(mask)?Handle_val(Value_of(mask)):NULL);
	if( i == -1 )
		raise_comctrl_error("imagelist_add");
	return Val_int(i);
}

CAMLprim value imagelist_addmasked( value imglist, value image, value maskcolor ) {
	int i = ImageList_AddMasked(Handle_val(imglist),Handle_val(image),color_val(maskcolor));
	if( i == -1 )
		raise_comctrl_error("imagelist_addmasked");
	return Val_int(i);
}

CAMLprim value imagelist_replace( value imglist, value index, value image, value mask ) {
	if( !ImageList_Replace(Handle_val(imglist),Int_val(index),Handle_val(image),Is_some(mask)?Handle_val(Value_of(mask)):NULL) )
		raise_comctrl_error("imagelist_replace");
	return Val_unit;
}

CAMLprim value imagelist_replaceicon( value imglist, value index, value icon ) {
	int i = ImageList_ReplaceIcon(Handle_val(imglist),Int_val(index),Handle_val(icon));
	if( i == -1 )
		raise_comctrl_error("imagelist_replaceicon");
	return Val_unit;
}

CAMLprim value imagelist_remove( value imglist, value index ) {
	if( !ImageList_Remove(Handle_val(imglist),Int_val(index)) )
		raise_comctrl_error("imagelist_remove");
	return Val_unit;
}

CAMLprim value imagelist_geticon( value imglist, value index, value flags ) {	
	HICON ico = ImageList_GetIcon(Handle_val(imglist),Int_val(index),flags_val(flags,img_draw_flag));
	if( ico == NULL )
		raise_comctrl_error("imagelist_geticon");
	return alloc_handle(ico);
}

CAMLprim value imagelist_setbkcolor( value imglist, value color ) {
	ImageList_SetBkColor(Handle_val(imglist), Is_some(color)?color_val(Value_of(color)):CLR_NONE);
	return Val_unit;
}

CAMLprim value imagelist_getbkcolor( value imglist ) {
	CAMLparam1(imglist);
	CAMLlocal1(result);

	COLORREF c = ImageList_GetBkColor(Handle_val(imglist));
	if( c == CLR_NONE )
		CAMLreturn(Val_none);

	result = alloc_color(c);
	result = alloc_some(result);
	CAMLreturn(result);
}

CAMLprim value imagelist_setoverlayimage( value imglist, value image, value overlay ) {
	if( !ImageList_SetOverlayImage(Handle_val(imglist),Int_val(image),Int_val(overlay)) )
		raise_comctrl_error("imagelist_setoverlayimage");
	return Val_unit;
}

CAMLprim value imagelist_getimagecount( value imglist ) {
	return Val_int( ImageList_GetImageCount(Handle_val(imglist)) );
}

CAMLprim value imagelist_seticonsize( value imglist, value size ) {
	SIZE s = size_val(size);
	if( !ImageList_SetIconSize(Handle_val(imglist),s.cx,s.cy) )
		raise_comctrl_error("imagelist_seticonsize");
	return Val_unit;
}

CAMLprim value imagelist_dragshownolock( value show ) {
	if( !ImageList_DragShowNolock(Bool_val(show)) )
		raise_comctrl_error("imagelist_dragshownolock");
	return Val_unit;
}

CAMLprim value imagelist_setdragcursorimage( value imglist, value index, value pos ) {
	POINT p = point_val(pos);
	if( !ImageList_SetDragCursorImage(Handle_val(imglist),Int_val(index),p.x,p.y) )
		raise_comctrl_error("imagelist_setdragcursorimage");
	return Val_unit;
}

CAMLprim value imagelist_getdragimage( value _ ) {
	CAMLparam1(_);
	CAMLlocal1(result);

	POINT a;
	POINT b;
	HIMAGELIST h = ImageList_GetDragImage(&a,&b);
	if( h == NULL )
		raise_comctrl_error("imagelist_getdragimage");

	result = alloc_tuple(3);
	Field(result,0) = alloc_point(a);
	Field(result,1) = alloc_point(b);
	Field(result,2) = alloc_handle(h);
	CAMLreturn(result);
}

CAMLprim value imagelist_begindrag( value imglist, value index, value pos ) {
	POINT p = point_val(pos);
	if( !ImageList_BeginDrag(Handle_val(imglist),Int_val(index),p.x,p.y) )
		raise_comctrl_error("imagelist_begindrag");
	return Val_unit;
}

CAMLprim value imagelist_enddrag( value _ ) {
	ImageList_EndDrag();
	return Val_unit;
}

CAMLprim value imagelist_dragmove( value pos ) {
	POINT p = point_val(pos);
	if( !ImageList_DragMove(p.x,p.y) )
		raise_comctrl_error("imagelist_dragmove");
	return Val_unit;
}

CAMLprim value imagelist_dragenter( value wnd, value pos ) {
	POINT p = point_val(pos);
	if( !ImageList_DragEnter(Handle_val(wnd),p.x,p.y) )
		raise_comctrl_error("imagelist_dragenter");
	return Val_unit;
}

CAMLprim value imagelist_dragleave( value wnd ) {
	if( !ImageList_DragLeave(Handle_val(wnd)) )
		raise_comctrl_error("imagelist_dragleave");
	return Val_unit;
}

/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
// TREE VIEW

static DWORD treeview_style_options[] = {
	TVS_HASBUTTONS
	, TVS_HASLINES
	, TVS_LINESATROOT
	, TVS_EDITLABELS
	, TVS_DISABLEDRAGDROP
	, TVS_SHOWSELALWAYS
	, TVS_RTLREADING
	, TVS_NOTOOLTIPS
	, TVS_CHECKBOXES
	, TVS_TRACKSELECT
	, TVS_SINGLEEXPAND
	, TVS_INFOTIP
	, TVS_FULLROWSELECT
	, TVS_NOSCROLL
	, TVS_NONEVENHEIGHT
};
Define_static_constants(treeview_style_options);

static DWORD tv_expand [] = {
	TVE_COLLAPSE
  , TVE_COLLAPSERESET
  , TVE_EXPAND
  , TVE_TOGGLE
};
Define_static_constants(tv_expand);

static DWORD tv_selstyle [] = {
	TVGN_CARET
  , TVGN_DROPHILITE
  , TVGN_FIRSTVISIBLE
};
Define_static_constants(tv_selstyle);

UINT tvstate_val( value v, UINT *state ) {
	UINT mask = 0;
	int n = 0;
	*state = 0;

	if( Is_some(Field(v,n++)) ) {
		BOOL b = Bool_val(Value_of(Field(v,n-1)));
		*state |= TVIS_BOLD*b;
		mask |= TVIS_BOLD;
	}

	if( Is_some(Field(v,n++)) ) {
		BOOL b = Bool_val(Value_of(Field(v,n-1)));
		*state |= TVIS_CUT*b;
		mask |= TVIS_CUT;
	}

	if( Is_some(Field(v,n++)) ) {
		BOOL b = Bool_val(Value_of(Field(v,n-1)));
		*state |= TVIS_DROPHILITED*b;
		mask |= TVIS_DROPHILITED;
	}

	if( Is_some(Field(v,n++)) ) {
		BOOL b = Bool_val(Value_of(Field(v,n-1)));
		*state |= TVIS_EXPANDED*b;
		mask |= TVIS_EXPANDED;
	}

	if( Is_some(Field(v,n++)) ) {
		BOOL b = Bool_val(Value_of(Field(v,n-1)));
		*state |= TVIS_EXPANDEDONCE*b;
		mask |= TVIS_EXPANDEDONCE;
	}

	if( Is_some(Field(v,n++)) ) {
		BOOL b = Bool_val(Value_of(Field(v,n-1)));
		*state |= TVIS_SELECTED*b;
		mask |= TVIS_SELECTED;
	}

	return mask;
}

value alloc_tvstate( UINT state, UINT mask ) {
	CAMLparam0();
	CAMLlocal1(result);
	int n = 0;

	result = alloc_tuple(6);

	if( mask & TVIS_BOLD)
		Field(result,n++) = alloc_some(Val_bool(state&TVIS_BOLD));
	else
		Field(result,n++) = Val_none;

	if( mask & TVIS_CUT)
		Field(result,n++) = alloc_some(Val_bool(state&TVIS_CUT));
	else
		Field(result,n++) = Val_none;

	if( mask & TVIS_DROPHILITED)
		Field(result,n++) = alloc_some(Val_bool(state&TVIS_DROPHILITED));
	else
		Field(result,n++) = Val_none;

	if( mask & TVIS_EXPANDED)
		Field(result,n++) = alloc_some(Val_bool(state&TVIS_EXPANDED));
	else
		Field(result,n++) = Val_none;

	if( mask & TVIS_EXPANDEDONCE)
		Field(result,n++) = alloc_some(Val_bool(state&TVIS_EXPANDEDONCE));
	else
		Field(result,n++) = Val_none;

	if( mask & TVIS_SELECTED)
		Field(result,n++) = alloc_some(Val_bool(state&TVIS_SELECTED));
	else
		Field(result,n++) = Val_none;

	CAMLreturn(result);
}

void tvitem_val( value v, TV_ITEM *item ) {	
	int n = 0;
	ZeroMemory(item,sizeof(TV_ITEM));

	if( Is_some(Field(v,n++)) ) {
		item->mask |= TVIF_CHILDREN;
		item->cChildren = Int_val(Value_of(Field(v,n-1)));
	}

	if( Is_some(Field(v,n++)) ) {
		item->mask |= TVIF_HANDLE;
		item->hItem = Handle_val(Value_of(Field(v,n-1)));
	}

	if( Is_some(Field(v,n++)) ) {
		item->mask |= TVIF_IMAGE;
		item->iImage = Int_val(Value_of(Field(v,n-1)));
	}

	if( Is_some(Field(v,n++)) ) {
		item->mask |= TVIF_SELECTEDIMAGE;
		item->iSelectedImage = Int_val(Value_of(Field(v,n-1)));
	}

	if( Is_some(Field(v,n++)) ) {
		item->mask |= TVIF_STATE;
		item->stateMask = tvstate_val(Value_of(Field(v,n-1)),&item->state);
	}
	
	if( Is_some(Field(v,n++)) ) {
		item->mask |= TVIF_TEXT;
		item->pszText = String_val(Value_of(Field(v,n-1)));
	}
}

value alloc_tvitem( TV_ITEM *item ) {
	CAMLparam0();
	CAMLlocal1(result);
	int n = 0;

	result = alloc_tuple(6);
	if( item->mask & TVIF_CHILDREN )
		Field(result,n++) = alloc_some(Val_int(item->cChildren>0?1:0));
	else
		Field(result,n++) = Val_none;

	if( item->mask & TVIF_HANDLE )
		Field(result,n++) = alloc_some(alloc_handle(item->hItem));
	else
		Field(result,n++) = Val_none;

	if( item->mask & TVIF_IMAGE )
		Field(result,n++) = alloc_some(Val_int(item->iImage));
	else
		Field(result,n++) = Val_none;

	if( item->mask & TVIF_SELECTEDIMAGE )
		Field(result,n++) = alloc_some(Val_int(item->iImage));
	else
		Field(result,n++) = Val_none;

	if( item->mask & TVIF_STATE )
		Field(result,n++) = alloc_some(alloc_tvstate(item->state,item->stateMask));
	else
		Field(result,n++) = Val_none;

	if( item->mask & TVIF_TEXT )
		Field(result,n++) = alloc_some(copy_string(item->pszText));
	else
		Field(result,n++) = Val_none;

	CAMLreturn(result);
}

CAMLprim value treeview_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, treeview_style_options));
}

CAMLprim value get_tvi_root( value _ )
{
	return alloc_handle(TVI_ROOT);
}

HTREEITEM tv_is_insertafter_val( value v ) {
	switch( Val_int(v) ) {
	case 0:
		return TVI_FIRST;
	case 1:
		return TVI_LAST;
	case 2:
		return TVI_SORT;
	}
	return NULL;
}

CAMLprim value issue_tvm_insertitem( value sender, value tvis ) {
	CAMLparam2(sender,tvis);
	CAMLlocal1(result);
	TV_INSERTSTRUCT is;
	is.hParent = Is_some(Field(tvis,0))?Handle_val(Value_of(Field(tvis,0))):NULL;	
	is.hInsertAfter = tv_is_insertafter_val(Field(tvis,1));
	tvitem_val(Field(tvis,2),&is.item);

	result = pack_message(TVM_INSERTITEM,0,(LPARAM)&is);
	result = callback(sender,result);
	if( !Int32_val(result) )
		raise_comctrl_error("issue_tvm_insertitem");
	result = alloc_handle( (void*)Int32_val(result) );

	CAMLreturn(result);
}

CAMLprim value issue_tvm_getitem( value sender, value item ) {
	CAMLparam2(sender,item);
	CAMLlocal1(result);
	TV_ITEM infos;
	result = pack_message(TVM_GETITEM,0,(LPARAM)&infos);
	callback(sender,result);
	result = alloc_tvitem(&infos);
	CAMLreturn(result);
}

CAMLprim value issue_tvm_hittest( value sender, value pos ) {
	CAMLparam2(sender,pos);
	CAMLlocal1(result);
	TVHITTESTINFO infos;
	infos.pt = point_val(pos);
	result = pack_message(TVM_HITTEST,0,(LPARAM)&infos);
	callback(sender,result);
	if( infos.hItem == NULL )
		CAMLreturn(Val_none);
	result = alloc_some( alloc_handle(infos.hItem) );
	CAMLreturn(result);
}


//--------------------------------------------------------------------
// PACK & UNPACK

UNPACK(nm_treeview)
	result = alloc_tuple(3);
	Field(result,0) = alloc_tvitem(&((NM_TREEVIEW*)val)->itemNew);
	Field(result,1) = alloc_tvitem(&((NM_TREEVIEW*)val)->itemOld);
	Field(result,2) = alloc_point(((NM_TREEVIEW*)val)->ptDrag);
ENDUNPACK

PACK(ltvitem,vtv)
	ALLOC(TV_ITEM,tv);
	tvitem_val(vtv,tv);
	LPACK_FREE(tv);
ENDPACK

PACK(wtvexpand,ve)
	WPACK( Constant_val(ve,tv_expand) );
ENDPACK

PACK(wtvsil,vs)
	WPACK( Int_val(vs)?TVSIL_STATE:TVSIL_NORMAL );  
ENDPACK

PACK(wtvselstyle,vs)
	WPACK( Constant_val(vs,tv_selstyle) );	
ENDPACK

/********************************************************************/
/********************************************************************/
/********************************************************************/
// HEADER CONTROL

static DWORD header_style_options[] = {
	HDS_BUTTONS,
	HDS_HIDDEN,
	HDS_HORZ
};
Define_static_constants(header_style_options);

static DWORD hd_formats[] = {
	HDF_CENTER,
	HDF_LEFT,
	HDF_RIGHT,
};
Define_static_constants(hd_formats);

static DWORD hd_hitflags[] = {
	HHT_NOWHERE
	, HHT_ONHEADER
	, HHT_ONDIVIDER
	, HHT_ONDIVOPEN
	, HHT_ABOVE
	, HHT_BELOW
	, HHT_TORIGHT
	, HHT_TOLEFT
};
Define_static_constants(hd_hitflags);

CAMLprim value header_style_of_option(value option)
{
    return copy_int64((int64)Constant_val(option, header_style_options));
}

void hditem_val( value v, HD_ITEM *h ) {
	int n = 0;
	int fmt = 0;
	ZeroMemory(h,sizeof(HD_ITEM));

	if( Is_some(Field(v,n++)) ) {
		h->cxy = Int_val(Value_of(Field(v,n-1)));
		h->mask |= HDI_WIDTH;
	}

	if( Is_some(Field(v,n++)) ) {
		h->cxy = Int_val(Value_of(Field(v,n-1)));
		h->mask |= HDI_HEIGHT;
	}

	if( Is_some(Field(v,n++)) ) {
		h->pszText = String_val(Value_of(Field(v,n-1)));
		h->cchTextMax = string_length(Value_of(Field(v,n-1)));
		h->mask |= HDI_TEXT;
		fmt = HDF_STRING;
	}

	if( Is_some(Field(v,n++)) ) {
		h->hbm = Handle_val(Value_of(Field(v,n-1)));
		h->mask |= HDI_BITMAP;
		fmt = fmt?HDF_OWNERDRAW:HDF_BITMAP;
	}

	if( Is_some(Field(v,n++)) ) {
		h->fmt = fmt | Constant_val(Value_of(Field(v,n-1)),hd_formats);
		h->mask |= HDI_FORMAT;
	}
}

/*
type hd_item = {
	hi_width : int option;
	hi_height : int option;
	hi_text : string option;
	hi_bitmap : hbitmap option;
	hi_format : hi_format option;	
}
*/
value alloc_hditem( HD_ITEM *h, int height ) {
	CAMLparam0();
	CAMLlocal1(result);
	int n = 0;	
	result = alloc_tuple(5);

	if( h->mask & HDI_WIDTH )
		Field(result,n++) = alloc_some(Val_int(h->cxy));
	else
		Field(result,n++) = Val_none;	

	if( h->mask & HDI_HEIGHT )
		Field(result,n++) = alloc_some(Val_int(h->cxy));
	else
		Field(result,n++) = (height >= 0)?alloc_some(Val_int(height)):Val_none;	
	
	if( h->mask & HDI_TEXT )
		Field(result,n++) = alloc_some(copy_string(h->pszText));
	else
		Field(result,n++) = Val_none;

	if( h->mask & HDI_BITMAP )
		Field(result,n++) = alloc_some(alloc_handle(h->hbm));
	else
		Field(result,n++) = Val_none;

	if( h->mask & HDI_FORMAT )
		Field(result,n++) = alloc_some(Val_constant(h->fmt,hd_formats));
	else
		Field(result,n++) = Val_none;

	CAMLreturn(result);
}

CAMLprim value issue_hdm_getitem( value sender, value index ) {
	CAMLparam2(sender,index);
	CAMLlocal1(result);
	HD_ITEM item;	
	int height;
	item.mask = HDI_HEIGHT;
	result = pack_message(HDM_GETITEM,Int_val(index),(LPARAM)&item);
	callback(sender,result);
	height = item.cxy;
	item.mask =	HDI_BITMAP | HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
	callback(sender,result);	
	result = alloc_hditem(&item,height);
	CAMLreturn(result);
}


CAMLprim value issue_hdm_layout( value sender, value vrect ) {
	CAMLparam2(sender,vrect);
	CAMLlocal1(result);
	WINDOWPOS pos;
	RECT rect;
	HD_LAYOUT layout;
	rect_val(vrect,&rect);
	layout.prc = &rect;
	layout.pwpos = &pos;
	result = pack_message(HDM_LAYOUT,0,(LPARAM)&layout);
	/*** check result TRUE/FALSE **/
	callback(sender,result);
	result = alloc_windowpos(&pos);
	CAMLreturn(result);
}

CAMLprim value issue_hdm_hittest( value sender, value vpoint ) {
	CAMLparam2(sender,vpoint);
	CAMLlocal1(result);
	HD_HITTESTINFO hit;
	int index;
	hit.pt = point_val(vpoint);
	result = pack_message(HDM_HITTEST,0,(LPARAM)&hit);
	index = Int_val(callback(sender,result));
	result = alloc_tuple(2);
	Field(result,0) = Val_constant(hit.flags,hd_hitflags);
	Field(result,1) = (index == -1)?Val_none:alloc_some(Int_val(hit.iItem));
	CAMLreturn(result);
}

PACK(lhditem,vhd)
	ALLOC(HD_ITEM,hd);
	hditem_val(vhd,hd);
	LPACK_FREE(hd);
ENDPACK

/********************************************************************/
