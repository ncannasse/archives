open Type

let fempty = []
let fis_empty l = l = []
let fiter = List.iter
let fmap = List.map
let ffind n l = List.find (fun f -> f.f_name = n) l
let fmem n l = List.exists (fun f -> f.f_name = n) l
let fadd f l = f :: l
let rec freplace f = function
	| [] -> [f]
	| { f_name = name } :: l when name = f.f_name -> f :: l
		| x :: l -> x :: (freplace f l)

let ffold f l v = List.fold_left f v l

let t_void = { t = TVoid; id = -1 }

let t_int = { t = TInt; id = -1 }

let t_float = { t = TFloat; id = -1 }

let t_bool = { t = TBool; id = -1 }

let c_empty = { c_fields = fempty; c_name = None; c_id = -1; }

let null_pos = { pfile = "<null>"; pmin = -1; pmax = -1 }

let s_type_path (p,s) = match p with [] -> s | _ -> String.concat "." p ^ "." ^ s

let constructor c =
	try 
		let cf = List.find (fun f -> f.cf_name = "new") c.c_class_fields in
		match cf.cf_expr with
		| None -> None
		| Some { texpr = TLambda (args,e,ret) } -> Some (args,e,ret)
		| _ -> assert false
	with
		Not_found -> None

let empty_class_context = {
	c_path = ([],"<no class>");
	c_params = [];
	c_super = None;
	c_interfaces = [];
	c_class_fields = [];
	c_style = [];
	c_pos = null_pos;
	c_inst = { c_fields = fempty; c_name = None; c_id = -1 };
	c_class = { c_fields = fempty; c_name = None; c_id = -1 };
}

let is_native c = List.exists ((=) PNative) c.c_style
let is_interface c = List.exists ((=) PInterface) c.c_style

let id_count = ref 0

let debug_id = ref true

let mk_id() =
	incr id_count;
	!id_count

let mk_t t = {
	t = t;
	id = mk_id();
}

let mk_e e t p = {
	texpr = e;
	ttype = t;
	tpos = p;
}

let etype e = e.ttype
let epos e = e.tpos
let eexpr e = e.texpr

let enone = mk_e TBreak t_void null_pos

let mk_abbrev t params v = 
	{
		t = TAbbrev (t,params,v);
		id = (if params = [] then -1 else mk_id());
	}

let t_string = mk_abbrev ([],"String") [] Upper

let mk_poly() =	mk_t TPoly

let mk_mono() =
	{
		t = TMono;
		id = -1;
	}

let mk_obj inf sup =
	mk_t (TObject {
		o_inf = inf;
		o_sup = sup;
	})

let mk_fun args ret =
	mk_t (TFunction {
		f_argnames = [];
		f_args = args;
		f_ret = ret
	})

let mk_field fname ftype =
	{
		c_name = None;
		c_fields = fadd { 
			f_name = fname;
			f_type = ftype;
		} fempty;
		c_id = mk_id();
	}

let t_number() = mk_t TNumber

let type_iter ?(in_classes=false) f t =
	let h = Hashtbl.create 0 in
	let rec loop t =
		f t;		
		match t.t with
		| TLink t -> loop t
		| TObject o when in_classes ->
			if not (Hashtbl.mem h t.id) then begin
				Hashtbl.add h t.id ();
				loop_class o.o_inf;
				(match o.o_sup with
				| None -> ();
				| Some c -> loop_class c);
			end;
		| TFunction func ->
			List.iter loop func.f_args;
			loop func.f_ret
		| TAbbrev _
		| TTemplate _
		| TLazy _
		| TObject _
		| TMono
		| TPoly
		| TInt
		| TFloat
		| TNumber
		| TBool
		| TVoid 
			->
			()
	and loop_class c =
		fiter (fun fi -> loop fi.f_type) c.c_fields
	in
	loop t

let rec is_recursive t t2 =
	if t2 == t then 
		true
	else match t2.t with
	| TLink t2 -> is_recursive t t2
	| TFunction f ->
		List.exists (is_recursive t) (f.f_ret :: f.f_args);
	| TAbbrev _
	| TLazy _
	| TObject _
	| TPoly
	| TMono
	| TTemplate _
	| TVoid
	| TInt
	| TFloat
	| TNumber
	| TBool ->
		false

let rec relax_int t =
	match t.t with
	| TLink t -> relax_int t
	| TNumber
	| TInt -> t_number()
	| TLazy _
	| TTemplate _
	| TFunction _
	| TObject _
	| TPoly
	| TMono
	| TAbbrev _
	| TVoid
	| TFloat	
	| TBool ->
		t

