/* ****************************************************************	*/
/*																	*/
/*	class WPairBTree												*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard template binary key/value search tree class		*/
/*																	*/
/* ****************************************************************	*/
#ifndef WPAIRBTREE_H
#define WPAIRBTREE_H

#ifndef NULL
#define NULL 0x00000000
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4786 )	// identifiers too long for debug mode
#endif

template <class KEY,class ITEM> class WPairBTree {

public:

	static int equals( KEY a, KEY b ) {
		if( a < b )
			return 1;
		if( a > b )
			return -1;
		return 0;
	}

	typedef  int	(*COMPARATOR)	( KEY, KEY  );// comparative function
	typedef  int	(*ANYCOMPARATOR)( KEY, void *);// comparative function
	typedef  void	(*ITERATOR)		( KEY, ITEM, void *);// iterative function

	// a cell of the list
	template <class CKEY,class CITEM> struct Cel {
		CKEY	key;
		CITEM	item;
		Cel<CKEY,CITEM> *left;
		Cel<CKEY,CITEM> *right;
	};

	WPairBTree() {
		tree  = NULL;
		comp  = equals;
		count = 0;
		depth = 0;
	}

	// constructor
	WPairBTree( COMPARATOR _comp ) {
		tree  = NULL;
		comp  = _comp;
		count = 0;
		depth = 0;
	}

	// destructor
	~WPairBTree() {
		Clean();
	}

	// return the number of items %C1%
	int Count() {
		return count;
	}
	
	// return the depth of the tree %C1%
	int Depth() {
		return depth;
	}

	// return true if the item specified can be found %CLogN%
	bool Exists( const KEY &k ) {
		return FindCel(k) != NULL;
	}

	// return the root of the tree or NULL if empty %C1%
	Cel<KEY,ITEM> *Tree() {
		return tree;
	}
	
	// add the specified item to the tree %CLogN%
	Cel<KEY,ITEM> *Add( const KEY &key, const ITEM &item ) {
		Cel<KEY,ITEM> *lcel = new Cel<KEY,ITEM>();
		lcel->key = key;
		lcel->item = item;
		lcel->left = NULL;
		lcel->right = NULL;
		count++;

		Cel<KEY,ITEM> *p = NULL;
		Cel<KEY,ITEM> *c = tree;
		int lprof = 1;
		int r;

		while( c != NULL ) {
			r = comp(c->key,key);
			p = c;
			lprof++;
			if( r < 0 )
				c = c->left;
			else
				c = c->right;
		}
		if( p == NULL ) {
			tree = lcel;
			depth = 1;
		}
		else {
			if( r < 0 )
				p->left = lcel;
			else
				p->right = lcel;
			if( lprof > depth )
				depth = lprof;
		}
		return lcel;
	}

	// add the specified item to the tree %CLogN%
	Cel<KEY,ITEM> *Replace( const KEY &key, const ITEM &item ) {
		Cel<KEY,ITEM> *lcel = new Cel<KEY,ITEM>();
		lcel->key = key;
		lcel->item = item;
		lcel->left = NULL;
		lcel->right = NULL;
		count++;

		Cel<KEY,ITEM> *p = NULL;
		Cel<KEY,ITEM> *c = tree;
		int lprof = 1;
		int r;

		while( c != NULL ) {
			r = comp(c->key,key);
			p = c;
			lprof++;
			if( r < 0 )
				c = c->left;
			else if( r == 0 )
				break;
			else
				c = c->right;
		}
		if( c != NULL ) {
			c->key = key;
			c->item = item;
			return c;
		}

		if( p == NULL ) {
			tree = lcel;
			depth = 1;
		}
		else {
			if( r < 0 )
				p->left = lcel;
			else
				p->right = lcel;
			if( lprof > depth )
				depth = lprof;
		}
		return lcel;
	}

	// find the specified item which match the specified key %CLogN%
	ITEM Find( const KEY &key ) {
		Cel<KEY,ITEM> *c = FindCel(key);
		return c == NULL?(ITEM)NULL:c->item;
	}

	// find the specified item matched by the specified compatator %CN%
	ITEM Find( ANYCOMPARATOR comp, void *param ) {
		Cel<KEY,ITEM> *c = FindAny(tree,comp,param);
		return c == NULL?NULL:c->item;
	}

	void Iter( ITERATOR it, void *param ) {
		RIter(it,param,tree);
	}

	// remove the item which match the specified key
	// returns true if successfull or false if
	// the item hasn't been found %CLogN%
	bool Remove( const KEY &key ) {
		Cel<KEY,ITEM> *c = RemoveCel(Key);
		delete c;
		return (c != NULL);
	}

	// same as %Remove% but also delete the instance of item %CLogN%
	bool Delete( const KEY &key ) {
		Cel<KEY,ITEM> *c = RemoveCel(key);
		if( c == NULL )
			return false;
		delete c->item;
		delete c->key;
		delete c;
		return true;
	}

	bool DeleteKeyOnly( const KEY &key ) {
		Cel<KEY,ITEM> *c = RemoveCel(key);
		if( c == NULL )
			return false;
		delete c->key;
		delete c;
		return true;
	}

	bool DeleteItemOnly( const KEY &key ) {
		Cel<KEY,ITEM> *c = RemoveCel(key);
		if( c == NULL )
			return false;
		delete c->item;
		delete c;
		return true;
	}

	// clear the list without deleting items %CN%
	void Clean() {
		RClean(tree);
		tree = NULL;
		depth = 0;
		count = 0;
	}

	// same as %Clean% but also delete the instance of each item %CN%
	void Delete() {
		RDelete(tree);
		tree = NULL;
		depth = 0;
		count = 0;
	}

	void DeleteItemOnly() {
		RDeleteItem(tree);
		tree = NULL;
		depth = 0;
		count = 0;
	}

	void DeleteKeyOnly() {
		RDeleteKey(tree);
		tree = NULL;
		depth = 0;
		count = 0;
	}

	void Optimize() {
		KEY **keys = new KEY*[count];
		ITEM **items = new ITEM*[count];
		unsigned int n = 0;
		OptimizeRecA(tree,keys,items,&n);
		depth = 0;
		count = 0;
		Cel<KEY,ITEM> *old = tree;
		tree = NULL;
		OptimizeRecB(keys,items,0,n);
		delete items;
		delete keys;
		RClean(old);
	}

private:

	void OptimizeRecA( Cel<KEY,ITEM> *c, KEY **keys, ITEM **items, unsigned int *n ) {
		if( c == NULL )
			return;
		OptimizeRecA(c->left,keys,items,n);
		keys[*n] = &c->key;
		items[*n] = &c->item;
		*n = *n + 1;
		OptimizeRecA(c->right,keys,items,n);
	}

	void OptimizeRecB( KEY **keys, ITEM **items, int min, int max ) {
		if( max <= min )
			return;
		int p = min + (max - min) / 2;
		Add(*keys[p],*items[p]);
		OptimizeRecB(keys,items,min,p);
		OptimizeRecB(keys,items,p+1,max);
	}

	// remove the specified item which match the specified key %CLogN%
	Cel<KEY,ITEM> *RemoveCel( const KEY &key ) {
		Cel<KEY,ITEM> *p;
		Cel<KEY,ITEM> *c = FindCel(key,&p);
		if( c == NULL )
			return NULL;
		count--;
		if( c->right == NULL ) {
			if( p != NULL ) {
				if( c == p->left )
					p->left = c->left;
				else
					p->right = c->left;
			}		
			else
				tree = c->left;
			return c;
		}
		else {
			Cel<KEY,ITEM> *t = c->right;
			p = NULL;
			while( t->left != NULL ) {
				p = t;
				t = t->left;
			}			
			ITEM itmp = c->item;
			KEY ktmp = c->key;
			c->item = t->item;
			c->key = t->key;
			t->item = itmp;
			t->key = ktmp;
			if( p == NULL )
				c->right = t->right;
			else
				p->left = t->right;
			return t;
		}
	}

	// find the specified item matched by the specified compatator %CN%
	Cel<KEY,ITEM> *FindAny( Cel<ITEM,KEY> *c, ANYCOMPARATOR comp, void *param ) {		
		if( c == NULL )
			return NULL;
		if( comp(c->key,param) == 0 )
			return c;
		Cel<ITEM,KEY> *r;
		if( (r = FindAny(c->left,comp,param)) != NULL )
			return r;
		return FindAny(c->right,comp,param);
	}

	// find the specified item which match the specified key %CLogN%
	Cel<KEY,ITEM> *FindCel( const KEY &key, Cel<KEY,ITEM> **parent = NULL ) {
		Cel<KEY,ITEM> *c = tree;
		int r;
		if( parent != NULL )
			*parent = NULL;
		while( c != NULL ) {
			r = comp(c->key,key);
			if( r < 0 ) {
				if( parent != NULL )
					*parent = c;
				c = c->left;
			}
			else {
				if( r > 0 ) {
					if( parent != NULL )
						*parent = c;
					c = c->right;
				}
				else
					return c;
			}
		}
		return NULL;
	}

	void RClean( Cel<KEY,ITEM> *c ) {
		if( c == NULL )
			return;
		RClean(c->left);
		RClean(c->right);
		delete c;
	}

	void RDeleteItem( Cel<KEY,ITEM> *c ) {
		if( c == NULL )
			return;
		RDeleteItem(c->left);
		RDeleteItem(c->right);
		delete c->item;
		delete c;
	}

	void RDeleteKey( Cel<KEY,ITEM> *c ) {
		if( c == NULL )
			return;
		RDeleteKey(c->left);
		RDeleteKey(c->right);
		delete c->key;
		delete c;
	}


	void RDelete( Cel<KEY,ITEM> *c ) {
		if( c == NULL )
			return;
		RDelete(c->left);
		RDelete(c->right);
		delete c->key;
		delete c->item;
		delete c;
	}

	void RIter( ITERATOR it, void *param, Cel<KEY,ITEM> *c ) {
		if( c == NULL )
			return;
		RIter(it,param,c->left);
		it(c->key,c->item,param);
		RIter(it,param,c->right);
	}

	Cel<KEY,ITEM> *tree;
	int count;
	int depth;
	COMPARATOR comp;

};//class

#endif
/* ****************************************************************	*/
