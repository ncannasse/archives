(*
 *  Copyright (c)2008 Motion-Twin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *)
open As3
open As3hl

type stack_type =
	| TAny
	| TGlobal
	| TString of (int * string)
	| TFunction of hl_method
	| TInst of hl_name
	| TBlock of (hl_name * stack_type) array
	| TArray of stack_type list
	| TClass of hl_class

let s_path path = String.concat "." (fst path @ [snd path])

let s_oid = function None -> "" | Some id -> "::" ^ id

let s_namespace = function
	| HNPrivate id -> "private" ^ s_oid id
	| HNPublic id -> "public" ^ s_oid id
	| HNInternal id -> "internal" ^ s_oid id
	| HNProtected id -> "protected::" ^ id
	| HNNamespace id -> id
	| HNExplicit id -> "explicit:" ^ id
	| HNStaticProtected id -> "static_protected" ^ s_oid id

let s_nset nset = "{" ^ String.concat "|" (List.map s_namespace nset) ^ "}"

let rec s_name = function
	| HMPath (pack,name) -> s_path (pack,name)
	| HMName (id,ns) -> s_namespace ns ^ "::" ^ id
	| HMMultiName (id,nset) -> s_nset nset ^ s_oid id
	| HMRuntimeName id -> "rt:" ^ id
	| HMRuntimeNameLate -> "<rtlate>"
	| HMMultiNameLate nset -> "late:" ^ s_nset nset
	| HMAttrib n -> "attr:" ^ s_name n
	| HMParams (n,nl) -> s_name n ^ "<" ^ String.concat "," (List.map s_name nl) ^ ">"
	| HMAny -> "~any"

let s_oname = function
	| None -> "*"
	| Some t -> s_name t

let rec s_type = function
	| TBlock l -> "block{" ^ String.concat "," (List.map (fun (n,t) -> s_name n ^ ":" ^ s_type t) (Array.to_list l)) ^ "}"
	| TString _ -> "string"
	| TAny -> "*"
	| TFunction _ -> "function"
	| TInst t -> s_name t
	| TGlobal -> "global"
	| TArray l -> "[" ^ String.concat "," (List.map s_type l) ^ "]"
	| TClass c -> "class:" ^ s_name c.hlc_name

let s_value = function
	| HVNone -> "<none>"
	| HVNull -> "null"
	| HVBool b -> if b then "true" else "false"
	| HVString s -> "\"" ^ s ^ "\""
	| HVInt i | HVUInt i -> Int32.to_string i
	| HVFloat f -> string_of_float f
	| HVNamespace (n,ns) -> s_namespace ns ^ "<" ^ string_of_int n ^ ">"

let s_escape s =
	let b = Buffer.create (String.length s) in
	for i = 0 to (String.length s) - 1 do
		match s.[i] with
		| '\n' -> Buffer.add_string b "\\n"
		| '\t' -> Buffer.add_string b "\\t"
		| '\r' -> Buffer.add_string b "\\r"
		| '"' -> Buffer.add_string b "\\\""
		| '\\' -> Buffer.add_string b "\\\\"
		| c -> Buffer.add_char b c
	done;
	Buffer.contents b

