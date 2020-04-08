open Expr
open Type
open Typetools

type imports = (string,type_path) Hashtbl.t

type priority = High | Low

type local_variable = {
	l_type : ttype;
	l_frame : int;
}

type context = {
	classes : (type_path,class_context) Hashtbl.t;
	files : (string,unit) Hashtbl.t;
	class_path : string list;
	imports : imports;
	locals : (string,local_variable) Hashtbl.t;
	todo : (int,priority * (unit -> unit)) Hashtbl.t;
	mutable current : class_context;
	mutable in_static : bool;
	mutable in_loop : bool;
	mutable returns : (ttype * pos) list;
	mutable cur_frame : int;
	mutable cur_polys : (string, (ttype * pos)) PMap.t;
}

type error_msg =
	| Class_not_found of type_path
	| Package_mistake of type_path
	| Missing_parameters of type_path
	| Have_no_field of ttype * string
	| Cannot_unify of ttype * ttype
	| Cannot_unify_field of string * ttype * ttype
	| Cannot_add of ttype * ttype
	| Error_stack of error_msg * error_msg
	| Unknown_identifier of string
	| Custom of string
	| Protect of error_msg
	| Cannot_downcast of ttype

exception Error of error_msg * pos
exception File_not_found of string

let verbose = ref false
let print_unify = ref false
let print_debug = ref false
let int_div_available = ref false
let float_mod_available = ref true
let compare_polymorph = ref true
let can_add_objects = ref true
let time_string = (
	let t = Unix.localtime (Unix.time()) in
	Printf.sprintf "%.2d-%.2d %.2d:%.2d:%.2d" (t.Unix.tm_mon + 1) t.Unix.tm_mday t.Unix.tm_hour t.Unix.tm_min t.Unix.tm_sec)

let todo_id = ref 0
let todo ctx p f = 
	incr todo_id;
	Hashtbl.add ctx.todo !todo_id (p,f)

let to_string ctx ?(print_class=false) t = if !print_debug then s_ttype ~ctx ~print_class:true t else to_string ~ctx ~print_class t

let verbose_msg msg =
	if !verbose then begin prerr_endline msg; flush stderr; end

let rec error_msg ?(ctx=print_context()) = function
	| Class_not_found path -> "Class not found " ^ s_type_path path
	| Package_mistake path -> "Package mistake " ^ s_type_path path
	| Missing_parameters path -> "Missing parameter for " ^ s_type_path path
	| Unknown_identifier name -> "Unknown identifer " ^ name
	| Have_no_field (t,f) ->		
		to_string ctx t ^ " have no field " ^ f
	| Cannot_unify (ta,tb) ->		
		"Cannot unify " ^ to_string ctx ta ^ " and " ^ to_string ctx tb
	| Cannot_unify_field (f,ta,tb) ->
		"Cannot unify " ^ f ^ " in " ^ to_string ctx ta ^ " and " ^ to_string ctx tb
	| Cannot_add (ta,tb) ->
		"Cannot add " ^ to_string ctx ta ^ " and " ^ to_string ctx tb
	| Cannot_downcast t ->
		"Cannot downcast not object type " ^ to_string ctx t
	| Error_stack (m1,m2) ->
		error_msg m2 ^ "\n  " ^ error_msg m1
	| Protect msg ->
		error_msg msg
	| Custom msg -> msg


let error m p = raise (Error (m,p))

let error_stack m1 m2 p = raise (Error (Error_stack (m1,m2),p))

let clean_polys ctx old =
	PMap.iter (fun x (t,p) ->
		if not (PMap.mem x old) then begin
			match t.t with
			| TTemplate _ ->
				t.t <- TPoly
			| _ ->
				error (Custom ("Not a template : " ^ x ^ " " ^ to_string (print_context()) t)) p
		end;
	) ctx.cur_polys;
	ctx.cur_polys <- old

let create class_path =
	{
		class_path = class_path;
		classes = Hashtbl.create 0;
		files = Hashtbl.create 0;
		imports = Hashtbl.create 0;
		locals = Hashtbl.create 0;
		in_static = false;
		in_loop = false;
		returns = [];
		todo = Hashtbl.create 0;
		cur_frame = 0;
		current = empty_class_context;
		cur_polys = PMap.empty;
	}

let rec suppose_true ta tb = function
	| [] -> false
	| (t1,t2) :: _ when (t1 == ta && t2 == tb) || (t1 == tb && t2 == ta) -> true
	| _ :: l -> suppose_true ta tb l

