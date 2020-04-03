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
open Dump

type dump_infos = {
	inf_contexts : stack_type array;
	inf_stacks : stack_type list array;
}

type ctx = {
	key : int array;
	permut : int array;
	known_classes : (hl_name, hl_class) Hashtbl.t;
	function_contexts : (int, dump_infos) Hashtbl.t;
	methods : (int, hl_method) Hashtbl.t;
	classes : (int, hl_class) Hashtbl.t;
	curclass : hl_class option;
	mutable curline : int;
	mutable curmethod : string;
	mutable stack : stack_type list;
	previous_scope : stack_type list;
	mutable scope : stack_type list;
	mutable ostrings : int list;
	mutable visited : (int,unit) PMap.t;
	mutable in_static : bool;
	mutable global_fields : hl_field array;
	localfunction : bool;
	contexts : stack_type array;
	registers : stack_type array;
	slots : (hl_name * stack_type) array list;
	exceptions : (hl_name * stack_type) array;
	mutable curinfos : dump_infos;
	bitmaps : (int, string) Hashtbl.t;
}

type dump_ctx = {
	dump_file : string;
	infos : unit IO.output;
	functions : (int, hl_method) Hashtbl.t;
	current : (int, dump_infos) Hashtbl.t;
}

exception Branch of int list

let real_obfu = ref true
let dump_context = ref false
let all_contexts = Hashtbl.create 0
let ignored_vars = Hashtbl.create 0
let ignored_patterns = ref []
let extern_classes = ref []

let empty_method = {
	hlmt_index = -1;
	hlmt_ret = None;
	hlmt_args = [];
	hlmt_native = false;
	hlmt_var_args = false;
	hlmt_arguments_defined = false;
	hlmt_uses_dxns = false;
	hlmt_new_block = false;
	hlmt_unused_flag = false;
	hlmt_debug_name = None;
	hlmt_dparams = None;
	hlmt_pnames = None;
	hlmt_function = None;
}

let empty_class = {
	hlc_index = -1;
	hlc_name = HMPath ([],"<empty>");
	hlc_super = None;
	hlc_sealed = false;
	hlc_final = false;
	hlc_interface = false;
	hlc_namespace = None;
	hlc_implements = [||];
	hlc_construct = empty_method;
	hlc_fields = [||];
	hlc_static_construct = empty_method;
	hlc_static_fields = [||];
}

let t_number = TInst (HMPath ([],"Number"))
let t_int = TInst (HMPath ([],"int"))
let t_bool = TInst (HMPath ([],"Boolean"))

let t_opt = function
	| None -> TAny
	| Some t -> TInst t

(* --------------------------------- TOOLS / PRINTING ----------------------------------- *)

exception Error of string

let warn msg = prerr_endline msg

let err msg = raise (Error msg)

let ext file e =
	let base = (try Filename.chop_extension file with _ -> file) in
	base ^ e

let opt f ctx v =
	match v with
	| None -> None
	| Some v -> Some (f ctx v)

let error ctx msg =
	let line = (if ctx.curline = 0 then "" else " line " ^ string_of_int ctx.curline) in
	err (msg ^ " in " ^ (match ctx.curclass with None -> "???" | Some c -> s_name c.hlc_name) ^ "::" ^ ctx.curmethod ^ line)

let get ctx arr p =
	try arr.(p) with _ -> error ctx "Read outside bounds"

let set ctx arr p v =
	try arr.(p) <- v with _ -> error ctx "Write outside bounds"

let amem f a =
	let rec loop n =
		if n = 0 then false else
		let n = n - 1 in
		let k = a.(n) in
		if f k then true else loop n
	in
	loop (Array.length a)

let afind f a =
	let rec loop n =
		if n = 0 then raise Not_found;
		let n = n - 1 in
		let k = a.(n) in
		if f k then k else loop n
	in
	loop (Array.length a)

let follow = function
	| TString _ -> TInst (HMPath ([],"String"))
	| TArray _ -> TInst (HMPath ([],"Array"))
	| TFunction _ -> TInst (HMPath ([],"Function"))
	| t -> t

let rec name_path n =
	match n with
	| HMPath (path,name) -> path @ [name]
	| HMName (name,ns) ->
		let path = (match ns with
			| HNPrivate (Some n) | HNPublic (Some n) | HNInternal (Some n) | HNProtected n ->
				ExtString.String.nsplit n "."
			| _ -> []
		) in
		path @ [name]
	| HMMultiName (Some name,[ns]) ->
		name_path (HMName (name,ns))
	| HMAny
	| HMMultiName _
	| HMRuntimeName _
	| HMRuntimeNameLate
	| HMMultiNameLate _
	| HMAttrib _
	| HMParams _ ->
		err ("Invalid name path " ^ s_name n)

let field_type f =
	match f.hlf_kind with
	| HFMethod m ->
		(match m.hlm_kind with
		| MK3Normal -> TFunction m.hlm_type
		| MK3Getter -> t_opt m.hlm_type.hlmt_ret
		| MK3Setter -> (match m.hlm_type.hlmt_args with Some t :: _ -> TInst t | _ -> TAny));
	| HFVar v -> t_opt v.hlv_type
	| HFFunction _
	| HFClass _ -> assert false

let rec raw_field_name = function
	| HMPath (_,id) | HMName (id,_) | HMMultiName (Some id,_)
	| HMRuntimeName id -> id
	| HMAttrib r ->
		raw_field_name r
	| HMAny
	| HMMultiName (None,_)
	| HMRuntimeNameLate
	| HMMultiNameLate _
	| HMParams _ ->
		""

let rec resolve_class ctx t =
	match t with
	| HMMultiName (Some id,ns) ->
		let rec loop = function
			| [] -> raise Not_found
			| n :: ns ->
				try					
					resolve_class ctx (HMName (id,n))
				with Not_found ->
					loop ns
		in
		loop ns
	| _ ->
		Hashtbl.find ctx.known_classes t

let getclass ctx t =
	try
		resolve_class ctx t
    with Not_found ->
		err ("Failed to resolve class " ^ s_name t)

(* --------------------------------- OBFUSCATION PROCESS ----------------------------------- *)

let obfu_cache = Hashtbl.create 0
let obfu_inverse_cache = Hashtbl.create 0

let init_key key =
	let hkey = Digest.string ("salt" ^ key ^  "_salt_" ^ key) in
	let hkey = Array.init 16 (fun i -> int_of_char (String.get hkey i)) in
	Rand.full_init hkey;
	let permut = Array.init 128 (fun i -> i) in
	for i = 1 to 500 do
		let ipos = i land 127 in
		let pos = Rand.int 128 in
		let k = permut.(pos) in
		permut.(pos) <- permut.(ipos);
		permut.(ipos) <- k;
	done;
	hkey, permut

let hash ctx n =
	(* 
		hash the string : first a permutation amount the first 128 codes,
		then a xor with the key
	*)
	let delta = ref 0 in
	let l = String.length n in
	for i = 0 to l - 1 do
		delta := !delta * 131 + int_of_char (String.get n i);
	done;
	let delta = !delta in
	let h = ref 0 in
	for i = l - 1 downto 0 do
		h := !h * 131;
		let cc = int_of_char (String.get n i) in
		h := !h + ctx.permut.((cc + delta) land 0x7F) lxor ctx.key.(i land 15);
	done;
	let b = Buffer.create 0 in
	if !h < 0 then h := - !h;
	while !h > 0 do
		(* some chars are not very accepted in the XML of describeType... *)
		Buffer.add_char b (match char_of_int (!h mod 128) with
		| '\000' -> '\001'			
		| '<' -> '('
		| '>' -> ')'
		| '"' -> '\''
		| '.' | ':' -> ';'			
		| c -> c);
		h := !h lsr 7;
	done;
	let rec loop h = 
		try
			let old = Hashtbl.find obfu_inverse_cache h in
			warn ("Obfuscation of '" ^ old ^ "' conflicts with obfuscation of '" ^ n ^ "'");
			loop (h ^ "_")
		with Not_found ->
			h
	in
	let h = loop (Buffer.contents b) in
	Hashtbl.add obfu_inverse_cache h n;
	h

