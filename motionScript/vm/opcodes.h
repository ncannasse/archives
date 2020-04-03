#ifndef OPCODES_H
#define OPCODES_H

enum OPCODE {
	  OPushNull
	, OPush
	, OPop
	, OJmpNot
	, OJmpIf
	, OJmp
	, OCall
	, OCallObj
	, ORet
	
	, OAccNull
	, OAccNot
	, OAccInt
	, OAccStack
	, OAccGlobal
	, OAccRef
	, OAccObj
	, OAccThis
	, OAccEnv
	, OAccEnvRaw
	, OSwap

	, OSetGlobal
	, OSetStack
	, OSetRef
	, OSetObj
	, OSetEnv
	, OIncr
	, ODecr
	, OClosure
	, ONewObj
	, OMakeRef
	, OArray

	, OpAdd
	, OpSub
	, OpMult
	, OpDiv
	, OpMod
	, OpEq
	, OpNeq
	, OpShl
	, OpShr
	, OpOr 
	, OpAnd
	, OpXor
	, OpLt
	, OpLte
	, OpGt
	, OpGte

	, OLast
};

#ifdef IS_LONG_TABLE
static bool is_long_opcode[] = {
	false, // OPushNull
	false, // OPush
	true, // OPop
	true, // OJmpNot
	true, // OJmpIf
	true, // OJmp
	true, // OCall
	true, // OCallObj
	true, // ORet
	
	false, // OAccNull
	false, // OAccNot
	true, // OAccInt
	true, // OAccStack
	true, // OAccGlobal
	true, // OAccRef
	true, // OAccObj
	false, // OAccThis
	true, // OAccEnv
	true, // OAccEnvRaw
	false, // OSwap

	true, // OSetGlobal
	true, // OSetStack
	true, // OSetRef
	true, // OSetObj
	true, // OSetEnv
	false, // OIncr
	false, // ODecr
	true, // OClosure
	true, // ONewObj
	true, // OMakeRef

	false, // OpAdd
	false, // OpSub
	false, // OpMult
	false, // OpDiv
	false, // OpMod
	false, // OpEq
	false, // OpNeq
	false, // OpShl
	false, // OpShr
	false, // OpOr
	false, // OpAnd
	false, // OpXor
	false, // OpLt
	false, // OpLte
	false, // OpGt
	false, // OpGte

	false, // OLast
};
#endif

#endif
