open Types
open Expr

type local = {
	l_type : ttype;
	l_frame : int;
	l_fun : func option;
}

type t = {
	vars : (string,ttype) Hashtbl.t;
	locals : (string,local) Hashtbl.t;
	classes : (type_path,t_class) Hashtbl.t;
	imports : (string,type_path) Hashtbl.t;
	polys : (string,ttype) Hashtbl.t;
	loader : t -> type_decl -> unit;
	mutable stack_frame : int;
	mutable in_static : bool;
	mutable returns : (pos * ttype) list;
	mutable cur_package : string list;
	mutable cur_class : t_class option;
	mutable cur_function : func option;
}

type error_msg =
	| Unknown_identifier of string
	| Function_redefinition of string
	| Cannot_access_notstatic of string
	| Have_no_field of ttype * string
	| Cannot_unify of ttype * ttype
	| Cannot_access_this
	| Cannot_access_super
	| Cannot_access_private of string
	| Class_not_found of type_path
	| Is_not_class of type_path
	| Is_not_interface of type_path
	| Cannot_unify_field of string * ttype * ttype 
	| Wrong_number_of_parameters
	| Error_stack of error_msg * error_msg
	| Missing_package of type_path

type field = 
	| Single of t_field
	| Fields of fields

exception Cannot_load_class of type_path
exception Error of error_msg * pos

let print_debug = ref false

let to_string t = if !print_debug then s_ttype ~print_class:true t else to_string t

let rec error_msg = function
	| Unknown_identifier i -> "Unknown identifier " ^ i
	| Cannot_access_notstatic f -> "Cannot access not static field " ^ f
	| Class_not_found p -> "Class not found " ^ s_type_path p
	| Function_redefinition s -> "Function redifinition " ^ s
	| Cannot_access_this -> "Cannot access 'this' outside of class or in static function"
	| Cannot_access_super -> "No super available here"
	| Is_not_class p -> s_type_path p ^ " is not a class"
	| Is_not_interface p -> s_type_path p ^ " is not an interface"
	| Wrong_number_of_parameters -> "Wrong number of parameters"
	| Cannot_access_private s -> "Cannot access private field " ^ s
	| Missing_package s -> "Missing package for class " ^ s_type_path s
	| Have_no_field (t,f) -> 
		let m = begin_mark() in
		mark m t;
		end_mark t;
		to_string t ^ " have no field " ^ f
	| Cannot_unify (ta,tb) -> 
		let m = begin_mark() in
		mark m ta;
		mark m tb;
		end_mark ta;
		end_mark tb;
		"Cannot unify " ^ to_string ta ^ " and " ^ to_string tb
	| Cannot_unify_field (f,ta,tb) -> 
		let m = begin_mark() in
		mark m ta;
		mark m tb;
		end_mark ta;
		end_mark tb;
		"Cannot unify " ^ f ^ " in " ^ to_string ta ^ " and " ^ to_string tb
	| Error_stack (m1,m2) ->
		error_msg m1 ^ "\n  " ^ error_msg m2

let error m p = raise (Error (m,p))

let error_stack m1 m2 p = raise (Error (Error_stack (m1,m2),p))

let make ?vars ?classes loader = 
	let vars = (match vars with None -> Hashtbl.create 0 | Some h -> h) in
	let classes = (match classes with None -> Hashtbl.create 0 | Some h -> h) in
	{
		vars = vars;
		classes = classes;
		loader = loader;
		locals = Hashtbl.create 0;
		polys = Hashtbl.create 0;
		imports = Hashtbl.create 0;
		in_static = false;
		stack_frame = 0;
		returns = [];
		cur_function = None;
		cur_package = [];
		cur_class = None;
	}

let load ctx name =
	ctx.loader (make ~vars:ctx.vars ~classes:ctx.classes ctx.loader) name

let debug = ref false

let create loader =
	let ctx = make loader in
	if not !debug then load ctx ([],"Std");
	ctx

let have_class_name c name =
	match c.c_name with
	| None -> false
	| Some path -> base_class_name path = name

