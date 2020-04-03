// -------------------------------------------------------------------------//
//#define TRACE

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "api.h"
#include "interp.h"
#include "vmcontext.h"
#ifdef TRACE
#	define IS_LONG_TABLE
#endif
#include "opcodes.h"
#include "objtable.h"
#ifndef __linux__
typedef unsigned int uint;
#else
#	include <sys/types.h>
#endif

// -------------------------------------------------------------------------//

#define STACK_SIZE (1 << 16) // 64-Ko Stack
#define MAX_ARGS	8

// -------------------------------------------------------------------------//

#ifdef TRACE
	extern value std_print( value *args, unsigned int argc );
#	define Instr(x)	case x: { \
		stringbuf s; \
		value str; \
		if( is_long_opcode[x] ) \
			sprintf(c->tmp,"%.4d   %s %d  -  ",pc - pcmin - 1,#x,*pc); \
		else \
			sprintf(c->tmp,"%.4d   %s          ",pc - pcmin - 1,#x); \
		s = alloc_stringbuf(c->tmp); \
		if( val_is_object(acc) && !val_is_string(acc) ) { \
			sprintf(c->tmp,"#object<%X>",((vobject*)acc)->t); \
			stringbuf_append(s,c->tmp); \
		} \
		else \
			val_to_string((value)acc,s); \
		str = stringbuf_to_string(s); \
		std_print(&str,1); \
	}
#else
#	define Instr(x)	case x:
#endif

__inline void set_error(context *c,VM_ERRORCODE e, uint epc) {
	c->error = e;
	c->error_pc = epc;
}

#define OnError(t,x)	if( t ) { set_error(c,x,((uint)pc-(uint)c->pcmin)/4); return NULLVAL; }

#ifdef FAST_INTERP
#	define Error(t,x)
#else
#	define Error(t,x)	OnError(t,x)
#endif

#define Next		goto next
#define Pop(n)		{\
		uint _npop = (n); \
		Error(sp + _npop > c->spmax, UNDERFLOW); \
		while( _npop-- > 0 ) \
			*sp++ = NULLVAL; \
	}
#define TestAcc		( acc != 0 && acc != 1 )

// -------------------------------------------------------------------------//

typedef int (*c_prim0)();
typedef int (*c_prim1)(int);
typedef int (*c_prim2)(int,int);
typedef int (*c_prim3)(int,int,int);
typedef int (*c_prim4)(int,int,int,int);
typedef int (*c_primN)(value*,int);

extern value create_instance( value baseclass );
extern value alloc_array( int size, value **ptr );

// -------------------------------------------------------------------------//

static field op_add;
static field op_sub;
static field op_mult;
static field op_div;

// -------------------------------------------------------------------------//

void init(binary *b) {
	context *c = get_std_context();
	c->error = NOERROR;
	c->_this = NULLVAL;
	c->csp = (uint*)alloc_root(STACK_SIZE);
	c->sp = c->csp + STACK_SIZE;
	c->spmin = c->csp;
	c->spmax = c->sp;
	c->csp--;
	c->pcmin = (int*)b->code;
	c->pcmax = c->pcmin + b->codesize + 1;
	c->globals_size = b->nglobals;
	c->globals = (uint*)alloc_root(b->nglobals);
	memcpy(c->globals,b->globals,b->nglobals*sizeof(value));

	op_add = val_id("__add");
	op_sub = val_id("__sub");
	op_mult = val_id("__mult");
	op_div = val_id("__div");
}

// -------------------------------------------------------------------------//

