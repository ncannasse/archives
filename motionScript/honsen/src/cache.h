/* ************************************************************************ */
/*																			*/
/*	Pointer Cache															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "../common/wlist.h"
#include "../common/wmap.h"
#include "../common/context.h"

class MTW;

template<class T> struct CacheItem {
	T* data;
	int refcount;
	int weakcount;
	void (*destructor)( T* );
	static void fdestructor( T* data ) { delete data; }
	CacheItem( T* data ) : data(data), refcount(0), weakcount(0), destructor(fdestructor) { }
	~CacheItem() { if( destructor ) destructor(data); }
};


template<class T> class Weak;

#ifdef _DEBUG
	extern _context *weak_context;
#	define WEAKCACHE	((WList<CacheItem<void>*>*)context_get_data(weak_context))
#	define WeakRelease { res->weakcount--; if( !res->data && !res->weakcount ) WEAKCACHE->Delete((CacheItem<void>*)res); }
#else
#	define WeakRelease { res->weakcount--; if( !res->data && !res->weakcount ) delete res; }
#endif

template<class T> class Cached {
private:
	CacheItem<T> *res;
	friend Weak<T>;
public:
	Cached( CacheItem<T> *res ) : res(res) { }
	Cached() : res(NULL) { }
	Cached( const Cached<T> &x ) : res(x.res) { if( res ) res->refcount++; }
	~Cached() { if( res ) res->refcount--; }
	Cached<T> & operator = ( const Cached<T> &x ) { if( res ) res->refcount--; res = x.res; if( res ) res->refcount++; return *this; }
	bool operator == ( const Cached<T> &x ) { return res && res == x.res; }
	void Release() { if( res ) res->refcount--; res = NULL; }
	bool Available() { return res != NULL; }
	T *Data() { return res?res->data:NULL; }
};

template<class T> class Weak {
private:
	CacheItem<T> *res;
	static void _release( CacheItem<void> *res );
public:
	Weak( CacheItem<T> *res ) : res(res) { }
	Weak( const Cached<T> &x ) : res(x.res) { if( res ) res->weakcount++; }
	Weak() : res(NULL) { }
	Weak( const Weak<T> &x ) : res(x.res) { if( res ) res->weakcount++; }
	~Weak() { if( res ) WeakRelease; }
	Weak<T> & operator = ( const Weak<T> &x ) { if( res ) WeakRelease; res = x.res; if( res ) res->weakcount++; return *this; }
	bool operator == ( const Cached<T> &x ) { return res && res == x.res; }
	void Release() { if( res ) WeakRelease; res = NULL; }
	bool Available() { return res != NULL && res->data != NULL; }
	Cached<T> Make() { if( res && res->data ) { res->refcount++; return Cached<T>(res); } else return Cached<T>(); }
	T *Data() { return res?res->data:NULL; }
};

extern _context *cache_context;

namespace Cache {

	#define CACHEDATA ((WMap< MTW *, CacheItem<void>* >*)context_get_data(cache_context))

	template<class T> Cached<T> Find( MTW *key ) {
		return Cached<T>( (CacheItem<T>*)CACHEDATA->Find(key) );
	}

	template<class T> Cached<T> Make( MTW *key, T* data ) {
		if( key == NULL || data == NULL )
			return Cached<T>();
		CacheItem<T> *res = (CacheItem<T>*)CACHEDATA->Find(key);
		if( res == NULL ) {
			res = new CacheItem<T>(data);
			CACHEDATA->Add(key,(CacheItem<void>*)res);
		}
		res->refcount++;
		return Cached<T>(res);
	}

	template<class T> Weak<T> MakeWeak( MTW *key, T* data ) {
		if( data == NULL )
			return Weak<T>();
		CacheItem<T> *res = (CacheItem<T>*)CACHEDATA->Find(key);
		if( res == NULL ) {
			res = new CacheItem<T>(data);
			CACHEDATA->Add(key,(CacheItem<void>*)res);
		}
		res->weakcount++;
		return Weak<T>(res);
	}

	void Init();
	bool Garbage();
	void GarbageAll();
	void Destroy();
};

/* ************************************************************************ */