let s_op op =
	let param k i = Printf.sprintf "%s %d" k i in
	let param2 k i1 i2 = Printf.sprintf "%s %d %d" k i1 i2 in
	let sparam k p n = Printf.sprintf "%s %s %d" k (s_name p) n in
	let prop k p = Printf.sprintf "%s %s" k (s_name p) in
	match op with
	| HBreakPoint -> "breakpoint"
	| HNop -> "nop"
	| HThrow -> "throw"
	| HGetSuper n -> prop "get_super" n
	| HSetSuper n -> prop "set_super" n
	| HDxNs s -> "dxns " ^ s
	| HDxNsLate -> "dxnslate"
	| HRegKill r -> param "regkill" r
	| HLabel -> "label"
	| HJump (j,n) -> param ("jump" ^ As3code.dump_jump j) n
	| HSwitch (n,nl) -> param "switch" n ^ " [" ^ String.concat "," (List.map string_of_int nl) ^ "]"
	| HPushWith -> "pushwith"
	| HPopScope -> "popscope"
	| HForIn -> "forin"
	| HHasNext -> "hasnext"
	| HNull -> "null"
	| HUndefined -> "undefined"
	| HForEach -> "foreach"
	| HSmallInt n -> param "smallint" n
	| HInt n -> param "int" n
	| HTrue -> "true"
	| HFalse -> "false"
	| HNaN -> "nan"
	| HPop -> "pop"
	| HDup -> "dup"
	| HSwap -> "swap"
	| HString s -> "string \"" ^ s_escape s ^ "\""
	| HIntRef i -> Printf.sprintf "intref %ld" i
	| HUIntRef i -> Printf.sprintf "uintref %ld" i
	| HFloat f -> Printf.sprintf "float %f" f
	| HScope -> "scope"
	| HNamespace ns -> "namespace " ^ s_namespace ns
	| HNext (r1,r2) -> param2 "next" r1 r2
	| HFunction f -> param "function" f.hlmt_index
	| HCallStack n -> param "callstack" n
	| HConstruct n -> param "construct" n
	| HCallMethod (s,n) -> param2 "callmethod" s n
	| HCallStatic (m,n) -> param "callstatic" n
	| HCallSuper (p,n) -> sparam "callsuper" p n
	| HCallProperty (p,n) -> sparam "callproperty" p n
	| HRetVoid -> "retvoid"
	| HRet -> "ret"
	| HConstructSuper n -> param "constructsuper" n
	| HConstructProperty (p,n) -> sparam "constructproperty" p n
	| HCallPropLex (p,n) -> sparam "callproplex" p n
	| HCallSuperVoid (p,n) -> sparam "callsupervoid" p n
	| HCallPropVoid (p,n) -> sparam "callpropvoid" p n
	| HApplyType n -> param "applytype" n
	| HObject n -> param "object" n
	| HArray n -> param "array" n
	| HNewBlock -> "newblock"
	| HClassDef c -> prop "class" c.hlc_name
	| HGetDescendants n -> prop "getdescendants" n
	| HCatch n -> param "catch" n
	| HFindPropStrict p -> prop "findpropstrict" p
	| HFindProp p -> prop "findprop" p
	| HFindDefinition p -> prop "findefinition" p
	| HGetLex p -> prop "getlex" p
	| HSetProp p -> prop "setprop" p
	| HReg r -> param "reg" r
	| HSetReg r -> param "setreg" r
	| HGetGlobalScope -> "getglobalscope"
	| HGetScope n -> param "getscope" n
	| HGetProp p -> prop "getprop" p
	| HInitProp p -> prop "initprop" p
	| HDeleteProp p -> prop "deleteprop" p
	| HGetSlot s -> param "getslot" s
	| HSetSlot s -> param "setslot" s
	| HToString -> "tostring"
	| HToXml -> "toxml"
	| HToXmlAttr -> "toxmlattr"
	| HToInt -> "toint"
	| HToUInt -> "touint"
	| HToNumber -> "tonumber"
	| HToBool -> "tobool"
	| HToObject -> "toobject"
	| HCheckIsXml -> "checkisxml"
	| HCast n -> prop "cast" n
	| HAsAny -> "asany"
	| HAsString -> "asstring"
	| HAsType p -> prop "as" p
	| HAsObject -> "asobject"
	| HIncrReg r -> param "incrreg" r
	| HDecrReg r -> param "decrreg" r
	| HTypeof -> "typeof"
	| HInstanceOf -> "instanceof"
	| HIsType p -> prop "is" p
	| HIncrIReg r -> param "incrireg" r
	| HDecrIReg r -> param "decrireg" r
	| HThis -> "this"
	| HSetThis -> "setthis"
	| HDebugReg (i,r,n) -> Printf.sprintf ".reg %s %d %d" i r n
	| HDebugLine n -> param ".line" n
	| HDebugFile f -> ".file " ^ f
	| HBreakPointLine n -> param ".break" n
	| HTimestamp -> ".time"
	| HOp op -> As3code.dump_op op
	| HUnk c -> param "unknown" (int_of_char c)