let rec suppose_true ta tb = function
	| [] -> false
	| (t1,t2) :: _ when (t1 == ta && t2 == tb) || (t1 == tb && t2 == ta) -> true
	| _ :: l -> suppose_true ta tb l

let rec unify_rec ?(suppose=[]) ?(prof = 0) p ta tb =
	if ta.t == tb.t then
		()
	else begin
		let cannot_unify() = error (Cannot_unify (ta,tb)) p in
		let unify = unify_rec ~suppose ~prof:(prof+1) in
		let link ta tb =
			if is_recursive ta tb then cannot_unify();
			ta.t <- TLink tb;
			if ta.id = -1 && tb.id <> -1 then ta.id <- mk_id()
		in
		let print = (
			if !debug then begin
				match ta.t , tb.t with
				| TLink _ , _ 
				| _ , TLink _
				| TMono _ , TMono _
				| TPoly _ , TPoly _ ->
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
		| TPoly _ , _ ->
			link ta tb;
		| _ , TPoly _ ->
			link tb ta;
		| TMono _ , _ ->
			link ta tb;
		| _ , TMono _ ->
			link tb ta;
		| TFunction fa, TFunction fb when fa.f_nargs = fb.f_nargs ->
			(try
				List.iter2 (unify p) fa.f_args fb.f_args;
				unify p fa.f_ret fb.f_ret
			with
				| Error (msg,_)->
					error_stack (Cannot_unify (ta,tb)) msg p)
		| TFunction _, TFunction _ ->
			error Wrong_number_of_parameters p
		| TObjLazy oa, TObjLazy ob when oa == ob ->
			tb.t <- TLink ta;
		| TObjLazy oa, _ ->
			ta.t <- TLink (duplicate ta);
			unify p ta tb;
		| _ , TObjLazy ob ->
			tb.t <- TLink (duplicate tb);
			unify p ta tb;
		| THash ta, THash tb ->
			unify p ta tb
		| TObject oa, TObject ob when oa == ob || suppose_true oa ob suppose ->
			()
		| TObject oa, TObject ob ->
			let unify = unify_rec ~suppose:((oa,ob)::suppose) ~prof:(prof+1) in
			let unify_fields ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				PMap.iter (fun _ f ->
					try
						let f2 = PMap.find f.f_name fb in
						unify p f.f_type f2.f_type
					with
						| Error (msg,_)->
							error_stack (Cannot_unify_field (f.f_name,ta,tb)) msg p
						| Not_found -> ()
				) fa
			in
			let make_union ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				PMap.foldi (fun name f acc ->
					if PMap.mem name fb then
						acc
					else
						PMap.add name f acc
				) fa fb
			in
			let inside ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				try
					PMap.iter (fun _ f -> ignore(PMap.find f.f_name fb)) fa;
					true;
				with
					Not_found -> false
			in
			let check_included type_a ca cb =
				if not (inside ca cb) then begin
					let fa = ca.c_fields in
					let fb = cb.c_fields in
					PMap.iter (fun _ f -> 
						if not (PMap.mem f.f_name fb) then error (Have_no_field ((if type_a then tb else ta),f.f_name)) p;
					) fa;
				end
			in
			let make_inter ca cb =
				let fa = ca.c_fields in
				let fb = cb.c_fields in
				let m = PMap.foldi (fun name f acc ->
					try
						let f2 = PMap.find name fb in
						PMap.add name f acc
					with
						| Not_found -> acc
				) fa PMap.empty in
				if PMap.is_empty m then cannot_unify();
				m
			in
			let rec is_super sup c =
				if c == c_empty then 
					false 
				else if c == sup then
					true
				else 
					is_super sup c.c_super
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
						c_privates = PMap.empty;
						c_static = c_empty;
						c_interface = false;
						c_super = c_empty;
						c_id = mk_id();
					}
			in
			let sup_inter ca cb =
				if is_super ca cb then
					ca
				else if is_super cb ca then
					cb
				else if inside ca cb then
					ca
				else if inside cb ca then
					cb
				else
					{ 
						c_name = (match ca.c_name , cb.c_name with Some (_,n1), Some (_,n2) -> Some ([],n1 ^ " | " ^ n2) | _ ,_ -> None);
						c_fields = make_inter ca cb;
						c_privates = PMap.empty;
						c_static = c_empty;
						c_interface = false;
						c_super = c_empty;
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
				unify_fields c ob.o_inf;
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
		| TInt , TInt -> ()
		| TFloat , TFloat -> ()
		| TNumber , TNumber -> ()
		| TVoid , TVoid -> ()
		| TBool , TBool -> ()
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
		| _ , _ ->
			cannot_unify());
		if print then prerr_endline (String.make (prof*2) ' ' ^ "R " ^ s_ttype ta ^ " AND " ^ s_ttype tb );
	end

