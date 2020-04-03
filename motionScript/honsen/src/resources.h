/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "cache.h"

class MTW;
typedef struct _resources resources;

resources *res_init( MTW *m );
MTW *res_find( resources *r, unsigned int rsid );
MTW *res_find_byname( resources *r, const char *name );
void res_destroy( resources *r );

template <class T, class P> Cached<T> res_load( resources *r, MTW *t, T *load( P *param, MTW *t ), P* param ) {
	if( t == NULL )
		return Cached<T>();
	Cached<T> c = Cache::Find<T>(t);
	if( c.Available() )
		return c;
	T *data = load(param,t);
	if( data == NULL )
		return Cached<T>();
	return Cache::Make<T>(t,data);
}

/* ************************************************************************ */