let rec relax_object t =
	match t.t with
	| TLink t -> relax_object t	
	| TObject o when o.o_inf != c_empty && o.o_sup <> None ->
		mk_obj c_empty (Some o.o_inf)
	| TLazy f -> 
		f();
		relax_object t
	| TAbbrev _
	| TObject _
	| TInt
	| TTemplate _
	| TFunction _	
	| TPoly
	| TMono
	| TVoid
	| TFloat
	| TNumber
	| TBool ->
		t

let rec is_string t =
	match t.t with
	| TLink t -> is_string t
	| TLazy f -> f(); is_string t
	| TAbbrev (([],"String"),[],_) -> true
	| TObject o ->
		o.o_inf.c_name = Some ([],"String") || (match o.o_sup with Some { c_name = Some ([],"String") } -> true | _ -> false)
	| _ ->
		false

let rec is_float t =
	match t.t with
	| TLink t -> is_float t
	| TLazy f -> f(); is_float t
	| TFloat -> true
	| _ -> false

let rec is_int t =
	match t.t with
	| TLink t -> is_int t
	| TLazy f -> f(); is_int t
	| TInt -> true
	| _ -> false

let rec is_any_number t =
	match t.t with
	| TLink t -> is_any_number t
	| TLazy f -> f(); is_any_number t
	| TInt | TFloat | TNumber -> true
	| _ -> false

let rec upcast t =
	match t.t with
	| TLink t -> upcast t
	| TAbbrev (t,params,_) -> 
		mk_abbrev t params Upper
	| TObject o ->
		(match o.o_sup with
		| None ->
			if o.o_inf != c_empty then mk_obj c_empty (Some o.o_inf) else t
		| Some c ->
			mk_obj c_empty (Some c));
	| TFunction f ->
		mk_t (TFunction {
			f_argnames = f.f_argnames;
			f_args = List.map downcast f.f_args;
			f_ret = upcast f.f_ret;
		})
	| TInt ->
		t_number()
	| TLazy _ ->
		assert false
	| TTemplate _
	| TPoly
	| TMono
	| TFloat
	| TNumber
	| TBool
	| TVoid ->
		t

and tconstraint ?(variance=false) t =
	match t.t with
	| TLink t -> tconstraint ~variance t
	| TAbbrev (t,params,v) -> mk_abbrev t params Constraint
	| TObject o ->
		(match o.o_sup with
		| None ->
			mk_obj o.o_inf (Some o.o_inf)
		| Some c ->
			mk_obj c (Some c));
	| TFunction f ->
		mk_t (TFunction {
			f_argnames = f.f_argnames;
			f_args = List.map (tconstraint ~variance:true) f.f_args;
			f_ret = tconstraint ~variance:false f.f_ret;
		})
	| TNumber ->
		if variance then
			t_float
		else
			t_int
	| TLazy _ ->
		assert false
	| TInt
	| TTemplate _
	| TPoly
	| TMono
	| TFloat
	| TBool
	| TVoid ->
		t

and downcast t =
	match t.t with
	| TLink t -> downcast t
	| TAbbrev (t,params,v) -> mk_abbrev t params Lower
	| TObject o -> 
		(match o.o_sup with
		| None -> t
		| Some c -> mk_obj c None);
	| TFunction f ->
		mk_t (TFunction {
			f_argnames = f.f_argnames;
			f_args = List.map downcast f.f_args;
			f_ret = upcast f.f_ret;
		})
	| TFloat ->
		t_number()
	| TLazy _ ->
		assert false
	| TTemplate _
	| TPoly
	| TMono
	| TInt
	| TNumber
	| TBool
	| TVoid ->
		t

let mono_to_poly t =
	let h = Hashtbl.create 5 in
	let rec loop t = 
		match t.t with
		| TLink t -> loop t
		| TAbbrev _
		| TLazy _	
		| TInt
		| TFloat
		| TNumber
		| TBool
		| TVoid
		| TTemplate _
		| TPoly -> ()
		| TObject o ->
			if not (Hashtbl.mem h t.id) then begin
				Hashtbl.add h t.id ();
				loop_class o.o_inf;
				match o.o_sup with
				| None -> ()
				| Some c -> loop_class c
			end;
		| TFunction f ->
			List.iter loop f.f_args;
			loop f.f_ret
		| TMono ->
			t.id <- mk_id();
			t.t <- TPoly
	and loop_class c =
		fiter (fun f -> loop f.f_type) c.c_fields
	in
	loop t

type print_context = {
	mutable ids : (t * int) list;
	mutable cid : int;
}

let print_context() = 
	{
		ids = [];
		cid = 0;
	}