and unify p ta tb = unify_rec p ta tb

and try_unify p t = function
	| [] -> assert false
	| [x] -> unify p t x; t
	| x :: l ->
		try 
			unify p t x;
			x
		with
			Error _ -> try_unify p t l

and find_class ctx p path = 
	try
		Hashtbl.find ctx.classes path
	with
		Not_found ->
			try
				match path with
				| [] , cname -> find_class ctx p (Hashtbl.find ctx.imports cname)
				| _ -> raise Not_found
			with
				Not_found ->
					(try load ctx path with Cannot_load_class _ -> error (Class_not_found path) p);
					try
						Hashtbl.find ctx.classes path
					with
						Not_found ->
							error (Missing_package path) p

and t_string ctx =
	let c = find_class ctx null_pos ([],"String") in
	mk_obj_lazy c_empty (Some c)

and t_array ctx =
	let c = find_class ctx null_pos ([],"Array") in
	mk_obj_lazy c_empty (Some c)

and new_class p f =
	{
		c_name = None;
		c_fields = (match f with Single f -> PMap.add f.f_name f PMap.empty | Fields fields -> fields);
		c_privates = PMap.empty;
		c_static = c_empty;
		c_interface = false;
		c_super = c_empty;
		c_id = mk_id();
	}

and new_var ctx p v t =
	if Hashtbl.mem ctx.vars v then error (Function_redefinition v) p;
	Hashtbl.add ctx.vars v t

and new_local ctx p v t =
	Hashtbl.add ctx.locals v { l_type = t; l_frame = ctx.stack_frame; l_fun = ctx.cur_function }

and new_stack_frame ctx =
	let s = ctx.stack_frame in
	ctx.stack_frame <- ctx.stack_frame + 1;
	s

and clean_stack_frame ctx s =
	Hashtbl.iter (fun name l ->
		if l.l_frame > s then Hashtbl.remove ctx.locals name;
	) ctx.locals;
	ctx.stack_frame <- s

and all_fields c = 
	{
		c_name = c.c_name;
		c_fields = PMap.fold (fun f acc -> PMap.add f.f_name f acc) c.c_fields c.c_privates;
		c_privates = PMap.empty;
		c_static = c_empty;
		c_interface = false;
		c_super = c_empty;
		c_id = mk_id();
	}

and get_array_argt t =
	match t.t with
	| TLink t -> get_array_argt t
	| TFunction f ->
		f.f_ret
	| TObjLazy o ->
		t.t <- TLink (duplicate t);
		get_array_argt t
	| TObject o->
		(match o.o_sup with
		| None -> assert false
		| Some c ->
			try
				let f = PMap.find "pop" c.c_fields in
				get_array_argt f.f_type
			with
				Not_found -> assert false)
	| _ ->
		assert false

