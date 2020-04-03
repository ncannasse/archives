#ifndef _VALUE_H
#define _VALUE_H

#define alloc_root		std_alloc_root
#define free_root		std_free_root

#ifndef NULL
#	define NULL			0
#endif

#define NULLVAL			0

typedef enum {
	VAL_NULL		= 0xFF,
	VAL_INT			= 0,
	VAL_FLOAT		= 1,
	VAL_FUNCTION	= 3,
	VAL_BLOCK		= 5,
	VAL_OBJECT		= 7,
	VAL_STRING		= VAL_OBJECT   | 8,
	VAL_PRIMITIVE	= VAL_FUNCTION | 8,
	VAL_32_BITS		= 0xFFFFFFFF
} val_t;

typedef unsigned int header;

struct _value {
	header h;
};

struct _otype;
struct _field;
struct _objtable;
struct _stringbuf;
typedef struct _value* value;
typedef struct _otype *otype;
typedef struct _field *field;
typedef struct _objtable* objtable;
typedef struct _stringbuf *stringbuf;
typedef double tfloat;

typedef void (*finalizer)(value v);

typedef struct {
	header h;
	tfloat f;
} vfloat;

typedef struct {
	header h;
	otype t;
	value data;
	objtable table;
} vobject;

typedef struct {
	header h;
	const char *name;
	int nargs;
	void *addr;
	value env;
} vfunction;

#define t_resource			((otype)0x00000001)
#define t_class				((otype)0x00000003)
#define val_head(v)			((header*)(v))
#define val_mkheader(t,sz)	(t | (((sz) / sizeof(value)) << 5))
#define val_data(v)			((value*) (((char*)(v))+sizeof(header)) )
#define val_is_int(v)		((((int)(v)) & 1) != 0)
#define val_is_bool(v)		(v == val_true || v == val_false)
#define val_tag(v)			((*(val_t*)val_head(v))&15)
#define val_short_tag(v)	((*(val_t*)val_head(v))&7)
#define val_is_number(v)	((v) != NULLVAL && (val_is_int(v) || val_tag(v) == VAL_FLOAT))
#define val_is_float(v)		((v) != NULLVAL && !val_is_int(v) && val_tag(v) == VAL_FLOAT)
#define val_is_string(v)	((v) != NULLVAL && !val_is_int(v) && val_tag(v) == VAL_STRING)
#define val_is_function(v)	((v) != NULLVAL && !val_is_int(v) && val_short_tag(v) == VAL_FUNCTION)
#define val_is_object(v)	((v) != NULLVAL && !val_is_int(v) && val_short_tag(v) == VAL_OBJECT)
#define val_is_ref(v)		((v) != NULLVAL && !val_is_int(v) && val_tag(v) == VAL_BLOCK && val_block_size(v) == 1)
#define val_is_obj(v,t)		(val_is_object(v) && val_otype(v) == t)
#define val_is_resource(v)	val_is_obj(v,t_resource)
#define val_check_obj(v,t)	{ if( !val_is_obj(v,t) ) return NULLVAL; }

#define val_type(v)			((v) == NULLVAL ? VAL_NULL : (val_is_int(v) ? VAL_INT : val_tag(v)))
#define val_int(v)			(((int)(v)) >> 1)
#define val_float(v)		((vfloat*)(v))->f
#define val_bool(v)			(v == val_true)
#define val_number(v)		(val_is_int(v)?val_int(v): CONV_FLOAT val_float(v))
#define val_string(v)		(((const char *)val_odata(v))+4)
#define val_strlen(v)		(*(int*)val_odata(v))
#define val_ref(v)			(*val_block_ptr(v))
#define val_resource(v)		((void*)val_odata(v))

#define val_block_size(v)	((*(int*)val_head(v))>>5)
#define val_block_ptr(v)	val_data(v)
#define val_fun_name(v)		((vfunction*)(v))->name
#define val_fun_nargs(v)	((vfunction*)(v))->nargs
#define val_otype(v)		((vobject*)(v))->t
#define val_odata(v)		((vobject*)(v))->data
#define alloc_int(v)		((value)(((v) << 1) | 1))
#define alloc_bool(b)		((value)((b)?3:1))
#define val_true			((value)3)
#define val_false			((value)1)
#define Max_block_size		((1 << 27) - 1)

#undef EXTERN
#undef EXPORT
#undef IMPORT
#ifdef _WIN32
#	define INLINE __inline
#	define EXPORT __declspec( dllexport )
#	define IMPORT __declspec( dllimport )
#else
#	define INLINE inline
#	define EXPORT
#	define IMPORT
#endif

