/* ************************************************************************ */
/*																			*/
/*	WXML																	*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "wlist.h"
#include "wxml.h"

WString escape( WString s ) {
	WString s2;
	const char *sc = s.c_str();
	int l = s.length();
	int i;
	for(i=0;i<l;i++)
		switch( sc[i] ) {
		case '\n':
			s2 += "\\n";
			break;
		case '\r':
			s2 += "\\r";
			break;
		case '\t':
			s2 += "\\t";
			break;
		case '\\':
			s2 += "\\\\";
			break;
		case '"':
			s2 += "\\\"";
			break;
		case '\'':
			s2 += "\\'";
			break;
		default:
			s2 += sc[i];
			break;
		}
	return s2;
}

WXml::WXml( WString tag ) : tag(tag) {
}

WXml::WXml( const WXml &cpy ) : tag(cpy.tag) {
	FOREACH(WAttrib*,cpy.attribs);
		WAttrib *a = new WAttrib;
		a->name = item->name;
		a->value = item->value;
		attribs.Add(a);
	ENDFOR;
	FOREACH(WXml*,cpy.childs);
		childs.Add(new WXml(*item));
	ENDFOR;
}

WXml::~WXml() {
	attribs.Delete();
	childs.Delete();
}

bool
WXml::HasAttrib( WString name ) {
	FOREACH(WAttrib*,attribs);
		if( strcmpi(item->name.c_str(),name.c_str()) )
			return true;
	ENDFOR;
	return false;
}

WString
WXml::Attrib( WString name ) {
	FOREACH(WAttrib*,attribs);
		if( item->name == name )
			return item->value;
	ENDFOR;
	return "";
}

int
WXml::AttribInt( WString name ) {
	FOREACH(WAttrib*,attribs);
		if( item->name == name )
			return atoi(item->value.c_str());
	ENDFOR;
	return -1;
}

WString
WXml::ToString() {
	WString s = "<"+tag;
	FOREACH(WAttrib*,attribs);
		s += " "+item->name+"=\""+escape(item->value)+"\"";
	ENDFOR;
	if( childs.Empty() )
		s += "/>";
	else {
		s += ">";
		FOREACH(WXml*,childs);
			s += item->ToString();
		ENDFOR;
		s += "</"+tag+">";
	}
	return s;
}

WString
WXml::ToStringFmt() {
	return ToString();
}

/* ************************************************************************ */
// PARSER

#define XMLEOF -256

typedef int (*getter)( void * );

enum STATE {
	IGNORE_SPACES,
	BEGIN,
	BEGIN2,
	TAG_NAME,
	BODY,
	ATTRIB_NAME,
	EQUALS,
	ATTVAL_BEGIN,
	ATTRIB_VAL,
	ESCAPE,
	CHILDS,
	CLOSE,
	WAIT_END
};

#define FAILED() { delete cur; return NULL; }

bool is_valid_char( int c ) {
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == ':' || c == '.' || c == '_' || c == '-';
}

WXml *GenParser( WXml *parent, getter f, void *p ) {
	STATE state = IGNORE_SPACES;
	STATE next = BEGIN;
	WXml *cur = NULL;
	WString tagname;
	WString attname;
	WString attvalue;
	int attval_endchar;
	int c = f(p);
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
				next = BEGIN2;
				tagname = "";
				break;
			default:
				FAILED();
			}
			break;
		case BEGIN2:
			switch( c ) {
			case '/':
				if( parent == NULL )
					FAILED();
				state = IGNORE_SPACES;
				next = CLOSE;
				break;
			default:
				state = TAG_NAME;
				continue;
			}
		case TAG_NAME:
			if( is_valid_char(c) )
				tagname += (char)c;
			else {
				if( tagname.length() == 0 )
					FAILED();
				cur = new WXml(tagname);
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
				continue;
			default:
				state = ATTRIB_NAME;
				attname = "";
				continue;
			}
			break;
		case ATTRIB_NAME:
			if( is_valid_char(c) )
				attname += (char)c;
			else {
				if( attname.length() == 0 )
					FAILED();
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
				FAILED();
			}
			break;
		case ATTVAL_BEGIN:
			switch( c ) {
			case '"':
			case '\'':
				state = ATTRIB_VAL;
				attvalue = "";
				attval_endchar = c;
				break;
			default:
				FAILED();
			}
			break;
		case ATTRIB_VAL:
			switch( c ) {
			case '\\':
				state = ESCAPE;
				break;
			default:
				if( c == attval_endchar ) {
					WXml::WAttrib *a = new WXml::WAttrib;
					a->name = attname;
					a->value = attvalue;
					cur->attribs.Add(a);
					state = IGNORE_SPACES;
					next = BODY;
				} else
					attvalue += (char)c;
				break;
			}
			break;
		case ESCAPE:
			switch( c ) {
			case 'n':
				attvalue += '\n';
				break;
			case 'r':
				attvalue += '\r';
				break;
			case 't':
				attvalue += '\t';
				break;
			case '\\':
				attvalue += '\\';
				break;
			case '\'':
				attvalue += '\'';
				break;
			case '"':
				attvalue += '"';
				break;
			default:
				FAILED();
			}
			break;
		case CHILDS: 
			while( true ) {
				WXml *x = GenParser(cur,f,p);
				if( x == NULL )
					FAILED();
				if( x == cur )
					return cur;
				cur->childs.Add(x);
			}
			break;
		case WAIT_END:
			switch( c ) {
			case '>': return cur;
			default : FAILED();
			}
			break;
		case CLOSE:
			if( is_valid_char(c) )
				tagname += (char)c;
			else {
				if( c != '>' )
					FAILED();
				if( tagname.length() == 0 )
					FAILED();
				if( strcmpi(parent->tag.c_str(),tagname.c_str()) != 0 )
					FAILED();
				return parent;
			}
			break;
		}
		c = f(p);
		if( c == XMLEOF ) FAILED();
	}
}

/* ************************************************************************ */

int get_file_char( FILE *f ) {
	char c;
	if( fread(&c,1,1,f) != 1 )
		return XMLEOF;
	return c;
}

WXml *
WXml::ParseFile( WString name ) {
	FILE *f = fopen(name.c_str(),"rb");
	if( f == NULL )
		return NULL;
	WXml *x = GenParser(NULL,(getter)get_file_char,f);
	fclose(f);
	return x;
}

struct XString {
	const char *chars;
	int len;
};

int get_string_char( XString *s ) {
	if( s->len <= 0 )
		return XMLEOF;
	s->len--;
	s->chars++;
	return s->chars[-1];
}

WXml *
WXml::ParseString( WString str ) {
	XString s;
	s.chars = str.c_str();
	s.len = str.length();
	return GenParser(NULL,(getter)get_string_char,&s);
}

WXml *
WXml::ParseString( const char *str, int *strlen ) {
	XString s;
	s.chars = str;
	s.len = *strlen;
	WXml *x = GenParser(NULL,(getter)get_string_char,&s);
	*strlen -= s.len;
	return x;
}

/* ************************************************************************ */