and type_decl ctx p (cpath,cname) =
	let rec load() =
		let c = find_class ctx p (cpath,cname) in
		mk_obj_lazy c_empty (Some c)
	in
	match cpath with
	| [] ->
		(match cname with
		| "number" | "Number" -> t_number()
		| "Function" -> mk_fun [] t_void
		| "float" -> t_float
		| "int" -> t_int
		| "hash" -> t_hash()
		| "bool" | "Boolean" -> t_bool
		| "void" | "Void" -> t_void
		| _ when cname.[0] = '\'' ->
			(try
				Hashtbl.find ctx.polys cname
			with
				Not_found -> 
					let p = mk_poly() in
					Hashtbl.add ctx.polys cname p;
					p)
		| _ ->
			load());
	| _ -> 
		let rec loop = function
			| [] -> assert false
			| [x] -> [] , x
			| x :: l ->
				let l , last = loop l in
				x :: l , last
		in
		(match cname with
		| "hash" -> 
			let t = type_decl ctx p (loop cpath) in
			mk_type (THash t)
		| "Array" ->
			let t = type_decl ctx p (loop cpath) in
			let ta = duplicate (t_array ctx) in
			unify p t (get_array_argt ta);
			ta
		| _ ->
			load())

and type_constant ctx p = function
	| Int _ -> t_number()
	| Float _ -> t_float
	| String _ -> t_string ctx
	| Ident "null" -> mk_obj c_empty None
	| Ident "undefined" -> mk_poly()
	| Ident "true" | Ident "false" -> t_bool
	| Ident "this" ->
		(match ctx.cur_class with
		| Some c when not ctx.in_static -> mk_obj c_empty (Some (all_fields c))
		| _ -> error Cannot_access_this p);
	| Ident "super" ->
		(match ctx.cur_class with
		| Some c when not ctx.in_static && c.c_super != c_empty -> mk_obj c_empty (Some (all_fields c.c_super))
		| _ -> error Cannot_access_super p);
	| Ident x ->
		try
			let l = Hashtbl.find ctx.locals x in
			l.l_type
		with Not_found -> try
			duplicate (Hashtbl.find ctx.vars x)
		with Not_found -> try
			match ctx.cur_class with
			| None -> raise Not_found
			| Some c ->
				if have_class_name c x then raise Not_found;
				try
					let f = (try PMap.find x c.c_fields with Not_found -> PMap.find x c.c_privates) in
					if ctx.in_static then error (Cannot_access_notstatic x) p;
					f.f_type
				with
					Not_found ->
						let f = (try PMap.find x c.c_static.c_fields with Not_found -> PMap.find x c.c_static.c_privates) in
						f.f_type
		with Not_found -> try
			if x.[0] < 'A' || x.[0] > 'Z' then raise Not_found;
			let c = find_class ctx p ([],x) in
			let c = (match ctx.cur_class with Some c2 when c == c2 -> all_fields c.c_static | _ -> c.c_static) in
			mk_obj_lazy c_empty (Some c)
		with Not_found ->
			error (Unknown_identifier x) p

and type_binop ctx op p1 t1 p2 t2 =
	let rec is_string t =
		match t.t with
		| TLink t -> is_string t
		| TObject o | TObjLazy o -> 
			(match o with
			| { o_sup = Some { c_name = Some ([],"String") } } -> true
			| _ -> false)
		| _ -> false
	in
	let rec is_int t =
		match t.t with
		| TLink t -> is_int t
		| TInt | TNumber -> true
		| _ -> false
	in
	let rec is_float t =
		match t.t with
		| TLink t -> is_float t
		| TFloat -> true
		| _ -> false
	in
	match op with
	| OpAssign ->
		unify (punion p1 p2) t1 t2;
		t1
	| OpEq | OpNotEq | OpGte | OpGt | OpLt | OpLte ->
		unify (punion p1 p2) t1 t2;
		t_bool
	| OpAssignOp op ->
		let tr = type_binop ctx op p1 t1 p2 t2 in
		unify (punion p1 p2) t1 tr;
		tr
	| OpBoolOr | OpBoolAnd ->
		unify p1 t1 t_bool;
		unify p2 t2 t_bool;
		t_bool
	| OpShr | OpShl | OpAnd | OpOr | OpXor ->
		unify p1 t1 t_int;
		unify p2 t2 t_int;
		t_int
	| OpMod | OpSub | OpMult ->
		unify p1 t1 (t_number());
		if is_float t1 then begin
			unify p2 t2 (t_number());
			t_float;
		end else begin
			unify p2 t2 (t_number());
			if is_float t2 then
				t_float
			else if is_int t2 then
				t1
			else
				t2
		end
	| OpDiv ->
		unify p1 t1 (t_number());
		unify p2 t2 (t_number());
		t_float
	| OpAdd ->
		let rec is_printable t = 
			match t.t with
			| TLink t -> is_printable t
			| TBool | TNumber | TInt | TFloat | TMono _ | TPoly _ -> true
			| _ -> false
		in
		if is_string t1 && is_printable t2 then
			t1
		else if is_printable t1 && is_string t2 then
			t2
		else begin
			let t = try_unify p1 t1 [t_number();t_string ctx] in
			if is_float t then begin
				unify p2 t2 (t_number());
				t_float;
			end else if is_int t then begin
				unify p2 t2 (t_number());
				t2
			end else begin
				unify p2 t t2;
				t
			end
		end

