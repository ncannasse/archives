open ExtString
open Type
open Typetools
open Nast

type context = {
	mutable exprs : Nast.expr list;
	mutable main : string option;
	mutable current : class_context;
	modules : (string,unit) Hashtbl.t;
}

let curpos = ref null_pos

let set_pos ctx e =
	let cpath , cname = ctx.current.c_path in
	let cstr = match cpath with [] -> cname | l -> String.concat "/" l ^ "/" ^ cname in
	curpos := { psource = cstr ^ ".mt"; pline = Lexer.get_error_line e.tpos }

let set_base_pos() =
	curpos := { psource = "mtypes"; pline = 0 }

let mk e = (e,!curpos)
let op o e1 e2 = mk (EBinop (o,e1,e2))
let call e el = mk (ECall (e,el))
let bcall b el = mk (ECall (mk (EConst (Builtin b)),el))
let const c = mk (EConst c)
let ident n = const (Ident n)
let field e f = mk (EField (e,f))
let block l = mk (EBlock l)
let func args l = mk (EFunction (args,block l))

let s_binop = function
	| OpAdd -> "+"
	| OpMult -> "*"
	| OpDiv -> "/"
	| OpSub -> "-"
	| OpAssign -> "="
	| OpEq -> "=="
	| OpNotEq -> "!="
	| OpGt -> ">"
	| OpGte -> ">="
	| OpLt -> "<"
	| OpLte -> "<="
	| OpAnd -> "&"
	| OpOr -> "|"
	| OpXor -> "^"
	| OpBoolAnd -> "&&"
	| OpBoolOr -> "||"
	| OpShl -> "<<"
	| OpShr -> ">>"
	| OpUShr -> ">>>"
	| OpMod -> "%"
	| OpAssignOp _ -> assert false

let path_ident p =
	match fst p with
	| [] -> snd p
	| l -> String.concat "_" l ^ "_" ^ snd p

let class_name c = path_ident c.c_path

let add ctx e =
	ctx.exprs <- e :: ctx.exprs

let unescape s p =
	let b = Buffer.create 0 in
	let rec loop esc i =
		if i = String.length s then
			()
		else
			let c = s.[i] in
			if esc then begin
				let inext = ref (i + 1) in
				(match c with
				| 'b' -> Buffer.add_char b '\b'
				| 'f' -> Buffer.add_char b (char_of_int 12)
				| 'n' -> Buffer.add_char b '\n'
				| 'r' -> Buffer.add_char b '\r'
				| 't' -> Buffer.add_char b '\t'
				| '"' | '\'' | '\\' -> Buffer.add_char b c
				| '0'..'3' ->
					let c = (try
						char_of_int (int_of_string ("0o" ^ String.sub s i 3))
					with _ ->
						raise (Lexer.Error (Lexer.Invalid_character c,p))
					) in
					Buffer.add_char b c;
					inext := !inext + 2;
				| 'x' ->
					let c = (try
						char_of_int (int_of_string ("0x" ^ String.sub s (i+1) 2))
					with _ ->
						raise (Lexer.Error (Lexer.Invalid_character c,p))
					) in
					Buffer.add_char b c;
					inext := !inext + 2;
				| 'u' ->
					let i = (try
						int_of_string ("0x" ^ String.sub s (i+1) 4)
					with _ ->
						raise (Lexer.Error (Lexer.Invalid_character c,p))
					) in
					let ub = UTF8.Buf.create 0 in
					UTF8.Buf.add_char ub (UChar.chr i);
					Buffer.add_string b (UTF8.Buf.contents ub);
					inext := !inext + 4
				| _ -> raise (Lexer.Error (Lexer.Invalid_character c,p)));
				loop false !inext;
			end else
				match c with
				| '\\' -> loop true (i + 1)
				| c ->
					Buffer.add_char b c;
					loop false (i + 1)
	in
	loop false 0;
	Buffer.contents b

let generate_constant ctx p = function
	| TConstInt i -> const (Int (try int_of_string i with _ -> assert false))
	| TConstFloat f -> const (Float f)
	| TConstString s -> call (field (ident "String") "new") [const (String (unescape s p))]
	| TConstBool b -> if b then const True else const False
	| TConstNull -> const Null
	| TConstThis -> const This