let rec unify_rec ctx ?(suppose=[]) ?(prof = 0) p ta tb =
	if ta.t == tb.t then begin
		if ta != tb && ta.t == TNumber && tb.t == TNumber then ta.t <- TLink tb;
	end else begin
		let cannot_unify() = error (Cannot_unify (ta,tb)) p in
		let unify = unify_rec ctx ~suppose ~prof:(prof+1) in
		let link ta tb =
			if is_recursive ta tb then cannot_unify();
			ta.t <- TLink tb;
			if ta.id = -1 then ta.id <- mk_id()
		in
		let print = (
			if !print_unify then begin
				match ta.t , tb.t with
				| TLink _ , _
				| _ , TLink _
				| TMono , TMono
				| TPoly , TPoly ->
					false
				| _ , _ ->
					prerr_endline (String.make (prof*2) ' ' ^ "U " ^ s_ttype ta ^ " AND " ^ s_ttype tb );
					true
			end else
				false)
		in
		(match ta.t , tb.t with
		| TLink t , _ ->
			unify p t tb
		| _ , TLink t ->
			unify p ta t
		| TLazy f , _ ->
			(try f() with Error (msg,p) -> error (Protect msg) p);
			unify p ta tb
		| _ , TLazy f ->
			(try f() with Error (msg,p) -> error (Protect msg) p);
			unify p ta tb
		| TTemplate _, TPoly ->
			tb.t <- TLink ta
		| TTemplate _, TMono ->
			tb.id <- mk_id();
			tb.t <- TLink ta
		| TPoly , TTemplate _ ->
			ta.t <- TLink tb
		| TMono , TTemplate _ ->
			ta.id <- mk_id();
			ta.t <- TLink tb
		| TPoly , _ ->
			link ta tb;
		| _ , TPoly ->
			link tb ta;
		| TMono , _ ->
			link ta tb;
		| _ , TMono ->
			link tb ta;
		| TAbbrev (ta,pl1,v1) , TAbbrev (tb,pl2,v2) when ta = tb ->
			List.iter2 (unify p) pl1 pl2
		| TFunction fa, TFunction fb when List.length fa.f_args = List.length fb.f_args ->
			(try
				List.iter2 (unify p) fa.f_args fb.f_args;
				unify p fa.f_ret fb.f_ret
			with
				| Error (Protect _,_) as e ->
					raise e
				| Error (msg,p)->
					error_stack (Cannot_unify (ta,tb)) msg p)
		| TFunction _, TFunction _ ->
			error (Custom "Wrong number of parameters") p
		| TObject oa, TObject ob when oa == ob || suppose_true oa ob suppose ->
			()
		| TObject oa, TObject ob ->
			let unify = unify_rec ctx ~suppose:((oa,ob)::suppose) ~prof:(prof+1) in
			let unify_fields ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				fiter (fun f ->
					try
						let f2 = ffind f.f_name fb in
						unify p f.f_type f2.f_type
					with
						| Error (Protect _,_) as e ->
							raise e
						| Error (msg,_)->
							error_stack (Cannot_unify_field (f.f_name,ta,tb)) msg p
						| Not_found -> ()
				) fa
			in
			let make_union ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				ffold (fun acc f ->
					if fmem f.f_name fb then
						acc
					else
						fadd f acc
				) fa fb
			in
			let inside ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				try
					fiter (fun f -> ignore(ffind f.f_name fb)) fa;
					true;
				with
					Not_found -> false
			in
			let check_included type_a ca cb =
				if not (inside ca cb) then begin
					let fa = ca.c_fields in
					let fb = cb.c_fields in
					fiter (fun f ->
						if not (fmem f.f_name fb) then error (Have_no_field ((if type_a then tb else ta),f.f_name)) p;
					) fa;
				end
			in
			let make_inter ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				let m = ffold (fun acc f ->
					try
						ignore(ffind f.f_name fb);
						fadd f acc
					with
						| Not_found -> acc
				) fa fempty in
				if fis_empty m then cannot_unify();
				m
			in
			let inf_union() =
				if inside oa.o_inf ob.o_inf then
					ob.o_inf
				else if inside ob.o_inf oa.o_inf then
					oa.o_inf
				else
					{
						c_name = (match oa.o_inf.c_name , ob.o_inf.c_name with Some (_,n1), Some (_,n2) -> Some ([],n1 ^ " | " ^ n2) | _ ,_ -> None);
						c_fields = make_union oa.o_inf ob.o_inf;
						c_id = mk_id();
					}
			in
			let sup_inter ca cb =
				if ca == cb then
					ca
				else if inside ca cb then
					ca
				else if inside cb ca then
					cb
				else
					{
						c_name = (match ca.c_name , cb.c_name with Some (_,n1), Some (_,n2) -> Some ([],n1 ^ " | " ^ n2) | _ ,_ -> None);
						c_fields = make_inter ca cb;
						c_id = mk_id();
					}
			in
			(match oa.o_sup, ob.o_sup with
			| None , None ->
				if oa.o_inf != ob.o_inf then begin
					unify_fields oa.o_inf ob.o_inf;
					let inf = inf_union() in
					if inf == oa.o_inf then
						tb.t <- TLink ta
					else if inf == ob.o_inf then
						ta.t <- TLink tb
					else
						let o = mk_obj inf None in
						ta.t <- TLink o;
						tb.t <- TLink o;
				end;
			| Some c , None ->
				check_included false ob.o_inf c;
				unify_fields ob.o_inf c;
				let inf = inf_union() in
				if inf == oa.o_inf then
					tb.t <- TLink ta
				else
					let o = mk_obj inf (Some c) in
					ta.t <- TLink o;
					tb.t <- TLink o;
			| None, Some c ->
				check_included true oa.o_inf c;
				unify_fields oa.o_inf c;
				let inf = inf_union() in
				if inf == ob.o_inf then
					ta.t <- TLink tb
				else
					let o = mk_obj inf (Some c) in
					ta.t <- TLink o;
					tb.t <- TLink o;
			| Some ca , Some cb ->
				check_included true oa.o_inf cb;
				check_included false ob.o_inf ca;
				unify_fields ca cb;
				let inf = inf_union() in
				let sup = sup_inter ca cb in
				if inf == oa.o_inf && sup == ca then
					tb.t <- TLink ta
				else if inf == ob.o_inf && sup == cb then
					ta.t <- TLink tb
				else
					let o = mk_obj inf (Some sup) in
					ta.t <- TLink o;
					tb.t <- TLink o;
			);
		| TNumber , TFloat ->
			ta.t <- TFloat;
			ta.id <- -1;
		| TNumber , TInt ->
			ta.t <- TInt;
			ta.id <- -1;
		| TInt , TNumber ->
			tb.t <- TInt;
			tb.id <- -1;
		| TFloat , TNumber ->
			tb.t <- TFloat;
			tb.id <- -1;
		| TAbbrev (p,_,v) , TObject o | TObject o , TAbbrev (p,_,v) when suppose_true (Obj.magic p) o suppose ->
			()
		| TAbbrev (path,_,v) , TObject o ->
			unify_rec ctx ~suppose:((Obj.magic path,o)::suppose) p (expand_abbrev ctx ta) tb
		| TObject o , TAbbrev (path,_,v) ->
			unify_rec ctx ~suppose:((o,Obj.magic path)::suppose) p ta (expand_abbrev ctx tb) 
		| TAbbrev (p1,_,v1) , TAbbrev (p2,_,v2) when suppose_true (Obj.magic p1) (Obj.magic p2) suppose ->
			()
		| TAbbrev (p1,_,v1) , TAbbrev (p2,_,v2) ->
			unify_rec ctx ~suppose:((Obj.magic p1,Obj.magic p2)::suppose) p (expand_abbrev ctx ta) (expand_abbrev ctx tb) 
		| _ , _ ->
			cannot_unify());
		if print then prerr_endline (String.make (prof*2) ' ' ^ "R " ^ s_ttype ta ^ " AND " ^ s_ttype tb );
	end

and unify ctx p ta tb =
	unify_rec ctx p ta tb

and load_file ctx file =	
	let rec loop = function
		| [] -> raise (File_not_found file)
		| path :: paths ->
			try
				let file = path ^ file in
				file , open_in file
			with
				_ -> loop paths
	in
	let file, ch = loop ctx.class_path in
	let expr, comments = (try
		Parser.parse (Lexing.from_channel ch) file
	with
		| exc ->
			close_in ch;
			raise exc
	) in
	close_in ch;
	if Hashtbl.mem ctx.files file then error (Custom ("Malformed file " ^ file)) null_pos;
	let cc = Checker.context() in
	List.iter (Checker.check cc) expr;
	Hashtbl.add ctx.files file ();
	verbose_msg ("Parsed " ^ file);
	let ctx = { ctx with imports = Hashtbl.create 0 } in
	List.iter (fun e -> ignore(type_expr ctx e)) expr

and resolve ctx ((cpath,cname) as path) =	
	if cpath = [] then try
		Hashtbl.find ctx.imports cname		
	with Not_found -> path else path

and load_class ctx path p =
	try
		Hashtbl.find ctx.classes path
	with
		Not_found ->
			let file_name = (match fst path with
				 | [] -> snd path ^ ".mt"
				 | _ -> String.concat "/" (fst path) ^ "/" ^ snd path ^ ".mt")
			in
			try
				load_file ctx file_name;
				Hashtbl.find ctx.classes path
			with
				| File_not_found _ -> error (Class_not_found path) p
				| Not_found -> error (Package_mistake path) p

