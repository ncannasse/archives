open Expr

type t_field = {
	f_name : string;
	f_type : ttype;
	f_pos : pos;
	mutable f_typed : bool;
}

and fields = (string,t_field) PMap.t

and t_class = {
	mutable c_fields : fields;
	mutable c_privates : fields;
	mutable c_static : t_class;
	mutable c_super : t_class;
	c_name : type_path option;
	c_interface : bool;
	c_id : int;
}

and t_function = {
	f_args : ttype list;
	f_ret : ttype;
	f_nargs : int;
}

and t_object = {
	mutable o_inf : t_class;
	o_sup : t_class option;
}

and ttype_decl =
	| TLink of ttype
	| TObject of t_object
	| TObjLazy of t_object
	| TFunction of t_function
	| TPoly of int
	| TMono of int
	| THash of ttype
	| TInt
	| TFloat
	| TNumber
	| TVoid
	| TBool

and ttype = {
	mutable t : ttype_decl;
	mutable id : int;
}

let id_count = ref 0

let mk_id() =
	incr id_count;
	!id_count

let mk_type t = {
	t = t;
	id = (incr id_count; !id_count);
}

let t_void = {
	t = TVoid;
	id = -1;
}

let t_int = {
	t = TInt;
	id = -1;
}

let t_float = {
	t = TFloat;
	id = -1;
}

let t_bool = {
	t = TBool;
	id = -1;
}

let immutable t =
	t.id = -1 && (match t.t with TMono _ -> false | _ -> true)

let t_number() = mk_type TNumber

let rec c_empty = { c_name = None; c_fields = PMap.empty; c_privates = PMap.empty; c_static = c_empty; c_interface = false; c_super = c_empty; c_id = -1 }

let rec is_recursive t t2 =
	if t2 == t then 
		true
	else match t2.t with
	| TLink t2 -> is_recursive t t2
	| TFunction f ->
		List.exists (is_recursive t) (f.f_ret :: f.f_args);
	| THash t2 -> is_recursive t t2
	| TObjLazy _
	| TObject _
	| TPoly _
	| TMono _
	| TInt
	| TFloat
	| TNumber
	| TVoid
	| TBool ->
		false

let poly_count = ref (-1)

let mk_poly() = 
	incr poly_count;
	mk_type (TPoly !poly_count)

let mk_mono() = 
	incr poly_count;
	{
		t = TMono !poly_count;
		id = -1;
	}

let t_hash() = mk_type (THash (mk_mono()))

let mk_fun args rt = 
	let f = {
		f_args = args;
		f_ret = rt;
		f_nargs = List.length args;
	} in
	{
		t = TFunction f;
		id = if List.for_all immutable (rt :: args) then -1 else mk_id();
	}

let mk_obj inf sup = 
	{
		t = TObject { o_inf = inf; o_sup = sup };
		id = mk_id(); (* TObject always mutable *)
	}

let mk_obj_lazy inf sup = 
	{
		t = TObjLazy { o_inf = inf; o_sup = sup };
		id = mk_id(); (* TObject always mutable *)
	}

let rec type_iter ?(in_classes=false) ?(h=Hashtbl.create 0) f t =
	f t;
	let iter_class c =
		if not (Hashtbl.mem h c) then begin
			Hashtbl.add h c ();
			PMap.iter (fun _ fi -> type_iter ~h f fi.f_type) c.c_fields
		end;
	in
	match t.t with
	| TLink t -> type_iter ~h f t
	| TObject o | TObjLazy o when in_classes ->
		(match o.o_sup with
		| None -> iter_class o.o_inf;
		| Some c -> iter_class c);
	| TFunction func ->
		List.iter (type_iter ~h f) func.f_args;
		type_iter ~h f func.f_ret
	| THash t -> type_iter ~h f t
	| TObjLazy _
	| TObject _
	| TMono _
	| TPoly _
	| TInt
	| TFloat
	| TNumber
	| TVoid
	| TBool ->
		()

let begin_mark() = ref 1

let mark m t =
	type_iter ~in_classes:true (fun t ->
		match t.t with
		| TPoly n when n > 0 -> 
			t.t <- TPoly (- !m);
			incr m
		| TMono n when n > 0 ->
			t.t <- TMono (- !m);
			incr m
		| _ ->
			()
	) t
	
let end_mark t =
	type_iter ~in_classes:true (fun t ->
		match t.t with
		| TPoly n when n < 0 ->
			t.t <- TPoly (-(n+1))
		| TMono n when n < 0 ->
			t.t <- TMono (-(n+1))
		| _ -> ()
	) t

