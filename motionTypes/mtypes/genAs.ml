open Type
open Typetools

type context = {
	mutable level : int;
	mutable tabs : bool;
	ch : unit IO.output;
	packages : (string list,unit) Hashtbl.t;
	mutable current : class_context;
}

let mts_mode = ref false

let s_type_path = function
	| [] , "Xml" when not !mts_mode -> s_type_path ([],"XML")
	| [] , "XmlNode" when not !mts_mode -> s_type_path ([],"XMLNode")
	| [] , "XmlSocket" when not !mts_mode -> s_type_path ([],"XMLSocket")
	| p -> s_type_path p

let mts_path (p,s) =
	if p = [] then
		s
	else
		String.concat "_" p ^ "_" ^ s

let newline ctx =
	IO.write ctx.ch '\n';
	ctx.tabs <- false

let level ctx b =
	ctx.level <- ctx.level + (if b then 1 else -1);
	newline ctx

let print ctx =
	if not ctx.tabs then begin
		IO.nwrite ctx.ch (String.make (ctx.level * 4) ' ');
		ctx.tabs <- true;
	end;
	IO.printf ctx.ch

let escape str =
	let last_char = ref '0' in
	let buffer = Buffer.create 0 in
	for i = 0 to (String.length str) - 1 do
		if !last_char <> '\\' && str.[i] = '"' then
			Buffer.add_string buffer "\\\""
		else
			Buffer.add_char buffer str.[i];
		last_char := str.[i]
	done;
	Buffer.contents buffer

let rec generate_package ctx = function
	| [] -> ()
	| p ->
		generate_package ctx (List.rev (List.tl (List.rev p)));
		try
			Hashtbl.find ctx.packages p
		with
			Not_found ->
				Hashtbl.add ctx.packages p ();
				let p = String.concat "." p in
				IO.printf ctx.ch "if( !%s ) %s = new Object();\n" p p

let rec generate_concat ctx sep f = function
	| [] -> ()
	| x :: [] -> f x
	| x :: l -> f x; print ctx "%s" sep; generate_concat ctx sep f l

let generate_constant ctx = function
	| TConstInt s -> print ctx "%s" s
	| TConstFloat s -> print ctx "%s" s
	| TConstString s -> print ctx "\"%s\"" (escape s)
	| TConstBool b -> print ctx "%s" (if b then "true" else "false")
	| TConstNull -> print ctx "null"
	| TConstThis -> print ctx "this"

