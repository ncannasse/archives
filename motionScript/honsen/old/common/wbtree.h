/* ****************************************************************	*/
/*																	*/
/*	class WBTree													*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard template binary search tree class					*/
/*																	*/
/* ****************************************************************	*/
#ifndef WBTREE_H
#define WBTREE_H

#ifndef NULL
#define NULL 0x00000000
#endif

template <class KEY,class ITEM> class WBTree {

public:

	typedef void	(*ITERATOR)		( ITEM ); // iterative function
	typedef  int	(*COMPARATOR)	( KEY, ITEM );// comparative function
	typedef  int	(*ANYCOMPARATOR)( ITEM, void *);// comparative function

	// a cell of the list
	template<class CITEM> struct Cel {
		CITEM	item;
		Cel<CITEM>	*left;
		Cel<CITEM>	*right;
	};

	// constructor
	WBTree( COMPARATOR _comp ) {
		tree  = NULL;
		comp  = _comp;
		count = 0;
		depth = 0;
	}

	// destructor
	~WBTree() {
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
		return Find(k) != NULL;
	}

	// return the root of the tree or NULL if empty %C1%
	Cel<ITEM> *Tree() {
		return tree;
	}

	// add the specified item to the tree %CLogN%
	Cel<ITEM> *Add( const KEY &key, const ITEM &item  ) {
		Cel<ITEM> *lcel = new Cel<ITEM>;
		lcel->item = item;
		lcel->left = NULL;
		lcel->right = NULL;
		count++;

		Cel<ITEM> *p = NULL;
		Cel<ITEM> *c = tree;
		int lprof = 1;
		int r;

		while( c != NULL ) {
			r = comp(key,c->item);
			p = c;
			lprof++;
			if( r < 0 )
				c = c->left;
			else {
				if( r == 0 ) {
					ITEM it = lcel->item;
					lcel->item = c->item;
					c->item = it;
				}
				c = c->right;
			}
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
		Cel<ITEM> *c = FindCel(key);
		return c == NULL?NULL:c->item;
	}

	// find the specified item matched by the specified compatator %CN%
	ITEM Find( ANYCOMPARATOR comp, void *param ) {
		Cel<ITEM> *c = FindAny(tree,comp,param);
		return c == NULL?NULL:c->item;
	}

	// remove the item which match the specified key
	// returns true if successfull or false if
	// the item hasn't been found %CLogN%
	bool Remove( const KEY &key, ITEM *it = NULL ) {
		Cel<ITEM> *c = RemoveCel(key,it);
		delete c;
		return (c != NULL);
	}

	// same as %Remove% but also delete the instance of item %CLogN%
	bool Delete( const KEY &key, ITEM *it = NULL ) {
		Cel<ITEM> *c = RemoveCel(key,it);
		if( c == NULL )
			return false;
		delete c->item;
		delete c;
		return true;
	}

	// clear the list without deleting items %CN%
	void Clean() {
		Cel<ITEM> *t = tree;
		tree = NULL;
		RClean(t);
		depth = 0;
		count = 0;
	}

	// same as %Clean% but also delete the instance of each item %CN%
	void Delete() {
		Cel<ITEM> *t = tree;
		tree = NULL;
		RDelete(t);
		depth = 0;
		count = 0;
	}

	void Iter( ITERATOR it ) {
		if( tree != NULL )
			_Iter(tree,it);
	}

	void RIter( ITERATOR it ) {
		if( tree != NULL )
			_RIter(tree,it);
	}

private:

	void _Iter( Cel<ITEM> *c, ITERATOR it ) {
		if( c->left != NULL )
			_Iter(c->left,it);
		it(c->item);
		if( c->right != NULL )
			_Iter(c->right,it);
	}

	void _RIter( Cel<ITEM> *c, ITERATOR it ) {
		if( c->right != NULL )
			_RIter(c->right,it);
		it(c->item);
		if( c->left != NULL )
			_RIter(c->left,it);
	}

	// remove the specified item which match the specified key %CLogN%
	Cel<ITEM> *RemoveCel( const KEY &k, ITEM *it ) {
		Cel<ITEM> *p;
		Cel<ITEM> *c;
		if( it == NULL )
			c = FindCel(k,&p);
		else
			c = FindExact(k,it,&p);
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
			Cel<ITEM> *t = c->right;
			p = NULL;
			while( t->left != NULL ) {
				p = t;
				t = t->left;
			}			
			ITEM itmp = c->item;
			c->item = t->item;
			t->item = itmp;
			if( p == NULL )
				c->right = t->right;
			else
				p->left = t->right;
			return t;
		}
	}

	// find the specified item matched by the specified compatator %CN%
	Cel<ITEM> *FindAny( Cel<ITEM> *c, ANYCOMPARATOR comp, void *param )	{		
		if( c == NULL )
			return NULL;
		if( comp(c->item,param) == 0 )
			return c;
		Cel<ITEM> *r;
		if( (r = FindAny(c->left,comp,param)) != NULL )
			return r;
		return FindAny(c->right,comp,param);
	}

	// find the specified item which match the specified key %CLogN%
	Cel<ITEM> *FindCel( const KEY &k, Cel<ITEM> **parent = NULL ) {
		Cel<ITEM> *c = tree;
		int r;
		if( parent != NULL )
			*parent = NULL;
		while( c != NULL ) {
			r = comp(k,c->item);
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

	Cel<ITEM> *FindExact( const KEY &k, const ITEM *it, Cel <ITEM> **parent ) {
		Cel<ITEM> *c = tree;
		int r;
		*parent = NULL;
		while( c != NULL ) {
			r = comp(k,c->item);
			if( r < 0 ) {
				if( parent != NULL )
					*parent = c;
				c = c->left;
			} else {
				if( r > 0 || c->item != *it ) {
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

	void RClean( Cel<ITEM> *c ) {
		if( c == NULL )
			return;
		RClean(c->left);
		RClean(c->right);
		delete c;
	}

	void RDelete( Cel<ITEM> *c ) {
		if( c == NULL )
			return;
		RDelete(c->left);
		RDelete(c->right);
		delete c->item;
		delete c;
	}

	Cel<ITEM> *tree;
	int count;
	int depth;
	COMPARATOR comp;

};//class

#endif
/* ****************************************************************	*/