value exec(uint entry, value env) {
	context *c = get_std_context();
	register int *pc = c->pcmin+entry;
	register int acc = 0;
	register uint *sp = c->sp;
	register uint *csp = c->csp;
	register uint *globs = c->globals;
	value _args[MAX_ARGS];
	value *args = _args;
	if( env == NULLVAL ) env = alloc_block(0);
next:
	Error( pc >= c->pcmax , OUT_CODE);
	switch( *pc++ ) {

	Instr(OPushNull);
		--sp;
		OnError( sp <= csp , OVERFLOW);
		*sp = NULLVAL;
		Next;

	Instr(OPush);
		--sp;
		OnError( sp <= csp , OVERFLOW);
		*sp = acc;
		Next;

	Instr(OPop);
		Error( *pc < 0 , INVALID_OPCODE);
		Pop( *pc++ );
		Next;

	Instr(OJmpNot);
		if( !TestAcc ) {
			pc += *pc++;
			Error( pc < c->pcmin , OUT_CODE);
		} else
			pc++;
		Next;

	Instr(OJmpIf);
		if( TestAcc ) {
			pc += *pc++;
			Error( pc < c->pcmin , OUT_CODE);
		} else
			pc++;
		Next;

	Instr(OJmp);
		pc += *pc++;
		Error( pc < c->pcmin , OUT_CODE);
		Next;

#define SetupBeforeCall(this_call) \
		uint old_this = c->_this; \
		void *old_env = c->env; \
		c->env = ((vfunction*)acc)->env; \
		c->_this = this_call; \
		c->sp = sp; \
		c->csp = csp;

#define RestoreAfterCall() \
		c->_this = old_this; \
		c->env = old_env; \
		if( c->error != NOERROR ) \
			return NULLVAL;

#define DoCall(this_call,_nargs) \
		if( !acc || (acc & 1) ) { \
			acc = NULLVAL; \
			Pop(*pc++); \
		} else if( val_tag(acc) == VAL_PRIMITIVE ) { \
			Error( sp + _nargs > c->spmax , UNDERFLOW); \
			if( _nargs == ((vfunction*)acc)->nargs ) { \
				SetupBeforeCall(this_call); \
				switch( _nargs ) { \
				case 0: \
					acc = ((c_prim0)((vfunction*)acc)->addr)(); \
					break; \
				case 1: \
					acc = ((c_prim1)((vfunction*)acc)->addr)(sp[0]); \
					break; \
				case 2: \
					acc = ((c_prim2)((vfunction*)acc)->addr)(sp[1],sp[0]); \
					break; \
				case 3: \
					acc = ((c_prim3)((vfunction*)acc)->addr)(sp[2],sp[1],sp[0]); \
					break; \
				case 4: \
					acc = ((c_prim4)((vfunction*)acc)->addr)(sp[3],sp[2],sp[1],sp[0]); \
					break; \
				default: \
					acc = NULLVAL; \
					break; \
				} \
				RestoreAfterCall(); \
			} \
			else if( ((vfunction*)acc)->nargs == -1 && _nargs <= MAX_ARGS ) { \
				int n; \
				SetupBeforeCall(this_call); \
				args += _nargs; \
				for(n=0;n<_nargs;n++) \
					*--args = (value)sp[n]; \
				acc = ((c_primN)((vfunction*)acc)->addr)(args,_nargs); \
				RestoreAfterCall(); \
			} else \
				acc = NULLVAL; \
			Pop(*pc++); \
		} else if( val_tag(acc) == VAL_FUNCTION ) { \
			if( _nargs == ((vfunction*)acc)->nargs ) { \
				OnError( csp + 3 >= sp , OVERFLOW); \
				*++csp = (uint)(pc+1); \
				*++csp = (uint)env; \
				*++csp = c->_this; \
				pc = c->pcmin + (uint)((vfunction*)acc)->addr; \
				c->_this = this_call; \
				env = ((vfunction*)acc)->env; \
			} \
			else { \
				acc = NULLVAL; \
				Pop(*pc++); \
			} \
		} else { \
			acc = NULLVAL; \
			Pop(*pc++); \
		}		

	Instr(OCall);
		Error( *pc < 0 , INVALID_OPCODE);
		DoCall(c->_this,*pc);
		Next;

	Instr(OCallObj);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( sp == c->spmax , UNDERFLOW);
		{
			int this_call = *sp;
			*sp++ = NULLVAL;
			DoCall(this_call,*pc);
		}
		Next;

	Instr(ORet);
		Error( *pc < 0 , INVALID_OPCODE);
		Pop(*pc++);
		Error( csp - 2 < c->spmin , UNDERFLOW);
		c->_this = *csp;
		*csp-- = NULLVAL;
		env = (value)*csp;
		*csp-- = NULLVAL;
		pc = (int*)*csp;
		*csp-- = NULLVAL;
		if( pc == (int*)CALLBACK_RETURN ) {
			c->sp = sp;
			c->csp = csp;
			return (value)acc;
		}
		Next;

	Instr(OAccNull);
		acc = NULLVAL;
		Next;

	Instr(OAccNot);
		acc = (int)(TestAcc?val_false:val_true);
		Next;

	Instr(OAccInt);
		acc = ((*pc++) << 1) | 1;
		Next;

	Instr(OAccStack);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( sp + *pc >= c->spmax , OUT_STACK);
		acc = sp[*pc++];
		Next;

	Instr(OAccRef);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( sp + *pc >= c->spmax , OUT_STACK);
		if( val_is_ref(sp[*pc]) )
			acc = (int)val_ref(sp[*pc++]);
		else {
			acc = NULLVAL;
			pc++;
		}
		Next;

	Instr(OAccGlobal);
		Error( *pc < 0 || *pc >= (int)c->globals_size , INVALID_OPCODE);
		acc = globs[*pc++];
		Next;

	Instr(OAccEnv);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( *pc >= val_block_size(env) , OUT_ENV);
		acc = (int)val_ref(val_block_ptr(env)[*pc++]);
		Next;

	Instr(OAccEnvRaw);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( *pc >= val_block_size(env) , OUT_ENV);
		acc = (int)val_block_ptr(env)[*pc++];
		Next;

	Instr(OAccObj);
		if( val_is_object(acc) ) {
			value *f = otable_find(((vobject*)acc)->table,(field)*pc);
			acc = f?(int)*f:NULLVAL;
		} else
			acc = NULLVAL;
		pc++;
		Next;

	Instr(OAccThis);
		acc = c->_this;
		Next;

	Instr(OSwap);
		Error( sp == c->spmax , OUT_STACK);
		{
			uint tmp = acc;
			acc = *sp;
			*sp = tmp;
		}
		Next;

	Instr(OSetGlobal);
		Error( *pc < 0 || *pc >= (int)c->globals_size , INVALID_OPCODE);
		globs[*pc++] = acc;
		Next;

	Instr(OSetStack);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( sp + *pc >= c->spmax , OUT_STACK);
		sp[*pc++] = acc;
		Next;

	Instr(OSetRef);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( sp + *pc >= c->spmax , OUT_STACK);
		if( val_is_ref(sp[*pc]) )
			val_ref(sp[*pc++]) = (value)acc;
		else
			sp[*pc++] = NULLVAL;		
		Next;

	Instr(OSetEnv);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( *pc >= val_block_size(env) , OUT_ENV);
		val_ref(val_block_ptr(env)[*pc++]) = (value)acc;
		Next;

	Instr(OSetObj);
		Error( sp == c->spmax , UNDERFLOW);
		{
			uint tmp = *sp;
			*sp++ = NULLVAL;			
			if( val_is_object(acc) ) {
				otable_replace( ((vobject*)acc)->table, (field)*pc++, (value)tmp );
				acc = tmp;
			}
			else {
				pc++;
				acc = NULLVAL;
			}
		}
		Next;

	Instr(OIncr);
		if( acc & 1 ) 
			acc += 2;
		else
			acc = NULLVAL;
		Next;

	Instr(ODecr);
		if( acc & 1 )
			acc -= 2;
		else
			acc = NULLVAL;
		Next;

	Instr(ONewObj);
		if( *pc == 0 )
			acc = (int)alloc_object(NULLVAL);
		else {
			Error( *pc < 0 || *pc > (int)c->globals_size , INVALID_OPCODE);		
			acc = (int)create_instance((value)globs[*pc - 1]);
		}
		pc++;
		Next;

#define AppendString(str,fmt,x,way) { \
		uint len, len2; \
		value v; \
		len = val_strlen(str); \
		len2 = sprintf(c->tmp,fmt,x); \
		v = alloc_raw_string(len+len2); \
		if( way ) { \
		 	memcpy((char*)val_string(v),val_string(str),len); \
			memcpy((char*)val_string(v)+len,c->tmp,len2+1); \
		} else { \
			memcpy((char*)val_string(v),c->tmp,len2); \
			memcpy((char*)val_string(v)+len2,val_string(str),len+1); \
		} \
		acc = (int)v; \
	}

#define ObjectOp(op) { \
		value _acc = (value)acc; \
		acc = (int)val_ocallN((value)*sp,op,&_acc,1); \
		if( acc == 0 && val_is_string(_acc) ) { \
			stringbuf b = alloc_stringbuf(NULL); \
			val_to_string((value)*sp,b); \
			val_to_string(_acc,b); \
			acc = (int)stringbuf_to_string(b); \
		} \
	}

	Instr(OpAdd);
		Error( sp == c->spmax , UNDERFLOW);
		if( !acc ) {
			if( val_is_object(*sp) ) {
				if( val_tag(*sp) == VAL_STRING ) {
					int len = val_strlen(*sp);
					value v = alloc_raw_string(len+4);
					char *s = (char*)val_string(v);
					memcpy(s,val_string(*sp),len);
					memcpy(s+len,"null",4);
					acc = (int)v;
				} else
					ObjectOp(op_add);
			}
		} else if( !(*sp) ) {
			if( val_is_string(acc) ) {
				int len = val_strlen(acc);
				value v = alloc_raw_string(len+4);
				char *s = (char*)val_string(v);
				memcpy(s,"null",4);
				memcpy(s+4,val_string(acc),len);
				acc = (int)v;
			} else
				acc = NULLVAL;
		} else if( (acc & 1) && (*sp & 1) )
			acc = (int)alloc_int(val_int(*sp) + val_int(acc));
		else if( acc & 1 ) {
			if( val_tag(*sp) == VAL_FLOAT )
				acc = (int)alloc_float(val_float(*sp) + val_int(acc));
			else if( val_tag(*sp) == VAL_STRING )
				AppendString(*sp,"%d",val_int(acc),true)
			else if( val_tag(*sp) == VAL_OBJECT )
				ObjectOp(op_add)
			else
				acc = NULLVAL;
		} else if( *sp & 1 ) {
			if( val_tag(acc) == VAL_FLOAT )
				acc = (int)alloc_float(val_int(*sp) + val_float(acc));
			else if( val_tag(acc) == VAL_STRING )
				AppendString(acc,"%d",val_int(*sp),false)	
			else
				acc = NULLVAL;
		} else if( val_tag(acc) == VAL_FLOAT ) {			
			if( val_tag(*sp) == VAL_FLOAT )
				acc = (int)alloc_float(val_float(*sp) + val_float(acc));
			else if( val_tag(*sp) == VAL_STRING )
				AppendString(*sp,"%.10g",val_float(acc),true)
			else if( val_tag(*sp) == VAL_OBJECT )
				ObjectOp(op_add)
			else
				acc = NULLVAL;
		} else if( val_tag(*sp) == VAL_FLOAT ) {
			if( val_tag(acc) == VAL_STRING )
				AppendString(acc,"%.10g",val_float(*sp),false)
			else
				acc = NULLVAL;
		} else if( val_tag(acc) == VAL_STRING && val_tag(*sp) == VAL_STRING ) {
			uint len1 = val_strlen(*sp);
			uint len2 = val_strlen(acc);
			value v = alloc_raw_string(len1+len2);
			memcpy((char*)val_string(v),val_string(*sp),len1);
			memcpy((char*)val_string(v)+len1,val_string(acc),len2+1);
			acc = (int)v;
		} else if( val_tag(*sp) == VAL_OBJECT )
			ObjectOp(op_add)
		else
			acc = NULLVAL;
		*sp++ = NULLVAL;
		Next;

