/* ************************************************************************ */
/*																			*/
/*	M3D-SE Vector Graphics Library											*/
/*	Julien Chevreux															*/
/*	(c)2004 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "../../common/wlist.h"

class EdgeList : public WList<Edge*> {
public:
	EdgeList() {
	}

	void ChangeLength( int delta ) {
		length += delta;
	}

	void SetQueue( Cel<Edge*> *c ) {
		queue = c;
	}

	Cel<Edge*> *AddIf( Edge *item, bool cf(Edge *,Edge*) ) {
		Cel<Edge*> *lproto = items, *last = NULL;
		while( lproto != NULL ) {
			if(cf(item, lproto->item)) {
				if(last == NULL)
					return AddFirst(item);
				Cel<Edge*> *lcel = new Cel<Edge*>(item,lproto);
				last->next = lcel;
				length++;
				return lcel;
			}
			last = lproto;
			lproto = lproto->next;
		}
		return Add(item);
	}

};

/* ************************************************************************ */