let dump_function ch f name =
	let w = IO.nwrite ch in
	w "function ";
	w (s_name name);
	w " : \n";
	Array.iter (fun (n,t,s,c) ->
		IO.printf ch "  [%d] %slocal %s : %s\n" s (if c then "const " else "") (s_name n) (s_oname t);
	) f.hlf_locals;
	Array.iter (fun tc ->
		IO.printf ch "  try-catch %d %s\n" tc.hltc_start (s_oname tc.hltc_type)
	) f.hlf_trys

let dump_method ch m =
	let w = IO.nwrite ch in
	w "(";
	let vals = ref (match m.hlmt_dparams with None -> [] | Some l -> List.rev l) in
	let args = List.rev (List.map (fun n -> n, (match !vals with [] -> None | x :: l -> vals := l; Some x)) (List.rev m.hlmt_args)) in
	let args = List.map (fun (n,v) -> s_oname n ^ (match v with None -> "" | Some v -> " = " ^ s_value v)) args in
	let args = if m.hlmt_var_args then args @ ["...arguments"] else args in
	w (String.concat ", " args);
	w ") : ";
	w (s_oname m.hlmt_ret)

let dump_meta ch f =
	let w = IO.nwrite ch in
	let loop m =
		IO.printf ch "\t[%s" m.hlmeta_name;
		let first = ref true in
		Array.iter (fun (k,v) ->
			if !first then begin
				first := false;
				w "(";
			end else w ", ";
			match k with
			| None -> w ("\"" ^ s_escape v ^ "\"")
			| Some k -> w (k ^ "=" ^ "\"" ^ s_escape v ^ "\"")
		) m.hlmeta_data;
		if not !first then w ")";
		w "]\n"
	in
	match f.hlf_metas with
	| None -> ()
	| Some m -> Array.iter loop m

let dump_field ch f =
	let w = IO.nwrite ch in
	match f.hlf_kind with
	| HFMethod m ->
		if m.hlm_final then w "final ";
		if m.hlm_override then w "override ";
		w "function ";
		(match m.hlm_kind with
		| MK3Normal -> ()
		| MK3Getter -> w "get "
		| MK3Setter -> w "set ");
		w (s_name f.hlf_name);
		dump_method ch m.hlm_type
	| HFVar v ->
		if v.hlv_const then w "const ";
		w "var ";
		w (s_name f.hlf_name);
		w " : ";
		w (s_oname v.hlv_type);
		(match v.hlv_value with
		| HVNone -> ()
		| _ ->
			w " = ";
			w (s_value v.hlv_value));
	| HFFunction m ->
		w "var ";
		w (s_name f.hlf_name);
		w " : function";
		dump_method ch m
	| HFClass c ->
		w (s_name f.hlf_name);
		w " = CLASS ";
		w (s_name c.hlc_name)

let dump_class ch c =
	let w = IO.nwrite ch in
	if not c.hlc_sealed then w "dynamic ";
	if c.hlc_final then w "final ";
	(match c.hlc_namespace with None -> () | Some ns -> IO.printf ch "%s " (s_namespace ns));
	w (if c.hlc_interface then "interface " else "class ");
	w (s_name c.hlc_name);
	(match c.hlc_super with None -> () | Some n -> IO.printf ch " extends %s" (s_name n));
	Array.iter (fun i -> IO.printf ch " implements %s" (s_name i)) c.hlc_implements;
	w " {\n";
	w "\tfunction construct";
	dump_method ch c.hlc_construct;
	w "\n";
	Array.iter (fun f ->
		dump_meta ch f;
		w "\t";
		dump_field ch f;
		w "\n";
	) c.hlc_fields;
	Array.iter (fun f ->
		dump_meta ch f;
		w "\t";
		w "static ";
		dump_field ch f;
		w "\n";
	) c.hlc_static_fields;
	w "}\n\n"
