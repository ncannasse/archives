/* **************************************************************************** */
/*																				*/
/*	VFS - Dragoon3 Virtual File System access									*/
/*																				*/
/* **************************************************************************** */
#include "vfs.h"

#define Is_some(v) ((v) != Val_int(0))
#define Value_of(v) (Field((v), 0))

extern "C" {
#	include <caml/mlvalues.h>
#	include <caml/alloc.h>
#	include <caml/callback.h>
};

 FTYPE
Vfs::get_file_type( FID id ) {
	static value *func = caml_named_value("vfs.get_file_type");
	return Int_val(callback(*func,Val_int(id)));
}

WString
Vfs::get_file_path( FID id ) {
	static value *func = caml_named_value("vfs.get_file_path");
	return String_val(callback(*func,Val_int(id)));
}

WString
Vfs::ftype_to_string( FTYPE ft ) {
	char buf[4];
	buf[0] = ft&255;
	buf[1] = (ft>>8)&255;
	buf[2] = ft>>16;
	buf[3] = 0;
	return buf;
}

FTYPE
Vfs::ftype( WString s ) {
	return s[0]+(s[1]<<8)+(s[2]<<16);
}

bool
Vfs::get_hlink( FID id, WString *filename ) {
	static value *func = caml_named_value("vfs.get_hlink");
	value result = callback(*func,Val_int(id));
	if( Is_some(result) ) {
		*filename = String_val(Value_of(result));
		return true;
	}
	return false;
}

FID
Vfs::find_hlink( WString filename ) {
	static value *func = caml_named_value("vfs.find_hlink");
	value result = callback(*func,copy_string(filename.c_str()));
	if( Is_some(result) )
		return Int_val(Value_of(result));
	else
		return 0;
}



/* **************************************************************************** */