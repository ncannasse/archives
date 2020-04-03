#include "value.h"
#include "datastruct.h"
#include <string.h>

#ifdef __linux__
#  include <strings.h>
#  undef strcmpi
#  define strcmpi(a,b) strcasecmp(a,b)
#endif

DEFINE_CLASS(std,xml);

field id_att, id_subs, id_node, id_attname, id_text;

value parse_xml( const char **p, value parent, value cur );

value xml_new1( value s ) {
	value o = val_this();
	val_check_obj(o,t_xml);
	if( !val_is_string(s) )
		return NULLVAL;	
	{
		const char *p = val_string(s);
		alloc_field(o,id_att,alloc_object(NULL));
		o = parse_xml(&p,NULL,o);
		return o;
	}
}

value xml_childs() {
	value o = val_this();
	value l , l2;
	litem p;
	val_check_obj(o,t_xml);
	l = val_field(o,id_subs);
	val_check_obj(l,t_list);
	p = val_list(l)->first;
	l2 = alloc_list();
	while( p != NULL ) {
		add_to_list(l2,p->item);
		p = p->next;
	}
	return l2;
}

value xml_toString() {
	return alloc_string("#xml");
}

value std_xml() {
	value o = alloc_class(&t_xml);
	Constr(o,xml,Xml,1);
	Method(o,xml,childs,0);
	Method(o,xml,toString,0);
	id_att = val_id("att");
	id_attname = val_id("attname");
	id_subs = val_id("subs");
	id_node = val_id("node");
	id_text = val_id("text");
	return o;
}

// -------------- parsing --------------------------

typedef enum {
	IGNORE_SPACES,
	BEGIN,
	BEGIN_NODE,
	TAG_NAME,
	BODY,
	ATTRIB_NAME,
	EQUALS,
	ATTVAL_BEGIN,
	ATTRIB_VAL,
	ESCAPE,
	CHILDS,
	CLOSE,
	WAIT_END,
	PCDATA,
	COMMENT
} STATE;

bool is_valid_char( int c ) {
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == ':' || c == '.' || c == '_' || c == '-';
}

value parse_xml( const char **lp, value parent, value cur ) {
	STATE state = IGNORE_SPACES;
	STATE next = BEGIN;	
	value aname;
	value childs;
	const char *start;
	const char *p = *lp;
	char c = *p;
	while( true ) {
		switch( state ) {
		case IGNORE_SPACES:
			switch( c ) {
			case '\n':
			case '\r':
			case '\t':
			case ' ':
				break;
			default:
				state = next;
				continue;
			}
			break;
		case BEGIN:
			switch( c ) {
			case '<':
				state = IGNORE_SPACES;
				next = BEGIN_NODE;
				break;
			default:
				start = p;
				state = PCDATA;				
				continue;
			}
			break;
		case PCDATA:
			if( c == '<' ) {
				if( parent == NULL )
					return NULL;
				alloc_field(parent,id_text,copy_string(start,p-start));
				state = IGNORE_SPACES;
				next = BEGIN_NODE;
			}
			break;
		case BEGIN_NODE:
			switch( c ) {
			case '!':
				state = COMMENT;
				break;
			case '/':
				if( parent == NULL )
					return NULL;
				start = p + 1;
				state = IGNORE_SPACES;
				next = CLOSE;
				break;
			default:
				state = TAG_NAME;
				start = p;
				continue;
			}
			break;
		case TAG_NAME:
			if( !is_valid_char(c) ) {
				if( p == start )
					return NULL;
				alloc_field(cur,id_node,copy_string(start,p-start));
				state = IGNORE_SPACES;
				next = BODY;
				continue;
			}
			break;
		case BODY:
			switch( c ) {
			case '/':
				state = WAIT_END;
				break;
			case '>':
				state = IGNORE_SPACES;
				next = CHILDS;
				break;
			default:
				state = ATTRIB_NAME;
				start = p;
				continue;
			}
			break;
		case ATTRIB_NAME:
			if( !is_valid_char(c) ) {
				if( start == p )
					return NULL;
				aname = copy_string(start,p-start);
				state = IGNORE_SPACES;
				next = EQUALS;
				continue;
			}
			break;
		case EQUALS:
			switch( c ) {
			case '=':
				state = IGNORE_SPACES;
				next = ATTVAL_BEGIN;
				break;
			default:
				return NULL;
			}
			break;
		case ATTVAL_BEGIN:
			switch( c ) {
			case '"':
			case '\'':
				state = ATTRIB_VAL;
				start = p;
				break;
			default:
				return NULL;
			}
			break;
		case ATTRIB_VAL:
			if( c == *start ) {
				value aval = copy_string(start+1,p-start-1);
				alloc_field(aval,id_attname,aname);
				alloc_field(val_field(cur,id_att),val_id(val_string(aname)),aval);
				state = IGNORE_SPACES;
				next = BODY;
			}
			break;
		case CHILDS:
			childs = alloc_list();
			alloc_field(cur,id_subs,childs);
			*lp = p;			
			while( true ) {
				value x = alloc_object(&t_xml);
				alloc_field(x,id_att,alloc_object(NULL));
				x = parse_xml(lp,cur,x);
				if( x == NULL )
					return NULL;
				if( x == cur ) {
					if( val_list(childs)->len == 0 ) {
						if( val_field(cur,id_text) == NULLVAL )
							alloc_field(cur,id_text,alloc_string(""));
						alloc_field(cur,id_subs,NULLVAL);
					}
					return cur;
				}				
				add_to_list(childs,x);
			}
			break;
		case WAIT_END:
			switch( c ) {
			case '>': 
				*lp = p+1;
				return cur;
			default :
				return NULL;
			}
			break;
		case CLOSE:
			if( !is_valid_char(c) ) {
				if( c != '>' )
					return NULL;
				if( start == p )
					return NULL;
				{
					value v = copy_string(start,p - start);
					if( strcmpi(val_string(val_field(parent,id_node)),val_string(v)) != 0 )
						return NULL;
				}
				*lp = p+1;
				return parent;
			}
			break;
		case COMMENT:
			if( c == '>' ) {
				state = IGNORE_SPACES;
				next = BEGIN;
			}
			break;
		}
		c = *++p;
		if( c == 0 )
			return NULL;
	}
}
