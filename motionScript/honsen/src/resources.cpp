/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "resources.h"
#include "../common/mtw.h"
#include "../common/wmap.h"
/* ------------------------------------------------------------------------ */

typedef void *(*loader)( MTW *t, void *custom );

struct _resources {
	MTW *root;
	WMap<unsigned int, MTW *> tree;
};

/* ------------------------------------------------------------------------ */

static void res_tree_init( resources *r, MTW *t, MTW *parent ) {
	if( t->IsGroup() ) {
		FOREACH(MTW*,*t->childs);
			res_tree_init(r,item,t);
		ENDFOR;
	} else if( t->tag == MAKETAG(rsid) )
		r->tree.Add(t->number,parent);
}

/* ------------------------------------------------------------------------ */

static MTW *res_find_tree( const char *name, MTW *t, MTW *parent ) {
	if( t->IsGroup() ) {
		FOREACH(MTW*,*t->childs);
			MTW *r = res_find_tree(name,item,t);
			if( r != NULL )
				return r;
		ENDFOR;
	} else if( t->tag == MAKETAG(RSNM) && strcmp(name,(char*)t->data) == 0 )
		return parent;
	return NULL;
}

/* ------------------------------------------------------------------------ */

MTW *res_find_byname( resources *r, const char *name ) {
	return res_find_tree(name,r->root,NULL);
}

/* ------------------------------------------------------------------------ */

resources *res_init( MTW *m ) {
	resources *r = new resources;
	r->root = m;
	if( m->IsGroup() )
		res_tree_init(r,m,NULL);
	r->tree.Optimize();
	Cache::Init();
	return r;
}

/* ------------------------------------------------------------------------ */

MTW *res_find( resources *r, unsigned int rsid ) {
	return r->tree.Find(rsid);
}

/* ------------------------------------------------------------------------ */

void res_destroy( resources *r ) {
	delete r;
	Cache::Destroy();
}

/* ************************************************************************ */