and force_class c =
	let forced = ref false in
	fiter (fun f ->
		if force_type f.f_type then forced := true;
	) c.c_fields;
	!forced

and force_type t =
	match t.t with
	| TLink t -> force_type t
	| TLazy f ->
		f();
		ignore(force_type t);
		true
	| TObject o ->
		false
	| TFunction f ->
		let forced = ref false in
		List.iter (fun t -> if force_type t then forced := true) f.f_args;
		if force_type f.f_ret then forced := true;
		!forced
	| TAbbrev _
	| TTemplate _
	| TPoly
	| TMono
	| TInt
	| TFloat
	| TNumber
	| TBool
	| TVoid ->
		false

and type_interface_super_field ctx c stat name p =	
	let rec loop = function
		| [] -> None
		| (c,params) :: l ->
			try 
				let t = (ffind name c.c_inst.c_fields).f_type in
				Some (apply_params c params (duplicate ~keep_templates:true t))
			with
				Not_found -> loop l
	in
	match stat , name with
	| _ , "new"
	| IsStatic, _ -> None
	| IsMember, _ | IsVolatile, _ ->
		loop (match c.c_super with None -> c.c_interfaces | Some s -> s :: c.c_interfaces)

and is_function_fully_typed f c =
	let rec has_return (e,p) =
		match e with
		| EBlock el -> List.exists has_return el
		| EFor (_,_,_,e) -> has_return e
		| EIf (_,e1,None) -> has_return e1
		| EIf (_,e1,Some e2) | ETry (e1,_,_,e2) -> has_return e1 || has_return e2
		| EWhile (_,e,_) -> has_return e
		| ESwitch (_,cl,None) -> List.exists (fun (_,e) -> has_return e) cl
		| ESwitch (_,cl,Some e) -> List.exists (fun (_,e) -> has_return e) cl || has_return e
		| EReturn None
		| EClass _
		| EInterface _
		| EVars _
		| EFunction _
		| EBreak
		| EContinue
		| EVal _
		| EImport _ ->
			false
		| EReturn (Some _) ->
			true
	in
	match f.fexpr with
	| None ->
		(** if return type not specified, assume Void
			if argument type not specified, assume Polymorph **)
		true
	| Some expr ->
		List.for_all (fun (_,argt) -> argt <> None) f.fargs && (f.ftype <> None || not (has_return expr))

and bind_local ctx name t =
	Hashtbl.add ctx.locals name { l_type = t; l_frame = ctx.cur_frame }

and new_stack_frame ctx =
	let s = ctx.cur_frame in
	ctx.cur_frame <- ctx.cur_frame + 1;
	s

and clean_stack_frame ctx frame =
	Hashtbl.iter (fun name l ->
		if l.l_frame > frame then Hashtbl.remove ctx.locals name;
	) ctx.locals;
	ctx.cur_frame <- frame

and expand_abbrev ctx t =
	match t.t with
	| TLink t -> expand_abbrev ctx t
	| TObject _ -> t
	| TAbbrev (path,params,tstyle) ->
		let c = load_class ctx path null_pos in
		let t = (match tstyle with
			| Lower -> mk_obj c.c_inst None
			| Upper -> mk_obj c_empty (Some c.c_inst)
			| Constraint -> mk_obj c.c_inst (Some c.c_inst)
		) in
		apply_params c params t
	| _ ->
		assert false

and type_decl ctx tstyle p = function
	| TypeStd tstd ->
		(match tstd with
		| TypeInt when tstyle = Upper -> t_number()
		| TypeInt -> t_int
		| TypeFloat when tstyle = Lower -> t_number()
		| TypeFloat -> t_float
		| TypeBool -> t_bool
		| TypeVoid -> t_void)
	| TypePath (path,params) ->
		let c = load_class ctx (resolve ctx path) p in
		if List.length params <> List.length c.c_params then error (Missing_parameters path) p;
		mk_abbrev c.c_path (List.map (type_decl ctx Constraint p) params) tstyle
	| TypeFun (t1,t2) ->
		let t1 = type_decl ctx Lower p t1 in
		let t2 = type_decl ctx Upper p t2 in
		(match t1.t , t2.t with
		| TVoid , _ -> mk_fun [] t2
		| _ , TFunction f -> mk_fun (t1 :: f.f_args) f.f_ret
		| _ , _ -> mk_fun [t1] t2)
	| TypeFields (f,fe)  ->
		let fbase = (match fe with None -> fempty | Some t ->
			let td = type_decl ctx Lower p t in
			let rec loop t =
				match t.t with
				| TAbbrev _ ->
					loop (expand_abbrev ctx t)
				| TObject o -> 
					o.o_inf.c_fields
				| _ -> error (Custom "Invalid type extension") p
			in
			loop td)
		in
		let fields = List.fold_left (fun acc (name,t) ->
			if fmem name acc then error (Custom ("Duplicate type field " ^ name)) p;
			let t = type_decl ctx tstyle p t in
			fadd { f_name = name; f_type = t } acc
		) fbase f in
		let o = {
			c_name = None;
			c_fields = fields;
			c_id = mk_id();
		} in
		(match tstyle with
		| Lower -> mk_obj o None
		| Upper -> mk_obj c_empty (Some o)
		| Constraint -> mk_obj o (Some o))
	| TypePoly s ->
		try
			snd (List.find (fun (name,_) -> name = s) ctx.current.c_params)
		with
			Not_found ->
				try
					fst (PMap.find s ctx.cur_polys)
				with
					Not_found ->
						let t = mk_t (TTemplate s) in
						ctx.cur_polys <- PMap.add s (t,p) ctx.cur_polys;
						t

and t_array ctx ts =
	let c = load_class ctx ([],"Array") null_pos in
	let h = Hashtbl.create 20 in
	let ct = duplicate ~h (match ts with
		| Lower -> mk_obj c.c_inst None
		| Upper -> mk_obj c_empty (Some c.c_inst)
		| Constraint -> mk_obj c.c_inst (Some c.c_inst)
	) in
	let pt = (try Hashtbl.find h (snd (List.hd c.c_params)).id with _ -> assert false) in
	ct , pt