let rec generate_builtin ctx = function
	| Cast e
	| Upcast e
	| Downcast e -> generate_expr ctx e
	| Trace _ -> const Null
	| ToInt e -> call (ident "__int") [generate_expr ctx e]
	| ToString e -> call (field (ident "String") "new") [generate_expr ctx e]
	| PrintType _ -> const Null
	| Callback (e,f,args) -> block [
			mk (EVars ["@tmp",Some (generate_expr ctx e)]);
			call (const (Builtin "closure")) (field (ident "@tmp") f :: (ident "@tmp") :: List.map (generate_expr ctx) args)
		]
	| SuperConstructor ->
		assert false

and generate_expr ctx e =
	set_pos ctx e;
	match e.texpr with
	| TVars vl -> mk (EVars (List.map (fun (v,t,e) -> v , (match e with None -> None | Some e -> Some (generate_expr ctx e))) vl))
	| TBlock (tl : texpr list) ->
		block (List.map (generate_expr ctx) tl)
	| TFor (el1,el2,el3,e) ->
		let rec test = function
			| [] -> const True
			| [v] -> generate_expr ctx v
			| v :: l ->
				op "&&" (generate_expr ctx v) (test l)
		in
		let wh = EWhile (test el2,block (generate_expr ctx e :: List.map (generate_expr ctx) el3),NormalWhile) in
		block (List.map (generate_expr ctx) el1 @ [mk wh])
	| TIf (e,e1,e2) ->
		mk (EIf (generate_expr ctx e , generate_expr ctx e1 , match e2 with None -> None | Some e -> Some (generate_expr ctx e)))
	| TWhile (e,e1,flag) ->
		mk (EWhile (generate_expr ctx e, generate_expr ctx e1, match flag with Type.NormalWhile -> NormalWhile | Type.DoWhile -> DoWhile))
	| TSwitch (e,cases,def) ->
		let rec test = function
			| [] -> const False
			| [v] -> op "==" (ident "@switch") (generate_expr ctx v)
			| v :: l ->
				op "||" (op "==" (ident "@switch") (generate_expr ctx v)) (test l)
		in
		let rec loop acc = function
			| [] ->
				(match def with
				| None -> const Null
				| Some e -> generate_expr ctx e)
			| (v,{ texpr = TBlock [] }) :: l ->
				loop (v :: acc) l
			| (v,e) :: l ->
				mk (EIf (test (v :: acc),generate_expr ctx e,Some (loop [] l)))
		in
		mk (EWhile (const False,
			block [
				mk (EVars ["@switch",Some (generate_expr ctx e)]);
				loop [] cases
		], DoWhile));
	| TTry (e,s,t,e2) ->
		let ecatch = (match t with
			| None -> generate_expr ctx e2
			| Some t ->
				let not_object = op "!=" (bcall "typeof" [ident s]) (const (Builtin "tobject")) in
				let not_proto = op "!=" (bcall "pcompare" [bcall "objgetproto" [ident s]; field (ident (path_ident t)) "prototype"]) (const (Int 0)) in
				mk (EBlock [
					mk (EIf (op "||" not_object not_proto,bcall "rethrow" [ident s], None));
					generate_expr ctx e2
				])
		) in
		mk (ETry (generate_expr ctx e,s,ecatch))
	| TReturn e -> mk (EReturn (match e with None -> None | Some e -> Some (generate_expr ctx e)))
	| TBreak -> mk (EBreak None)
	| TContinue -> mk EContinue
	| TConst c -> generate_constant ctx e.tpos c
	| TArray (v,idx) -> mk (EArray (field (generate_expr ctx v) "@a",generate_expr ctx idx))
	| TBinop (OpAssign,{ texpr = TArray(a,idx) },v) -> call (field (generate_expr ctx a) "set") [generate_expr ctx idx;generate_expr ctx v]
	| TBinop (OpAssign,{ texpr = TMember(f,c) },v) when f.[0] = '_' ->
		generate_expr ctx { e with texpr = TCall ({ e with texpr = TMember ("set" ^ f,c) },[v]) }
	| TBinop (OpAssign,{ texpr = TField(e,f) },v) when f.[0] = '_' ->
		generate_expr ctx { e with texpr = TCall ({ e with texpr = TField (e,"set" ^ f) },[v]) }
	| TField (e,f) when f.[0] = '_' ->
		generate_expr ctx { e with texpr = TCall ({ e with texpr = TField (e,"get" ^ f) },[]) }
	| TMember (f,c) when f.[0] = '_' ->
		generate_expr ctx { e with texpr = TCall ({ e with texpr = TMember ("get" ^ f,c) },[]) }
	| TBinop (OpAssignOp o,e1,e2) ->
		generate_expr ctx { e with texpr = TBinop (OpAssign, e1 , { e with texpr = TBinop (o,e1,{ e with texpr = TParenthesis e2 }) }) }
	| TBinop (o,e1,e2) -> op (s_binop o) (generate_expr ctx e1) (generate_expr ctx e2)
	| TField (e,f) -> field (generate_expr ctx e) f
	| TLocal s -> ident s
	| TMember (f,c) -> field (const This) f
	| TClass c -> ident (class_name c)
	| TParenthesis e -> mk (EParenthesis (generate_expr ctx e))
	| TObjDecl fl -> mk (EObject (List.map (fun (f,v) -> f , generate_expr ctx v) fl))
	| TArrayDecl el ->
		call (field (ident "Array") "new1") [call (const (Builtin "array")) (List.map (generate_expr ctx) el)]
	| TCall ({ texpr = TMember (f,c) },el) when c != ctx.current ->
		call (const (Builtin "call")) [
			field (ident ("@" ^ class_name c)) f;
			const This;
			call (const (Builtin "array")) (List.map (generate_expr ctx) el)
		]
	| TCall ({ texpr = TBuiltin SuperConstructor },el) ->
		(match ctx.current.c_super with
		| None -> assert false
		| Some (c,_) ->
			call (const (Builtin "call")) [
				field (ident (class_name c)) "__construct";
				const This;
				call (const (Builtin "array")) (List.map (generate_expr ctx) el)
			])
	| TCall ({ texpr = TField ({ texpr = TClass { c_path = [] ,"Std" } },"neko") },[{ texpr = TConst (TConstString s) }]) ->
		mk (ENeko s)
	| TCall (e,el) ->
		call (generate_expr ctx e) (List.map (generate_expr ctx) el)
	| TNew (c,_,el) ->
		call (field (ident (class_name c)) "new") (List.map (generate_expr ctx) el)
	| TUnop (uop,flag,e) ->
		let e = generate_expr ctx e in
		(match uop with
		| Increment
		| Decrement ->
			(match flag with
			| Prefix ->
				op "=" e (op (if uop = Increment then "+" else "-") e (const (Int 1)))
			| Postfix ->
				block [
					mk (EVars ["@tmp",Some e]);
					op "=" e (op (if uop = Increment then "+" else "-") (ident "@tmp") (const (Int 1)));
					ident "@tmp"
				]
			)
		| Not -> call (const (Builtin "not")) [e]
		| Neg -> op "-" (const (Int 0)) e
		| NegBits -> assert false)
	| TLambda (args,e,_) ->
		mk (EFunction (List.map fst args , generate_expr ctx e))
	| TBuiltin b ->
		generate_builtin ctx b
	| TDiscard e ->
		generate_expr ctx e

