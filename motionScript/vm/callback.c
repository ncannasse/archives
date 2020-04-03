#include <string.h>
#include "value.h"
#include "interp.h"
#include "objtable.h"
#include "vmcontext.h"
#include "datastruct.h" // for array arguments callback

typedef value (*c_prim0)();
typedef value (*c_prim1)(value);
typedef value (*c_prim2)(value,value);
typedef value (*c_prim3)(value,value,value);
typedef value (*c_prim4)(value,value,value,value);
typedef value (*c_primN)(value*,int);

EXTERN value val_callN( value f, value *args, int nargs ) {
	if( !f || ((unsigned int)f) & 1 || nargs < 0 )
		return NULLVAL;
	if( val_tag(f) == VAL_PRIMITIVE ) {
		context *c = get_std_context();
		value v = NULLVAL;
		void *old_env = c->env;
		c->env = ((vfunction *)f)->env;
		if( nargs == ((vfunction*)f)->nargs ) {
			switch( nargs ) {
			case 0:
				v = ((c_prim0)((vfunction*)f)->addr)();
				break;
			case 1:
				v = ((c_prim1)((vfunction*)f)->addr)(args[0]);
				break;
			case 2:
				v = ((c_prim2)((vfunction*)f)->addr)(args[0],args[1]);
				break;
			case 3:
				v = ((c_prim3)((vfunction*)f)->addr)(args[0],args[1],args[2]);
				break;
			case 4:
				v = ((c_prim4)((vfunction*)f)->addr)(args[0],args[1],args[2],args[3]);
				break;
			default:
				break;
			}
		} else if( ((vfunction*)f)->nargs == -1 )
			v = (value)((c_primN)((vfunction*)f)->addr)(args,nargs);
		c->env = old_env;
		return v;
	} else if( val_tag(f) == VAL_FUNCTION ) {
		if( nargs == ((vfunction*)f)->nargs )  {
			context *c = get_std_context();
			int n;
			value v;
			if( c->csp + 3 >= c->sp - nargs ) {
				c->error = OVERFLOW;
				return NULLVAL;
			}
			for(n=0;n<nargs;n++)
				*--c->sp = (unsigned int)args[n];
			*++c->csp = CALLBACK_RETURN;
			*++c->csp = NULLVAL;
			*++c->csp = c->_this;
			v = exec((unsigned int)((vfunction*)f)->addr,((vfunction*)f)->env);
			if( c->error != 0 )
				return NULLVAL;
			return v;
		}
	}
	return NULLVAL;
}

EXTERN value val_ocallN( value o, field f, value *args, int nargs ) {
	context *c = get_std_context();
	value *meth = otable_find(((vobject*)o)->table,f);
	unsigned int old_this = c->_this;
	value r;
	if( meth == NULL )
		return NULLVAL;
	c->_this = (unsigned int)o;
	r = val_callN(*meth,args,nargs);
	c->_this = old_this;
	return r;
}

EXTERN value val_call0( value f ) {
	return val_callN(f,NULL,0);
}

EXTERN value val_call1( value f, value v ) {
	return val_callN(f,&v,1);
}

EXTERN value val_call2( value f, value v1, value v2 ) {
	value args[2] = { v1, v2 };
	return val_callN(f,args,2);
}

EXTERN value val_ocall0( value o, field f ) {
	return val_ocallN(o,f,NULL,0);
}

EXTERN value val_ocall1( value o, field f, value arg ) {
	return val_ocallN(o,f,&arg,1);
}

EXTERN value val_this() {
	return (value)get_std_context()->_this;
}

value std_callback_apply( value *args, int nargs ) {
	value *env = (value*)get_std_context()->env;
	value f = val_field(env[0],(field)env[1]);
	int env_size = val_fun_nargs(f) + 2;
	int i;
	for(i=0;i<nargs;i++)	
		env[env_size - nargs + i] = args[i];
	return val_ocallN(env[0],(field)env[1],env+2,env_size - 2);
}

value std_callback( value *args, int nargs ) {
	value f;
	field id;
	value *env;	
	int fnargs;

	if( nargs < 2 )
		return NULLVAL;
	if( !val_is_object(args[0]) || !val_is_string(args[1]) )
		return NULLVAL;
	id = val_id(val_string(args[1]));
	f = val_field(args[0],id);	
	if( !val_is_function(f) )
		return NULLVAL;
	fnargs = val_fun_nargs(f);
	if( fnargs >= 0 && fnargs < nargs - 2 )
		return NULL;
	env = (value*)alloc_small((fnargs + 2) * sizeof(value));
	memcpy(env,args,nargs * sizeof(value));	
	env[1] = (value)id;
	f = alloc_fun(val_string(args[1]),std_callback_apply,-1);	
	((vfunction *)f)->env = (value)env;
	return f;
}

value std_field_call( value o, value f, value args ) {
	if( !val_is_object(o) )
		return NULLVAL;
	val_check_obj(args,t_array);
	return val_ocallN(o,(field)val_int(f),val_array(args),val_array_size(args));
}

DEFINE_PRIM(std_field_call,3);
DEFINE_PRIM_MULT(std_callback);