let obfu_id ctx n =
	try
		Hashtbl.find obfu_cache n
	with Not_found ->
		let starts_with p =			
			let pl = String.length p in
			String.length n >= pl && String.sub n 0 pl = p
		in
		let h = if Hashtbl.mem ignored_vars [n] || List.exists starts_with (!ignored_patterns) then
			n 
		else if not (!real_obfu) then
			n ^ "_"
		else
			hash ctx n
		in
		Hashtbl.add obfu_cache n h;
		h

let obfu_path ctx (path,name) =
	let rec is_protected l =
		Hashtbl.mem ignored_vars ("*" :: l) || (match l with [] -> false | _ :: l -> is_protected l)
	in
	let rec loop = function
		| [] -> []
		| (x :: l) as all -> if Hashtbl.mem ignored_vars all then all else obfu_id ctx x :: loop l
	in
	let p = name :: List.rev path in
	if is_protected p then
		(path,name)
	else match loop p with
	| [] -> assert false
	| name :: path -> List.rev path, name

(*
	If we have a namespace of the form :
		a.b.c.d
	Then we are trying to lookup it by appending the classname which
	it is associated with. This enable to obfuscate a given class
		a.b.c.d.MyClass
	without obfuscating the namespace alone.

	In the case the namespace contains the full class name :
		a.b.c.d:MyClass
	Then the parameter is simply ignored
*)
let obfu_path_id clname ctx i =
	match List.rev (ExtString.String.nsplit i ".") with
	| [] -> ""
	| x :: p ->
		let p, x, sep = (try
			let a, b = ExtString.String.split x ":" in
				List.rev (a :: p), b, true
			with _ ->
				List.rev (x :: p), clname, false
		) in
		match p with
		| "flash" :: _ -> i
		| x :: _ when String.length x > 7 && String.sub x 0 7 = "http://" -> i
		| _ ->
			match obfu_path ctx (p,x) with
			| [], x -> if sep then x else ""
			| p, x -> String.concat "." p ^ (if sep then ":" ^ x else "")

let rec obfu_ns clname ctx ns =
	match ns with
	| HNPrivate i -> HNPrivate (opt (obfu_path_id clname) ctx i)
	| HNPublic i -> HNPublic (opt (obfu_path_id clname) ctx i)
	| HNInternal i -> HNInternal (opt (obfu_path_id clname) ctx i)
	| HNProtected i -> HNProtected (obfu_path_id clname ctx i)
	| HNNamespace i -> HNNamespace (obfu_path_id clname ctx i)
	| HNExplicit i -> HNExplicit (obfu_path_id clname ctx i)
	| HNStaticProtected i -> HNStaticProtected (opt (obfu_path_id clname) ctx i)

