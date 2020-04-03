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

	typedef  int		(*COMPARATOR)	( ITEM, ITEM );		// comparative function
	typedef  void*		(*ITERATOR)		( ITEM, void * );	// iterative function

	// a cell of the list
	template <class CITEM> struct Cel {
		CITEM		item;
		Cel<CITEM>	*next;
		Cel( CITEM item, Cel<CITEM> *next ) : item(item), next(next) { }
	};

private:

	static void *CopyList( ITEM i, void *p ) {
		WList<ITEM>*l = (WList<ITEM>*)p;
		l->Add(i);
		return NULL;
	}

public:

	// constructor
	WList()
		{
			items = NULL;
			queue = NULL;
			length = 0;
		}

	// copy constructor
	WList( const WList<ITEM>&l )
		{
			items = NULL;
			queue = NULL;
			length = 0;
			((WList<ITEM>*)&l)->Apply(CopyList,this);
		}

	// destructor
	~WList()
		{
			Clean();
		}

	// copy operator
	WList<ITEM>& operator = ( const WList<ITEM> &copy ) {
		Clean();
		((WList<ITEM>*)&copy)->Apply(CopyList,this);
		return (*this);
	}

	// return the number of items %C1%
	int Length()
		{
			return length;
		}

	// return the cell at specified index or NULL if not a valid index %CN%
	Cel<ITEM> *AtIndex( int Index )
		{
			Cel<ITEM> *lcursor = items;
			if( Index < 0 )
				return NULL;
			while( Index > 0 ) {
				if( lcursor != NULL )
					lcursor = lcursor->next;
				else
					return NULL;
				Index--;
			}
			return lcursor;
		}

	// return the item at specified index of "NULL" item is not a valid index %CN%
	ITEM ItemAtIndex( int Index )
		{
			Cel<ITEM> *c = AtIndex(Index);
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
	int IndexOf( ITEM Item )
		{
			Cel<ITEM> *lcursor = items;
			int i = 0;

			while( lcursor != NULL )
			{
				if( lcursor->item == Item )
					return i;
				i++;
				lcursor = lcursor->next;
			}
			return -1;
		}

	// return true if the item specified can be found %CN%
	bool Exists( ITEM Item )
		{
			return (IndexOf( Item ) != -1);
		}

	// return the first cell of the list or NULL if empty %C1%
	Cel<ITEM> *AtStart()
		{
			return items;
		}
		
	// return the last cell of the list or NULL if empty %C1%
	Cel<ITEM> *AtQueue()
		{
			return queue;
		}

	// sort list by Comp result %CN*N%
	void Sort( COMPARATOR Comp )
		{
			Cel<ITEM> *litem = items;
			Cel<ITEM> *titem;
			
			while( litem != NULL )
			{
				int pass = 0;

				titem = litem->next;
				while( titem != NULL )
				{
					if( Comp(litem->item,titem->item) > 0 )
					{
						ITEM it = litem->item;
						litem->item = titem->item;
						titem->item = it;
						pass = 1;
					}
					titem = titem->next;
				}
				if( !pass )
					break;
				litem = litem->next;
			}

	}

	// return the first item EQUAL for Comp with the specified item
	// or "NULL" item if this item cannot be found %CN%
	ITEM Equals( COMPARATOR Comp, ITEM Item )
		{
			Cel<ITEM> *litem = items;
			while( litem != NULL )
			{
				if( Comp(litem->item, Item) != NULL )
					return litem->item;
				litem = litem->next;
			}
			return NULL;
		}

	// apply Iterator with Param for each item until it returns not NULL
	// then returns the current item or "NULL" item if all calls returns
	// NULL. %CN%
	ITEM Find( ITERATOR Iterator, void *Param )
		{
			Cel<ITEM> *litem = items;
			while( litem != NULL )
			{
				if( Iterator((ITEM)litem->item, Param) != NULL )
					return (ITEM)litem->item;
				litem = litem->next;
			}
			return NULL;
		}
	
	// apply Iterator with Param for each item until it returns not NULL
	// then returns the returned value or Iterator or NULL either. %CN%
	void *Apply( ITERATOR Iterator, void *param )
		{
			Cel<ITEM> *litem = items;
			void *retvalue;
			while( litem != NULL )
			{
				if( (retvalue=Iterator(litem->item, param)) != NULL )
					return retvalue;
				litem = litem->next;
			}
			return NULL;
		}

	// reverse the list %CN%
	void Reverse()
		{
			long i,j;
			Cel<ITEM> *qtmp;
			Cel<ITEM> *litem;
			ITEM itmp;
			
			for(i=length; i>=2; i--)
			{
				qtmp = queue;
				litem = AtIndex(length-i);
				for(j=0; j<i-1; j++ )
				{
					itmp = litem->item;
					litem->item = qtmp->item;
					qtmp->item = itmp;

					litem = litem->next;
				}
			}
		}

	// add the specified item to the list %C1%
	Cel<ITEM> *Add( ITEM item )
		{
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
	Cel<ITEM> *AddFirst( ITEM item )
		{
			Cel<ITEM> *lcel = new Cel<ITEM>(item,items);
			if( items == NULL )				
				queue = lcel;
			items = lcel;
			length++;
			return lcel;
		}

	// insert the specified item at the specified position %CN%
	Cel<ITEM> *Insert( ITEM item, int pos )
		{
			if( pos <= 0 )
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
	int	Add( WList<ITEM> &l )
		{
			int n = length;
			l.Apply(CopyList,this);
			return (length-n);
		}

	// clear the list without deleting items %CN%
	void Clean()
		{
			Cel<ITEM> *lproto = items;

			while( lproto != NULL )
			{
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
	bool Remove( ITEM item )
		{
			return GenRemove(item,NULL);
		}

	// same as %Remove% but also delete the instance of item %CN%
	bool Delete( ITEM item )
		{
			delete item;
			return GenRemove(item,NULL);
		}

	// same as %Clean% but also delete the instance of each item %CN%
	void Delete()
		{
			Cel<ITEM> *lproto = items;

			while( lproto != NULL )
			{
				items = lproto->next;
				length--;
				delete lproto->item;
				delete lproto;
				lproto = items;
			}
			queue = NULL;
		}

	// same as %Delete% but delete the instance of item only if only present one time %CN%
	bool SafeDelete( ITEM item )
		{
			bool b;
			bool r = GenRemove(item,&b);
			if( r && !b )
				delete item;
			return r;
		}

	// same as %Delete% but checks for multiple references of the same object %CN%
	void SafeDelete()
		{
			while( length > 0 )
				SafeDelete(items->item);
		}

protected:

	bool GenRemove( ITEM item, bool *check )
		{	
			Cel<ITEM> *lproto = items;
			Cel<ITEM> *lprev = NULL;
			bool del = false;
			while( lproto != NULL )
			{
				if( lproto->item == item )
				{
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
				else
				{
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
									WList< type >::Cel< type > *__next = (list).AtStart(); \
									type item; \
									while( __next != NULL ) \
									{ \
										item = __next->item; \
										__next = __next->next;
#define ENDFOR				}}

#endif
/* ****************************************************************	*/