#ifdef MTSVM_SOURCE
#	define EXTERN EXPORT
#else
#	define EXTERN IMPORT
#endif

#ifdef __cplusplus
#	define C_FUNCTION_BEGIN extern "C" {
#	define C_FUNCTION_END	};
#else
#	define C_FUNCTION_BEGIN
#	define C_FUNCTION_END
#	ifndef true
#		define true 1
#		define false 0
		typedef int bool;
#	endif
#endif

#ifndef CONV_FLOAT
#	define CONV_FLOAT
#endif

#define DEFINE_PRIM_MULT(func) C_FUNCTION_BEGIN EXPORT void *func##__MULT() { return &func; } C_FUNCTION_END
#define DEFINE_PRIM(func,nargs) C_FUNCTION_BEGIN EXPORT void *func##__##nargs() { return &func; } C_FUNCTION_END
#define DEFINE_CLASS(dll,name) extern value dll##_##name(); otype t_##name = (otype)dll##_##name; DEFINE_PRIM(dll##_##name,0);

#ifdef SCRIPT_IMPORTS
#	define DECLARE_PRIM(func,nargs) C_FUNCTION_BEGIN IMPORT void *func##__##nargs(); C_FUNCTION_END
#	define DECLARE_CLASS(name) C_FUNCTION_BEGIN IMPORT extern otype t_##name; C_FUNCTION_END
#else
#	define DECLARE_PRIM(func,nargs) C_FUNCTION_BEGIN EXPORT void *func##__##nargs(); C_FUNCTION_END
#	define DECLARE_CLASS(name) C_FUNCTION_BEGIN EXPORT extern otype t_##name; C_FUNCTION_END
#endif

typedef value (*PRIM0)();
typedef value (*PRIM1)( value v );
typedef value (*PRIM2)( value v1, value v2 );

#define CALL_PRIM(func) ((PRIM0)func##__0())()
#define CALL_PRIM1(func,v) ((PRIM1)func##__1())(v)
#define CALL_PRIM2(func,v1,v2) ((PRIM2)func##__2())(v1,v2)

C_FUNCTION_BEGIN

	EXTERN value val_this();

	EXTERN value val_call0( value f );
	EXTERN value val_call1( value f, value arg );
	EXTERN value val_call2( value f, value arg1, value arg2 );
	EXTERN value val_callN( value f, value *args, int nargs );

	EXTERN field val_id( const char *str );
	EXTERN value val_field( value o, field f );
	EXTERN value val_ocall0( value o, field f );
	EXTERN value val_ocall1( value o, field f, value arg );
	EXTERN value val_ocallN( value o, field f, value *args, int nargs );

	EXTERN char *alloc_small( unsigned int nbytes );
	EXTERN char *alloc_small_no_scan( unsigned int nbytes );
	EXTERN value alloc_string( const char *str );
	EXTERN value alloc_raw_string( unsigned int size );
	EXTERN value alloc_float( tfloat v );
	EXTERN value alloc_fun( const char *name, void *c_prim, unsigned int nargs );
	EXTERN value alloc_block( unsigned int n );
	EXTERN value alloc_resource( void *res );

	EXTERN value copy_string( const char *str, unsigned int size );
	EXTERN value copy_object( value o );
	EXTERN value alloc_object( otype *t );
	EXTERN value alloc_class( otype *t );
	EXTERN void alloc_field( value obj, field f, value v );

	EXTERN stringbuf alloc_stringbuf( const char *init );
	EXTERN void stringbuf_append( stringbuf b, const char *s );
	EXTERN void stringbuf_append_sub( stringbuf b, const char *s, int len );
	EXTERN value stringbuf_to_string( stringbuf b );
	EXTERN void val_to_string( value v, stringbuf b );
	EXTERN int val_compare( value a, value b );
	EXTERN void val_print( value s );

	EXTERN value *alloc_root( unsigned int nvals );
	EXTERN void free_root( value *r );

	EXTERN void val_gc( value v, finalizer f );

C_FUNCTION_END

#define Constr(o,t,ut,nargs) { field f__new_##nargs = val_id(#ut "#" #nargs); alloc_field(o,f__new_##nargs,alloc_fun(#ut".new",t##_new##nargs,nargs) ); }
#define Method(o,t,name,nargs) { field f__##name = val_id(#name); alloc_field(o,f__##name,alloc_fun(#t"."#name,t##_##name,nargs) ); }
#define MethodMult(o,t,name) { field f__##name = val_id(#name); alloc_field(o,f__##name,alloc_fun(#t"."#name,t##_##name,-1) ); }
#define Property(o,t,name)	Method(o,t,get_##name,0); Method(o,t,set_##name,1)

#endif