and type_function ctx f p =
	let old_returns = ctx.returns in
	let old_static = ctx.in_static in
	let old_inloop = ctx.in_loop in
	let old_polys = ctx.cur_polys in
	ctx.in_loop <- false;
	ctx.in_static <- f.fstatic = IsStatic;
	ctx.returns <- [];
	let frame = new_stack_frame ctx in
	let argst = List.map (fun (argname,argt) ->
		let t = (match argt with
			| None -> mk_mono()
			| Some t -> type_decl ctx Constraint p t)
		in
		bind_local ctx argname t;
		t
	) f.fargs in
	let rett = (match f.ftype with None -> mk_mono() | Some t -> type_decl ctx Constraint p t) in
	(match type_interface_super_field ctx ctx.current f.fstatic f.fname p with
	| None -> ()
	| Some t ->
		let t = tconstraint (duplicate t) in
		unify ctx p t (mk_fun argst rett));
	let etyped = (match f.fexpr with
	| None ->
		(* if is_function_fully_typed, we're doing here an after-lazy-typing-check *)
		None
	| Some e ->		
		let et = type_expr ctx e in
		(match ctx.returns with
		| [] -> unify ctx p rett t_void
		| _ ->
			if f.fname = "new" then unify ctx p rett t_void;
			List.iter (fun (t,p) ->
				let t = relax_object t in
				unify ctx p t rett
			) ctx.returns;
		);
		Some et
	) in
	clean_stack_frame ctx frame;
	ctx.in_loop <- old_inloop;
	ctx.returns <- old_returns;
	ctx.in_static <- old_static;
	(* dup rett in order to separate it from linked argument , in order to correctly downcast *)
	let rett = duplicate rett in
	let newt = mk_t (TFunction { f_argnames = List.map fst f.fargs; f_args = argst; f_ret = rett}) in
	(* relax locals so they become unifiable and the function becomes polymorphic *)
	List.iter mono_to_poly (rett :: argst);
	clean_polys ctx old_polys;
	let newt = downcast newt in
	match etyped with
	| None -> None , newt 
	| Some e ->
		let e = mk_e (TLambda (List.map2 (fun (a,_) t -> a,t) f.fargs argst,e,rett)) newt p in
		Some e , newt

and add_class_field c name t stat e =
	let cf = {
		cf_name = name;
		cf_type = t;
		cf_static = (stat = IsStatic);
		cf_volatile = (stat = IsVolatile);
		cf_expr = e
	} in
	c.c_class_fields <- cf :: c.c_class_fields;
	cf

and type_lazy_function ctx f p t cf () =
	let ctx = {
		ctx with
		locals = Hashtbl.create 0;
		cur_polys = PMap.empty;
		in_static = (f.fstatic = IsStatic)
	} in
	verbose_msg ("Typing " ^ s_type_path ctx.current.c_path ^ "." ^ f.fname);
	(* setup temporary monomorphic type for recursion *)
	t.t <- TLazy (fun () -> error (Custom ("Recursive call of " ^ s_type_path ctx.current.c_path ^ "." ^ f.fname ^ " need explicit typing")) p);
	let e , nt = type_function ctx f p in
	t.t <- TLink nt;
	cf.cf_expr <- e

and type_check_function ctx f t p cf () =
	let ft = mk_t TVoid in
	type_lazy_function ctx f p ft cf ();
	unify ctx p t ft;

and type_class_fun ctx c f p t =
	let cf = add_class_field ctx.current f.fname t f.fstatic None in
	t.t <- (match type_interface_super_field ctx c f.fstatic f.fname p with
		| None ->
			(match is_function_fully_typed f c with
			| true ->
				if f.fexpr <> None then todo ctx Low (type_check_function ctx f t p cf);
				let ft = TFunction {
					f_argnames = List.map fst f.fargs;
					f_args = List.map (fun (_,argt) -> match argt with None -> mk_mono() | Some t -> type_decl ctx Lower p t) f.fargs;
					f_ret = (match f.ftype with None -> t_void | Some t -> type_decl ctx Upper p t);
				} in
				clean_polys ctx PMap.empty;
				ft
			| false ->
				todo ctx High (fun () -> ignore(force_type t));				
				TLazy (type_lazy_function ctx f p t cf))
		| Some t ->
			if f.fexpr <> None then todo ctx Low (type_check_function ctx f t p cf);
			TLink t
	);
	mono_to_poly t

and type_class_var ctx c vname vt vinit stat p t =
	let cf = add_class_field c vname t stat None in
	match vt , vinit with
	| None , None -> error (Custom "Variable type required") p
	| Some vt, None ->
		t.t <- TLink (type_decl ctx Constraint p vt);
		clean_polys ctx PMap.empty;
	| None , Some v when stat = IsStatic ->
		t.t <- TLazy (fun () ->
			let id = t.id in
			t.id <- -1;
			let et = type_val ctx v in
			cf.cf_expr <- Some et;
			t.t <- TLink (etype et);
			t.id <- id
		)
	| Some vt, Some v when stat = IsStatic ->
		let t2 = type_decl ctx Constraint p vt in
		clean_polys ctx PMap.empty;
		t.t <- TLazy (fun () ->
			let id = t.id in
			let et = type_val ctx v in
			cf.cf_expr <- Some et;
			t.t <- TLink (etype et);
			t.id <- id;
			unify ctx (pos v) t t2;
		)
	| _ , _ ->
		error (Custom "Cannot initialize member variable in declaration") p

and type_class_fields ctx c (e,p) =
	let make_class_field stat name f =
		let cl, name = (match stat, name with
			| IsStatic , "new" -> error (Custom "Cannot have static constructor") p
			| IsStatic, _ -> c.c_class , name
			| IsMember , "new" -> c.c_class , name
			| IsMember, _ | IsVolatile , _ -> c.c_inst , name)
		in
		if fmem name cl.c_fields then error (Custom ("Cannot redefine field " ^ name)) p;
		let t = mk_t TVoid in
		t.t <- TLazy (fun () ->
			let id = t.id in
			t.id <- -1;
			f t;
			t.id <- id
		);
		cl.c_fields <- fadd { f_name = name; f_type = t } cl.c_fields
	in
	match e with
	| EBlock el -> List.iter (type_class_fields ctx c) el
	| EVars (stat,vl) ->
		List.iter (fun (vname,vt,vinit) ->			
			make_class_field stat vname (type_class_var ctx c vname vt vinit stat p)
		) vl
	| EFunction f ->
		make_class_field f.fstatic f.fname (type_class_fun ctx c f p)
	| _ ->
		assert false

and force_class_fields c =
	let loop f = 
		match f.f_type.t with
		| TLazy f -> f() (* no recursive, one single pass *)
		| _ -> ()
	in
	fiter loop c.c_class.c_fields;
	fiter loop c.c_inst.c_fields

and type_class ctx path params herits is_interf e p =
	let herits , is_native = (match herits with
		| HNative :: l -> l , true
		| _ -> herits, false)
	in
	verbose_msg ("Typing " ^ s_type_path path);
	let params = List.map (fun p ->
		p , mk_t (TTemplate (snd path ^ "." ^ (String.sub p 1 (String.length p - 1))))
	) params in
	let c = {
		c_super = None;
		c_interfaces = [];
		c_class = {
			c_fields = fempty;
			c_name = Some (fst path,"#" ^ snd path);
			c_id = mk_id();
		};
		c_inst = {
			c_fields = fempty;
			c_name = Some path;
			c_id = mk_id();
		};
		c_style = (if is_interf then [PInterface] else []) @ (if is_native then [PNative] else []);
		c_class_fields = [];
		c_path = path;
		c_params = params;
		c_pos = p;
	} in
	Hashtbl.add ctx.classes path c;
	let ctx = {	ctx with 
		current = c;
		cur_polys = PMap.empty;
	} in
	type_class_fields ctx c e;
	List.iter (fun h ->
		match h with
		| HNative -> assert false
		| HExtends (path,params) when c.c_super = None ->
			let cls = load_class ctx (resolve ctx path) p in
			if is_interface cls then error (Custom "Cannot extend an interface") p;
			let tparams = List.map (type_decl ctx Upper p) params in
			let tsup = type_decl ctx Upper p (TypePath (path,params)) in
			c.c_super <- Some (cls,tparams);
			let rec loop t =
				match t.t with
				| TObject { o_sup = Some o } ->
					fiter (fun f ->
						if not (fmem f.f_name c.c_inst.c_fields) then c.c_inst.c_fields <- fadd f c.c_inst.c_fields;
					) o.c_fields
				| TAbbrev _ ->
					t.t <- TLink (expand_abbrev ctx t);
					loop t
				| TLink t -> loop t
				| _ -> assert false
			in
			loop tsup
		| HExtends _ -> error (Custom "multiple inheritance not allowed") p;
		| HImplements _ ->
			()
	) herits;
	List.iter (fun h ->
		match h with
		| HImplements (path,params) ->
			let cls = load_class ctx (resolve ctx path) p in
			let tparams = List.map (type_decl ctx Upper p) params in
			if not (is_interface cls) then error (Custom "Cannot implement a class") p;
			ignore(type_decl ctx Lower p (TypePath (path,params)));
			fiter (fun f ->
				if not (fmem f.f_name c.c_inst.c_fields) then
					error (Custom ("Missing field " ^ f.f_name ^ " needed by " ^ s_type_path path )) p
			) cls.c_inst.c_fields;
			c.c_interfaces <- (cls,tparams) :: c.c_interfaces
		| HNative 
		| HExtends _ ->
			()
	) herits;
	force_class_fields c

and type_number_op ctx p t1 t2 =
	match t1.t , t2.t with
	| TLink t1 , _ -> type_number_op ctx p t1 t2
	| _ , TLink t2 -> type_number_op ctx p t1 t2
	| TInt , TInt -> t_number()
	| TFloat , _ | _ , TInt -> t1
	| _ , TFloat | TInt , _ -> t2
	| _ , _ ->
		unify ctx p t1 t2; t1

and type_binop ctx op p t1 p1 t2 p2 =
	match op with
	| OpAssign ->
		(* no upcast on objects : this need to be manual *)
		let t2 = relax_int t2 in
		unify ctx p t1 t2;
		t1
	| OpAssignOp op ->
		let t = type_binop ctx op p t1 p1 t2 p2 in
		type_binop ctx OpAssign p t1 p1 t p
	| OpAdd ->
		let rec loop t =
			match t.t with
			| TLink t -> loop t
			| TFunction f ->
				f.f_args = [] && is_string f.f_ret
			| _ -> false
		in
		let find c infer =
			try
				loop (ffind "toString" c.c_fields).f_type
			with
				Not_found ->
					if infer then begin
						let t = mk_fun [] t_string in
						c.c_fields <- fadd { f_name = "toString"; f_type = t } c.c_fields;
						true
					end else
						false
		in
		let rec has_to_string o =
			!can_add_objects &&
			match o.o_sup with
			| None -> find o.o_inf true
			| Some c -> find c false
		in
		let rec is_printable t =
			match t.t with
			| TLink t -> is_printable t
			| TInt | TFloat | TNumber | TBool -> true
			| TLazy f -> f(); is_printable t
			| TObject o -> is_string t || has_to_string o
			| TVoid | TTemplate _
			| TPoly | TMono | TFunction _ -> false
			| TAbbrev _ -> is_printable (expand_abbrev ctx t)
		in
		if is_string t1 && is_printable t2 then
			mk_abbrev ([],"String") [] Constraint
		else if is_printable t1 && is_string t2 then
			mk_abbrev ([],"String") [] Constraint
		else begin
			let rec loop t1 t2 =
				match t1.t , t2.t with
				| TLink t1 , _ -> loop t1 t2
				| _ , TLink t2 -> loop t1 t2
				| TInt, TInt ->
					t_number()
				| TInt , TFloat
				| TNumber, TFloat
				| TInt , TNumber ->
					t2
				| TNumber, TNumber ->
					if t1 != t2 then t2.t <- TLink t1;
					t1
				| TNumber, TInt -> t1
				| TFloat, TInt
				| TFloat, TFloat
				| TFloat, TNumber -> t1
				| TPoly , TInt
				| TMono , TInt ->
					t1.t <- TNumber;
					t1.id <- mk_id();
					t1
				| TPoly , TNumber
				| TMono , TNumber ->
					t1.t <- TLink t2;
					t1.id <- mk_id();
					t2
				| TPoly , TFloat
				| TMono , TFloat ->
					t1.t <- TNumber;
					t1.id <- mk_id();
					t2
				| TInt, TPoly 
				| TInt, TMono ->
					t2.t <- TNumber;
					t2.id <- mk_id();
					t2
				| TNumber, TPoly
				| TNumber, TMono ->
					t2.t <- TLink t1;
					t2.id <- mk_id();
					t1
				| TFloat, TPoly
				| TFloat, TMono ->
					t2.t <- TNumber;
					t2.id <- mk_id();
					t1
				| TPoly, TPoly
				| TMono, TMono
				| TPoly, TMono
				| TMono, TPoly ->
					t1.t <- TNumber;
					t1.id <- mk_id();
					if t1 != t2 then begin
						t2.t <- TLink t1;
						t2.id <- mk_id();
					end;
					t1
				| TMono, TAbbrev _
				| TPoly, TAbbrev _ 
				| TMono, TObject _
				| TPoly, TObject _ when is_string t2 ->
					t1.t <- TLink t2;
					t1.id <- mk_id();
					t2
				| TAbbrev _ , TMono
				| TAbbrev _ , TPoly
				| TObject _ , TMono
				| TObject _ , TPoly when is_string t1 ->
					t2.t <- TLink t1;
					t2.id <- mk_id();
					t1
				| TAbbrev _ , TAbbrev _
				| TObject _ , TObject _ when is_string t1 && is_string t2 ->
					t1
				| _ , _ ->
					error (Cannot_add (t1,t2)) p
			in
			loop t1 t2
		end
	| OpMult
	| OpSub ->
		unify ctx p1 t1 (t_number());
		unify ctx p2 t2 (t_number());
		type_number_op ctx p t1 t2
	| OpDiv ->
		unify ctx p1 t1 (t_number());
		unify ctx p1 t2 (t_number());
		if !int_div_available then
			type_number_op ctx p t1 t2
		else
			t_float
	| OpGt
	| OpGte
	| OpLt
	| OpLte ->
		if !compare_polymorph then begin
			if not (is_any_number t1) || not(is_any_number t2) then unify ctx p t1 t2
		end else begin
			unify ctx p1 t1 (t_number());
			unify ctx p1 t2 (t_number());
		end;
		t_bool
	| OpAnd
	| OpOr
	| OpXor
	| OpShl
	| OpUShr
	| OpShr ->
		unify ctx p1 t1 t_int;
		unify ctx p2 t2 t_int;
		t_number()
	| OpEq
	| OpNotEq ->
		unify ctx p t1 t2;
		t_bool
	| OpBoolAnd
	| OpBoolOr ->
		unify ctx p1 t1 t_bool;
		unify ctx p2 t2 t_bool;
		t_bool
	| OpMod ->
		if !float_mod_available then begin
			unify ctx p1 t1 (t_number());
			unify ctx p2 t2 (t_number());
			type_number_op ctx p t1 t2
		end else begin
			unify ctx p1 t1 t_int;
			unify ctx p2 t2 t_int;
			t_number();
		end

and type_ident ctx name p =
	(* local variable lookup *)
	try
		let loc = Hashtbl.find ctx.locals name in
		mk_e (TLocal name) loc.l_type p
	with
		Not_found ->
	(* member variable lookup *)
	try
		let f = ffind name ctx.current.c_inst.c_fields in
		if ctx.in_static then error (Custom ("Cannot access member variable " ^ name ^" in static function")) p;
		mk_e (TMember (name,ctx.current)) (duplicate ~keep_templates:true f.f_type) p
	with
		Not_found ->
	(* static variable lookup *)
	try
		let f = ffind name ctx.current.c_class.c_fields in
		let e = mk_e (TClass ctx.current) (mk_obj c_empty (Some ctx.current.c_class)) p in
		mk_e (TField (e,name)) (duplicate ~keep_templates:true f.f_type) p
	with
		Not_found ->
			error (Unknown_identifier name) p

and type_constant ctx c p =
	let const c t =
		mk_e (TConst c) t p
	in
	match c with
	| Int s -> const (TConstInt s) (t_number())
	| Float s -> const (TConstFloat s) t_float
	| String s -> const (TConstString s) t_string
	| Ident "true" -> const (TConstBool true) t_bool
	| Ident "false" -> const (TConstBool false) t_bool
	| Ident "null" -> const TConstNull (mk_mono())
	| Ident "this" ->
		if ctx.in_static then error (Custom "Cannot access this in static function") p;
		const TConstThis (mk_abbrev ctx.current.c_path (List.map snd ctx.current.c_params) Upper)
	| Ident "super" -> error (Custom "super is not a value") p
	| Ident "__LINE__" ->
		const (TConstInt (string_of_int (Lexer.get_error_line p))) (t_number()) 
	| Ident "__FILE__" ->
		const (TConstString p.pfile) t_string
	| Ident "__CLASS__" ->
		const (TConstString (s_type_path ctx.current.c_path)) t_string
	| Ident "__TIME__" ->
		const (TConstString time_string) t_string
	| Ident name ->
		type_ident ctx name p
	| Name _ ->
		assert false

and type_unop ctx op flag t p =
	match op with
	| Not ->
		unify ctx p t t_bool;
		t
	| NegBits ->
		unify ctx p t t_int;
		t_int
	| Increment
	| Decrement
	| Neg ->
		unify ctx p t (t_number());
		if is_float t then
			t_float
		else
			t_number()

and type_constructor ctx class_path class_params p =
	let c = load_class ctx (resolve ctx class_path) p in
	let class_params = (match class_params with
		| [] ->
			List.map (fun _ -> mk_mono()) c.c_params
		| _ ->
			if List.length c.c_params <> List.length class_params then error (Custom "Missing type parameter") p;
			class_params
	) in
	let cc = mk_obj c_empty (Some c.c_class) in
	let cc = apply_params c class_params cc in
	cc , mk_abbrev c.c_path class_params Upper

and type_val ctx (v,p) =
	match v with
	| EConst (Name n) ->
		type_val ctx (EStatic ([],n),p)
	| EConst c ->
		type_constant ctx c p
	| EParenthesis v ->
		let e = type_val ctx v in
		mk_e (TParenthesis e) e.ttype p
	| EBinop (op,v1,v2) ->
		let e1 = type_val ctx v1 in
		let e2 = type_val ctx v2 in
		let t = type_binop ctx op p (etype e1) (pos v1) (etype e2) (pos v2) in
		mk_e (TBinop (op,e1,e2)) t p
	| ECall ((EConst (Ident "super"),superpos),args) ->
		let eargs = List.map (type_val ctx) args in
		let fnew = mk_fun (List.map etype eargs) t_void in
		let onew = mk_obj (mk_field "new" fnew) None in
		(match ctx.current.c_super with
		| None -> error (Custom "No super constructor") p
		| Some (cl,cp) ->
			let tc, _ = type_constructor ctx cl.c_path cp p in
			unify ctx p (duplicate ~keep_templates:true tc) onew;
			mk_e (TCall (mk_e (TBuiltin SuperConstructor) tc superpos,eargs)) t_void p)
	| ECall ((EConst (Ident "callback"),_), obj :: (EConst (Ident s),_) :: args) ->
		let eargs = List.map (type_val ctx) args in
		let e = type_val ctx (EField (obj,s),p) in
		(match (etype e).t with
		| TFunction f ->
			let rec loop = function
				| e1 :: l1 , x2 :: l2 ->
					unify ctx (epos e1) (etype e1) x2;
					loop (l1,l2)
				| [] , l -> l
				| _ , [] -> error (Custom "Too many callback arguments") p
			in
			let ft = mk_fun (loop (eargs,f.f_args)) f.f_ret in
			mk_e (TBuiltin (Callback (type_val ctx obj,s,eargs))) ft p
		| _ -> error (Custom "Callback not a function") p)
	| ECall ((EConst (Ident "TRACE"),_),[arg]) ->
		let e = type_val ctx arg in
		mk_e (TBuiltin (Trace (e,p.pfile,Lexer.get_error_line p))) t_void p
	| ECall ((EConst (Ident "TYPE"),_),[arg]) ->
		let e = type_val ctx arg in		
		print_endline ("TYPE = " ^ to_string (print_context()) ~print_class:true (etype e));
		mk_e (TBuiltin (PrintType e)) t_void p
	| ECall ((EField ((EConst (Name "Std"),_),"cast"),_),[arg])
	| ECall ((EConst (Ident "cast"),_),[arg]) ->
		let e = type_val ctx arg in
		mk_e (TBuiltin (Cast e)) (mk_mono()) p
	| ECall ((EConst (Ident "upcast"),_),[arg]) ->
		let e = type_val ctx arg in
		let t = duplicate ~keep_templates:true (etype e) in
		mk_e (TBuiltin (Upcast e)) (upcast t) p
	| ECall ((EConst (Ident "downcast"),_),[arg]) ->
		let e = type_val ctx arg in
		let rec loop t =
			match t.t with
			| TLink t -> loop t
			| TObject _ -> duplicate ~keep_templates:true t
			| TAbbrev _ -> 
				(* do not change variance *)
				duplicate ~keep_templates:true (expand_abbrev ctx t)
			| _ ->
				error (Cannot_downcast t) p
		in
		mk_e (TBuiltin (Downcast e)) (downcast (loop (etype e))) p		 
	| ECall ((EConst (Ident "int"),_),[arg]) ->
		let e = type_val ctx arg in
		mk_e (TBuiltin (ToInt e)) (t_number()) p
	| ECall ((EConst (Ident "string"),_),[arg]) ->
		let e = type_val ctx arg in
		mk_e (TBuiltin (ToString e)) t_string p
	| ECall (fv,args) ->
		let e = type_val ctx fv in
		let eargs = List.map (type_val ctx) args in
		let ret = mk_mono() in
		let ft = mk_fun (List.map etype eargs) ret in
		unify ctx p (etype e) ft;
		mk_e (TCall (e,eargs)) ret p
	| EField ((EConst (Ident "super"),_),fname) ->
		let rec loop (c,params) =
			try
				let f = List.find (fun cf -> cf.cf_name = fname) c.c_class_fields in
				c , apply_params c params (duplicate ~keep_templates:true f.cf_type)
			with
				Not_found ->
					match c.c_super with
					| Some c -> loop c
					| None -> error (Have_no_field (mk_abbrev ctx.current.c_path [] Upper,fname)) p
		in
		let csup , ft = loop (match ctx.current.c_super with
				| None -> error (Custom "Class does not have a super") p
				| Some c -> c) in
		mk_e (TMember (fname,csup)) ft p
	| ENew (cp,tparams,args) ->
		let eargs = List.map (type_val ctx) args in
		let fnew = mk_fun (List.map etype eargs) t_void in
		let onew = mk_obj (mk_field "new" fnew) None in
		let tparams = List.map (type_decl ctx Upper p) tparams in
		let tc, ti = type_constructor ctx cp tparams p in
		unify ctx p (duplicate ~keep_templates:true tc) onew;
		let cl = load_class ctx (resolve ctx cp) p in
		mk_e (TNew (cl,tparams,eargs)) ti p
	| EUnop (op,flag,v) ->
		let e = type_val ctx v in
		let t = type_unop ctx op flag (etype e) p in
		mk_e (TUnop (op,flag,e)) t p
	| EStatic cpath ->
		let c = load_class ctx (resolve ctx cpath) p in
		let t = mk_obj c_empty (Some c.c_class) in
		mk_e (TClass c) t p
	| EField (v,f) ->
		let e = type_val ctx v in
		let field add c =
			try
				duplicate (ffind f c.c_fields).f_type
			with
				Not_found when not add -> error (Have_no_field (etype e,f)) p
		in
		let rec loop t =
			match t.t with
			| TLink t -> loop t
			| TLazy f -> f(); loop t
			| TMono ->
				let ft = mk_mono() in
				t.t <- TObject { o_inf = mk_field f ft; o_sup = None };
				t.id <- mk_id();
				ft
			| TPoly ->
				let ft = mk_mono() in
				t.t <- TObject { o_inf = mk_field f ft; o_sup = None };
				ft
			| TAbbrev _ ->
				loop (expand_abbrev ctx t)
			| TObject o ->
				(match o.o_sup with
				| None ->
					(try
						field true o.o_inf
					with
						Not_found ->
							let ft = mk_mono() in
							o.o_inf.c_fields <- fadd { f_name = f; f_type = ft } o.o_inf.c_fields;
							ft
					)
				| Some c -> field false c)
			| _ -> error (Have_no_field (t,f)) p
		in
		mk_e (TField (e,f)) (loop (etype e)) p
	| EQuestion (q,v1,v2) ->
		let e = type_val ctx q in
		unify ctx (pos q) (etype e) t_bool;
		let e1 = type_val ctx v1 in
		let e2 = type_val ctx v2 in
		unify ctx (punion (pos v1) (pos v2)) (etype e1) (etype e2);
		mk_e (TIf (e,e1,Some e2)) (etype e1) p 
	| EArray (v1,v2) ->
		let ct , pt = t_array ctx Lower in
		let e2 = type_val ctx v2 in
		unify ctx (pos v2) (etype e2) t_int;
		let e1 = type_val ctx v1 in
		unify ctx (pos v1) ct (etype e1);
		mk_e (TArray (e1,e2)) (upcast pt) p
	| EArrayDecl vl ->
		let t = mk_mono() in
		let eargs = List.map (type_val ctx) vl in
		List.iter (fun e -> unify ctx (epos e) t (etype e)) eargs;
		let ct , pt = t_array ctx Upper in
		unify ctx p t pt;
		mk_e (TArrayDecl eargs) ct p
	| EObjDecl fields ->
		let efields = List.map (fun (name,v) -> name , type_val ctx v) fields in
		let fields = List.fold_left (fun acc (name,e) ->			
			fadd { f_name = name; f_type = etype e } acc
		) fempty efields in
		let o = mk_obj c_empty (Some { c_name = None; c_fields = fields; c_id = mk_id(); }) in
		mk_e (TObjDecl efields) o p
	| ELambda f ->
		let etyped , et = type_function ctx f p in
		match etyped with
		| None -> assert false
		| Some e -> e

and type_expr ctx (e,p) =
	match e with
	| EImport path ->
		if fst path = [] then error (Custom "Import without path is not allowed") p;
		Hashtbl.add ctx.imports (snd path) path;
		enone
	| EClass (path,params,herits,e) -> 
		type_class ctx path params herits false e p;
		enone
	| EInterface (path,params,e) ->
		type_class ctx path params [] true e p;
		enone
	| EBlock el ->
		let frame = new_stack_frame ctx in
		let el = List.map (type_expr ctx) el in
		clean_stack_frame ctx frame;
		mk_e (TBlock el) t_void p
	| EVal v ->
		mk_e (TDiscard (type_val ctx v)) t_void p
	| EReturn v ->
		let e = (match v with
			| None -> mk_e (TReturn None) t_void p
			| Some v ->
				let e = type_val ctx v in
				mk_e (TReturn (Some e)) (etype e) p)
		in
		ctx.returns <- (etype e,p) :: ctx.returns;
		e
	| EFunction f ->
		error (Custom "Local function declaration is not allowed") p;
	| EVars (IsStatic,_) | EVars (IsVolatile,_) ->
		error (Custom "Local static or volatile variable is not allowed") p;
	| EVars (IsMember,vars) ->
		let evars = List.map (fun (vname,vtype,vval) ->
			match vval , vtype with
			| None , None -> vname , mk_mono() , None
			| Some v , None -> 
				let e = type_val ctx v in
				vname , etype e , Some e
			| None, Some t ->
				let t =	type_decl ctx Constraint p t in
				vname , t , None
			| Some v, Some t ->
				let e = type_val ctx v in
				let t = type_decl ctx Constraint p t in
				unify ctx p (etype e) t;
				vname , t , Some e
		) vars in
		List.iter (fun (vname,t,_) ->
			if Hashtbl.mem ctx.locals vname then error (Custom ("Local variable overriden : " ^ vname)) p;
			Hashtbl.add ctx.locals vname { l_type = t; l_frame = ctx.cur_frame };
		) evars;
		mk_e (TVars evars) t_void p
	| EFor (inits,conds,incrs,e) ->
		let s = new_stack_frame ctx in
		let einits = List.map (type_expr ctx) inits in
		let econds = List.map (fun v ->
			let e = type_val ctx v in
			unify ctx (pos v) (etype e) t_bool;
			e
		) conds in
		let eincrs = List.map (type_val ctx) incrs in
		let old = ctx.in_loop in
		ctx.in_loop <- true;
		let e = type_expr ctx e in
		ctx.in_loop <- old;
		clean_stack_frame ctx s;
		mk_e (TFor (einits,econds,eincrs,e)) t_void p
	| EIf (cond,e,eelse) ->
		let econd = type_val ctx cond in
		unify ctx (pos cond) (etype econd) t_bool;
		let e = type_expr ctx e in
		let eelse = (match eelse with
			| None -> None
			| Some e2 -> Some (type_expr ctx e2)
		) in
		mk_e (TIf (econd,e,eelse)) t_void p
	| EWhile (v,e,flag) ->
		let old = ctx.in_loop in
		ctx.in_loop <- true;
		let e = (match flag with
		| NormalWhile ->
			let econd = type_val ctx v in
			unify ctx (pos v) (etype econd) t_bool;
			let e = type_expr ctx e in
			mk_e (TWhile (econd,e,flag)) t_void p
		| DoWhile ->
			let e = type_expr ctx e in
			let econd = type_val ctx v in
			unify ctx (pos v) (etype econd) t_bool;
			mk_e (TWhile (econd,e,flag)) t_void p) in
		ctx.in_loop <- old;
		e
	| ESwitch (v,cases,def) ->
		let e = type_val ctx v in
		let old = ctx.in_loop in
		ctx.in_loop <- true;
		let ecases = List.map (fun (v,exp) -> 
			let ec = type_val ctx v in
			unify ctx (pos v) (etype e) (etype ec);			
			ec , type_expr ctx exp
		) cases in
		let edef = (match def with None -> None | Some e -> Some (type_expr ctx e)) in
		ctx.in_loop <- old;
		mk_e (TSwitch (e,ecases,edef)) t_void p
	| ETry (e,s,t,e2) ->
		let e = type_expr ctx e in
		let et = (match t with None -> mk_mono() | Some t -> type_decl ctx Constraint p (TypePath (t,[]))) in
		if Hashtbl.mem ctx.locals s then error (Custom ("Local variable overriden : " ^ s)) p;		
		Hashtbl.add ctx.locals s { l_type = et; l_frame = ctx.cur_frame };
		let e2 = type_expr ctx e2 in
		Hashtbl.remove ctx.locals s;
		mk_e (TTry (e,s,t,e2)) t_void p
	| EBreak ->
		if not ctx.in_loop then error (Custom "Break is not available outside a loop") p;
		mk_e TBreak t_void p
	| EContinue ->
		if not ctx.in_loop then error (Custom "Continue is not available outside a loop") p;
		mk_e TContinue t_void p

let finalize ctx =
	verbose_msg "Finalizing...";
	let rec loop() =
		let next = ref false in
		Hashtbl.iter (fun id (p,f) ->
			if p = High then begin
				next := true;
				Hashtbl.remove ctx.todo id;
				f()
			end
		) ctx.todo;
		if not !next then begin
			try
				Hashtbl.iter (fun id (p,f) ->
					if p = Low then begin
						Hashtbl.remove ctx.todo id;
						f();
						raise Exit
					end
				) ctx.todo;
				Hashtbl.iter (fun _ c ->
					if force_class c.c_inst then raise Exit;
					if force_class c.c_class then raise Exit;
				) ctx.classes;
			with
				Exit -> loop()
		end else
			loop()
	in
	loop();
	Hashtbl.iter (fun _ c ->
		c.c_class_fields <- List.sort (fun f1 f2 ->
			match f1.cf_expr , f2.cf_expr with
			| None , Some _ -> -1
			| Some _ , None -> 1
			| None , None -> 0
			| Some e1 , Some e2 -> compare (epos e1).pmin (epos e2).pmin
		) c.c_class_fields
	) ctx.classes

let dump ctx =
	let pctx = print_context() in
	Hashtbl.iter (fun _ c ->
		let tc = mk_obj c_empty (Some c.c_class) in
		let ti = mk_obj c_empty (Some c.c_inst) in		
		print_endline (to_string pctx ~print_class:true tc);
		print_endline (to_string pctx ~print_class:true ti);
	) ctx.classes

type generated =
	| NotYet
	| Generating
	| Done

let generate ctx f =
	let hgen = Hashtbl.create 0 in
	let generated c = 
		try
			Hashtbl.find hgen c.c_path
		with
			Not_found -> NotYet
	in
	let rec loop_class c =
		match generated c with
		| Done -> ()
		| Generating -> prerr_endline ("Warning : loop in generation for class " ^ s_type_path c.c_path)
		| NotYet ->
			Hashtbl.add hgen c.c_path Generating;
			(match c.c_super with None -> () | Some (c,_) -> loop_class c);
			List.iter (fun (c,_) -> loop_class c) c.c_interfaces;
			List.iter (fun f ->
				if f.cf_static then begin
					match f.cf_expr with
					| None -> ()
					| Some { texpr = TLambda _ } -> ()
					| Some e -> loop_expr c e
				end;
			) c.c_class_fields;
			f c;
			Hashtbl.replace hgen c.c_path Done;
	and loop_expr c e =
		expr_iter (fun e ->
			match e.texpr with
			| TClass c2
			| TNew (c2,_,_) when c != c2 -> loop_class c2
			| _ -> ()
		) e
	in
	Hashtbl.iter (fun _ c -> loop_class c) ctx.classes

let getclass ctx path =
	Hashtbl.find ctx.classes path

let map_classes f ctx =
	let h = Hashtbl.create 0 in
	Hashtbl.iter (fun _ c -> 
		let c = f c in
		Hashtbl.add h c.c_path c
	) ctx.classes;
	{ ctx with classes = h }

let add_class ctx c =
	Hashtbl.add ctx.classes c.c_path c