let context_id ctx t =
	try
		List.assq t ctx.ids
	with
		Not_found ->
			let k = ctx.cid in
			ctx.cid <- k + 1;
			ctx.ids <- (t,k) :: ctx.ids;
			k

let rec to_string ?(ctx = print_context()) ?(parent=false) ?(print_class=false) ?(n=1) t =
	let field_to_string s f =
		s ^ Printf.sprintf "\n%s%s : %s" 
			(String.make (n*2) ' ')
			f.f_name
			(to_string ~ctx ~n:(n+1) f.f_type)
	in
	match t.t with	
	| TPoly | TMono ->
		let n = context_id ctx t in
		if n < 26 then
			Printf.sprintf "'%c" (char_of_int (n + int_of_char 'a'))
		else
			Printf.sprintf "'%d" n
	| TTemplate s -> s
	| TAbbrev (t,[],v) -> s_type_path t
	| TAbbrev (t,pl,v) -> s_type_path t ^ "<" ^ String.concat ", " (List.map (to_string ~ctx ~n:(n+1)) pl) ^ ">"
	| TVoid -> "void"
	| TInt -> "int"
	| TFloat -> "float"
	| TNumber -> "number"
	| TBool -> "bool"
	| TLink t -> to_string ~ctx ~parent ~n ~print_class t
	| TLazy _ -> "<...>"
	| TFunction f -> 
		let s = (match f.f_args with
			| [] -> "void -> " ^ to_string ~ctx ~parent:true ~n f.f_ret
			| l -> String.concat " -> " (List.map (to_string ~ctx ~parent:true ~n) f.f_args) ^ " -> " ^ to_string ~ctx ~parent:true ~n f.f_ret)
		in
		if parent then "(" ^ s ^ ")" else s
	| TObject _ when n >= 5 -> "{...}"
	| TObject o ->
		let c = (match o.o_sup with None -> o.o_inf | Some c -> if o.o_inf.c_name <> None then o.o_inf else c) in
		let class_data() =
			let delta = String.make ((n-1)*2) ' ' in
			let vars = "{" in
			let vars = ffold field_to_string c.c_fields vars in
			vars ^ "\n" ^ delta ^ "}"
		in
		match c.c_name with
		| Some p ->
			if print_class then
				s_type_path p ^ " " ^ class_data()
			else
				s_type_path  p
		| None -> class_data()

let rec s_ttype ?(ctx=print_context()) ?(print_class=false) ?(n=1) t =
	let field_to_string s f =
		s ^ Printf.sprintf "\n%s%s : %s" 
			(String.make (n*2) ' ')
			f.f_name
			(s_ttype ~ctx ~n:(n+1) f.f_type)
	in
	match t.t with
	| TInt -> "int"
	| TFloat -> "float"
	| TNumber when !debug_id -> Printf.sprintf "number(%d)" t.id
	| TNumber -> "number"
	| TBool -> "bool"
	| TAbbrev (t,p,v) -> 
		(match v with Lower -> "-" | Upper -> "+" | Constraint -> "`") ^ s_type_path t ^
		(match p with [] -> "" | _ -> "<" ^ String.concat ", " (List.map (s_ttype ~ctx ~n:(n+1)) p) ^ ">")
	| TPoly when !debug_id -> Printf.sprintf "p(%d)" t.id
	| TMono when !debug_id -> Printf.sprintf "m(%d)" t.id
	| TTemplate s when !debug_id -> Printf.sprintf "t(%s:%d)" s t.id
	| TPoly -> 
		let n = context_id ctx t in
		if n < 26 then
			Printf.sprintf "'%c" (char_of_int (n + int_of_char 'a'))
		else
			Printf.sprintf "'%d" n
	| TMono ->
		let n = context_id ctx t in
		if n < 26 then
			Printf.sprintf "'_%c" (char_of_int (n + int_of_char 'a'))
		else
			Printf.sprintf "'_%d" n		
	| TTemplate s -> s
	| TLazy _ -> "<...>"
	| TVoid -> "void"
	| TLink t -> "~" ^ s_ttype ~ctx ~n ~print_class t
	| TFunction f -> 
		(match f.f_args with
		| [] -> "(void -> " ^ s_ttype ~ctx ~print_class ~n f.f_ret ^ ")"
		| l -> "(" ^ String.concat " -> " (List.map (s_ttype ~ctx~print_class ~n) f.f_args) ^ " -> " ^ s_ttype ~ctx ~print_class ~n f.f_ret ^ ")");
	| TObject _ when n >= 5 -> "{...}"
	| TObject o ->
		let class_data c inf =
			let delta = String.make ((n-1)*2) ' ' in
			let vars = if inf then "[-" else "[+" in
			let vars = ffold field_to_string c.c_fields vars in
			vars ^ "\n" ^ delta ^ "]"
		in
		let data = match o.o_inf.c_name with
			| None -> if fis_empty o.o_inf.c_fields then "" else class_data o.o_inf true
			| Some p -> "[-" ^ s_type_path p ^ ":" ^ string_of_int o.o_inf.c_id ^ "]" ^ if print_class then class_data o.o_inf true else ""
		in
		match o.o_sup with
		| None -> if data = "" then "empty" else data
		| Some c ->
			match c.c_name with
			| None -> data ^ class_data c false
			| Some p -> data ^ "[+" ^ s_type_path p ^ ":" ^ string_of_int c.c_id ^ "]" ^ if print_class then class_data c false else ""

