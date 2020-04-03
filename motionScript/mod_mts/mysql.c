#include <value.h>
#include <datastruct.h>
#include <stdlib.h>
typedef int SOCKET;
#include <mysql.h>
#include <malloc.h>

#define MYSQLDATA(o)	((MYSQL*)val_odata(o))
#define RESULT(o)		((result*)val_odata(o))

DEFINE_CLASS(std_mysql,connection);
DEFINE_CLASS(std_mysql,result);

static void error( MYSQL *m, const char *msg ) {
	val_print( alloc_string(msg) );
	val_print( alloc_string(mysql_error(m)) );	
}

// ---------------------------------------------------------------
// Result

#undef CONV_FLOAT
typedef enum {
	CONV_INT,
	CONV_STRING,
	CONV_STRINGLEN,
	CONV_FLOAT
} CONV;

typedef struct {
	MYSQL_RES *r;
	int nfields;
	CONV *fields_convs;
	field *fields_ids;
	MYSQL_ROW current;
	bool need_length;
} result;

static field current_id;

void free_result( value o ) {
	result *r = RESULT(o);
	mysql_free_result(r->r);
	free(r->fields_ids);
	free(r->fields_convs);
	free(r);
}

value result_toString() {
	value o = val_this();
	val_check_obj(o,t_result);
	return alloc_string("#Sql.Result");
}

value empty_result_toString() {
	return alloc_string("#Sql.EmptyResult");
}

value empty_result_get_length() {
	return val_field(val_this(),val_id("_n"));
}

value result_get_length() {
	value o = val_this();
	val_check_obj(o,t_result);
	return alloc_int( (int)mysql_num_rows(RESULT(o)->r) );
}

value result_get_nfields() {
	value o = val_this();
	val_check_obj(o,t_result);
	return alloc_int(RESULT(o)->nfields);
}

value result_next() {
	value o = val_this();
	result *r;
	MYSQL_ROW row;
	val_check_obj(o,t_result);
	r = RESULT(o);
	row = mysql_fetch_row(r->r);
	if( row == NULL )
		return val_false;
	{
		int i;
		value cur = alloc_object(NULL);
		unsigned long *lengths = r->need_length?mysql_fetch_lengths(r->r):NULL;
		alloc_field(o,current_id,cur);
		r->current = row;
		for(i=0;i<r->nfields;i++)
			if( row[i] != NULL ) {
				value v;
				switch( r->fields_convs[i] ) {
				case CONV_INT:
					v = alloc_int(atoi(row[i]));
					break;
				case CONV_STRING:
					v = alloc_string(row[i]);
					break;
				case CONV_FLOAT:
					v = alloc_float(atof(row[i]));
					break;
				case CONV_STRINGLEN:
					v = copy_string(row[i],lengths[i]);
					break;
				default:
					v = NULL;
					break;
				}
				alloc_field(cur,r->fields_ids[i],v);
			}
		return val_true;
	}
}

value result_results() {
	value o = val_this();
	value l;
	val_check_obj(o,t_result);
	l = alloc_list();
	while( result_next() == val_true )
		add_to_list(l,val_field(o,current_id));
	return l;
}

value result_getResult( value n ) {
	value o = val_this();	
	result *r;
	const char *s;
	val_check_obj(o,t_result);
	r = RESULT(o);
	if( !val_is_int(n) || val_int(n) < 0  || val_int(n) >= r->nfields )
		return NULLVAL;
	if( !r->current ) {
		result_next();
		if( !r->current )
			return NULL;
	}
	s = r->current[val_int(n)];
	return alloc_string( s?s:"" );
}

value result_getIntResult( value n ) {
	value o = val_this();
	value cur;
	result *r;
	const char *s;
	val_check_obj(o,t_result);
	r = RESULT(o);
	if( !val_is_int(n) || val_int(n) < 0  || val_int(n) >= r->nfields )
		return NULLVAL;
	if( !r->current ) {
		result_next();
		if( !r->current )
			return NULL;
	}
	cur = val_field(o,current_id);
	s = r->current[val_int(n)];
	return alloc_int( s?atoi(s):0 );
}

value result_getFloatResult( value n ) {
	value o = val_this();
	result *r;
	const char *s;
	val_check_obj(o,t_result);
	r = RESULT(o);
	if( !val_is_int(n) || val_int(n) < 0  || val_int(n) >= r->nfields )
		return NULLVAL;
	if( !r->current ) {
		result_next();
		if( !r->current )
			return NULL;
	}
	s = r->current[val_int(n)];
	return alloc_float( s?atof(s):0 );
}

value std_mysql_result() {
	value o = alloc_class(&t_result);
	Method(o,result,get_nfields,0);
	Method(o,result,toString,0);
	Method(o,result,get_length,0);
	Method(o,result,getResult,1);
	Method(o,result,getIntResult,1);
	Method(o,result,getFloatResult,1);
	Method(o,result,results,0);
	Method(o,result,next,0);
	current_id = val_id("current");	
	return o;
}