let generate_version c =
	let hplus x y = x lxor y in
	let hash = Hashtbl.hash in
	let cache = Hashtbl.create 0 in
	let cache_id = ref 0 in
	let hash_unique t =
		try
			Hashtbl.find cache t.id
		with
			Not_found ->
				let h = hplus (hash "unique") (hash (!cache_id)) in
				incr cache_id;
				Hashtbl.add cache t.id h;
				h
	in
	let rec hash_obj o =
		match o.c_name with
		| Some c ->
			hash c
		| _ ->
			List.fold_left (fun acc f ->
				hplus (hash_type f.f_type) (hplus (hash f.f_name) acc)
			) (hash 0) o.c_fields
	and hash_type t =
		match t.t with
		| TLink t ->
			hash_type t
		| TLazy _ ->
			assert false
		| TObject o ->
			hplus (hash_obj o.o_inf) (match o.o_sup with None -> hash 0 | Some o -> hash_obj o)
		| TFunction f ->
			List.fold_left (fun acc t -> hplus (hash_type t) acc) (hash_type f.f_ret) f.f_args
		| TPoly
		| TMono ->
			hash_unique t
		| TTemplate s ->
			hash s
		| TAbbrev (t,params,v) ->
			List.fold_left (fun acc t -> hplus (hash_type t) acc) (hplus (hash t) (hash v)) params
		| TInt ->
			hash 1
		| TFloat ->
			hash 2
		| TNumber ->
			hash 3
		| TBool ->
			hash 4
		| TVoid ->
			hash 5
	in
	let hash_count = ref 0 in
	List.iter (fun f ->
		if not f.cf_static && f.cf_expr = None then begin
			Hashtbl.clear cache;
			cache_id := 0;
			hash_count := hplus (!hash_count) (Hashtbl.hash f.cf_name);
			hash_count := hplus (!hash_count) (hash_type f.cf_type);
		end;
	) c.c_class_fields;
	const (Int (!hash_count))