and type_unop p op flag t =
	match op with
	| Not ->
		unify p t t_bool;
		t
	| Increment
	| Decrement
	| Neg ->
		unify p t (t_number());
		t

and type_field ctx p t s =
	match t.t with
	| TLink t -> type_field ctx p t s 
	| TPoly _ | TMono _ ->
		let ft = mk_poly() in
		let f = { 
			f_name = s;
			f_type = ft;
			f_pos = p;
			f_typed = false;
		} in
		let c = new_class p (Single f) in
		t.t <- TLink (mk_obj c None);
		t.id <- mk_id();
		ft
	| THash t ->
		t
	| TObjLazy o ->
		t.t <- TLink (duplicate t);
		type_field ctx p t s
	| TObject o ->
		let ft , fpriv = (match o.o_sup with
		| None -> mk_poly() , false
		| Some c ->
			(try
				let f = PMap.find s c.c_fields in
				f.f_type , false
			with
				Not_found ->
					if PMap.mem s c.c_privates then
						(match ctx.cur_class with 
						| Some c2 when c.c_name = c2.c_name ->
							(PMap.find s c.c_privates).f_type , true
						| _ ->
							error (Cannot_access_private s) p)
					else
						error (Have_no_field (t,s)) p))
		in
		(try
			let f = PMap.find s (if fpriv then o.o_inf.c_privates else o.o_inf.c_fields) in
			f.f_type
		with
			Not_found ->
				let c = o.o_inf in
				let fields = PMap.add s { 
						f_name = s; 
						f_type = ft; 
						f_pos = p;
						f_typed = false;
				} (if fpriv then c.c_fields else c.c_privates) in
				o.o_inf <- { 
					c_name = None; 
					c_fields = if fpriv then c.c_fields else fields; 
					c_privates = if fpriv then fields else c.c_privates;
					c_static = c.c_static; 
					c_interface = false; 
					c_super = c.c_super;
					c_id = mk_id();
				};
				ft
		)
	| _ ->
		error (Have_no_field (t,s)) p		