let rec get_var_class ctx c name =
	let rname = raw_field_name name in
	(* 
		first look if the field comes from an interface
	*)
	let interf = ref None in
	let rec search_interf c =
		Array.iter (fun t ->
			let i = getclass ctx t in
			if amem (fun f -> raw_field_name f.hlf_name = rname) i.hlc_fields then begin
				match !interf with				
				| Some i2 when i.hlc_index >= 0 && i2.hlc_index < 0 -> () (* privilege extern interfaces *)
				| _ -> interf := Some i;
			end;
			search_interf i
		) c.hlc_implements;
		match c.hlc_super with
		| None -> ()
		| Some t -> search_interf (getclass ctx t)
	in
	search_interf c;
	match !interf with
	| Some i -> Some i
	| None ->
	(*
		try to find the actual field implementation, by browsing the class tree
	*)
	let rec loop c =
		try
			let f = afind (fun f -> 
				raw_field_name f.hlf_name = rname &&
				(* don't allow override, since we want to know the real class *)
				match f.hlf_kind with
				| HFMethod { hlm_override = true } -> false
				| _ -> true
			) c.hlc_fields in
			Some (c,f)
		with Not_found -> try
			(* lookup statics *)
			let f = afind (fun f -> raw_field_name f.hlf_name = rname) c.hlc_static_fields in
			Some (c,f)
		with Not_found ->
			match c.hlc_super with
			| None -> None
			| Some t -> loop (getclass ctx t)
	in
	match loop c with
	| None ->
		(* if our class is one of the known dynamic classes then it's better to protect it *)
		if Hashtbl.mem ignored_vars ("*" :: List.rev (name_path c.hlc_name)) then
			Some c
		else
			(* we have not found the field. Maybe it's actually a class ? *)
			(* if it's not, then it means it's dynamic, then ignore class context *)
			(try Some (resolve_class ctx name) with Not_found -> None)		
	| Some (c,_) -> Some c

let rec obfu_name_space ctx n =
	match n with
	| HMAttrib n -> HMAttrib (obfu_name_space ctx n)
	| HMName (id,ns) -> HMName (id, obfu_ns id ctx ns)
	| HMMultiName (id,nset) -> HMMultiName (id,List.map (obfu_ns (match id with None -> "" | Some id -> id) ctx) nset)
	| HMMultiNameLate ns -> HMMultiNameLate (List.map (obfu_ns "" ctx) ns)
	| _ -> n

let rec obfu_name ctx c n =
	let c = (match c with None -> None | Some c -> get_var_class ctx c n) in
	match c with
	| Some { hlc_index = idx } when idx < 0 -> obfu_name_space ctx n
	| _ ->
	let cpath = match c with None -> [] | Some c -> name_path c.hlc_name in
	match n with
	| HMPath ([],name) ->
		let _, name = obfu_path ctx (cpath,name) in
		HMPath ([],name)
	| HMPath (pack,name) ->
		let pack,name = obfu_path ctx (pack,name) in
		HMPath (pack,name)
	| HMParams (n,pl) ->
		HMParams (obfu_name ctx c n, List.map (obfu_name ctx c) pl)
	| HMAttrib n ->
		HMAttrib (obfu_name ctx c n)
	| HMName (id,ns) ->
		let ns = obfu_ns id ctx ns in
		let _, id = obfu_path ctx (cpath,id) in
		HMName (id,ns)
	| HMMultiName (id,nset) ->
		let id, clname = (match id with
			| None -> None, ""
			| Some id ->
				(*
					if the multiname id is the same as the context class name then
					we remove the class name from the class path
				*)
				let cpath = (match List.rev cpath with cname :: l when id = cname -> List.rev l | _ -> cpath) in
				let _, oid = obfu_path ctx (cpath,id) in
				Some oid, id
		) in
		HMMultiName (id,List.map (obfu_ns clname ctx) nset)
	| HMRuntimeName id ->
		let _, id = obfu_path ctx (cpath,id) in
		HMRuntimeName id
	| HMRuntimeNameLate ->
		HMRuntimeNameLate
	| HMMultiNameLate ns ->
		HMMultiNameLate (List.map (obfu_ns "" ctx) ns)
	| HMAny ->
		HMAny

let rec obfu_type ctx t =
	match t with
	| HMParams (v,pl) -> HMParams (v,List.map (obfu_type ctx) pl)
	| _ ->
	try		
		let c = resolve_class ctx t in
		if c.hlc_index < 0 then raise Not_found;
		obfu_name ctx (Some c) t
	with Not_found ->
		t

let rec name ctx pos n =
	match follow ctx.contexts.(pos) with
	| TInst t ->
		(try
			obfu_name ctx (Some (resolve_class ctx t)) n
		with Not_found ->
			obfu_name_space ctx n)
	| TGlobal ->
		(try
			let c = resolve_class ctx n in
            if c.hlc_index < 0 then raise Not_found;
			obfu_name ctx (Some c) n
		with Not_found ->
			obfu_name_space ctx n)
	| TAny | TBlock _ -> obfu_name ctx None n
	| TClass c -> obfu_name ctx (Some c) n
	| TArray _ | TString _ | TFunction _ ->
		assert false

(* --------------------------------- STACK/CONTEXT MANAGEMENT ----------------------------------- *)

let pop_value ctx =
	match ctx.stack with
	| [] -> error ctx "Stack underflow"
	| x :: l -> ctx.stack <- l; x

let pop ctx name = ignore(pop_value ctx)

let push ctx v = ctx.stack <- v :: ctx.stack

let rec pop_args ctx n =
	if n > 0 then begin
		pop ctx "";
		pop_args ctx (n-1);
	end

let conv ctx t =
	pop ctx "";
	push ctx t

let set_context ctx pos t = ctx.contexts.(pos) <- t

let rec getprop ctx t name =
	match name with
	| HMMultiName (Some id,ns) ->
		let rec loop = function
			| [] -> raise Not_found
			| n :: ns ->
				try
					getprop ctx t (HMName (id,n))
				with Not_found ->
					loop ns
		in
		loop ns
	| _ ->
	match follow t with
	| TGlobal ->
		TInst (resolve_class ctx name).hlc_name
	| TInst t ->
		let rname = raw_field_name name in
		let rec loop t =
			let c = resolve_class ctx t in
			let cname = List.rev (name_path c.hlc_name) in
			try
				(*
					we are using raw_field_name here because we might have a look at superclass
					to check if the overriden field is part of the framework or not.
					And the namespace will depend of the current class for protected vars,
					so we only compare the fields names here.
				*)
				let f = afind (fun f -> raw_field_name f.hlf_name = rname) c.hlc_fields in
				if Hashtbl.mem ignored_vars ("*" :: rname :: cname) then
					TGlobal
				else
					field_type f
			with Not_found -> try
				(* this is necessary for AS3 : GetLex on statics in local class *)
				let f = afind (fun f -> f.hlf_name = name) c.hlc_static_fields in
				field_type f
			with Not_found ->
				match c.hlc_super with
				| None -> raise Not_found
				| Some t -> loop t
		in
		loop t
	| TAny -> 
		TAny
	| TBlock l ->
		snd (afind (fun (n,_) -> n = name) l)
	| TClass c ->
		let f = afind (fun f -> f.hlf_name = name) c.hlc_static_fields in
		field_type f
	| TArray _ | TString _ | TFunction _ ->
		assert false

let findprop ctx name =
	let rec loop = function
		| [] -> raise Not_found
		| t :: l ->
			(try
				t, getprop ctx t name
			with
				Not_found -> loop l)
	in
	try
		loop ctx.scope
	with Not_found -> try
		loop ctx.previous_scope
	with Not_found ->
		let c = resolve_class ctx name in
		TGlobal, TClass c

let get_enum_path ctx obj =
	try
		let t = getprop ctx obj (HMPath ([],(match obj with TClass _ -> "__isenum" | _ -> "__enum__"))) in
		if t <> TInst (HMPath ([],"Boolean")) then raise Exit;		
		let name = (match obj with TClass c -> c.hlc_name | TInst n -> n | _ -> assert false) in
		match name with
		| HMPath (pack,n) -> Some (pack @ [n])
		| _ -> None
	with _ ->
		None

let isboot ctx obj =
	match obj with
	| TClass { hlc_name = HMPath (["flash"],"Boot") } -> true
	| TClass { hlc_name = HMPath ([],x) } when String.length x > 5 && String.sub x 0 5 = "Boot_" -> true
	| _ -> false

let pop_rt ctx = function
	| HMMultiNameLate _ ->
		pop ctx "ns"
	| HMRuntimeName _ ->
		pop ctx "ns"
	| HMRuntimeNameLate ->
		pop ctx "name";
		pop ctx "ns"
	| _ ->
		()

let rec execute ctx pos op =
	match op with
	| HBreakPoint ->
		()
	| HNop ->
		()
	| HRegKill _ ->
		()
	| HLabel ->
		()
	| HThrow ->
		pop ctx "value";
		raise (Branch [])
	| HJump (j,n) ->
		(match j with
		| J3Always -> ()
		| J3True | J3False -> pop ctx "value"
		| _ ->
			pop ctx "value";
			pop ctx "value");
		raise (Branch (pos + n :: (if j = J3Always then [] else [pos + 1])))
	| HSwitch (def,cases) ->
		pop ctx "value";
		raise (Branch (List.map (fun p -> pos + p) (def::cases)))
	| HPushWith ->
		ctx.scope <- pop_value ctx :: ctx.scope
	| HPopScope ->
		ctx.scope <- (match ctx.scope with [] -> error ctx "Scope underflow" | _ :: l -> l)
	| HForIn ->
		pop ctx "index";
		pop ctx "object";
		push ctx (TString (-1,""))
	| HHasNext ->
		pop ctx "index";
		pop ctx "object";
		push ctx t_int
	| HNull | HUndefined ->
		push ctx TAny
	| HForEach ->
		pop ctx "index";
		pop ctx "object";
		push ctx TAny
	| HSmallInt _ | HInt _ ->
		push ctx t_int
	| HTrue | HFalse ->
		push ctx t_bool
	| HNaN ->
		push ctx t_number
	| HPop ->
		pop ctx "value"
	| HDup ->
		let v = pop_value ctx in
		push ctx v;
		push ctx v;
	| HSwap ->
		let v1 = pop_value ctx in
		let v2 = pop_value ctx in
		push ctx v2;
		push ctx v1;
	| HString s ->
		push ctx (TString (pos,s))
	| HIntRef _ | HUIntRef _ ->
		push ctx t_int
	| HFloat _ ->
		push ctx t_number
	| HScope ->
		ctx.scope <- pop_value ctx :: ctx.scope
	| HNamespace _ ->
		push ctx TAny
	| HNext _ ->
		push ctx t_bool
	| HFunction f ->
		push ctx (TFunction f)
	| HCallStack n ->
		pop_args ctx n;
		pop ctx "object";
		let f = pop_value ctx in
		push ctx (match f with TFunction { hlmt_ret = Some t } -> TInst t | _ -> TAny)
	| HConstruct n ->
		pop_args ctx n;
		pop ctx "object";
		push ctx TAny
	| HCallMethod (_,n) ->
		pop_args ctx n;
		pop ctx "object";
		push ctx TAny
	| HCallStatic (f,n) ->
		pop_args ctx n;
		pop ctx "object";
		push ctx (t_opt f.hlmt_ret)
	| HCallSuper (name,n) | HCallProperty (name,n) | HCallPropLex (name,n) ->
		let args = Array.init n (fun _ -> pop_value ctx) in
		pop_rt ctx name;
		let obj = pop_value ctx in
		set_context ctx pos obj;
		let p = (try getprop ctx obj name with Not_found -> TAny) in
		(match name with
		| HMPath ([],"__unprotect__") when isboot ctx obj ->
			(match args.(0) with
			| TString (pos,_) -> ctx.ostrings <- pos :: ctx.ostrings
			| _ -> ())
		| _ -> ());
		(match p with
		| TFunction { hlmt_ret = Some t } -> push ctx (TInst t)
		| TGlobal -> push ctx TGlobal
		| TInst _ when obj = TGlobal && n = 1 -> push ctx p (* in case of cast with MyClass(value) *)
		| _ -> push ctx TAny)
	| HRetVoid ->
		raise (Branch [])
	| HRet ->
		pop ctx "value";
		raise (Branch [])
	| HConstructSuper n ->
		pop_args ctx n;
		pop ctx "object"
	| HConstructProperty (name,n) ->
		let args = Array.init n (fun _ -> pop_value ctx) in
		pop_rt ctx name;
		let obj = pop_value ctx in
		(match get_enum_path ctx (TInst name) with
		| None -> ()
		| Some path ->
			match args.(n-1) with
			| TString (p,s) -> if snd (obfu_path ctx (path,s)) <> s then ctx.ostrings <- p :: ctx.ostrings
			| _ -> assert false
		);
		set_context ctx pos obj;
		push ctx (TInst name)
	| HCallPropVoid (name,n) | HCallSuperVoid (name,n) ->
		pop_args ctx n;
		pop_rt ctx name;
		let obj = pop_value ctx in
		set_context ctx pos obj
	| HApplyType n ->
		pop_args ctx (n + 1);
		push ctx (TInst (HMPath ([],"Vector")))
	| HObject n ->
		for i = 1 to n do
			pop ctx "value";
			match pop_value ctx with
			| TString (pos,_) -> ctx.ostrings <- pos :: ctx.ostrings;
			| t -> error ctx ("Unknown ident " ^ s_type t)
		done;
		push ctx TAny
	| HArray n ->
		let values = Array.init n (fun _ -> pop_value ctx) in
		push ctx (TArray (Array.to_list values))
	| HNewBlock ->
		push ctx (TBlock (List.hd ctx.slots))
	| HClassDef c ->
		pop ctx "basetype";
		push ctx (TClass c)
	| HGetDescendants name ->
		pop_rt ctx name;
		let obj = pop_value ctx in
		set_context ctx pos obj;
		push ctx (TInst (HMPath ([],"XMLList")));
	| HCatch n ->
		push ctx (TBlock [|ctx.exceptions.(n)|])
	| HFindPropStrict name | HFindProp name ->
		pop_rt ctx name;
		let obj, t = (try findprop ctx name with Not_found -> TAny, TAny) in
		set_context ctx pos obj;
		push ctx obj
	| HFindDefinition name ->
		(* ??? *)
		push ctx (TInst name);
	| HGetLex name ->
		let obj, t = (try findprop ctx name with Not_found -> TAny, TAny) in
		set_context ctx pos obj;
		push ctx t;
	| HSetSuper name | HSetProp name ->
		pop_rt ctx name;
		let value = pop_value ctx in
		let obj = pop_value ctx in
		set_context ctx pos obj;
		if name = HMPath ([],"__constructs__") then begin
			match get_enum_path ctx obj with
			| None -> ()
			| Some path ->			
				(match value with
				| TArray l -> 
					List.iter (fun v ->
						match v with
						| TString (pos,str) ->
							if snd (obfu_path ctx (path,str)) <> str then ctx.ostrings <- pos :: ctx.ostrings
						| _ -> assert false
					) l
				| _ -> assert false)
		end;
	| HReg r ->
		push ctx (get ctx ctx.registers r)
	| HSetReg r ->
		set ctx ctx.registers r (pop_value ctx)
	| HGetGlobalScope ->
		push ctx TGlobal
	| HGetScope n ->
		push ctx (try List.nth (List.rev ctx.scope) n with _ -> error ctx "Invalid scope access")
	| HGetSuper name | HGetProp name ->
		pop_rt ctx name;
		let obj = pop_value ctx in
		let v = (try getprop ctx obj name with Not_found -> TAny) in
		set_context ctx pos obj;
		push ctx v
	| HInitProp name ->
		pop_rt ctx name;
		let v = pop_value ctx in
		let obj = pop_value ctx in
		set_context ctx pos (match v with TClass c -> TInst c.hlc_name | _ -> obj);
	| HDeleteProp name ->
		pop_rt ctx name;
		set_context ctx pos (pop_value ctx);
		push ctx t_bool
	| HGetSlot r ->
		(match pop_value ctx with
		| TBlock t -> push ctx (snd (get ctx t (r - 1)))
		| TGlobal ->
			let rec loop k =
				if k = Array.length ctx.global_fields then error ctx ("Slot " ^ string_of_int r ^ " not found in global");
				let f = ctx.global_fields.(k) in
				if f.hlf_slot = r then
					f
				else
					loop (k + 1)
			in
			(match (loop 0).hlf_kind with
			| HFClass c -> push ctx (TClass c)
			| _ -> assert false)
		| t -> error ctx ("Invalid block '" ^ s_type t ^ "' scope = [" ^ String.concat ", " (List.map s_type (List.rev ctx.scope)) ^ "]"))
	| HSetSlot r ->
		pop ctx "value";
		pop ctx "slot";
	| HToString -> conv ctx (TString (-1,""))
	| HToXml -> conv ctx TGlobal
	| HToXmlAttr -> conv ctx TGlobal
	| HToInt -> conv ctx t_int
	| HToUInt -> conv ctx t_int
	| HToNumber -> conv ctx t_number
	| HToBool -> conv ctx t_bool
	| HToObject -> conv ctx TAny
	| HCheckIsXml -> ()
	| HCast t -> conv ctx (TInst t)
	| HAsAny -> conv ctx TAny
	| HAsString -> conv ctx (TString (-1,""))
	| HAsType t -> conv ctx (TInst t)
	| HAsObject -> conv ctx TAny
	| HIncrReg r ->
		set ctx ctx.registers r t_number
	| HDecrReg r ->
		set ctx ctx.registers r t_number
	| HTypeof ->
		conv ctx (TString (-1,""))
	| HInstanceOf ->
		pop ctx "type";
		pop ctx "value";
		conv ctx t_bool
	| HIsType _ ->
		pop ctx "value";
		conv ctx t_bool
	| HIncrIReg r ->
		set ctx ctx.registers r t_int
	| HDecrIReg r ->
		set ctx ctx.registers r t_int
	| HThis ->
		let v = (match ctx.curclass with
			| None -> TGlobal
			| Some c ->
				if isboot ctx (TClass c) then
					TGlobal
				else if ctx.localfunction then
					TBlock [||] (* don't allow any getlex/getprop to succeed *)
				else if ctx.in_static then
					TClass c
				else
					TInst c.hlc_name
		) in
		push ctx v
	| HSetThis ->
		pop ctx "value"
	| HDebugLine l ->
		ctx.curline <- l
	| HDebugReg _
	| HDebugFile _
	| HBreakPointLine _ ->
		()
	| HTimestamp ->
		()
	| HDxNs _ | HDxNsLate ->
		()
	| HOp op ->
		(match op with
		| A3ONeg | A3OIncr | A3ODecr | A3ONot | A3OBitNot ->
			pop ctx "value";
			push ctx t_number
		| A3OIIncr | A3OIDecr | A3OINeg ->
			pop ctx "value";
			push ctx t_int
		| A3OAs ->
			pop ctx "value";
		| A3OAdd | A3OSub | A3OMul | A3ODiv | A3OMod ->
			pop ctx "value";
			pop ctx "value";
			push ctx t_number
		| A3OShl | A3OShr | A3OUShr | A3OAnd | A3OOr | A3OXor | A3OIAdd | A3OISub | A3OIMul ->
			pop ctx "value";
			pop ctx "value";
			push ctx t_int			
		| A3OEq | A3OPhysEq | A3OLt | A3OLte | A3OGt | A3OGte | A3OIs | A3OIn ->
			pop ctx "value";
			pop ctx "value";
			push ctx t_bool
		| A3OSign16|A3OSign8|A3OSign1 ->
			pop ctx "value";
			push ctx t_int
		|A3OMemGetDouble|A3OMemGetFloat ->
			pop ctx "address";
			push ctx t_number
		|A3OMemGet32|A3OMemGet16|A3OMemGet8 ->
			pop ctx "address";
			push ctx t_int
		|A3OMemSetDouble|A3OMemSetFloat|A3OMemSet32|A3OMemSet16|A3OMemSet8 ->
			pop ctx "address";
			pop ctx "value";
		)
	| HUnk _ -> ()

(* --------------------------------- OBFU ----------------------------------- *)

and obfu_opcode ctx pos op =
	match op with
	| HBreakPoint
	| HNop
	| HThrow
	| HRegKill _
	| HLabel
	| HJump _
	| HSwitch _
	| HPushWith
	| HPopScope
	| HForIn
	| HHasNext
	| HNull
	| HUndefined
	| HForEach
	| HSmallInt _
	| HInt _
	| HTrue
	| HFalse
	| HNaN
	| HPop
	| HDup
	| HSwap
	| HString _
	| HIntRef _
	| HUIntRef _
	| HFloat _
	| HScope
	| HNext _
	| HCallStack _
	| HConstruct _
	| HCallMethod _
	| HRetVoid
	| HRet
	| HConstructSuper _
	| HApplyType _
	| HObject _
	| HArray _
	| HNewBlock
	| HCatch _
	| HReg _
	| HSetReg _
	| HGetGlobalScope
	| HGetScope _
	| HGetSlot _
	| HSetSlot _
	| HToString
	| HToXml
	| HToXmlAttr
	| HToInt
	| HToUInt
	| HToNumber
	| HToBool
	| HToObject
	| HCheckIsXml
	| HAsAny
	| HAsString
	| HAsObject
	| HIncrReg _
	| HDecrReg _
	| HTypeof
	| HInstanceOf
	| HIncrIReg _
	| HDecrIReg _
	| HThis
	| HSetThis
	| HDebugReg _
	| HDebugLine _
	| HDebugFile _
	| HBreakPointLine _
	| HTimestamp
	| HDxNs _
	| HDxNsLate
	| HOp _
	| HUnk _
		-> op
	| HNamespace n -> HNamespace (obfu_ns "" ctx n)
	| HGetSuper n -> HGetSuper (name ctx pos n)
	| HSetSuper n -> HSetSuper (name ctx pos n)
	| HFunction f ->
		HFunction (obfu_local_function ctx f)
	| HCallStatic (f,nargs) -> HCallStatic (obfu_local_function ctx f,nargs)
	| HCallSuper (n,nargs) -> HCallSuper (name ctx pos n,nargs)
	| HCallProperty (n,nargs) -> HCallProperty (name ctx pos n,nargs)
	| HConstructProperty (n,nargs) -> HConstructProperty (obfu_type ctx n,nargs)
	| HCallPropLex (n,nargs) -> HCallPropLex (name ctx pos n,nargs)
	| HCallSuperVoid (n,nargs) -> HCallSuperVoid (name ctx pos n,nargs)
	| HCallPropVoid (n,nargs) -> HCallPropVoid (name ctx pos n,nargs)
	| HClassDef c -> HClassDef (obfu_class ctx c)
	| HFindPropStrict n -> HFindPropStrict (name ctx pos n)
	| HFindProp n -> HFindProp (name ctx pos n)
	| HFindDefinition n -> HFindDefinition (obfu_type ctx n)
	| HGetLex n -> HGetLex (name ctx pos n)
	| HSetProp n -> HSetProp (name ctx pos n)
	| HGetProp n -> HGetProp (name ctx pos n)
	| HInitProp n -> HInitProp (name ctx pos n)
	| HDeleteProp n -> HDeleteProp (name ctx pos n)
	| HCast t -> HCast (obfu_type ctx t)
	| HAsType t -> HAsType (obfu_type ctx t)
	| HIsType t -> HIsType (obfu_type ctx t)
	| HGetDescendants n -> HGetDescendants (name ctx pos n)

and obfu_value ctx v =
	match v with
	| HVNamespace (k,n) -> HVNamespace (k,obfu_ns "" ctx n)
	| _ -> v

and walk_code ctx code pos =
	if pos >= Array.length code then error ctx "Outside code";
	execute ctx pos code.(pos);
	if !dump_context then ctx.curinfos.inf_stacks.(pos) <- ctx.stack;
	walk_code ctx code (pos + 1)

and walk_branches ctx code l =
	List.iter (fun pos ->
		if PMap.mem pos ctx.visited then
			()
		else
			let old, olds = ctx.stack, ctx.scope in
			ctx.visited <- PMap.add pos () ctx.visited;
			(try walk_code ctx code pos with Branch l -> walk_branches ctx code l);
			ctx.stack <- old;
			ctx.scope <- olds
	) l

and obfu_local_function ctx f =
	obfu_method { ctx with
		previous_scope = List.map (fun s -> TBlock s) ctx.slots;
		curmethod = ctx.curmethod ^ ".sub";
		localfunction = true;
	} f

and obfu_function m ctx f =
	let contexts = Array.create (Array.length f.hlf_code) TAny in
	let init_slots() =
		let h = Hashtbl.create 0 in
		Array.iter (fun (n,t,slot,_) -> Hashtbl.add h slot (n , t_opt t)) f.hlf_locals;
		let rec loop n =
			try
				let s = Hashtbl.find h n in
				s :: loop (n + 1)
			with Not_found ->
				[]
		in
		Array.of_list (loop 1)
	in
	let ctx = { ctx with
		curline = 0;
		stack = [];
		scope = [];
		ostrings = [];
		visited = PMap.empty;
		registers = Array.create f.hlf_nregs TAny;
		contexts = contexts;
		slots = init_slots() :: ctx.slots;
		exceptions = Array.map (fun t -> (match t.hltc_name with None -> HMPath ([],"") | Some n -> n), t_opt t.hltc_type) f.hlf_trys;
	} in
	if !dump_context then begin
		ctx.curinfos <- {
			inf_contexts = ctx.contexts;
			inf_stacks = Array.create (Array.length f.hlf_code) [];
		};
		Hashtbl.add ctx.function_contexts m.hlmt_index ctx.curinfos;
	end;
	let reg = ref 0 in
	List.iter (fun p ->
		incr reg;
		ctx.registers.(!reg) <- t_opt p;
	) m.hlmt_args;
	if m.hlmt_arguments_defined then begin
		incr reg;
		ctx.registers.(!reg) <- TGlobal;
	end;
	walk_branches ctx f.hlf_code [0];
	Array.iter (fun t ->
		ctx.scope <- [];
		ctx.stack <- [t_opt t.hltc_type];
		walk_branches ctx f.hlf_code [t.hltc_handle];
	) f.hlf_trys;
	let code = Array.mapi (obfu_opcode ctx) f.hlf_code in
	let rec loop = function
		| [] -> ()
		| p :: l ->
			if p >= 0 && not (List.mem p l) then code.(p) <- (match code.(p) with HString s -> HString (snd (obfu_path ctx ([],s))) | _ -> assert false);
			loop l
	in
	loop ctx.ostrings;
	{ f with
		hlf_code = code;
		hlf_trys = Array.map (fun t ->
			{ t with
				hltc_type = opt obfu_type ctx t.hltc_type;
				hltc_name = opt (fun ctx t -> obfu_name ctx None t) ctx t.hltc_name;
			}
		) f.hlf_trys;
		hlf_locals = Array.map (fun (n,t,s,c) ->
			obfu_name ctx None n, opt obfu_type ctx t, s, c
		) f.hlf_locals;
	}

and obfu_method ctx m =
	try
		Hashtbl.find ctx.methods m.hlmt_index
	with Not_found ->
		let m2 = {
			m with
			hlmt_ret = opt obfu_type ctx m.hlmt_ret;
			hlmt_args = List.map (opt obfu_type ctx) m.hlmt_args;
			hlmt_dparams = (match m.hlmt_dparams with None -> None | Some l -> Some (List.map (obfu_value ctx) l));
			hlmt_debug_name = None; (* erase *)
			hlmt_pnames = None; (* erase *)
			hlmt_function = None;
		} in
		Hashtbl.add ctx.methods m.hlmt_index m2;
		m2.hlmt_function <- opt (obfu_function m) ctx m.hlmt_function;
		m2

and obfu_class ctx c =
	try
		Hashtbl.find ctx.classes c.hlc_index
	with Not_found ->
		let cpath = name_path c.hlc_name in
		let rec is_protected path =
			Hashtbl.mem ignored_vars ("__methods__" :: path) || (match path with [] -> false | _ :: l -> is_protected l)
		in
		if is_protected (List.rev cpath) then begin
			Hashtbl.add ctx.classes c.hlc_index c;
			c
		end else
		let c2 = {
			c with
			hlc_name = obfu_type ctx c.hlc_name;
			hlc_super = opt obfu_type ctx c.hlc_super;
			hlc_implements = Array.map (obfu_type ctx) c.hlc_implements;
			hlc_namespace = opt (obfu_ns (match List.rev cpath with [] -> assert false | n :: _ -> n)) ctx c.hlc_namespace;
		} in
		Hashtbl.add ctx.classes c.hlc_index c2;
		let ctx = { ctx with
			curclass = Some c;
		} in
		ctx.curmethod <- "<construct>";
		ctx.in_static <- false;
		c2.hlc_construct <- obfu_method ctx c.hlc_construct;
		c2.hlc_fields <- Array.map (obfu_field ctx (Some c)) c.hlc_fields;
		ctx.curmethod <- "<static_construct>";
		ctx.in_static <- true;
		c2.hlc_static_construct <- obfu_method ctx c.hlc_static_construct;
		c2.hlc_static_fields <- Array.map (obfu_field ctx (Some c)) c.hlc_static_fields;
		c2

and obfu_field ctx c f =
	let ctx = { ctx with
		curmethod = (match f.hlf_name with HMPath ([],name) -> name | _ -> "???");
		previous_scope = [];
		slots = [];
		localfunction = false;
	} in
	{
		hlf_name = obfu_name ctx c f.hlf_name;
		hlf_slot = f.hlf_slot;
		hlf_kind = (match f.hlf_kind with
			| HFMethod m -> HFMethod { m with hlm_type = obfu_method ctx m.hlm_type }
			| HFVar v -> HFVar { v with hlv_type = opt obfu_type ctx v.hlv_type; hlv_value = obfu_value ctx v.hlv_value; }
			| HFFunction m -> HFFunction (obfu_method ctx m)
			| HFClass c -> HFClass (obfu_class ctx c)
		);
		hlf_metas = f.hlf_metas;
	}

and obfu_static ctx s =
	ctx.global_fields <- s.hls_fields;
	{
		hls_method = obfu_method ctx s.hls_method;
		hls_fields = Array.map (obfu_field ctx None) s.hls_fields;
	}

let obfu_classname ctx c =
	let path = (match List.rev (ExtLib.String.nsplit c.Swf.f9_classname ".") with
		| [] -> assert false
		| x :: l -> List.rev l, x
	) in
	let path = obfu_path ctx path in
	{
		Swf.f9_cid = c.Swf.f9_cid;
		Swf.f9_classname = s_path path;
	}

let register_class ctx c =
	Hashtbl.add ctx.known_classes c.hlc_name c;
	(*
		for interface inheritance, of for some classes, we need
		to add a specific name with NS-Set
	*)
	match c.hlc_name with
	| HMPath (pack,name) ->
		let ns = HNPublic (match pack with [] -> None | _ -> Some (String.concat "." pack)) in
		let n = HMMultiName (Some name,[ns]) in
		Hashtbl.add ctx.known_classes n c;
		Hashtbl.add ctx.known_classes (HMName (name,ns)) c;
	| HMName (id,ns) ->
		let n = HMMultiName (Some id,[ns]) in
		Hashtbl.add ctx.known_classes n c;
	| _ ->
		()

let rec browse_field ctx f =
	match f.hlf_kind with
	| HFClass c -> register_class ctx c
	| _ ->
		Hashtbl.add ctx.known_classes f.hlf_name empty_class

let obfu_bitmap_data ctx data width height =
	let n = ref ((width + height * 51) land 0xFFFFF) in
	let h = ref 5381 in
	let masks = Array.init 11 (fun p ->
		h := (!h lsl 5) + !h + (!n land 0xFF);
		h := (!h lsl 5) + !h + ((!n lsr 8) land 0xFF);
		h := (!h lsl 5) + !h + ((!n lsr 16) land 0xFF);
		h := (!h lsl 5) + !h + (!n lsr 24);
		n := (!h + p) land 0x3FFFFFFF;
		!n
	) in
	for p = 0 to width * height - 1 do
		let change pos bits =
			let v = char_of_int (int_of_char (String.get data pos) lxor ((masks.(p mod 11) lsr bits) land 0xFF)) in
			String.unsafe_set data pos v;
		in
		let pos = p lsl 2 in
		change (pos + 1) 16;
		change (pos + 2) 8;
		change (pos + 3) 0;
	done

let rec obfu_tag ctx codes t =
	let tag data = { t with Swf.tdata = data } in	
	match t.Swf.tdata with
	| Swf.TActionScript3 (id,_) ->
		let code = (match !codes with [] -> assert false | c :: l -> codes := l; c) in
		let code = List.map (obfu_static ctx) code in
		tag (Swf.TActionScript3 (id,As3hlparse.flatten code))
	| Swf.TF9Classes cl ->
		tag (Swf.TF9Classes (List.map (obfu_classname ctx) cl))
	| Swf.TClip cl ->		
		tag (Swf.TClip { cl with Swf.c_tags = List.map (obfu_tag ctx (ref [])) cl.Swf.c_tags })
	| Swf.TPlaceObject2 po2 ->
		tag (Swf.TPlaceObject2 {
			po2 with Swf.po_inst_name = (match po2.Swf.po_inst_name with None -> None | Some inst -> Some (obfu_id ctx inst));
		})
	| Swf.TPlaceObject3 po2 ->
		tag (Swf.TPlaceObject3 {
			po2 with Swf.po_inst_name = (match po2.Swf.po_inst_name with None -> None | Some inst -> Some (obfu_id ctx inst));
		})
	| Swf.TEditText t ->
		tag (Swf.TEditText {
			t with Swf.edt_variable = obfu_id ctx t.Swf.edt_variable;
		})
	| Swf.TBitsLossless l | Swf.TBitsLossless2 l when Hashtbl.mem ctx.bitmaps l.Swf.bll_id ->		
		let cid = l.Swf.bll_id in
		let name = Hashtbl.find ctx.bitmaps cid in
		Hashtbl.remove ctx.bitmaps cid;
		if l.Swf.bll_format <> 5 then err ("Invalid color format for " ^ name);
		let data = l.Swf.bll_data in
		let data2 = Extc.unzip data in
		obfu_bitmap_data ctx data2 l.Swf.bll_width l.Swf.bll_height;
		let data2 = Extc.zip data2 in
		Printf.printf "Obfuscated '%s' (size x%.1f +%.1fKB)\n" name (float_of_int (String.length data2) /. float_of_int (String.length data)) (float_of_int (String.length data2 - String.length data) /. 1024.);
		let l = { l with Swf.bll_data = data2 } in
		tag (match t.Swf.tdata with
			| Swf.TBitsLossless _ -> Swf.TBitsLossless l
			| Swf.TBitsLossless2 _ -> Swf.TBitsLossless2 l
			| _ -> assert false
		)
	| _ ->
		t

let init_obfu key =
	let key, permut = init_key key in
	{
		key = key;
		permut = permut;
		known_classes = Hashtbl.create 0;
		function_contexts = Hashtbl.create 0;
		methods = Hashtbl.create 0;
		classes = Hashtbl.create 0;
		ostrings = [];
		scope = [];
		previous_scope = [];
		stack = [];
		registers = [||];
		contexts = [||];
		global_fields = [||];
		visited = PMap.empty;
		curclass = None;
		curmethod = "<init>";
		curline = 0;
		slots = [];
		exceptions = [||];
		in_static = true;
		localfunction = false;
		bitmaps = Hashtbl.create 0;
		curinfos = {
			inf_contexts = [||];
			inf_stacks = [||];
		};
	}

let obfu key bitmaps output file =
	let ch = (try IO.input_channel (open_in_bin file) with _ -> err ("Failed to open file " ^ file)) in
	let head, swf = SwfParser.parse ch in
	IO.close_in ch;
	let ctx = init_obfu (!key) in
	Hashtbl.add all_contexts file ctx.function_contexts;
	let codes = ref [] in
	let delta_cl = ref 0 and delta_mt = ref 0 in
	let clip_ids = Hashtbl.create 0 in
	List.iter (register_class ctx) (!extern_classes);
	List.iter (fun t ->
		match t.Swf.tdata with
		| Swf.TActionScript3 (id,as3) ->
			let code = As3hlparse.parse ~delta_cl:!delta_cl ~delta_mt:!delta_mt as3 in
			delta_cl := !delta_cl + Array.length as3.as3_classes;
			delta_mt := !delta_mt + Array.length as3.as3_method_types;
			codes := code :: !codes;
			List.iter (fun s ->
				Array.iter (browse_field ctx) s.hls_fields
			) code
		| Swf.TF9Classes cl ->
			List.iter (fun c ->
				match c.Swf.f9_cid with
				| None -> ()
				| Some cid -> Hashtbl.add clip_ids c.Swf.f9_classname cid
			) cl;
		| _ -> ()
	) swf;
	Hashtbl.iter (fun b () ->
		try
			let cid = Hashtbl.find clip_ids b in
			Hashtbl.add ctx.bitmaps cid b;
		with Not_found ->
			err ("Bitmap not found '" ^ b ^ "'")
	) bitmaps;
	let codes = ref (List.rev !codes) in
	let swf = List.map (obfu_tag ctx codes) swf in
	Hashtbl.iter (fun _ b ->
		err ("Class '" ^ b ^ "' is not a lossless bitmap");
	) ctx.bitmaps;
	let out = output file "" in
	Hashtbl.add all_contexts out ctx.function_contexts;
	let ch = (try IO.output_channel (open_out_bin out) with _ -> err ("Failed to create file " ^ out)) in
	SwfParser.write ch (head,swf);
	IO.close_out ch

(* --------------------------------- DUMP ----------------------------------- *)

let rec dump_function ctx m name =
	if Hashtbl.mem ctx.functions m.hlmt_index then
		()
	else begin
		Hashtbl.add ctx.functions m.hlmt_index m;
		match m.hlmt_function with
		| None -> ()
		| Some f ->
			Array.iter (function
				| HFunction m | HCallStatic (m,_) ->
					dump_function ctx m (HMPath ([],Printf.sprintf "<local %d>" m.hlmt_index));
				| _ -> ()
			) f.hlf_code;
			Dump.dump_function ctx.infos f name;
			let fctx = (try
				let i = Hashtbl.find ctx.current m.hlmt_index in
				if Array.length i.inf_contexts <> Array.length f.hlf_code then
					None
				else
					Some i
			with
				Not_found -> None) in
			Array.iteri (fun i o ->
				let op = Dump.s_op o in
				let txt = (match fctx with
					| None -> op
					| Some ctx ->
						let pad str n =
							let len = n - String.length str in
							str ^ (if len > 0 then String.make len ' ' else " ")
						in
						let op = (pad op 50) ^ Dump.s_type ctx.inf_contexts.(i) in
						let op = (pad op 80) ^ "[" ^ String.concat ", " (List.map s_type ctx.inf_stacks.(i)) ^ "]" in
						op
				) in
				IO.printf ctx.infos "\t%4d %s\n" i txt;
			) f.hlf_code;
			IO.printf ctx.infos "\n"
	end

let dump_class_field ctx f =
	match f.hlf_kind with
	| HFMethod m ->
		dump_function ctx m.hlm_type f.hlf_name
	| HFVar _ ->
		()
	| HFFunction m ->
		dump_function ctx m f.hlf_name
	| HFClass _ ->
		assert false

let dump_field ctx f =
	match f.hlf_kind with
	| HFClass c ->
		dump_class ctx.infos c;
		dump_function ctx c.hlc_construct (HMPath ([],"construct"));
		dump_function ctx c.hlc_static_construct (HMPath ([],"static_construct"));
		Array.iter (dump_class_field ctx) c.hlc_fields;
		Array.iter (dump_class_field ctx) c.hlc_static_fields;
	| _ ->
		dump_field ctx.infos f;
		IO.printf ctx.infos "\n\n"

let dump file =
	let ch = (try IO.input_channel (open_in_bin file) with _ -> err ("Failed to open file " ^ file)) in
	let head, swf = SwfParser.parse ch in
	let ctx = {
		dump_file = file;
		functions = Hashtbl.create 0;
		infos = IO.output_channel (open_out_bin (ext file ".as"));
		current = try Hashtbl.find all_contexts file with Not_found -> Hashtbl.create 0;
	} in
	IO.close_in ch;
	List.iter (fun t ->
		match t.Swf.tdata with
		| Swf.TActionScript3 (_,code) ->
			let code = As3hlparse.parse code in
			List.iter (fun s ->
				Array.iter (dump_field ctx) s.hls_fields;
				dump_function ctx s.hls_method (HMPath ([],"<init>"));
			) code
		| _ -> ()
	) swf;
	IO.close_out ctx.infos

(* --------------------------------- EXTRACT/LOAD ----------------------------------- *)

let rec get_name = function
	| HMPath (_,n)
	| HMName (n,_)
	| HMMultiName (Some n,_)
	| HMRuntimeName n ->
		n
	| HMAny
	| HMMultiName (None,_)
	| HMRuntimeNameLate
	| HMMultiNameLate _ ->
		""
	| HMAttrib n | HMParams (n,_) -> get_name n

let extract_config output file =
	let ch = IO.input_channel (open_in_bin file) in
	let _, swf = SwfParser.parse ch in
	IO.close_in ch;
	let out = IO.output_channel (open_out_bin (output file ".cfg")) in
	List.iter (fun t ->
		match t.Swf.tdata with
		| Swf.TActionScript3 (_,code) ->
			let code = As3hlparse.parse code in
			List.iter (fun s ->
				Array.iter (fun f ->
					match f.hlf_kind with
					| HFClass c ->
						let p = name_path c.hlc_name in
						IO.printf out "%s\n" (String.concat "." p);
						let dump_field f = IO.printf out "%s\n" (s_path (p,get_name f.hlf_name)) in
						Array.iter dump_field c.hlc_fields;
						Array.iter dump_field c.hlc_static_fields;						
					| _ ->
						()
				) s.hls_fields
			) code
		| _ -> ()
	) swf;
	IO.close_out out

let extract_classes output file =
	let ch = IO.input_channel (open_in_bin file) in
	let _, swf = SwfParser.parse ch in
	IO.close_in ch;
	let classes = ref [] in
	List.iter (fun t ->
		match t.Swf.tdata with
		| Swf.TActionScript3 (_,code) ->
			let code = As3hlparse.parse code in
			List.iter (fun s ->
				Array.iter (fun f ->
					match f.hlf_kind with
					| HFClass c -> 
						let clean m = 
							{ m with 
								hlmt_function = None;
								hlmt_debug_name = None;
								hlmt_pnames = None;
							}
						in
						let clean_field f =
							{ f with
								hlf_metas = None;
								hlf_kind = (match f.hlf_kind with
									| HFMethod mf -> HFMethod { mf with hlm_type = clean mf.hlm_type }
									| HFFunction f -> HFFunction (clean f)
									| HFVar v -> HFVar v
									| HFClass _ -> assert false
								);
							}
						in
						(* remove all Object statics : since obfu doesn't make difference between member/static access
						   we don't want to have false-positive, and they are not used anyway *)
						if c.hlc_name = HMPath([],"Object") then c.hlc_static_fields <- [||];
						let c = {
							c with
								hlc_index = -1;
								hlc_construct = clean c.hlc_construct;
								hlc_fields = Array.map clean_field c.hlc_fields;
								hlc_static_construct = clean c.hlc_static_construct;
								hlc_static_fields = Array.map clean_field c.hlc_static_fields;
						} in
						classes := c :: !classes;
					| _ ->
						classes := { empty_class with hlc_name = f.hlf_name } :: !classes
				) s.hls_fields
			) code
		| _ -> ()
	) swf;
	let out = open_out_bin (output file ".cl") in
	Marshal.to_channel out (List.rev !classes) [];
	close_out out

let open_file load opt file =
	let ch = (try Some (open_in_bin file) with _ when opt -> warn ("Could not load " ^ file); None) in
	match ch with
	| None -> ()
	| Some ch -> load file ch

let load_config file ch =	
	let lines = Std.input_list ch in
	close_in ch;
	List.iter (fun l ->
		let l = ExtString.String.strip l in		
		if l <> "" && l.[0] <> '#' then begin
			let parts = List.rev (ExtString.String.nsplit l ".") in
			Hashtbl.add ignored_vars parts ();
			match parts with
			| [x] when x.[String.length x - 1] = '*' ->
				ignored_patterns := (String.sub x 0 (String.length x - 1)) :: !ignored_patterns
			| _ -> ()
		end;
	) lines

let load_classes file ch =
	let classes : hl_class list = Marshal.from_channel ch in
	extern_classes := classes @ !extern_classes;
	close_in ch

(* --------------------------------- KEY CHECKING ------------------------------ *)

let check_key prefix pn pk =
	let l = String.length pk in
	let s = Array.init l (fun i -> String.make 1 (String.get pk i)) in
	let vars = Array.init l (fun _ -> "") in
	for i = 0 to l - 1 do
		for y = 0 to l - 1 do
			vars.(y) <- s.(y) ^ vars.(y);
		done;
		Array.sort String.compare vars;
	done;
	let pk = ref pk in
	for i = 0 to l - 1 do
		if String.get vars.(i) (l - 1) = 'z' then pk := String.sub vars.(i) 0 (l - 1);
	done;
	let pk = !pk in
	let l = String.length pk / 2 in
	let j = ref 0 in
	let decode c =
		if c >= '0' && c <= '9' then
			int_of_char c - int_of_char '0'
		else if c >= 'a' && c <= 'z' then
			((int_of_char c - int_of_char 'a') land 7) + 10
		else if c >='A' && c <= 'Z' then
			((int_of_char c - int_of_char 'A') land 7) + 10
		else
			assert false
	in
 	let id = Array.init l (fun i ->
		let c = pk.[!j] in
		incr j;
		let sp = decode c in
		let c = pk.[!j] in
		incr j;
		let kp = decode c in
		(sp lsl 4) lor kp
	) in
	let j = ref 0 in
	for i = 0 to 26 do 
		if i < 5 || i > 8 then begin
			id.(i) <- id.(i) lxor id.(((!j) mod 4) + 5);
			incr j;
		end;
	done;
	let prod = ref 0 in
	for i = 0 to 25 do
		prod := !prod lxor id.(i);
	done;
	if !prod <> id.(26) then raise Exit;
	let dis = prefix ^ pn in
	let di = DynArray.create() in
	UTF8.iter (fun c -> DynArray.add di (UChar.uint_code c)) dis;
	DynArray.set di 0 id.(5);
	DynArray.set di 1 id.(6);
	DynArray.set di 2 id.(7);
	DynArray.set di 3 id.(8);
	let di_str = String.create (DynArray.length di) in
	DynArray.iteri (fun i c -> String.set di_str i (char_of_int c)) di;
	let kd2 = Digest.string di_str in
	for i = 0 to 4 do
		if id.(i) <> int_of_char kd2.[i] then raise Exit;
	done;
	let get_int x =
		(id.(x) land 0xFF) lor ((id.(x+1) land 0xFF) lsl 8) lor ((id.(x+2) land 0xFF) lsl 16) lor ((id.(x+3) land 0xFF) lsl 24)
	in
	let version = get_int 9 in
	let updateVersion = get_int 13 in
(*	let flags = id.(14) in
	let quantity = get_int 18 in
	let quantityIndex = get_int 22 in
*)	updateVersion, version

exception Invalid_key

let do_check_key v fchk a b =
	try
		let _, v2 = fchk a b in
		if v <> v2 then raise Exit;
	with _ ->
		raise Invalid_key

(* --------------------------------- STARTUP ----------------------------------- *)

let execute() =
	let version = 103 in
	let version_str = Printf.sprintf "%d.%.2d" (version / 100) (version mod 100) in
	let ext_str = if Sys.os_type = "Win32" then ".exe" else "" in
	let usage = "Obfu9 " ^ version_str ^ " - (c)2008-2009 Motion-Twin\n Usage : obfu9" ^ ext_str ^ " [options] <file>\n Options :" in
	let files = ref [] in
	let output = ref None in
	let dumps = ref [] in
	let key = ref "" in
	let user_name = ref "" in
	let user_key = ref "" in
	let bitmaps = Hashtbl.create 0 in
	let run = ref (obfu key bitmaps) in
	let dump_all() =
		dumps := List.map (fun file -> match !output with None -> ext file "_obfu.swf" | Some o -> o) !files @ !files @ !dumps
	in
	let args_spec = [
		("-o", Arg.String (fun o -> output := Some o), "<file> : set output file");
		("-key", Arg.String (fun k -> key := k), "<string> : use the specific obfuscation key");
		("-config",Arg.String (fun file -> open_file load_config false file),"<file> : load specified configuration file");
		("-dump",Arg.String (fun f -> dumps := f :: !dumps),"<file> : dump the SWF sources content");
		("-fake",Arg.Unit (fun() -> real_obfu := false),": enable 'fake' obfuscation, for debugging purposes only");
		("--use-classes",Arg.String (fun file -> open_file load_classes false file),"<file> : load specified classes file");
		("--dump-all", Arg.Unit dump_all, ": dump all input and output files");
		("--dump-context", Arg.Unit (fun() -> dump_context := true),": dump obfu context informations");
		("--extract-config",Arg.Unit (fun() -> run := extract_config)," : extract configuration file from SWF file");
		("--extract-classes",Arg.Unit (fun() -> run := extract_classes)," : extract classes data from SWF file");
		("-user",Arg.String (fun s -> user_name := s)," : set the authentification user name");
		("-skey",Arg.String (fun s -> user_key := s)," : set the authentification key");
		("-bmp",Arg.String (fun s -> Hashtbl.add bitmaps s ())," : obfuscate bitmap resource");
	] in
	let cfg_dir = Extc.executable_path() ^ "cfg/" in
	open_file load_config true (cfg_dir ^ "default.cfg");
	open_file load_classes true (cfg_dir ^ "fplayer.cl");
	open_file load_classes true (cfg_dir ^ "flex.cl");
	open_file load_classes true (cfg_dir ^ "rpc.cl");
	Arg.parse args_spec (fun f -> files := f :: !files) usage;
	if !files = [] && !dumps = [] then Arg.usage args_spec usage;
(*	do_check_key 1000 (check_key "CRoAy#35h") (!user_name) (!user_key); *)
	List.iter ((!run) (fun file e -> match !output with None -> if e = "" then file else ext file e | Some o -> o)) (List.rev !files);
	List.iter dump (List.rev !dumps);

;;
SwfParser.init Extc.input_zip Extc.output_zip;
Swf.warnings := false;
try
	execute()
with Error msg ->
	prerr_endline msg;
	exit(1)
