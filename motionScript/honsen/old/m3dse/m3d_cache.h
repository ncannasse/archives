/* ************************************************************************ */
/*																			*/
/*	M3D-SE Pointer Cache													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "../common/wlist.h"
#include "../common/wbtree.h"
#include "m3d_context.h"

namespace M3D {

	template<class T> struct CacheItem {
		T* data;
		int refcount;
		int weakcount;
//		WList< CacheItem<void>* > deps;
		void (*destructor)( T* );
		static void fdestructor( T* data ) { delete data; }
		CacheItem( T* data ) : data(data), refcount(0), weakcount(0), destructor(fdestructor) { }
		~CacheItem() { /*deps.Delete();*/ if( destructor ) destructor(data); }
	};

	template<class T> class Weak;
#ifdef _DEBUG
#	define WEAKCACHE	((WList<CacheItem<void>*>*)c->weak_cache)
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
//		template <class ANY> void Depends( ANY *dep ) { if( res ) { res->deps.Add( (CacheItem<void>*)new CacheItem<ANY>(dep) ); } } 
	};

	template<class T> class Weak {
	private:
#ifdef _DEBUG
		Context *c;
#endif
		CacheItem<T> *res;
		static void _release( CacheItem<void> *res );
	public:
#ifdef _DEBUG
		Weak( Context *c, CacheItem<T> *res ) : c(c), res(res) { }
#else
		Weak( CacheItem<T> *res ) : res(res) { }
#endif
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

	namespace Cache {
		#define CACHEDATA ((WBTree< void *, CacheItem<void>* >*)c->cache)

		template<class T> Cached<T> Make( Context *c, T* data ) {
			if( data == NULL )
				return Cached<T>();
			CacheItem<T> *res = (CacheItem<T>*)CACHEDATA->Find(data);
			if( res == NULL ) {
				res = new CacheItem<T>(data);
				CACHEDATA->Add(data,(CacheItem<void>*)res);
			}
			res->refcount++;
			return Cached<T>(res);
		}

		template<class T> Weak<T> MakeWeak( Context *c, T* data ) {
			if( data == NULL )
				return Weak<T>();
			CacheItem<T> *res = (CacheItem<T>*)CACHEDATA->Find(data);
			if( res == NULL ) {
				res = new CacheItem<T>(data);
				CACHEDATA->Add(data,(CacheItem<void>*)res);
			}
			res->weakcount++;
#			ifdef _DEBUG
			return Weak<T>(c,res);
#			else
			return Weak<T>(res);
#			endif
		}
/*
		template<class A,class B> void Depends( A *a, B *b, bool parentClean = false ) {
			if( a == NULL || b == NULL )
				return;
			CacheItem<A> *res = (CacheItem<A>*)cachedata.Find(a);
			if( res == NULL ) {
				res = new CacheItem<A>(a);
				cachedata.Add(a,(CacheItem<void>*)res);
			}

			Cached<B> *bt = new Cached<B>(Make(b));
			res->deps.Add( (CacheItem<void>*)new CacheItem< Cached<B> >(bt) );
			if( parentClean )
				res->destructor = NULL;
		}
*/
		void Init( Context *c );
		bool Garbage( Context *c );
		void GarbageAll( Context *c );
		void Close( Context *c );
	};
};

/* ************************************************************************ */
