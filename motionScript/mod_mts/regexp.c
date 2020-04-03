#include <value.h>
#include <memory.h>
#define PCRE_STATIC
#include <pcre.h>

#define PCRE(o)		((pcredata*)val_odata(o))
#define NMATCHS		(10 * 3)

typedef struct {
	value str;
	pcre *r;	
	int nmatchs;
	int matchs[NMATCHS];
} pcredata;

DEFINE_CLASS(std,regexp);

field id_pos;
field id_len;

static void free_regexp( value p ) {
	pcre_free( ((pcredata*)p)->r );
}

value regexp_new1( value s ) {
	value o = val_this();
	val_check_obj(o,t_regexp);	
	if( !val_is_string(s) )
		return NULLVAL;
	{
		const char *error;
		int err_offset;
		pcre *p = pcre_compile(val_string(s),0,&error,&err_offset,NULL);
		pcredata *pdata;
		if( p == NULL ) {			
			val_print(alloc_string("Regexp compilation error : "));
			val_print(alloc_string(error));
			return NULLVAL;
		}
		pdata = (pcredata*)alloc_small(sizeof(pcredata));
		pdata->r = p;
		pdata->str = NULLVAL;
		pdata->nmatchs = 0;
		pcre_fullinfo(p,NULL,PCRE_INFO_CAPTURECOUNT,&pdata->nmatchs);
		val_gc((value)pdata,free_regexp);
		val_odata(o) = (value)pdata;
	}
	return o;
}

value regexp_match( value s ) {
	pcredata *d;
	value o = val_this();
	val_check_obj(o,t_regexp);	
	if( !val_is_string(s) )
		return NULLVAL;
	d = PCRE(o);
	if( pcre_exec(d->r,NULL,val_string(s),val_strlen(s),0,0,d->matchs,NMATCHS) >= 0 ) {
		d->str = s;
		return val_true;
	} else {
		d->str = NULLVAL;	
		return val_false;
	}
}

value regexp_exactMatch( value s ) {
	value v = regexp_match(s);
	if( v == val_true ) {
		pcredata *d = PCRE(val_this());
		if( d->matchs[0] == 0 && d->matchs[1] == val_strlen(s) )
			return val_true;
		d->str = NULLVAL;
		return val_false;
	}
	return v;
	
}

value do_replace( value s, value s2, bool all ) {	
	value o = val_this();
	val_check_obj(o,t_regexp);	
	if( !val_is_string(s) || !val_is_string(s2) )
		return NULLVAL;	
	{
		pcredata *d = PCRE(o);
		stringbuf b = alloc_stringbuf("");
		int pos = 0;
		int len = val_strlen(s);
		const char *str = val_string(s);
		const char *str2 = val_string(s2);
		int len2 = val_strlen(s2);
		while( pcre_exec(d->r,NULL,str,len,pos,0,d->matchs,NMATCHS) >= 0 ) {
			stringbuf_append_sub(b,str+pos,d->matchs[0] - pos);
			stringbuf_append_sub(b,str2,len2);
			pos = d->matchs[1];
			if( !all )
				break;
		}
		d->str = NULLVAL;
		stringbuf_append_sub(b,str+pos,len-pos);
		return stringbuf_to_string(b);
	}
}

value regexp_replace( value s, value s2 ) {	
	return do_replace(s,s2,false);
}

value regexp_replaceAll( value s, value s2 ) {
	return do_replace(s,s2,true);
}

value regexp_matched( value n ) {
	pcredata *d;
	int m;
	value o = val_this();
	val_check_obj(o,t_regexp);	
	d = PCRE(o);
	if( !val_is_int(n) )
		return NULLVAL;
	m = val_int(n);
	if( m < 0 || m > d->nmatchs || d->str == NULL )
		return NULLVAL;
	{
		int start = d->matchs[m*2];
		int len = d->matchs[m*2+1] - start;
		value str = alloc_raw_string(len);
		memcpy((char*)val_string(str),val_string(d->str)+start,len);
		return str;
	}
}

value regexp_matchedPos( value n ) {
	pcredata *d;
	int m;
	value o = val_this();
	val_check_obj(o,t_regexp);	
	d = PCRE(o);
	if( !val_is_int(n) )
		return NULLVAL;
	m = val_int(n);
	if( m < 0 || m > d->nmatchs || d->str == NULL )
		return NULLVAL;
	{
		int start = d->matchs[m*2];
		int len = d->matchs[m*2+1] - start;
		value o = alloc_object(NULL);
		alloc_field(o,id_pos,alloc_int(start));
		alloc_field(o,id_len,alloc_int(len));
		return o;
	}
}
value regexp_toString() {
	value o = val_this();
	val_check_obj(o,t_regexp);
	return alloc_string("#regexp");
}

value std_regexp() {
	value o = alloc_class(&t_regexp);
	Constr(o,regexp,Regexp,1);
	Method(o,regexp,toString,0);
	Method(o,regexp,match,1);
	Method(o,regexp,exactMatch,1);
	Method(o,regexp,replace,2);
	Method(o,regexp,replaceAll,2);
	Method(o,regexp,matched,1);
	Method(o,regexp,matchedPos,1);
	id_pos = val_id("pos");
	id_len = val_id("len");
	return o;
}