let generate_class ctx c =
	set_base_pos();
	let st_name = class_name c in
	let cl_name = "@" ^ st_name in
	let superc = (match c.c_super with None -> const Null | Some (c,_) -> ident ("@" ^ class_name c)) in
	ctx.current <- c;
	add ctx (op "="
				(ident cl_name)
				(call (const (Builtin "new")) [superc]));
	add ctx (op "="
				(ident st_name)
				(call (const (Builtin "new")) [const Null]));
	add ctx (op "="
				(field (const (Builtin "exports")) st_name)
				(ident st_name));
	let args, e = (match constructor c with
		| None -> [] , { texpr = TBlock []; ttype = t_void; tpos = Typetools.null_pos }
		| Some (args,e,_) -> List.map fst args , e
	) in
	add ctx (op "=" (field (ident st_name) "new") (func args [
		op "=" (const This) (call (const (Builtin "new")) [const Null]);
		call (const (Builtin "objsetproto")) [const This; ident cl_name];
		call (const (Builtin "call")) [
			field (ident st_name) "__construct";
			const This;
			call (const (Builtin "array")) (List.map ident args)
		];
		mk (EReturn (Some (const This)));
	]));
	add ctx (op "=" (field (ident st_name) "__construct") (func args [generate_expr ctx e]));
	add ctx (op "=" (field (ident st_name) "prototype") (ident cl_name));
	let statics = ref [] in
	List.iter (fun cf ->
		if cf.cf_name <> "new" then begin
			match cf.cf_expr with
			| None ->
				if not cf.cf_static then add ctx (op "=" (field (ident cl_name) cf.cf_name) (const Null))
			| Some e ->
				if cf.cf_static then
					statics := (cf.cf_name,e) :: !statics
				else begin
					add ctx (op "=" (field (ident cl_name) cf.cf_name) (generate_expr ctx e));
					if cf.cf_name = "toString" then
						add ctx (op "=" (field (ident cl_name) "__string") (func [] [
							(EVars ["s", Some (call (field (const This) "toString") [])],!curpos);
							(EIf (op "==" (bcall "typeof" [ident "s"]) (const (Builtin "tobject")),
								 field (ident "s") "@s",
								 Some (ident "s")
							),!curpos)
						]))
				end;
		end;
	) c.c_class_fields;
	let version_code = generate_version c in
	add ctx (op "=" (field (ident cl_name) "__serialize") (EFunction ([],bcall "array" [const (String (s_type_path c.c_path)); version_code]),!curpos));
	List.iter (fun (name,e) ->
		if name = "main" && (match e.texpr with TLambda _ -> true | _ -> false) then begin
			(match ctx.main with
			| None -> ()
			| Some st -> failwith ("Duplicate static function main " ^ st ^ " and " ^ st_name));
			ctx.main <- Some st_name;
		end;
		add ctx	(op "=" (field (ident st_name) name) (generate_expr ctx e))
	) (List.rev !statics);
	cl_name, s_type_path c.c_path , version_code

