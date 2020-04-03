/* **************************************************************************** */
/*																				*/
/*   M3D Resources Management													*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#pragma once
#include <vfs.h>
#include <m3d.h>
#include <wbtree.h>

/* ---------------------------------------------------------------------------- */

#define CACHE_TRACE(msg)	//printf("%s\n",((WString)(msg)).c_str())
#define RES(T)				ResData<T>*
#define ANYRES				RES(void*)

/* ---------------------------------------------------------------------------- */

namespace M3D {


class ResCache {
	static int comp_int(void*,FID);
public:	
	static void *cache;
	static void Init();
	static void Close();
	static FID lastID;
};

/* ---------------------------------------------------------------------------- */

template<class T> class Res {

public:

	template <class T> struct FLoad {
		T (*func)(WIStream *);
		FLoad( T func( WIStream *) ) : func(func) {}
	};

	template <class T> struct FRelease {
		void (*func)( T );
		FRelease( void func( T ) ) : func(func) {}
	};

private:

	template <class T> struct ResData {
		FID id;
		FTYPE type;
		FLoad<T> load;
		FRelease<T> free;
		T data;
		int icount;
		ResData( FID id, FTYPE type, FLoad<T> load, FRelease<T> free, T data) : id(id),type(type),load(load),free(free),data(data),icount(0) { }
	};

	friend ResCache;

	RES(T) res;

	static WBTree<FID,ANYRES>* cachedata() {
		return (WBTree<FID,ANYRES>*)ResCache::cache;
	}

	static int comp_ptr(ANYRES a,void* b) {
		return (char*)a->data - (char*)b;
	}


	static void release( RES(T) res ) {
		if( res ) {
			res->icount--;
			if( res->icount <= 0 ) {
				CACHE_TRACE("Cache Dispose "+htos((int)res->data));
			}
		}
	}

	static int garbage_proc( ANYRES a, void *list ) {
		if( a->icount <= 0 )
			((WList<ANYRES>*)list)->Add(a);
		return -1;
	}

public:

	Res() {
		res = NULL;
	}

	Res( const Res<T> &r ) {
		res = r.res;
		if( res )
			res->icount++;
	}

	Res( RES(T) data ) {
		res = data;
		if( res )
			res->icount++;
	}

	Res<T> &
	operator = ( const Res<T> &r ) {
		release(res);
		res = r.res;
		if( res )
			res->icount++;
		return *this;
	}

	bool
	operator == ( const Res<T> &r) {
		if( !res || !r.res )
			return false;
		return res == r.res;
	}

	~Res() {
		release(res);
	}
	
	bool IsLoaded() {
		return res != NULL;
	}

	void Release() {
		release(res);
		res = NULL;
	}

	T Data() {
		return res?res->data:NULL;
	}

/* ---------------------------------------------------------------------------- */

	static bool
	Garbage() {
		WList<ANYRES> to_garbage;
		cachedata()->Find(garbage_proc,&to_garbage);
		FOREACH(ANYRES,to_garbage);
			CACHE_TRACE("Cache Garbage "+htos((int)item->data));
			item->free.func(item->data);
			if( cachedata() )
				cachedata()->Delete(item->id);
		ENDFOR;
		return to_garbage.Length() != 0;
	}

/* ---------------------------------------------------------------------------- */

	static Res<T>
	Load( FID id, DWORD type, FLoad<T> load, FRelease<T> free ) {
		RES(T) c = (RES(T))cachedata()->Find(id);
		ResCache::lastID = id;

		if( c != NULL ) {
			CACHE_TRACE("Cache Reuse "+Vfs::get_file_path(id));
			return Res<T>(c);
		}

		DWORD t = Vfs::get_file_type(id);	
		WString lasterror;

		if( t == 0 ) {
			lasterror = "File ID #"+itos(id)+" does not exists";
			M3D::Log(lasterror);
			return Res<T>();
		}
		if( t != type ) {
			lasterror = "File "+Vfs::get_file_path(id)+" has type "+Vfs::ftype_to_string(t)+" but is loaded with type "+Vfs::ftype_to_string(type);
			M3D::Log(lasterror);
			return Res<T>();
		}
		WString file;
		if( !Vfs::get_hlink(id,&file) ) {
			lasterror = "File "+Vfs::get_file_path(id)+" is not hardlinked";
			M3D::Log(lasterror);
			return Res<T>();
		}	

		filebuf filedata;
		filedata.open(file.c_str(),ios::binary | ios::in | ios::nocreate);

		WIStream data(&filedata);
		WString error;

		CACHE_TRACE("Cache Load "+Vfs::get_file_path(id));

		T result = load.func(&data);

		if( !result ) {
			lasterror = "An error occurs while loading "+Vfs::get_file_path(id)+" ("+error+")";
			Log(lasterror);
			return Res<T>();
		}

		CACHE_TRACE("Shared as "+htos((int)result));

		c = new ResData<T>(id,type,load,free,result);
		cachedata()->Add(id,(ANYRES)c);
		
		return Res<T>(c);
	}

/* ---------------------------------------------------------------------------- */

	static Res<T>
	Share( FID id, T data, FRelease<T> free ) {
		if( data == NULL )
			return Res<T>();

		RES(T) c = (RES(T))cachedata()->Find(id);
		if( c != NULL ) {
			CACHE_TRACE("Cache Reuse "+htos((int)c->data));
			return Res<T>(c);
		}

		c = (RES(T))cachedata()->Find(comp_ptr,(void*)data);
		if( c != NULL ) {
			CACHE_TRACE("Cache Reuse "+htos((int)c->data));
			return Res<T>(c);
		}

		CACHE_TRACE("Cache Share "+htos((int)data));
		c = new ResData<T>(id,0,NULL,free,data);
		cachedata()->Add(id,(ANYRES)c);

		return Res<T>(c);
	}

/* ---------------------------------------------------------------------------- */

};

#undef RES
#undef ANYRES

/* **************************************************************************** */
}; // namespace
/* **************************************************************************** */