and type_val ctx (v,p) =
	match v with
	| EConst c -> type_constant ctx p c
	| EBinop (op,v1,v2) ->
		let t1 = type_val ctx v1 in
		let t2 = type_val ctx v2 in
		type_binop ctx op (pos v1) t1 (pos v2) t2;
	| EField (v,s) ->
		let t = type_val ctx v in
		type_field ctx p t s
	| ECall ((EConst (Ident "super"),sp),args) ->
		let args = List.map (type_val ctx) args in
		let ft = mk_fun args (mk_mono()) in
		(match ctx.cur_class, ctx.cur_function with
		| None , _ | _ , None -> error Cannot_access_super sp
		| Some c , Some f ->
			if not (have_class_name c f.fname) || c.c_super == c_empty then error Cannot_access_super sp;
			let f = {
				f_name = (match c.c_super.c_name with None -> assert false | Some path -> base_class_name path);
				f_type = ft;
				f_pos = p;
				f_typed = false;
			} in
			let ctmp = new_class p (Single f) in
			unify p (mk_obj c_empty (Some (all_fields c.c_super.c_static))) (mk_obj ctmp None);
			t_void
		)
	| ECall (f,args) ->
		let ft = duplicate (type_val ctx f) in
		contravariant ft;
		let args = List.map (type_val ctx) args in
		let ret = mk_poly() in
		unify p ft (mk_fun args ret);
		covariant ret;
		ret
	| EParenthesis v ->
		type_val ctx v
	| EConstraint (v,t) ->
		let t = type_decl ctx p t in
		let vt = type_val ctx v in
		unify p t vt;
		vt
	| EQuestion (v,v1,v2) ->
		let t = type_val ctx v in
		unify (pos v) t t_bool;
		let t1 = type_val ctx v1 in
		let t2 = type_val ctx v2 in
		unify p t1 t2;
		t1
	| ENew (cpath,el) ->
		let c1 = find_class ctx p cpath in
		let dc1 = duplicate_class c1 in
		let argst = List.map (type_val ctx) el in
		let f = {
			f_name = base_class_name cpath;
			f_type = mk_fun argst t_void;
			f_pos = p;
			f_typed = false;
		} in
		let c2 = new_class p (Single f) in
		let t1 = mk_obj c_empty (Some (match ctx.cur_class with Some c2 when c1 == c2 -> all_fields dc1.c_static | _ -> dc1.c_static)) in
		let t2 = mk_obj c2 None in
		unify p t1 t2;
		mk_obj c_empty (Some dc1)
	| EUnop (op,flag,v) ->
		let vt = type_val ctx v in
		type_unop (pos v) op flag vt
	| EObjDecl el ->
		let fields = List.fold_left (fun acc (s,e) ->
			let t = type_val ctx e in
			PMap.add s {
				f_name = s;
				f_type = t;
				f_pos = (pos e);
				f_typed = false;
			} acc
		) PMap.empty el in
		mk_obj c_empty (Some (new_class p (Fields fields)))
	| EArrayDecl el ->
		let ta = mk_mono() in
		List.iter (fun e -> 
			let t = type_val ctx e in
			unify (pos e) ta t;
		) el;
		let at = duplicate (t_array ctx) in
		let rt = get_array_argt at in
		unify p rt ta;
		at
	| EArray (e1,e2) ->
		let t1 = type_val ctx e1 in
		let t2 = type_val ctx e2 in
		unify (pos e1) t1 (duplicate (t_array ctx));
		unify (pos e2) t2 t_int;
		get_array_argt t1

and type_var ctx p vtype vval =
	match vval , vtype with
	| None , None -> mk_mono()
	| Some v , None -> type_val ctx v
	| None, Some t -> 
		let t = type_decl ctx p t in
		t
	| Some v, Some t ->
		let vt = type_val ctx v in
		let tt = type_decl ctx p t in
		unify p vt tt;
		tt

and unify_field ctx c p fname t =
	let rec loop = function
		| [] -> assert false
		| f :: l ->
			try
				PMap.find fname f
			with
				Not_found -> loop l
	in
	let f = loop [c.c_fields;c.c_privates;c.c_static.c_fields;c.c_static.c_privates] in
	let t2 = duplicate f.f_type in
	covariant t2;
	unify f.f_pos t2 t;
	f.f_typed <- true

and type_class ctx name stat ((acc_pub,acc_priv) as acc) (e,p) =
	let type_field = function
		| Some ([],x) when x.[0] = '\'' -> type_decl ctx p ([],x)
		| _ -> mk_mono()
	in
	match e with
	| EBlock el -> List.fold_left (type_class ctx name stat) acc el
	| EVars (pub,vstat,vl) when vstat = stat ->
		List.fold_left (fun (acc_pub,acc_priv)  (v,vt,_) -> 
			if stat = IsMember && (PMap.mem v acc_pub || PMap.mem v acc_priv) then Checker.error (Checker.Duplicate_field v) p;
			let f = { f_name = v; f_type = mk_mono(); f_pos = p; f_typed = false } in			
			if pub = IsPublic then
				PMap.add v f acc_pub , acc_priv
			else
				acc_pub , PMap.add v f acc_priv
		) acc vl
	| EFunction f when (f.fstatic = stat && f.fname <> name) || (stat = IsStatic && f.fname = name) ->
		if stat = IsMember && (PMap.mem f.fname acc_pub || PMap.mem f.fname acc_priv) then 
			acc
		else
			let t = mk_fun (List.map (fun (_,at) -> type_field at) f.fargs) (type_field f.ftype) in
			let fi = { f_name = f.fname; f_type = t; f_pos = p; f_typed = false }  in
			if f.fpublic = IsPublic then
				PMap.add f.fname fi acc_pub , acc_priv
			else
				acc_pub , PMap.add f.fname fi acc_priv
	| EVars _ 
	| EFunction _ ->
		acc
	| _ -> assert false

