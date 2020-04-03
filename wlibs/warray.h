/* ****************************************************************	*/
/*																	*/
/*	class WArray													*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard template array class								*/
/*																	*/
/* ****************************************************************	*/
#ifndef WARRAY_H
#define WARRAY_H

#ifndef NULL
#define NULL 0x00000000
#endif

template <class ITEM> class WArray {

public:

	WArray() {
		inull = NULL;
		items = NULL;
		length = 0;
		size = 0;
	}

	WArray( ITEM inull ) : inull(inull) {
		items = NULL;
		length = 0;
		size = 0;
	}

	WArray( const WArray<ITEM>&l ) : inull(l.inull) {
		items = NULL;
		*this = l;
	}

	~WArray() {
		Clean();
	}

	const WArray<ITEM>& operator = ( const WArray<ITEM> &copy ) {
		Clean();
		length = copy.length;
		size = copy.length;
		items = new ITEM[size];
		int i;
		for(i=0;i<size;i++)
			items[i] = copy.items[i];
		return (*this);
	}

	const WArray<ITEM>& operator << ( const ITEM &it ) {
		Add(it);
		return (*this);
	}

	const WArray<ITEM>& operator >> ( const ITEM &it ) {
		Remove(it);
		return (*this);
	}

	inline ITEM& operator []( int index )
	{
		if( index < 0 )
			return *(ITEM *)NULL;
		if( index >= size )
			Resize(index+1);
		if( index >= length )
			length = index+1;
		return items[index];
	}
	
	inline ITEM operator[] (int index) const
	{
		if( index < 0 || index >= length )
			return inull;
		return array[index];
	}

	int Length() {
		return length;
	}

	bool Empty() {
		return (length == 0);
	}

	ITEM AtIndex( int index ) {
		if( index < 0 || index >= length )
			return inull;
		return items[index];
	}

	ITEM Last() {
		if( length == 0 )
			return inull;
		return items[length-1];
	}

	ITEM First() {
		if( length == 0 )
			return inull;
		return items[0];
	}

	int IndexOf( const ITEM &item ) {
		int i;
		for(i=0;i<length;i++)
			if( items[i] == item )
				return i;
		return -1;
	}

	bool Exists( const ITEM &item ) {
		return IndexOf( item ) != -1;
	}

	void Sort( int (*comp)(ITEM a, ITEM b) ) {
		int i,j;
		bool modified;
		for(i=0;i<length;i++) {
			modified = false;
			for(j=i+1;j<length;j++)
				if( comp(items[i],items[j]) > 0 ) {
					ITEM tmp = items[i];
					items[i] = items[j];
					items[j] = tmp;
					modified = true;
				}
			if( !modified )
				break;
		}
	}

	void Reverse() {
		int i;
		for(i=0;i<length/2;i++) {
			ITEM tmp = items[i];
			items[i] = items[length-i-1];
			items[length-i-1] = tmp;
		}
	}

	int Add( const ITEM &item ) {
		if( length >= size )
			Resize( (size == 0)?1:(size << 1) );
		items[length++] = item;
		return length - 1;
	}

	void Resize( int nsize ) {
		ITEM *nitems = new ITEM[nsize];
		int i;
		for(i=0;i<length;i++)
			nitems[i] = items[i];
		for(i=length;i<nsize;i++)
			nitems[i] = inull;
		delete items;
		items = nitems;
		size = nsize;
	}

	void Set( const ITEM &item, int index ) {
		if( index < 0 )
			*(char*)NULL = 1;
		if( index >= size )
			Resize( index+1 );
		if( index >= length )
			length = index+1;
		items[index] = item;
	}

	void Clean() {
		delete items;
		items = NULL;
		size = 0;
		length = 0;
	}

	bool Remove( const ITEM &item ) {
		if( length == 0 )
			return false;
		int n = IndexOf(item);
		if( n == -1 )
			return false;
		int i;
		length--;
		for(i=n;i<length;i++)
			items[i] = items[i+1];
		items[length] = inull;
	}

	bool Delete( const ITEM &item ) {
		delete item;
		return Remove(item);
	}

	void Delete() {
		int i;
		for(i=0;i<length;i++)
			delete items[i];
		delete items;
		items = NULL;
		size = 0;
		length = 0;
	}

	ITEM *Items( int *max = NULL ) {
		if( max != NULL )
			*max = length;
		return items;
	}

	ITEM *GetContents() {
		ITEM *c = items;
		items = NULL;
		size = 0;
		length = 0;
		return c;
	}

protected:

	ITEM inull;
	ITEM *items;
	int size;
	int length;

};//class

#define AFOREACH(type,arr){ \
									int __max; \
									type *items = (arr).Items(&__max); \
									int __counter = 0; \
									type item; \
									while( __counter < __max ) \
									{ \
										item = items[__counter++];

#ifndef ENDFOR
#define ENDFOR				}}
#endif

#endif
/* ****************************************************************	*/
