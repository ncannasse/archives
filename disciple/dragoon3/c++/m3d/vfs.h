/* **************************************************************************** */
/*																				*/
/*	VFS - Dragoon3 Virtual File System access									*/
/*																				*/
/* **************************************************************************** */
#pragma once
#include <wstring.h>

typedef int		FID;
typedef long	FTYPE;

namespace Vfs {

	FTYPE get_file_type( FID id );
	WString get_file_path( FID id );
	WString ftype_to_string( FTYPE ft );
	FTYPE ftype( WString s );

	bool get_hlink( FID id, WString *filename );
	FID find_hlink( WString filename );

};

/* **************************************************************************** */