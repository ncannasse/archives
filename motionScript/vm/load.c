#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interp.h"
#include "vmcontext.h"
#define IS_LONG_TABLE
#include "opcodes.h"
#ifndef __linux__
typedef unsigned int uint;
#endif
// -------------------------------------------------------------------------//

typedef struct {
	char code[4];
	uint codesize;
	uint nglobals;
	uint nincludes;
	uint nids;
} head;

typedef unsigned char uchar;
typedef value (*class_init)();

extern value std_print( value *args, unsigned int argc );

// -------------------------------------------------------------------------//

#define LOAD_ERROR() \
			{\
				free_code(b);\
				c->error = INVALID_FILE;\
				return NULL;\
			}

// -------------------------------------------------------------------------//

char *read_string(read_h h,read_f read) {
	char *buf = alloc_small_no_scan(16);
	int buflen = 16;
	int pos = 0;
	while( true ) {
		if( pos == buflen ) {
			char *nbuf = alloc_small_no_scan(buflen*2);
			memcpy(nbuf,buf,buflen);
			buflen *= 2;			
			buf = nbuf;
		}
		if( read(h,buf+pos,1) != 1 )			
			return NULL;		
		if( !buf[pos] )
			return buf;
		pos++;
	}	
}

// -------------------------------------------------------------------------//

void skip_string(read_h h,read_f read) {
	char c;
	while( true ) {
		if( read(h,&c,1) != 1 || !c )
			return;
	}
}

// -------------------------------------------------------------------------//

void free_code(binary *b) {
	free_root(b->globals);
	free(b->code);
	free(b);
}

// -------------------------------------------------------------------------//

binary *generic_load(read_h h, read_f read ) {
	head head;
	binary *b;
	context *c = get_std_context();
	c->error = NOERROR;
	if( read(h,&head,sizeof(head)) != sizeof(head) || strcmp(head.code,"MTS") != 0 || head.codesize > 50000000 || head.nglobals > 1000000 ) {
		c->error = INVALID_FILE;
		return NULL;
	}
	if( head.nincludes > 0 ) {
		c->error = NOT_LINKED;
		return NULL;
	}
	b = malloc(sizeof(binary));
	b->nglobals = head.nglobals;
	b->codesize = head.codesize;
	b->globals = alloc_root(b->nglobals);
	b->code = malloc((b->codesize+1)*sizeof(uint));
	{
		int i;
		uchar byte;
		char *str;
		uchar nargs;
		uint addr;
		int pos = 0;

		for(i=0;i<b->nglobals;i++) {
			if( read(h,&byte,1) != 1 )
				LOAD_ERROR();
			switch(byte) {
			case 0: // NULL
				b->globals[i] = NULLVAL;
				break;
			case 1: // NAMED
				skip_string(h,read);
				b->globals[i] = NULLVAL;
				break;
			case 2: // FLOAT
				str = read_string(h,read);
				if( str == NULL )
					LOAD_ERROR()
				b->globals[i] = alloc_float(atof(str));
				break;
			case 3: // STRING 
				str = read_string(h,read);
				if( str == NULL )
					LOAD_ERROR()
				b->globals[i] = alloc_string(str);
				break;
			case 4: // FUNCTION
				str = read_string(h,read);
				read(h,&nargs,1);
				read(h,&addr,4);
				b->globals[i] = alloc_fun(str,(void*)addr,nargs-1);
				b->globals[i]->h = val_mkheader(VAL_FUNCTION,sizeof(vfunction));
				break;
			case 6: // PRIMITIVE
				skip_string(h,read);
				read(h,&nargs,1);
				str = read_string(h,read);
				if( str == NULL ) 
					LOAD_ERROR();
				read(h,&byte,1);
				{
					void *ptr = c->params.resolve?c->params.resolve(c,str,nargs-1):NULL;
					if( ptr == NULL ) {
						free_code(b);
						c->error = PRIMITIVE_NOT_FOUND;
						return NULL;
					}
					if( nargs-1 == 0 && (byte & 6) ) // CLASS or STATIC
						b->globals[i] = ((class_init)ptr)();
					else
	 					b->globals[i] = alloc_fun(str,ptr,nargs-1);
				}
				break;
			case 5: // CLOSURE
			case 7: // IMPORT
				free_code(b);
				c->error = NOT_LINKED;
				return NULL;
			case 8: // RESOURCE
				read(h,&addr,4);
				if( c->params.loader == NULL ) {
					free_code(b);
					c->error = NO_RESLOADER;
					return NULL;
				}
				b->globals[i] = c->params.loader(c,addr);
				break;
			default:
				LOAD_ERROR();
				break;
			}
		}
		for(i=0;i<(int)head.nids;i++)
			skip_string(h,read);
		while( pos < b->codesize ) {
			uchar op;
			uint bits;
			if( read(h,&op,1) != 1 )
				LOAD_ERROR();
			bits = op & 3;
			op >>= 2;
			if( op >= OLast )
				LOAD_ERROR();
			b->code[pos++] = op;
			if( bits != 3 || is_long_opcode[op] ) {
				if( bits == 2 ) {
					read(h,&byte,1);
					bits = byte;
				} else if( bits == 3 )
					read(h,&bits,4);
				b->code[pos++] = bits;
			}
		}
		if( pos != b->codesize )
			LOAD_ERROR();
		b->code[pos] = OLast;
	}
	return b;
}

// -------------------------------------------------------------------------//