let rec to_string ?(print_class=false) ?(n=1) t =
	let field_to_string super pub stat _ f s =
		if PMap.mem f.f_name super.c_fields || (super.c_static != super && PMap.mem f.f_name super.c_static.c_fields) then
			s 
		else
		s ^ Printf.sprintf "\n%s%s%s%s : %s" 
			(String.make (n*2) ' ')
			(if pub = IsPrivate then "private " else "public ") 
			(if stat = IsStatic then "static " else "")
			f.f_name
			(to_string ~n:(n+1) f.f_type)
	in
	match t.t with
	| TPoly _ -> "any"
	| TMono _ -> "undef"
	| TInt -> "int"
	| TFloat -> "float"
	| TNumber -> "number"
	| TVoid -> "void"
	| TBool -> "bool"
	| THash t -> to_string ~n ~print_class t ^ " hash"
	| TLink t -> to_string ~n ~print_class t
	| TFunction f -> 
		(match f.f_args with
		| [] -> "(void -> " ^ to_string ~n f.f_ret ^ ")"
		| l -> "(" ^ String.concat " -> " (List.map (to_string ~n) f.f_args) ^ " -> " ^ to_string ~n f.f_ret ^ ")");
	| TObject o | TObjLazy o ->
		let c = (match o.o_sup with None -> o.o_inf | Some c -> if o.o_inf.c_name <> None then o.o_inf else c) in
		let class_data() =
			let delta = String.make ((n-1)*2) ' ' in
			let vars = "{" in
			let vars = PMap.foldi (field_to_string c.c_super IsPublic IsStatic) c.c_static.c_fields vars in
			let vars = PMap.foldi (field_to_string c.c_super IsPrivate IsStatic) c.c_static.c_privates vars in
			let vars = PMap.foldi (field_to_string c.c_super IsPublic IsMember) c.c_fields vars in
			let vars = PMap.foldi (field_to_string c.c_super IsPrivate IsMember) c.c_privates vars in
			vars ^ "\n" ^ delta ^ "}"
		in
		match c.c_name with
		| Some p ->
			if print_class then
				s_type_path p ^ " " ^ class_data()
			else
				s_type_path  p
		| None -> class_data()

let rec s_ttype ?(print_class=false) ?(n=1) t =
	let field_to_string pub stat _ f s =
		s ^ Printf.sprintf "\n%s%s%s%s : %s" 
			(String.make (n*2) ' ')
			(if pub = IsPrivate then "private " else "public ") 
			(if stat = IsStatic then "static " else "")
			f.f_name
			(s_ttype ~n:(n+1) f.f_type)
	in
	match t.t with
	| TPoly n when n < 26 -> Printf.sprintf "'%c" (char_of_int (n + int_of_char 'a'))
	| TPoly n -> Printf.sprintf "'%d" n
	| TMono n when n < 26 -> Printf.sprintf "'_%c" (char_of_int (n + int_of_char 'a'))
	| TMono n -> Printf.sprintf "'_%d" n
	| TInt -> "int"
	| TFloat -> "float"
	| TNumber -> "number"
	| TVoid -> "void"
	| TBool -> "bool"
	| THash t -> s_ttype ~n ~print_class t ^ " hash"
	| TLink t -> "~" ^ s_ttype ~n ~print_class t
	| TFunction f -> 
		(match f.f_args with
		| [] -> "(void -> " ^ s_ttype ~n f.f_ret ^ ")"
		| l -> "(" ^ String.concat " -> " (List.map (s_ttype ~n) f.f_args) ^ " -> " ^ s_ttype ~n f.f_ret ^ ")");
	| TObject o | TObjLazy o ->
		let class_data c inf =
			let delta = String.make ((n-1)*2) ' ' in
			let vars = if inf then "[-" else "[+" in
			let vars = PMap.foldi (field_to_string IsPublic IsStatic) c.c_static.c_fields vars in
			let vars = PMap.foldi (field_to_string IsPrivate IsStatic) c.c_static.c_privates vars in
			let vars = PMap.foldi (field_to_string IsPublic IsMember) c.c_fields vars in
			let vars = PMap.foldi (field_to_string IsPrivate IsMember) c.c_privates vars in
			vars ^ "\n" ^ delta ^ "]"
		in
		let data = match o.o_inf.c_name with
			| None -> if PMap.is_empty o.o_inf.c_fields then "" else class_data o.o_inf true
			| Some p -> "[-" ^ s_type_path p ^ "]" ^ if print_class then class_data o.o_inf true else ""
		in
		match o.o_sup with
		| None -> if data = "" then "empty" else data
		| Some c ->
			match c.c_name with
			| None -> data ^ class_data c false
			| Some p -> data ^ "[+" ^ s_type_path p ^ "]" ^ if print_class then class_data c false else ""