let generate_unserialize ctx ver =
	let error = bcall "throw" [op "+" (const (String "Invalid serialized data : ")) (bcall "string" [ident "@v"])] in
	let unknown = bcall "throw" [op "+" (const (String "Class not found : ")) (bcall "string" [ident "@cl"])] in
	let label = "invalid" in
	let invalid = bcall "throw" [op "+" (const (String "Class has been modified : ")) (bcall "string" [ident "@cl"])] in
	let check = (EIf (op "!=" (bcall "typeof" [ident "@v"]) (const (Builtin "tarray")),error,None),!curpos) in
	let init = (EVars [
		"@cl", Some (EArray (ident "@v",const (Int 0)),!curpos);
		"@id", Some (EArray (ident "@v",const (Int 1)),!curpos);
	],!curpos) in
	let disp = (EReturn (Some (ESwitch (ident "@cl", (List.map (fun (cl,c,v) ->
		const (String c) , (EIf (op "==" (ident "@id" ) v,
			ident cl,
			Some (bcall "goto" [ident label])
		),!curpos)
	) ver) , Some unknown), !curpos)), !curpos) in
	add ctx (op "=" (field (const (Builtin "exports")) "__unserialize") (func ["@v"] [check;init;disp;(ELabel label,!curpos);invalid]))

let generate_native_class ctx c =
	try
		let m = List.find (fun cf -> cf.cf_name = "MODULE") c.c_class_fields in
		(match m.cf_expr with
		| Some { texpr = TConst (TConstString m) } ->
			let mm = String.concat "_" (String.nsplit m "/") in
			set_base_pos();
			if not (Hashtbl.mem ctx.modules m) then begin
				add ctx (op "=" (ident mm) (call (field (const (Builtin "loader")) "loadmodule") [const (String m); const (Builtin "loader")]));
				Hashtbl.add ctx.modules m ();
			end;
			add ctx (op "=" (ident (class_name c)) (field (ident mm) (class_name c)))
		| _ -> raise Not_found)
	with
		Not_found -> failwith ("Class " ^ s_type_path c.c_path ^ " doesn't have a MODULE")

let generate file t =
	let ctx = {
		current = empty_class_context;
		exprs = [];
		main = None;
		modules = Hashtbl.create 0;
	} in
	set_base_pos();
	let high = ref [] in
	let medium = ref [] in
	let low = ref [] in
	Typer.generate t (fun c ->
		if is_native c then
			high := c :: !high
		else if List.exists (fun f -> f.cf_name = "__init" && f.cf_static) c.c_class_fields then
			medium := c :: !medium
		else
			low := c :: !low
	);
	List.iter (generate_native_class ctx) (List.rev !high);
	let ver = List.map (generate_class ctx) (List.rev !medium) in
	let ver = ver @ (List.map (generate_class ctx) (List.rev !low)) in
	generate_unserialize ctx ver;
	set_base_pos();
	(match ctx.main with
	| None -> ()
	| Some c -> add ctx (call (field (ident c) "main") []));
	let neko = Filename.chop_extension file ^ ".neko" in
	let ch = IO.output_channel (open_out neko) in
	let e = block (List.rev ctx.exprs) in
	if !Plugin.verbose then
		Nxml.write_fmt ch (Nxml.to_xml e)
	else
		Nxml.write ch (Nxml.to_xml e);
	IO.close_out ch;
	let cmd = "nekoc " ^ neko in
	if !Plugin.verbose then print_endline cmd;
	if Sys.command cmd <> 0 then failwith "Compilation Aborted";
	if not !Plugin.verbose then Sys.remove neko

;;
let output = ref None in
Plugin.add
	[
		("-neko",Arg.String (fun f ->
			Typer.can_add_objects := false;
			output := Some f
		),"<file> : generate Neko source into target file");
	]
	(fun t ->
		match !output with
		| None -> ()
		| Some file -> generate file t)