let rec generate_expr_rec ctx e =
	match e.texpr with
	| TDiscard e ->
		generate_expr ctx e;
	| TVars [] -> ()
	| TVars tl ->
		print ctx "var ";
		generate_concat ctx ", " (fun (v,_,e) ->
			match e with
			| None -> print ctx "%s" v;
			| Some e -> print ctx "%s = " v; generate_expr ctx e
		) tl;
	| TBlock tl ->
		print ctx "{";
		level ctx true;
		List.iter (generate_expr ctx) tl;
		level ctx false;
		print ctx "}";
		newline ctx;
	| TFor ([{ texpr = TVars [v,t,i] } as ve],conds,incrs,efor) when !mts_mode ->
		print ctx "{";
		level ctx true;
		generate_expr ctx { ve with texpr = TVars [v,t,i] };
		generate_expr ctx { e with texpr = TFor ([],conds,incrs,efor) };
		level ctx false;
		print ctx "}";
		newline ctx;
	| TFor (inits,conds,incrs,e) ->
		print ctx "for(";
		generate_concat ctx ", " (generate_expr_rec ctx) inits;
		print ctx ";";
		generate_concat ctx ", " (generate_expr ctx) conds;
		print ctx ";";
		generate_concat ctx ", " (generate_expr ctx) incrs;
		print ctx ") ";
		generate_expr ctx e
	| TIf (cond,e1,Some e2) when e.ttype != t_void ->		
		generate_expr ctx cond;
		print ctx "?";
		generate_expr ctx e1;
		print ctx ":";
		generate_expr ctx e2;
	| TWhile (econd,e,NormalWhile) ->
		print ctx "while( ";
		generate_expr ctx econd;
		print ctx " ) ";
		generate_expr ctx e;
	| TWhile (econd,e,DoWhile) ->
		print ctx "do ";
		generate_expr ctx e;
		print ctx "while( ";
		generate_expr ctx econd;
		print ctx " )";
	| TIf (e,eif,eelse) ->
		print ctx "if( ";
		generate_expr ctx e;
		print ctx " ) ";		
		generate_expr ctx eif;		
		(match eelse with
		| None -> ()
		| Some e ->
			print ctx "else ";			
			generate_expr ctx e);
	| TSwitch (e,cases,def) ->
		print ctx "switch( ";
		generate_expr ctx e;
		print ctx " ) {";
		newline ctx;
		List.iter (fun (c,e) ->
			print ctx "case ";
			generate_expr ctx c;
			print ctx ":";
			level ctx true;
			generate_expr ctx e;
			level ctx false;
		) cases;
		(match def with
		| None -> ()
		| Some e ->
			print ctx "default:";
			level ctx true;
			generate_expr ctx e;
			level ctx false);
		print ctx "}";
	| TTry (e,s,t,e2) ->
		print ctx "try ";
		generate_expr ctx e;
		print ctx " catch(%s%s) " s (match t with None -> "" | Some t -> " : " ^ s_type_path t);
		generate_expr ctx e2;
	| TReturn t ->
		print ctx "return ";
		(match t with
		| None -> ()
		| Some e -> generate_expr ctx e; print ctx ";")
	| TBreak ->
		print ctx "break"
	| TContinue ->
		print ctx "continue";
	| TLambda (args,e,_) ->
		generate_function ctx "" args e true
	| TConst c ->
		generate_constant ctx c
	| TArray (e1,e2) ->
		generate_expr ctx e1;
		print ctx "[";
		generate_expr ctx e2;
		print ctx "]"
	| TBinop (op,e1,e2) ->
		generate_expr ctx e1;
		print ctx " %s " (Expr.s_binop op);
		generate_expr ctx e2;
	| TField (e,s) ->
		generate_expr ctx e;
		(match s with
		| "add" when not !mts_mode -> print ctx "[\"%s\"]" s
		| _ -> print ctx ".%s" s)
	| TLocal s ->
		print ctx "%s" s
	| TMember (s,c) ->
		print ctx "this.%s" s
	| TClass c ->
		let f = (if !mts_mode then mts_path else s_type_path) in
		if !mts_mode && not (is_native c) then
			print ctx "_%s" (f c.c_path)
		else
			print ctx "%s" (f c.c_path)
	| TParenthesis e ->
		print ctx "(";
		generate_expr ctx e;
		print ctx ")"
	| TObjDecl fl ->
		print ctx "{";
		level ctx true;
		generate_concat ctx ", " (fun (f,e) -> print ctx "%s : " f; generate_expr ctx e) fl;
		level ctx false;
		print ctx "}";
	| TArrayDecl tl ->
		print ctx "[";
		generate_concat ctx ", " (generate_expr ctx) tl;
		print ctx "]";	
	| TCall (e,el) ->
		(match e.texpr with
		| TMember (s,c) when c != ctx.current ->
			if !mts_mode then
				print ctx "super.%s(" s
			else
				print ctx "%s.prototype.%s.call(this%s" (s_type_path c.c_path) s (if el = [] then "" else ",")
		| _ ->
			generate_expr ctx e;
			print ctx "(";
		);
		generate_concat ctx "," (generate_expr ctx) el;
		print ctx ")";
	| TNew ({ c_path = ([],"Resource") },_,[{ texpr = TConst (TConstString s) }]) when !mts_mode ->
		print ctx "'%s" s
	| TNew (c,_,el) ->
		print ctx "new %s(" ((if !mts_mode then mts_path else s_type_path) c.c_path);
		generate_concat ctx "," (generate_expr ctx) el;
		print ctx ")";		
	| TUnop (op,Prefix,e) ->
		print ctx "%s" (Expr.s_unop op);
		generate_expr ctx e
	| TUnop (op,Postfix,e) ->
		generate_expr ctx e;
		print ctx "%s" (Expr.s_unop op)
	| TBuiltin b ->
		match b with
		| Cast e
		| Upcast e 
		| Downcast e -> generate_expr ctx e
		| Trace (e,f,l) ->
			(match !Plugin.trace with
			| None -> ()
			| Some f ->
				print ctx "%s(" f;
				generate_expr ctx e;
				print ctx ",\"%s\",\"%s\",\"%d\")" (s_type_path ctx.current.c_path) f l)
		| ToInt e when !mts_mode -> print ctx "Std.int("; generate_expr ctx e; print ctx ")"
		| ToInt e -> print ctx "int("; generate_expr ctx e; print ctx ")"
		| ToString e when !mts_mode -> print ctx "Std.string("; generate_expr ctx e; print ctx ")"
		| ToString e -> print ctx "string("; generate_expr ctx e; print ctx ")"
		| PrintType _ -> ()
		| SuperConstructor -> print ctx "super"
		| Callback (e,f,el) ->
			print ctx "Std.callback("; generate_expr ctx e; print ctx ",\"%s\"%s" f (if el = [] then "" else ",");
			generate_concat ctx "," (generate_expr ctx) el;
			print ctx ")"

and generate_expr ctx e =
	generate_expr_rec ctx e;
	if e.ttype == t_void && ctx.tabs then begin
		print ctx ";";
		newline ctx;
	end;

and generate_function ctx fname args e lambda =
	if lambda && !mts_mode then
		print ctx "fun(%s) " (String.concat "," (List.map fst args))
	else
		print ctx "function%s(%s) " (if !mts_mode then " " ^ fname else "") (String.concat "," (List.map fst args));
	generate_expr ctx e