and type_expr ?(no_type=false) ctx (e,p) = 
	match e with
	| EInterface (ipath,e) ->
		let iname = base_class_name ipath in
		let fields, privs = type_class ctx iname IsMember (PMap.empty,PMap.empty) e in
		let c = {
			c_name = Some ipath;
			c_fields = fields;
			c_privates = privs;
			c_static = c_empty;
			c_interface = true;
			c_super = c_empty;
			c_id = mk_id();
		} in
		Hashtbl.add ctx.classes ipath c;
		Hashtbl.add ctx.imports iname ipath;
		ctx.cur_class <- Some c;
		ignore(type_expr ctx e);
		ctx.cur_class <- None;
		mk_obj c_empty (Some c)
	| EClass (cpath,herits,e) ->
		let cname = base_class_name cpath in
		let parent = ref c_empty in
		let interfaces = List.fold_left (fun acc h ->
			match h with
			| HExtends name ->
				let c = find_class ctx p name in
				if c.c_interface then error (Is_not_class name) p;
				parent := duplicate_class c;
				acc
			| HImplements name ->
				let c = find_class ctx p name in
				if not c.c_interface then error (Is_not_interface name) p;
				duplicate_class c :: acc
			| HIntrinsic ->
				acc
		) [] herits in
		let parent = !parent in
		let t_parent = mk_obj parent None in
		List.iter (fun intf ->
			unify p t_parent (mk_obj intf None)
		) interfaces;
		let sfields, sprivs = type_class ctx cname IsStatic (parent.c_static.c_fields,parent.c_static.c_privates) e in
		let statics = {
			c_name = Some (fst cpath,"class:" ^ cname);
			c_fields = sfields;
			c_privates = sprivs;
			c_static = c_empty;
			c_interface = false;
			c_super = parent.c_static;
			c_id = mk_id();
		} in
		let fields, privs = type_class ctx cname IsMember (parent.c_fields,parent.c_privates) e in
		let c = {
			c_name = Some cpath;
			c_fields = fields;
			c_privates = privs;
			c_static = statics;
			c_interface = false;
			c_super = parent;
			c_id = mk_id();
		} in
		Hashtbl.add ctx.classes cpath c;
		Hashtbl.add ctx.imports cname cpath;
		ctx.cur_class <- Some c;
		ignore(type_expr ~no_type ctx e);
		ctx.cur_class <- None;
		let t = mk_obj c_empty (Some c) in
		unify p t_parent t;
		mk_obj c_empty (Some c)
	| EBlock el ->
		let s = new_stack_frame ctx in
		List.iter (fun e -> ignore(type_expr ~no_type ctx e)) el;
		clean_stack_frame ctx s;
		t_void	
	| EVars (pub,stat,vl) ->
		let type_var (vname,vtype,vval) =
			let t = type_var ctx p vtype vval in
			(match ctx.cur_function , ctx.cur_class with
			| None , None -> assert false (* checked by parser *)
			| None , Some c ->
				unify_field ctx c p vname t;
			| Some f , _ ->
				new_local ctx p vname t);
			t
		in
		(match vl with
		| [v] -> type_var v
		| _ -> 
			if not no_type then List.iter (fun v -> ignore(type_var v)) vl;
			t_void);
	| EFunction f ->
		let oldf = ctx.cur_function in
		let oldreturns = ctx.returns in
		let oldstat = ctx.in_static in
		let s = new_stack_frame ctx in
		ctx.cur_function <- Some f;
		ctx.in_static <- ctx.in_static || f.fstatic = IsStatic;
		ctx.returns <- [];
		let argst = List.map (fun (arg,argt) -> 
			let t = (match argt with None -> mk_poly() | Some t -> type_decl ctx p t) in
			new_local ctx p arg t;
			t
		) f.fargs in
		let rt = (match f.ftype with None -> mk_poly() | Some t -> type_decl ctx p t) in
		let ft = mk_fun argst rt in
		(match ctx.cur_class , oldf with
		| None , None -> 
			new_var ctx p f.fname ft
		| Some c , None -> 
			unify_field ctx c p f.fname ft;
		| _, Some _ -> 
			Hashtbl.add ctx.locals f.fname { l_type = ft; l_frame = s; l_fun = oldf };
		);
		if not no_type then (match f.fexpr with
		| None -> ()
		| Some e ->
			ignore(type_expr ctx e);
			match ctx.returns with
			| [] -> unify p rt t_void
			| _ -> 
				List.iter (fun (p,t) ->
					let t = duplicate t in
					cofree t;
					unify p t rt
				) ctx.returns
		);
		covariant rt;
		ctx.cur_function <- oldf;
		ctx.returns <- oldreturns;
		ctx.in_static <- oldstat;
		clean_stack_frame ctx s;
		ft
	| EVal v ->
		type_val ctx v
	| EType v ->
		let t = type_val ctx v in
		let pos = Lexer.get_error_pos (fun file line -> file ^ ":" ^ string_of_int line) p in
		print_endline (pos ^ " : type " ^ ((if !print_debug then s_ttype else Types.to_string) ~print_class:true t));
		t
	| EIf (cond,e,eelse) ->
		let t = type_val ctx cond in
		unify (pos cond) t t_bool;
		ignore(type_expr ctx e);
		(match eelse with
		| None -> ()
		| Some e2 -> ignore(type_expr ctx e2));
		t_void
	| EReturn v ->
		let t = (match v with
			| None -> t_void
			| Some v -> type_val ctx v)
		in
		ctx.returns <- (p,t) :: ctx.returns;
		t_void
	| EWhile (v,e,flag) ->
		(match flag with
		| NormalWhile -> 
			let t = type_val ctx v in
			unify (pos v) t t_bool;
			ignore(type_expr ctx e);
			t_void
		| DoWhile -> 
			ignore(type_expr ctx e);
			let t = type_val ctx v in
			unify (pos v) t t_bool;
			t_void);
	| EFor (inits,evals,incrs,e) ->
		let s = new_stack_frame ctx in
		List.iter (fun e -> ignore(type_expr ctx e)) inits;
		List.iter (fun v -> unify (pos v) (type_val ctx v) t_bool) evals;
		List.iter (fun v -> ignore(type_val ctx v)) incrs;
		ignore(type_expr ctx e);
		clean_stack_frame ctx s;
		t_void
	| EForIn (init,eval,e) ->
		let s = new_stack_frame ctx in
		let t = type_expr ctx init in
		let et = type_val ctx eval in
		let ta = duplicate (t_array ctx) in
		unify (pos eval) et ta;
		unify p t t_int;
		ignore(type_expr ctx e);
		clean_stack_frame ctx s;
		t_void;
	| ESwitch (v,el,eo) ->
		let t = type_val ctx v in
		List.iter (fun (v,e) ->
			let vt = type_val ctx v in
			unify (pos v) t vt;
			ignore(type_expr ctx e);
		) el;
		(match eo with None -> () | Some e -> ignore(type_expr ctx e));
		t_void;
	| EImport cpath ->
		Hashtbl.add ctx.imports (base_class_name cpath) cpath;
		t_void
	| ENoType e ->
		type_expr ~no_type:true ctx e
	| EBreak
	| EContinue ->
		t_void