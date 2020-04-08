open Type
open Typetools

let trans loop e =
	{ e with texpr = match e.texpr with
		| TVars vl -> TVars (List.map (fun (v,t,i) -> v, t, match i with None -> None | Some e -> Some (loop e)) vl)
		| TBlock tl -> TBlock (List.map loop tl)
		| TFor (inits,conds,incrs,e) -> TFor (List.map loop inits,List.map loop conds,List.map loop incrs,loop e)
		| TIf (e,e1,e2) -> TIf (loop e,loop e1, match e2 with None -> None | Some e2 -> Some (loop e2))
		| TWhile (e1,e2,f) -> TWhile(loop e1,loop e2,f)
		| TSwitch (e1,el,eo) -> TSwitch(loop e1,List.map (fun (e1,e2) -> loop e1, loop e2) el,match eo with None -> None | Some e -> Some (loop e))
		| TReturn eo -> TReturn (match eo with None -> None | Some e -> Some (loop e))
		| TBreak -> TBreak
		| TContinue -> TContinue
		| TConst c -> TConst c
		| TArray (e1,e2) -> TArray(loop e1,loop e2)
		| TBinop (op,e1,e2) -> TBinop(op,loop e1,loop e2)
		| TField (e,s) -> TField(loop e,s)
		| TLocal s -> TLocal s
		| TMember (s,cl) -> TMember (s,cl)
		| TClass c -> TClass c
		| TParenthesis e -> TParenthesis (loop e)
		| TObjDecl fl -> TObjDecl (List.map (fun (s,e) -> s , loop e) fl)
		| TArrayDecl tl -> TArrayDecl (List.map loop tl)
		| TCall (e,tl) -> TCall (loop e,List.map loop tl)
		| TNew (c,t,tl) -> TNew (c,t,List.map loop tl)
		| TUnop (op,f,e) -> TUnop (op,f,loop e)
		| TLambda (args,e,t) -> TLambda(args,loop e,t)
		| TDiscard e -> TDiscard (loop e)
		| TBuiltin b -> TBuiltin 
			(match b with
			| Cast e -> Cast (loop e)
			| Upcast e -> Upcast (loop e)
			| Downcast e -> Downcast (loop e)
			| Trace (e,f,l) -> Trace (loop e,f,l)
			| ToInt e -> ToInt (loop e)
			| ToString e -> ToString (loop e)
			| PrintType e -> PrintType (loop e)
			| Callback (e,f,tl) -> Callback (loop e,f,List.map loop tl)
			| SuperConstructor -> SuperConstructor)
	}

let find_outside_locals e =
	let h = Hashtbl.create 0 in
	let locals = ref [] in
	let rec loop e =
		match e.texpr with
		| TVars vl ->
			List.iter (fun (v,t,i) ->
				(match i with
				| None -> ()
				| Some e -> expr_iter loop e);
				locals := v :: !locals
			) vl;
			raise Exit
		| TBlock tl ->
			let old = !locals in
			List.iter (expr_iter loop) tl;
			locals := old;
			raise Exit
		| TLambda (args,e,_) ->
			let old = !locals in
			locals := List.fold_left (fun acc (a,_) -> a :: acc) !locals args;
			expr_iter loop e;
			locals := old;
			raise Exit
		| TLocal l ->
			if not (List.exists ((=) l) !locals) then Hashtbl.replace h l e.ttype;
		| _ ->
			()
	in
	expr_iter loop e;
	Hashtbl.fold (fun l t acc -> (l,t) :: acc) h []

let remove_lambdas genid cl e =
	let lambdas = ref [] in
	let ecl = mk_e (TClass cl) (mk_obj c_empty (Some cl.c_class)) null_pos in
	let rec loopt l t =
		match t.t with
		| TLink t -> loopt l t
		| TFunction f ->
			let names , types = List.split l in
			mk_t (TFunction {
				f_argnames = names @ f.f_argnames;
				f_args = types @ f.f_args;
				f_ret = f.f_ret
			})
		| _ ->			
			assert false
	in
	let rec loop e =
		match e.texpr with
		| TLambda (l,sube,ret) ->			
			let locals = find_outside_locals e in
			let ft = loopt locals e.ttype in
			let e = mk_e (TLambda (locals @ l,sube,ret)) ft e.tpos in
			let e = trans loop e in
			let id = genid() in
			cl.c_class_fields <- {
				cf_name = id;
				cf_type = ft;
				cf_static = true;
				cf_expr = Some e;
			} :: cl.c_class_fields;
			mk_e (TBuiltin (Callback (ecl,id,List.map (fun (l,t) -> mk_e (TLocal l) t e.tpos) locals))) e.ttype e.tpos
		| _ -> trans loop e
	in
	trans loop e

let unlambda t =
	let id = ref 0 in
	let genid() = incr id; "l" ^ string_of_int !id in
	let lambdas = {
		c_path = ([],"Lambdas");
		c_params = [];
		c_super = None;
		c_interfaces = [];
		c_class_fields = [];
		c_style = [];
		c_inst = c_empty;
		c_class = c_empty;
		c_pos = null_pos;
	} in
	let t = Typer.map_classes (fun cl ->
		{ cl with c_class_fields = 
			List.map (fun f ->
				{ f with cf_expr = (match f.cf_expr with
					| None -> None
					| Some e -> Some (remove_lambdas genid lambdas e)
				)}
			) cl.c_class_fields
		}
	) t in
	Typer.add_class t lambdas;
	t

		