/* ****************************************************************	*/
/*																	*/
/*	class WList														*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard template list class								*/
/*	+ FOREACH macro													*/
/*																	*/
/* ****************************************************************	*/
#ifndef WLIST_H
#define WLIST_H

#ifndef NULL
#define NULL 0x00000000
#endif

template <class ITEM> class WList {

public:

	// a cell of the list
	template <class CITEM> struct Cel {
		CITEM		item;
		Cel<CITEM>	*next;
		Cel( CITEM item, Cel<CITEM> *next ) : item(item), next(next) { }
	};

	typedef Cel<ITEM> LCel;

	// constructor
	WList() {
		items = NULL;
		queue = NULL;
		length = 0;
	}

	// copy constructor
	WList( const WList<ITEM>&l ) {
		items = NULL;
		queue = NULL;
		length = 0;
		*this = l;
	}

	// destructor
	~WList() {
		Clean();
	}

	// copy operator
	const WList<ITEM>& operator = ( const WList<ITEM> &copy ) {
		Clean();
		Cel<ITEM> *lcursor = copy.items;
		while( lcursor != NULL ) {
			Add(lcursor->item);
			lcursor = lcursor->next;
		}
		return (*this);
	}

	// return the number of items %C1%
	int Length() {
		return length;
	}

	// true is the list contains at least one element, false otherwise %C1%
	bool Empty() {
		return (items == NULL);
	}

	// return the cell at specified index or NULL if not a valid index %CN%
	Cel<ITEM> *CelAtIndex( int index ) {
		Cel<ITEM> *lcursor = items;
		if( index < 0 )
			return NULL;
		while( index > 0 ) {
			if( lcursor != NULL )
				lcursor = lcursor->next;
			else
				return NULL;
			index--;
		}
		return lcursor;
	}

	// return the item at specified index of "NULL" item is not a valid index %CN%
	ITEM AtIndex( int index ) {
		Cel<ITEM> *c = CelAtIndex(index);
		return c==NULL?NULL:c->item;
	}

	// return last item
	ITEM Last() {
		return queue?queue->item:NULL;
	}

	// return first item
	ITEM First() {
		return items?items->item:NULL;
	}

	// return the first index where item specified can be found
	// or -1 if the item cannot be found %CN%
	int IndexOf( const ITEM &item ) {
		Cel<ITEM> *lcursor = items;
		int i = 0;

		while( lcursor != NULL ) {
			if( lcursor->item == item )
				return i;
			i++;
			lcursor = lcursor->next;
		}
		return -1;
	}

	// return true if the item specified can be found %CN%
	bool Exists( const ITEM &item ) {
		return IndexOf( item ) != -1;
	}

	// return the first cell of the list or NULL if empty %C1%
	Cel<ITEM> *Begin() {
		return items;
	}
		
	// return the last cell of the list or NULL if empty %C1%
	Cel<ITEM> *End() {
		return queue;
	}

	// reverse the list %CN%
	void Reverse() {
		int i,j;
		Cel<ITEM> *qtmp;
		Cel<ITEM> *litem;
		ITEM itmp;
		for(i=length; i>=2; i--) {
			qtmp = queue;
			litem = CelAtIndex(length-i);
			for(j=0; j<i-1; j++ ) {
				itmp = litem->item;
				litem->item = qtmp->item;
				qtmp->item = itmp;
				litem = litem->next;
			}
		}
	}

	// add the specified item to the list %C1%
	Cel<ITEM> *Add( const ITEM &item ) {
		Cel<ITEM> *lcel = new Cel<ITEM>(item,NULL);
		if( queue == NULL )
			items = lcel;
		else
			queue->next = lcel;
		queue = lcel;
		length++;
		return lcel;
	}

	// add the specified item to the list %C1%
	Cel<ITEM> *AddFirst( const ITEM &item ) {
		Cel<ITEM> *lcel = new Cel<ITEM>(item,items);
		if( items == NULL )				
			queue = lcel;
		items = lcel;
		length++;
		return lcel;
	}

	// insert the specified item at the specified position %CN%
	Cel<ITEM> *Insert( const ITEM &item, int pos ) {
		if( pos <= 0 || !items )
			return AddFirst(item);
		pos--;

		Cel<ITEM> *lproto = items;
		while( lproto != NULL && pos > 0 ) {
			lproto = lproto->next;
			pos--;
		}
		if( lproto == NULL )
			lproto = queue;
		Cel<ITEM> *lcel = new Cel<ITEM>(item,lproto->next);
		lproto->next = lcel;
		length++;
		return lcel;
	}

	// add the specified list to the list %CN%
	// returns number of items added
	int	Add( WList<ITEM> &l ) {
		int n = length;
		Cel<ITEM> *lcursor = l.items;
		while( lcursor != NULL ) {
			Add(lcursor->item);
			lcursor = lcursor->next;
		}
		return (length-n);
	}

	// pop first element, %C1%
	ITEM Pop() {
		if( items == NULL )
			return NULL;
		Cel<ITEM> *t = items;
		ITEM i = t->item;
		if( queue == items )
			queue = NULL;
		items = t->next;
		delete t;
		length--;
		return i;
	}

	// clear the list without deleting items %CN%
	void Clean() {
		Cel<ITEM> *lproto = items;
		while( lproto != NULL ) {
			items = lproto->next;
			length--;
			delete lproto;
			lproto = items;
		}
		queue = NULL;
	}

	// remove the specified item from the list
	// returns true if successfull or false if
	// the item hasn't been found %CN%
	bool Remove( const ITEM &item ) {
		return GenRemove(item,NULL);
	}

	// same as %Remove% but also delete the instance of item %CN%
	bool Delete( const ITEM &item ) {
		delete item;
		return GenRemove(item,NULL);
	}

	// same as %Clean% but also delete the instance of each item %CN%
	void Delete() {
		Cel<ITEM> *lproto = items;
		while( lproto != NULL ) {
			items = lproto->next;
			length--;
			delete lproto->item;
			delete lproto;
			lproto = items;
		}
		queue = NULL;
	}

	// same as %Delete% but delete the instance of item only if only present one time %CN%
	bool SafeDelete( const ITEM &item ) {
		bool b;
		bool r = GenRemove(item,&b);
		if( r && !b )
			delete item;
		return r;
	}

	// same as %Delete% but checks for multiple references of the same object %CN%
	void SafeDelete() {
		while( length > 0 )
			SafeDelete(items->item);
	}

protected:

	bool GenRemove( const ITEM &item, bool *check ) {	
		Cel<ITEM> *lproto = items;
		Cel<ITEM> *lprev = NULL;
		bool del = false;
		while( lproto != NULL ) {
			if( lproto->item == item ) {
				if( del ) {
					*check = true;
					return true;
				}
				if( lprev == NULL )
					items = lproto->next;
				else
					lprev->next = lproto->next;
				if( lproto == queue )
					queue = lprev;
				
				lprev = lproto->next;
				delete lproto;
				length--;
				del = true;
				if( check == NULL )
					return true;
				lproto = lprev;
			}
			else {
				lprev = lproto;
				lproto = lproto->next;
			}
		}
		if( del )
			*check = false;
		return del;
	}

	Cel<ITEM> *items; // list of items;
	Cel<ITEM> *queue; // queue of the list
	int length; // number of items in one list

};//class

#define FOREACH(type,list){ \
									WList< type >::LCel *__next = (list).Begin(); \
									type item; \
									while( __next != NULL ) \
									{ \
										item = __next->item; \
										__next = __next->next;
#define ENDFOR				}}

#endif
/* ****************************************************************	*/