#define NumberOp(op,oop) \
			Error( sp == c->spmax , UNDERFLOW); \
			if( !acc ) { \
				if( val_is_object(*sp) ) \
					ObjectOp(oop); \
			} else if( !*sp ) \
				acc = NULLVAL; \
			else if( (acc & 1) && (*sp & 1) ) \
				acc = (int)alloc_int(val_int(*sp) op val_int(acc)); \
			else if( acc & 1 ) { \
				if( val_tag(*sp) == VAL_FLOAT ) \
					acc = (int)alloc_float(val_float(*sp) op val_int(acc)); \
				else if( val_tag(*sp) == VAL_OBJECT ) \
					ObjectOp(oop) \
				else \
					acc = NULLVAL; \
			} else if( *sp & 1 ) { \
				if( val_tag(acc) == VAL_FLOAT ) \
					acc = (int)alloc_float(val_int(*sp) op val_float(acc)); \
				else \
					acc = NULLVAL; \
			} else if( val_tag(acc) == VAL_FLOAT && val_tag(*sp) == VAL_FLOAT ) \
				acc = (int)alloc_float(val_float(*sp) op val_float(acc)); \
			else if( val_tag(*sp) == VAL_OBJECT ) \
				ObjectOp(oop) \
			else \
				acc = NULLVAL; \
			*sp++ = NULLVAL; \
			Next;

	Instr(OpSub); NumberOp(-,op_sub);
	Instr(OpMult); NumberOp(*,op_mult);
	Instr(OpDiv);
		if( acc == 1 ) { // division by integer 0
			Error( sp == c->spmax , UNDERFLOW);
			acc	= NULLVAL;
			*sp++ = NULLVAL;
		} else {
			NumberOp(/,op_div);
		}
		Next;

