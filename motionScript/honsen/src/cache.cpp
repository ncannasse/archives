/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "cache.h"

_context *cache_context = context_new();
#ifdef _DEBUG
#include <stdio.h>
#include <windows.h>
_context *weak_context = context_new();
#endif

namespace Cache {

	typedef CacheItem<void>* ITEM;

	int cache_garbage_one( MTW *key, ITEM item, void *k ) {
		if( item->refcount <= 0 ) {
			*(MTW**)k = key;
			return 0;
		}
		return -1;
	}

	int cache_garbage( MTW *key, ITEM item, void *list ) {
		if( item->refcount <= 0 )
			((WList<MTW*>*)list)->Add(key);
		return -1;
	}

	void DoGarbage( MTW *item ) {
		ITEM it = CACHEDATA->Find(item);
		CACHEDATA->Remove(item);
		if( !it->weakcount ) {
			delete it;
			return;
		}
		delete it->data;
		it->data = NULL;
#ifdef _DEBUG
		WEAKCACHE->Add(it);
#endif
	}

	bool Garbage() {
		MTW *k = NULL;
		ITEM togarbage = CACHEDATA->Find(cache_garbage_one,&k);
		if( !togarbage )
			return false;
		DoGarbage(k);
		return true;
	}

	void GarbageAll() {
		bool rflag = false;
		WList<MTW*> togarbage;
		CACHEDATA->Find(cache_garbage,&togarbage);
		FOREACH(MTW*,togarbage);
			DoGarbage(item);
			rflag = true;
		ENDFOR;
		if( rflag )
			GarbageAll();
		CACHEDATA->Optimize();
	}

	void Init() {		
#		ifdef _DEBUG
		context_set_data(weak_context,new WList<CacheItem<void>*>());
#		endif
		context_set_data(cache_context,new WMap<MTW*,ITEM>());
	}

	void Destroy() {
#ifdef _DEBUG
		GarbageAll();
		if( CACHEDATA->Count() || WEAKCACHE->Length() ) {
			char buf[200];
			sprintf(buf,"Cache is not empty : %d (%d weak) items remaining",CACHEDATA->Count(),WEAKCACHE->Length());
			MessageBox(NULL,buf,"Error",MB_OK);
		}
#endif
		CACHEDATA->DeleteItemOnly();
		delete CACHEDATA;
#ifdef _DEBUG
		WEAKCACHE->Delete();
		delete WEAKCACHE;
#endif
	}

};

/* ************************************************************************ */
