#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

#define hash_replace std_hash_replace

#include "value.h"

typedef struct _litem {
	value item;
	struct _litem *next;
} *litem;

typedef struct _list {
	litem first;
	litem last;
	int len;
} *list;

typedef struct hitem {
	value str;
	value v;
	struct hitem *next;
} hitem;

#define HASH_SIZE 256

#define val_list(o)	((list)val_odata(o))
#define val_array(o)	val_block_ptr(val_odata(o))
#define val_array_size(o)	val_block_size(val_odata(o))
#define hash_item(o,k)	((hitem**)val_odata(o))[k]

C_FUNCTION_BEGIN

	EXTERN otype t_array;
	EXTERN otype t_list;
	EXTERN otype t_hash;

	EXTERN value alloc_list();
	EXTERN void add_to_list( value l, value v);

	EXTERN value alloc_array( int size, value **ptr );

	EXTERN value alloc_hash();
	EXTERN bool hash_replace( value o, const char *k, int klen, value v );

C_FUNCTION_END

#endif