#define IntOp(op) \
			Error( sp == c->spmax , UNDERFLOW); \
			if( (acc & 1) && (*sp & 1) ) \
				acc = (int)alloc_int(val_int(*sp) op val_int(acc)); \
			else \
				acc = NULLVAL; \
			*sp++ = NULLVAL; \
			Next
			

	Instr(OpMod);
		if( acc == 1 ) { // checked mod 0
			Error( sp == c->spmax , UNDERFLOW);
			acc = NULLVAL;
			*sp++ = NULLVAL;
		} else {
			IntOp(%);
		}
		Next;

	Instr(OpShl); IntOp(<<);
	Instr(OpShr); IntOp(>>);
	Instr(OpOr); IntOp(|);
	Instr(OpAnd); IntOp(&);
	Instr(OpXor); IntOp(^);

#define Test(test) \
			Error( sp == c->spmax , UNDERFLOW); \
			acc = (int)val_compare((value)*sp,(value)acc); \
			*sp++ = NULLVAL; \
			acc = (acc == 0xFF)?NULLVAL:((acc test 0)?3:1); \
			Next

	Instr(OpEq);
		Error( sp == c->spmax , UNDERFLOW);
		if( *sp == NULLVAL || acc == NULLVAL ) {
			acc = (*sp == (unsigned int)acc)?3:1;
			*sp++ = NULLVAL;
		} else {
			acc = (int)val_compare((value)*sp,(value)acc);
			*sp++ = NULLVAL;
			acc = (acc == 0xFF)?NULLVAL:((acc == 0)?3:1);
		}
		Next;

	Instr(OpNeq);
		Error( sp == c->spmax , UNDERFLOW);
		if( *sp == NULLVAL || acc == NULLVAL ) {
			acc = (*sp == (unsigned int)acc)?1:3;
			*sp++ = NULLVAL;
		} else {
			acc = (int)val_compare((value)*sp,(value)acc);
			*sp++ = NULLVAL;
			acc = (acc == 0xFF)?NULLVAL:((acc != 0)?3:1);
		}
		Next;

	Instr(OpLt); Test(<);
	Instr(OpLte); Test(<=);
	Instr(OpGt); Test(>);
	Instr(OpGte); Test(>=);

	Instr(OClosure);
		Error( *pc < 0 || *pc >= (int)c->globals_size , INVALID_OPCODE);
		Error( sp == c->spmax , UNDERFLOW);
		Error( acc <= 1 || !(acc & 1) , UNDERFLOW);
		acc >>= 1;
		{
			value new_env = alloc_block(acc);
			Error( sp + acc > c->spmax , UNDERFLOW);
			while( acc-- ) {
				// TODO : test that *sp is a ref
				val_block_ptr(new_env)[acc] = (value)*sp;
				*sp++ = NULLVAL;
			}
			if( val_is_function(globs[*pc]) ) {
				acc = (int)alloc_fun(val_fun_name(globs[*pc]),
						((vfunction*)globs[*pc])->addr,
						((vfunction*)globs[*pc])->nargs);
				((value)acc)->h = val_mkheader(VAL_FUNCTION,sizeof(vfunction));
				((vfunction*)acc)->env = new_env;
			} else
				acc = NULLVAL;
			pc++;
		}
		Next;

	Instr(OMakeRef);
		Error( *pc < 0 , INVALID_OPCODE);
		Error( sp + *pc >= c->spmax , OUT_STACK);
		{
			value ref = alloc_block(1);
			*val_block_ptr(ref) = (value)sp[*pc];
			sp[*pc++] = (int)ref;
			acc = (int)ref;
		}
		Next;

	Instr(OArray);
		Error( *pc < 0 , INVALID_OPCODE );
		Error( sp + *pc > c->spmax , UNDERFLOW );
		{
			value *ptr;
			uint nelts = *pc++;
			acc = (int)alloc_array(nelts,&ptr);			
			while( nelts-- > 0 ) {
				*ptr++ = (value)*sp;
				*sp++ = NULLVAL;
			}
		}
		Next;

	Instr(OLast);
		Error( sp != c->spmax || csp != c->spmin - 1 , ABORTED);
		return (value)acc;

	default:
		c->error = INVALID_OPCODE;
		c->error_pc = ((uint)pc-(uint)c->pcmin)/4;
		return NULLVAL;
	}
}

// -------------------------------------------------------------------------//