let pt s t =
	prerr_endline (s ^ " " ^ (s_ttype ~print_class:true t))

let duplicate t =
	let h = Hashtbl.create 0 in
	let hc = Hashtbl.create 0 in
	let rec loop t = 
		if t.id = -1 then
			t
		else
			try
				Hashtbl.find h t.id
			with
				Not_found ->
					let nt = mk_type TVoid in
					Hashtbl.add h t.id nt;
					nt.t <- (match t.t with
						| TLink t -> TLink (loop t)
						| TObject o | TObjLazy o -> TObject { o_inf = loop_class o.o_inf; o_sup = (match o.o_sup with None -> None | Some c -> Some (loop_class c)) }
						| TFunction f -> TFunction { f_args = List.map loop f.f_args; f_ret = loop f.f_ret; f_nargs = f.f_nargs }
						| TPoly n -> TPoly n
						| TNumber -> TNumber
						| THash t -> THash (loop t)
						| TInt | TFloat | TVoid | TBool | TMono _ -> assert false
					);
					nt
	and loop_class c = 
		if c.c_id = -1 then 
			c
 		else
			try
				Hashtbl.find hc c.c_id
			with
				Not_found ->
					let cc = {
						c_name = c.c_name;
						c_fields = PMap.empty;
						c_privates = PMap.empty;
						c_static = c_empty;
						c_interface = c.c_interface;
						c_super = c_empty;
						c_id = mk_id();
					} in
					Hashtbl.add hc c.c_id cc;
					cc.c_fields <- PMap.map loop_field c.c_fields;
					cc.c_privates <- PMap.map loop_field c.c_privates;
					cc.c_static <- if c.c_static == c then cc else loop_class c.c_static;
					cc.c_super <- loop_class c.c_super;
					cc
	and loop_field f =
		if f.f_typed then
			{
				f_name = f.f_name;
				f_type = loop f.f_type;
				f_pos = f.f_pos;
				f_typed = false;
			}
		else
			f
	in
	loop t

let duplicate_class c =
	let t = duplicate (mk_type (TObject { o_inf = c; o_sup = None })) in
	match t.t with
	| TObject o -> o.o_inf
	| _ -> assert false

let rec contravariant t =
	match t.t with
	| TLink t -> contravariant t
	| TFunction f -> List.iter contravariant f.f_args; contravariant f.f_ret
	| TObject o ->
		(match o.o_sup with
		| None -> ()
		| Some c ->
			t.t <- TObject { o_inf = c; o_sup = None })
	| TObjLazy o ->
		(match o.o_sup with
		| None -> ()
		| Some c ->
			t.t <- TObjLazy { o_inf = c; o_sup = None })
	| _ ->
		()

let rec covariant t =
	match t.t with
	| TLink t -> covariant t
	| TFunction f -> List.iter covariant f.f_args; covariant f.f_ret
	| TObject o when o.o_sup = None ->
		if o.o_inf != c_empty then t.t <- TObject { o_inf = c_empty; o_sup = Some o.o_inf }
	| TObjLazy o when o.o_sup = None ->
		if o.o_inf != c_empty then t.t <- TObjLazy { o_inf = c_empty; o_sup = Some o.o_inf }
	| TObject o ->
		t.t <- TObject { o_inf = c_empty; o_sup = o.o_sup }
	| TObjLazy o ->
		t.t <- TObjLazy { o_inf = c_empty; o_sup = o.o_sup }
	| _ ->
		()

let rec cofree t =
	match t.t with
	| TLink t -> cofree t
	| TObject o ->
		t.t <- TObject (match o.o_sup with
			| None -> { o_inf = c_empty; o_sup = (if o.o_inf == c_empty then None else Some o.o_inf) }
			| Some _ -> { o_inf = c_empty; o_sup = o.o_sup })
	| TObjLazy o when o.o_sup <> None ->
		t.t <- TObjLazy (match o.o_sup with
			| None -> { o_inf = c_empty; o_sup = (if o.o_inf == c_empty then None else Some o.o_inf) }
			| Some _ -> { o_inf = c_empty; o_sup = o.o_sup })
	| _ ->
		()
