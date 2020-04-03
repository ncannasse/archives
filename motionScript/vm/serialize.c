#include <string.h>
#include "value.h"
#include "datastruct.h"
#include "objtable.h"

#define BUF_SIZE	4096

typedef struct strlist {
	char *str;
	int slen;
	struct strlist *next;
} strlist;

typedef struct odatalist {
	value data;
	struct odatalist *next;
} odatalist;

typedef struct {
	odatalist *refs;
	strlist *olds;
	unsigned char *cur;
	bool error;
	int size;
	int pos;
	int totlen;
} buffer;

void buffer_alloc( buffer *b, int size ) {
	strlist *str = (strlist*)alloc_small(sizeof(strlist));
	str->str = b->cur;
	str->slen = b->pos;
	str->next = b->olds;
	b->olds = str;
	b->totlen += b->pos;
	b->pos = 0;
	b->size = size;
	b->cur = alloc_small_no_scan(size);
}

void write_char( buffer *b, char c ) {
	if( b->pos == b->size )
		buffer_alloc(b,BUF_SIZE);
	b->cur[b->pos++] = c;
}

int read_char( buffer *b ) {
	if( b->pos >= b->size )
		return -1;
	return b->cur[b->pos++];
}

int peek_char( buffer *b ) {
	if( b->pos >= b->size )
		return -1;
	return b->cur[b->pos];
}

void write_str( buffer *b, int len, const void *s ) {
	int left = b->size - b->pos;
	if( left == 0 ) {
		buffer_alloc(b,BUF_SIZE);
		left = b->size - b->pos;
	}
	if( left >= len ) {
		memcpy(b->cur + b->pos,s,len);
		b->pos += len;
	} else {
		memcpy(b->cur + b->pos,s,left);
		b->pos += left;
		write_str(b,len - left, (char*)s + left);
	}
}

void read_str( buffer *b, int len, void *s ) {
	if( b->pos + len > b->size ) {
		b->error = true;
		return;
	}
	memcpy(s,b->cur + b->pos, len);
	b->pos += len;
}

void write_int( buffer *b, int n ) {
	write_str(b,4,&n);
}

int read_int( buffer *b ) {
	int n;
	if( b->pos + 4 > b->size ) {
		b->error = true;
		return 0;
	}
	memcpy(&n,b->cur + b->pos,4);
	b->pos += 4;
	return n;
}

void add_ref( buffer *b, value o ) {
	odatalist *d = (odatalist*)alloc_small(sizeof(odatalist));
	d->data = o;
	d->next = b->refs;
	b->refs = d;
}

bool write_ref( buffer *b, value o ) {
	int i = 0;
	odatalist *d = b->refs;
	while( d != NULL ) {
		if( d->data == o ) {
			write_char(b,'r');
			write_int(b,i);
			return true;
		}
		i++;
		d = d->next;
	}
	add_ref(b,o);
	return false;
}

static void serialize_fields_rec( value data, field id, void *b );

void serialize_rec( buffer *b, value o ) {
	switch( val_type(o) ) {
	case VAL_PRIMITIVE:
		write_char(b,'p');
		break;
	case VAL_FUNCTION:
		write_char(b,'l');
		break;
	case VAL_NULL:
		write_char(b,'n');
		break;
	case VAL_INT:
		write_char(b,'i');
		write_int(b,val_int(o));
		break;
	case VAL_FLOAT:
		write_char(b,'f');
		write_str(b,8,&val_float(o));
		break;
	case VAL_STRING:
		if( !write_ref(b,o) ) {
			write_char(b,'s');
			write_int(b,val_strlen(o));
			write_str(b,val_strlen(o),val_string(o));
		}
		break;
	case VAL_OBJECT:
		if( !write_ref(b,o) ) {
			if( val_otype(o) == t_array ) {
				value *a = val_array(o);
				int n = val_array_size(o);
				int i;
				write_char(b,'a');
				write_int(b,n);
				for(i=0;i<n;i++)
					serialize_rec(b,a[i]);
			} else if( val_otype(o) == t_list ) {
				litem l = val_list(o)->first;
				write_char(b,'L');
				while( l != NULL ) {
					serialize_rec(b,l->item);
					l = l->next;
				}
				write_char(b,':');
			} else if( val_otype(o) == t_hash ) {
				int p;
				write_char(b,'h');
				for(p=0;p<HASH_SIZE;p++) {
					hitem *h = hash_item(o,p);
					while( h != NULL ) {
						serialize_rec(b,h->v);
						write_int(b,val_strlen(h->str));
						write_str(b,val_strlen(h->str),val_string(h->str));						
						h = h->next;
					}
				}
				write_char(b,':');
			} else {
				write_char(b,'o');
				otable_iter(((vobject*)o)->table,serialize_fields_rec,b);
				write_int(b,0);
			}
		}
		break;
	case VAL_BLOCK:
		if( !write_ref(b,o) ) {
			int i;
			int n = val_block_size(o);
			write_char(b,'b');
			write_int(b,n);
			for(i=0;i<n;i++)
				serialize_rec(b,val_block_ptr(o)[i]);
		}
		break;
	default:
		b->error = true;
		break;
	}
}