let generate file t =
	let ch = IO.output_channel (open_out file) in
	let main = ref None in
	let ctx = {
		tabs = false;
		level = 0;
		ch = ch;
		current = empty_class_context;
		packages = Hashtbl.create 0;
	} in
	Typer.generate t (fun c ->
		if not (is_native c) then begin
			ctx.current <- c;
			print ctx "//---------------------------------------------------------\n";
			print ctx "// Code generated from %s\n\n" c.c_pos.pfile;
			if !mts_mode then begin 
				print ctx "class %s " (mts_path c.c_path);
				(match c.c_super with None -> () | Some (c,_) -> print ctx "extends %s " (mts_path c.c_path));
				print ctx "{";
				level ctx true;
			end else
				generate_package ctx (fst c.c_path);
			(match constructor c with
			| None when !mts_mode ->
				print ctx "function %s() { };\n" (mts_path c.c_path);
			| None -> 				
				print ctx "%s = function() { };\n" (s_type_path c.c_path)
			| Some (args,e,_) -> 				
				if not !mts_mode then print ctx "%s = " (s_type_path c.c_path);
				generate_function ctx (if !mts_mode then mts_path c.c_path else snd c.c_path) args e false;
				print ctx ";";
				newline ctx);
			if not !mts_mode then (match c.c_super with
				| None -> ()
				| Some (cs,_) -> 
					print ctx "%s.prototype.__proto__ = %s.prototype;" (s_type_path c.c_path) (s_type_path cs.c_path);
					newline ctx;
					print ctx "%s.prototype.__constructor__ = %s;" (s_type_path c.c_path) (s_type_path cs.c_path);
					newline ctx;
			);
			let statics = ref [] in			
			List.iter (fun cf ->
				if cf.cf_name <> "new" then match cf.cf_expr with
				| None -> ()
				| Some e when !mts_mode ->
					if cf.cf_static && cf.cf_name <> "main" then
						statics := (cf.cf_name,e) :: !statics
					else (match e.texpr with
					| TLambda (args,e,_) ->
						if cf.cf_name = "main" then begin
							if !main <> None then failwith "Muliple main declared !";
							main := Some (c,e);
						end else
							generate_function ctx cf.cf_name args e false
					| _ ->
						if cf.cf_name <> "main" then assert false;
						statics := (cf.cf_name,e) :: !statics)
				| Some e ->					
					let name = (match cf.cf_name with
						| "add" -> "[\"" ^ cf.cf_name ^ "\"]"
						| _ -> "." ^ cf.cf_name)
					in
					if cf.cf_static then
						print ctx "%s%s = " (s_type_path c.c_path) name
					else
						print ctx "%s.prototype%s = " (s_type_path c.c_path) name;
					generate_expr ctx e;
					print ctx ";";
					newline ctx
			) c.c_class_fields;
			if !mts_mode then begin
				level ctx false;
				print ctx "}";
				if !statics <> [] then begin
					newline ctx;
					newline ctx;
					print ctx "_%s = new Object();" (mts_path c.c_path);					
					List.iter (fun (v,e) ->
						newline ctx;
						print ctx "_%s.%s = " (mts_path c.c_path) v;
						generate_expr ctx e;
					) (List.rev !statics);					
				end;
			end;
			newline ctx;
			newline ctx;
		end;
	);
	(match !main with
	| None -> ()
	| Some (c,e) ->
		generate_function ctx "main" [] e false;
		print ctx "_%s.main = main;" (snd c.c_path)
	);
	IO.close_out ch

;;
let gen_only = ref false in
let output = ref None in
Plugin.add
	[
		("-as",Arg.String (fun f -> output := Some (f,false)),"<file> : generate AS1 code into target file");
		("-mtx-gen-only",Arg.Unit (fun() -> gen_only := true),": only generate mts file, do not compile");
		("-mtx",Arg.String (fun f -> 
			Typer.int_div_available := true;
			output := Some (f,true)
		),"<file> : generate MotionScript binary into target file");
	]
	(fun t ->
		match !output with
		| None -> ()
		| Some (file,mts) ->
			mts_mode := mts;
			let output = (if !mts_mode then Filename.chop_extension file ^ ".mts" else file) in
			generate output t;
			if !mts_mode && not !gen_only then begin
				let std = (try Plugin.find_file "std.mts" with Not_found -> failwith "std.mts not found, please specify path") in
				let verb = (if !Plugin.verbose then "-v " else "") in
				let cmd = "mts " ^ verb ^ "-c " ^ std ^ "  " ^ output in
				if !Plugin.verbose then print_endline cmd;
				if Sys.command cmd <> 0 then failwith "Compilation Aborted";
				let cmd = "mts " ^ verb ^ "-l " ^ file ^ " " ^ Filename.chop_extension std ^ ".mto " ^ Filename.chop_extension file ^ ".mto" in
				let cmd = (if !Plugin.verbose then cmd ^ " -ldbg" else cmd) in
				if !Plugin.verbose then print_endline cmd;
				if Sys.command cmd <> 0 then failwith "Linkage Aborted";
				Sys.remove (Filename.chop_extension file ^ ".mto");
				if not !Plugin.verbose then Sys.remove output;
			end;
	)