let pt ?(recursive=false) s t =
	prerr_string (s ^ " ");
	prerr_endline (s_ttype ~print_class:recursive t)

let duplicate ?(keep_templates=false) ?(keep_numbers=false) ?(h=Hashtbl.create 5) ?(hc=Hashtbl.create 5) t =
	Hashtbl.add hc (-1) c_empty;
	let rec loop t =
		if t.id = -1 then
			t
		else match t.t with
		| TLink t2 -> loop t2
		| TNumber when keep_numbers -> t
		| TTemplate _ when keep_templates -> t
		| TLazy f -> f(); loop t
		| _ ->
			try
				Hashtbl.find h t.id
			with
				Not_found ->
					let nt = mk_t TVoid in					
					Hashtbl.add h t.id nt;
					nt.t <- (
						match t.t with
						| TFunction f ->
							TFunction { f_argnames = f.f_argnames; f_args = List.map loop f.f_args; f_ret = loop f.f_ret }
						| TObject o ->
							TObject { o_inf = loop_class o.o_inf; o_sup = (match o.o_sup with None -> None | Some c -> Some (loop_class c)) }
						| TNumber ->
							TNumber
						| TTemplate t ->
							nt.id <- -1;
							TMono
						| TPoly ->
							TPoly
						| TAbbrev (path,params,v) ->
							TAbbrev (path,List.map loop params,v)
						| _ ->
							assert false
					);
					nt

	and loop_class c =
		try
			Hashtbl.find hc c.c_id
		with
			Not_found ->
				let c2 = {
					c_fields = fempty;
					c_name = c.c_name;
					c_id = mk_id();
				} in
				Hashtbl.add hc c.c_id c2;
				c2.c_fields <- fmap (fun f ->
					{
						f_name = f.f_name;
						f_type = loop f.f_type;
					}
				) c.c_fields;
				c2
	in
	loop t

let apply_params c p t =
	if p = [] then
		t
	else
		let h = Hashtbl.create 0 in
		List.iter2 (fun (_,p) p2 -> Hashtbl.add h p.id p2) c.c_params p;
		duplicate ~h t

let rec expr_iter f e =
	try
		f e;
		let loop = expr_iter f in
		match e.texpr with
		| TVars vl -> List.iter (fun (_,_,v) -> match v with None -> () | Some e -> loop e) vl
		| TArrayDecl tl
		| TBlock tl -> List.iter loop tl
		| TFor (l1,l2,l3,e) -> 
			List.iter loop l1;
			List.iter loop l2;
			List.iter loop l3;
			loop e
		| TIf (e1,e2,eo) -> loop e1; loop e2; (match eo with None -> () | Some e -> loop e)
		| TWhile (e1,e2,_) | TTry (e1,_,_,e2) -> loop e1; loop e2
		| TSwitch (e,cases,eo) ->
			loop e;
			List.iter (fun (c,e) -> loop c; loop e) cases;
			(match eo with None -> () | Some e -> loop e)
		| TReturn eo ->
			(match eo with None -> () | Some e -> loop e)
		| TBreak 
		| TContinue
		| TConst _ ->
			()
		| TArray (e1,e2) 
		| TBinop (_,e1,e2) -> loop e1; loop e2
		| TUnop (_,_,e)
		| TField (e,_) -> loop e
		| TLocal _
		| TMember _
		| TClass _ -> ()
		| TParenthesis e -> loop e
		| TObjDecl fl -> List.iter (fun (_,e) -> loop e) fl
		| TCall (e,el) -> loop e; List.iter loop el
		| TNew (c,_,el) -> List.iter loop el
		| TDiscard e 
		| TLambda (_,e,_) -> loop e
		| TBuiltin b ->
			match b with
			| Cast e
			| Upcast e
			| Downcast e
			| Trace (e,_,_)
			| ToInt e
			| ToString e
			| PrintType e -> loop e
			| Callback (e,_,el) -> loop e; List.iter loop el
			| SuperConstructor -> ()
	with
		Exit -> ()