CONV convert_type( enum enum_field_types t ) {
	switch( t ) {
	case FIELD_TYPE_TINY:
	case FIELD_TYPE_LONG:
		return CONV_INT;
	case FIELD_TYPE_LONGLONG:
	case FIELD_TYPE_DECIMAL:
	case FIELD_TYPE_FLOAT:
	case FIELD_TYPE_DOUBLE:
		return CONV_FLOAT;
	default:
		return CONV_STRINGLEN;
	}
}

value alloc_result( MYSQL_RES *r ) {
	value o = alloc_object(&t_result);
	int num_fields = mysql_num_fields(r);
	int i,j;
	MYSQL_FIELD *fields = mysql_fetch_fields(r);
	result *res = malloc(sizeof(result));
	res->r = r;
	res->current = NULL;
	res->nfields = num_fields;
	res->fields_ids = malloc(sizeof(field)*num_fields);
	res->fields_convs = malloc(sizeof(CONV)*num_fields);
	res->need_length = false;
	for(i=0;i<num_fields;i++) {
		field id = val_id(fields[i].name);
		CONV cv;
		for(j=0;j<i;j++)
			if( res->fields_ids[j] == id ) {
				val_print(alloc_string("Error, same field ids for : <b>"));
				val_print(alloc_string(fields[i].name));
				val_print(alloc_string(":"));
				val_print(alloc_int(i));
				val_print(alloc_string("</b> and <b>"));
				val_print(alloc_string(fields[j].name));
				val_print(alloc_string(":"));
				val_print(alloc_int(j));
				val_print(alloc_string("</b>.<br/>"));
			}
		res->fields_ids[i] = id;
		cv = convert_type(fields[i].type); 
		if( cv == CONV_STRINGLEN )
			res->need_length = true;
		res->fields_convs[i] = cv;
	}
	val_odata(o) = (value)res;
	val_gc(o,free_result);
	return o;
}

// ---------------------------------------------------------------
// Connection

value connection_toString() {
	value o = val_this();
	val_check_obj(o,t_connection);
	return alloc_string("#Sql.Connection");
}

value connection_close() {
	value o = val_this();
	val_check_obj(o,t_connection);
	mysql_close(MYSQLDATA(o));
	val_odata(o) = NULL;
	val_otype(o) = NULL;
	val_gc(o,NULL);
	return val_true;
}

value connection_selectDB( value db ) {
	value o = val_this();
	val_check_obj(o,t_connection);
	if( !val_is_string(db) )
		return NULLVAL;
	if( mysql_select_db(MYSQLDATA(o),val_string(db)) != 0 ) {
		error(MYSQLDATA(o),"Failed to select database :");
		return NULLVAL;
	}
	return val_true;
}

value connection_request( value r )  {
	value o = val_this();
	MYSQL_RES *res;
	val_check_obj(o,t_connection);
	if( !val_is_string(r) )
		return NULLVAL;
	if( mysql_real_query(MYSQLDATA(o),val_string(r),val_strlen(r)) != 0 ) {
		error(MYSQLDATA(o),val_string(r));
		return NULLVAL;
	}
	res = mysql_store_result(MYSQLDATA(o));
	if( res == NULL ) {
		if( mysql_field_count(MYSQLDATA(o)) == 0 ) {
			// no result : insert for example
			int n = (int)mysql_affected_rows(MYSQLDATA(o));
			o = alloc_object(NULL);
			alloc_field(o,val_id("_n"),alloc_int(n));
			Method(o,empty_result,toString,0);
			Method(o,empty_result,get_length,0);
			return o;
		} else {
			error(MYSQLDATA(o),val_string(r));
			return NULLVAL;
		}
	}
	return alloc_result(res);
}

value std_mysql_connection() {
	value o = alloc_class(&t_connection);
	Method(o,connection,toString,0);
	Method(o,connection,selectDB,1);
	Method(o,connection,request,1);
	Method(o,connection,close,0);
	return o;
}

void free_connection( value o ) {
	mysql_close(MYSQLDATA(o));
}

value alloc_connection( MYSQL *m ) {
	value o = alloc_object(&t_connection);
	val_odata(o) = (value)m;
	val_gc(o,free_connection);
	return o;
}

// ---------------------------------------------------------------
// Sql

value std_mysql_connect( value params  ) {
	value host, port, user, pass, socket;
	if( !val_is_object(params) )
		return NULL;
	host = val_field(params,val_id("host"));
	port = val_field(params,val_id("port"));
	user = val_field(params,val_id("user"));
	pass = val_field(params,val_id("pass"));
	socket = val_field(params,val_id("socket"));
	if( !(
		val_is_string(host) &&
		val_is_int(port) &&
		val_is_string(user) &&
		val_is_string(pass) &&
		(val_is_string(socket) || socket == NULLVAL)
	) )
		return NULLVAL;
	{
		MYSQL *m = mysql_init(NULL);		
		if( mysql_real_connect(m,val_string(host),val_string(user),val_string(pass),NULL,val_int(port),(socket == NULLVAL)?NULL:val_string(socket),0) == NULL ) {
			error(m,"Failed to connect to mysql server :");
			mysql_close(m);
			return NULL;
		}

		return alloc_connection(m);
	}	
}

DEFINE_PRIM(std_mysql_connect,1);
