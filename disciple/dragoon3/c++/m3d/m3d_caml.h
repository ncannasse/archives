/* **************************************************************************** */
/*																				*/
/* Caml Defs																	*/
/*		(c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */

extern "C" {
#	include <caml/mlvalues.h>
#	include <caml/alloc.h>
#	include <caml/callback.h>
#	include <caml/custom.h>
#	include <caml/fail.h>
#	include <caml/memory.h>
}

#define Handle_val(v) (*((HWND*)(v)))
#define Is_some(v) ((v) != Val_int(0))
#define Is_none(v) ((v) == Val_int(0))
#define Value_of(v) (Field((v), 0))
#define Val_none (Val_int(0))

#undef Bool_val
#define Bool_val(v) (Int_val(v)?true:false)

/* **************************************************************************** */

