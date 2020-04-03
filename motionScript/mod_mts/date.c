#include <value.h>
#include <time.h>
#include <stdio.h>
#include <memory.h>
#include <locale.h>

DEFINE_CLASS(std,date);

#define val_date(o)		((time_t*)val_odata(o))

field id_h;
field id_m;
field id_s;
field id_y;
field id_d;

static value date_new1( value s ) {
	value o = val_this();
	val_check_obj(o,t_date);
	val_odata(o) = (value)alloc_small_no_scan(sizeof(time_t));
	if( s == NULL )
		*val_date(o) = time(NULL);
	else {
		struct tm t;
		bool recal = true;
		if( !val_is_string(s) )
			return NULLVAL;
		memset(&t,0,sizeof(struct tm));
		switch( val_strlen(s) ) {
		case 19:
			sscanf(val_string(s),"%4d-%2d-%2d %2d:%2d:%2d",&t.tm_year,&t.tm_mon,&t.tm_mday,&t.tm_hour,&t.tm_min,&t.tm_sec);
			t.tm_isdst = -1;
			break;
		case 8:
			sscanf(val_string(s),"%2d:%2d:%2d",&t.tm_hour,&t.tm_min,&t.tm_sec);
			*val_date(o) = t.tm_sec + t.tm_min * 60 + t.tm_hour * 60 * 60;
			recal = false;
			break;
		case 10:
			sscanf(val_string(s),"%4d-%2d-%2d",&t.tm_year,&t.tm_mon,&t.tm_mday);
			break;
		default:
			val_print(alloc_string("Invalid date format : "));
			val_print(s);
			return NULL;
		}
		if( recal ) {
			t.tm_year -= 1900;
			t.tm_mon--;
			*val_date(o) = mktime(&t);
		}
	}
	return o;
}

static value date_format( value fmt ) {
	value o = val_this();
	char buf[128];
	struct tm *t;
	val_check_obj(o,t_date);
	if( !val_is_string(fmt) )
		return NULL;
	t = localtime(val_date(o));
	if( t == NULL )
		return alloc_string("(date before 1970)");
	strftime(buf,127,val_string(fmt),t);
	return alloc_string(buf);
}

static value date_toString() {
	return date_format(alloc_string("%Y-%m-%d %H:%M:%S"));
}

static value date_setTime( value h, value m, value s ) {
	value o = val_this();
	struct tm *t;
	val_check_obj(o,t_date);
	if( !val_is_int(h) || !val_is_int(m) || !val_is_int(s) )
		return NULLVAL;
	t = localtime(val_date(o));
	t->tm_hour = val_int(h);
	t->tm_min = val_int(m);
	t->tm_sec = val_int(s);
	*val_date(o) = mktime(t);
	return val_true;
}

static value date_setDay( value y, value m, value d ) {
	value o = val_this();
	struct tm *t;
	val_check_obj(o,t_date);
	if( !val_is_int(y) || !val_is_int(m) || !val_is_int(d) )
		return NULLVAL;
	t = localtime(val_date(o));
	t->tm_year = val_int(y) - 1900;
	t->tm_mon = val_int(m) - 1;
	t->tm_mday = val_int(d);
	*val_date(o) = mktime(t);
	return val_true;
}

static value date_getDay() {
	value r;
	value o = val_this();
	struct tm *t;
	val_check_obj(o,t_date);
	t = localtime(val_date(o));
	if( t == NULL )
		return NULL;
	r = alloc_object(NULL);
	alloc_field(r,id_y,alloc_int(t->tm_year + 1900));
	alloc_field(r,id_m,alloc_int(t->tm_mon + 1));
	alloc_field(r,id_d,alloc_int(t->tm_mday));
	return r;
}

static value date_getTime() {
	value r;
	value o = val_this();
	struct tm *t;
	val_check_obj(o,t_date);
	t = localtime(val_date(o));
	if( t == NULL )
		return NULL;
	r = alloc_object(NULL);
	alloc_field(r,id_h,alloc_int(t->tm_hour));
	alloc_field(r,id_m,alloc_int(t->tm_min));
	alloc_field(r,id_s,alloc_int(t->tm_sec));
	return r;
}

static value date_compare( value d ) {
	value o = val_this();
	int r;
	val_check_obj(o,t_date);
	val_check_obj(d,t_date);
	r = *val_date(o) - *val_date(d);
	return alloc_int( (r == 0)? 0 : ((r < 0) ? -1 : 1) );
}

static value date_sub( value d ) {
	value r;
	value o = val_this();
	val_check_obj(o,t_date);
	val_check_obj(d,t_date);
	r = alloc_object(&t_date);
	val_odata(r) = (value)alloc_small_no_scan(sizeof(time_t));
	*val_date(r) = *val_date(o) - *val_date(d);
	return r;
}

static value date_add( value d ) {
	value r;
	value o = val_this();
	val_check_obj(o,t_date);
	val_check_obj(d,t_date);
	r = alloc_object(&t_date);
	val_odata(r) = (value)alloc_small_no_scan(sizeof(time_t));
	*val_date(r) = *val_date(o) + *val_date(d);
	return r;
}

static value date_delta( value d ) {
	value o = val_this();
	val_check_obj(o,t_date);
	if( !val_is_int(d) )
		return NULLVAL;	
	*val_date(o) = *val_date(o) + val_int(d);
	return val_true;
}

static value date_get_time() {
	value o = val_this();
	val_check_obj(o,t_date);
	return alloc_int( (int)*val_date(o) );
}

static value date_set_time( value v ) {
	value o = val_this();
	val_check_obj(o,t_date);
	if( !val_is_int(v) )
		return NULLVAL;
	*val_date(o) = (time_t)val_int(v);
	return v;
}

value std_date() {
	value o = alloc_class(&t_date);
	Constr(o,date,Date,1);
	Method(o,date,setTime,3);
	Method(o,date,setDay,3);
	Method(o,date,getDay,0);
	Method(o,date,getTime,0);
	Method(o,date,format,1);
	Method(o,date,sub,1);
	Method(o,date,add,1);
	Method(o,date,delta,1);
	Method(o,date,compare,1);
	Method(o,date,toString,0);
	Property(o,date,time);
	id_h = val_id("h");
	id_m = val_id("m");
	id_s = val_id("s");
	id_y = val_id("y");
	id_d = val_id("d");
	return o;
}

static value std_date_now() {
	value o = alloc_object(&t_date);
	val_odata(o) = (value)alloc_small_no_scan(sizeof(time_t));
	*val_date(o) = time(NULL);
	return o;
}
static value std_set_locale(l) {
	if( !val_is_string(l) )
		return NULLVAL;
	return alloc_bool(setlocale(LC_TIME,val_string(l)) != NULL);
}

DEFINE_PRIM(std_set_locale,1);
DEFINE_PRIM(std_date_now,0);