static void serialize_fields_rec( value data, field id, void *b ) {
	write_int(b,(int)id);
	serialize_rec(b,data);
}

value std_serialize( value o ) {
	buffer b;
	value v;
	char *s;
	strlist *l;
	b.olds = NULL;
	b.refs = NULL; 
	b.cur = alloc_small_no_scan(BUF_SIZE);
	b.size = BUF_SIZE;
	b.pos = 0;
	b.totlen = 0;
	b.error = false;
	serialize_rec(&b,o);
	if( b.error )
		return NULLVAL;
	v = alloc_raw_string(b.pos + b.totlen);
	s = (char*)val_string(v);
	s += b.totlen;
	l = b.olds;
	memcpy(s,b.cur,b.pos);
	while( l != NULL ) {
		s -= l->slen;
		memcpy(s,l->str,l->slen);
		l = l->next;
	}
	return v;
}

value unserialize_rec( buffer *b ) {
	switch( read_char(b) ) {
	case 'i':
		return alloc_int(read_int(b));
	case 'f':
		{
			double d;
			read_str(b,8,&d);
			return alloc_float(d);
		}
	case 's':
		{
			int l = read_int(b);
			value v;
			if( l < 0 || l >= 0x1000000 ) {
				b->error = true;
				return NULL;
			}
			v = alloc_raw_string(l);
			add_ref(b,v);
			read_str(b,l,(char*)val_string(v));
			return v;
		}
	case 'o':
		{
			int f;
			value o = alloc_object(NULL);
			add_ref(b,o);
			while( (f = read_int(b)) != 0 ) {
				value fval = unserialize_rec(b);
				otable_replace(((vobject*)o)->table,(field)f,fval);
			}
			return o;
		}
	case 'b':
		{
			int i;
			int l = read_int(b);
			value v;
			if( l < 0 || l >= 0x10000 ) {
				b->error = true;
				return NULL;
			}
			v = alloc_block(l);
			add_ref(b,v);
			for(i=0;i<l;i++)
				val_block_ptr(v)[i] = unserialize_rec(b);
			return v;
		}
	case 'r':
		{
			int n = read_int(b);
			odatalist *d = b->refs;
			while( n > 0 && d != NULL ) {
				d = d->next;
				n--;
			}
			if( d == NULL ) {
				b->error = true;
				return NULLVAL;
			}
			return d->data;
		}
	case 'a':
		{
			int i;
			int n = read_int(b);
			value o;
			value *t;
			if( n < 0 || n >= 0x10000 ) {
				b->error = true;
				return NULL;
			}
			o = alloc_array(n,&t);
			add_ref(b,o);
			for(i=0;i<n;i++)
				t[i] = unserialize_rec(b);
			return o;

		}
	case 'L':
		{
			value o = alloc_list();
			while( true ) {
				int c = peek_char(b);
				if( c == ':' || c == -1 )
					break;
				add_to_list(o,unserialize_rec(b));
			}
			b->pos++;
			return o;
		}
	case 'h':
		{
			value o = alloc_hash();
			value v,s;
			int slen;
			while( true ) {
				int c = peek_char(b);				
				if( c == ':' || c == -1 )
					break;				
				v = unserialize_rec(b);
				slen = read_int(b);
				s = alloc_raw_string(slen);
				read_str(b,slen,(char*)val_string(s));
				hash_replace(o,val_string(s),slen,v);
			}
			b->pos++;
			return o;
		}
	case 'n':
	case 'l':
	case 'p':
		return NULLVAL;
	default:
		b->error = true;
		return NULLVAL;
	}
}

value std_unserialize( value s ) {
	value v;
	buffer b;
	if( !val_is_string(s) )
		return NULLVAL;
	b.cur = (char*)val_string(s);
	b.pos = 0;
	b.error = false;
	b.olds = NULL;
	b.refs = NULL;
	b.size = val_strlen(s);
	b.totlen = 0;
	v = unserialize_rec(&b);
	if( b.error )
		return NULLVAL;
	return v;
}

DEFINE_PRIM(std_serialize,1);
DEFINE_PRIM(std_unserialize,1);
