/* **************************************************************************** */
/*																				*/
/*   M3D Resources Management													*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include "m3d_res.h"

namespace M3D {

/* ---------------------------------------------------------------------------- */

void *ResCache::cache = NULL;
FID ResCache::lastID = 0;

/* ---------------------------------------------------------------------------- */

int ResCache::comp_int(void *rdata,FID id) {
	return ((Res<void*>::ResData<void*>*)rdata)->id-id;
}

/* ---------------------------------------------------------------------------- */

void ResCache::Init() {
	Close();
	cache = new WBTree<void*,FID>(ResCache::comp_int);
}

/* ---------------------------------------------------------------------------- */

void ResCache::Close() {
	if( cache ) {
		while(true) 
			if( !Res<void*>::Garbage() )
				break;
		CACHE_TRACE("Free Cache");
		int count = ((WBTree<void*,FID>*)cache)->Count();
		delete ((WBTree<void*,FID>*)cache);		
#ifdef _DEBUG
		if( count > 0 )			
			MessageBox(NULL,("Some objects are style cached : "+itos(count)).c_str(),"DEBUG WARNING",MB_OK|MB_ICONWARNING|MB_TOPMOST);
#endif
		cache = NULL;
	}
}

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */