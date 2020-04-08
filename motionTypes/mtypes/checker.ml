open Type
open Expr

type error_msg =
	| Invalid_expression
	| Local_override of string
	| Duplicate_field of string
	| Missing_return
	| Not_initialized of string

exception Error of error_msg * pos

let error_msg = function
	| Invalid_expression -> "Invalid expression"
	| Local_override s -> "Local variable overridden " ^ s
	| Duplicate_field s -> "Duplicate field " ^ s
	| Missing_return -> "Every path must returns a value"
	| Not_initialized name -> "Variable " ^ name ^ " not initialized"

let error m p = raise (Error (m,p))

type local = {
	l_pos : pos;
	l_frame : int;
	l_fun : func option;
	l_is_var_member : bool;
}

type context = {
	mutable has_return : bool;
	mutable in_fun : bool;
	mutable cur_fun : func option;
	mutable in_class : bool;
	mutable in_intrinsic : bool;
	mutable frame : int;
	mutable locals : (string,local) Hashtbl.t;
}

type vars = {
	vdone : string list;
	vtodo : string list;
}

let context() = {
	in_fun = false;
	has_return = false;
	cur_fun = None;
	in_intrinsic = false;
	in_class = false;
	frame = 0;
	locals = Hashtbl.create 0;
}

let local ctx p vname member =
	try
		let l = Hashtbl.find ctx.locals vname in
		if l.l_fun <> ctx.cur_fun then raise Not_found;
		if ctx.cur_fun = None then error (Duplicate_field vname) p else error (Local_override vname) p
	with
		Not_found ->
			Hashtbl.add ctx.locals vname { l_pos = p; l_fun = ctx.cur_fun; l_frame = ctx.frame; l_is_var_member = member }

let new_frame ctx =
	let f = ctx.frame in
	ctx.frame <- ctx.frame + 1;
	f

let clean_frame ctx f =
	Hashtbl.iter (fun lname l ->
		if l.l_frame > f then Hashtbl.remove ctx.locals lname;
	) ctx.locals

let rec check_returns l = 
	match l with
	| [] -> false
	| (e,p) :: l ->
		let rec has_break (e,p) =
			match e with
			| EClass _
			| EInterface _
			| EVars _ 
			| EImport _
			| EFunction _
			| EFor _ 
			| EContinue 
			| EVal _
			| EWhile _
			| ESwitch _
			| EReturn _ ->
				false
			| EBlock el ->
				List.exists has_break el
			| EIf (_,e,None) ->
				has_break e
			| EIf (_,e,Some e2)
			| ETry (e,_,_,e2) ->
				has_break e || has_break e2
			| EBreak ->
				true
		in
		match e with
		| EClass _
		| EInterface _
		| EVars _ 
		| EImport _
		| EFunction _
		| EFor _
		| EBreak
		| EContinue
		| EVal _ ->
			check_returns l
		| EBlock el ->
			check_returns el || check_returns l
		| EIf (_,e,Some e2)
		| ETry (e,_,_,e2) ->
			(check_returns [e] && check_returns [e2]) || check_returns l
		| EWhile ((EParenthesis (EConst (Ident "true"),_),_),e,_)
		| EWhile ((EConst (Ident "true"),_),e,_) ->
			if not (has_break e) then true else check_returns l
		| EIf _
		| EWhile _ ->
			check_returns l
		| ESwitch (_,el,eo) ->
			(match eo with
			| None -> check_returns l
			| Some de ->
				let rets = List.for_all (fun (_,e) -> check_returns [e]) el in
				(rets && check_returns [de]) || check_returns l);		
		| EReturn _ ->
			true

let rec check_valid ctx (e,p) =
	let error p = error Invalid_expression p in
	match ctx.in_class , ctx.in_fun with
	| false , false ->
		(match e with
		| EImport _
		| EClass _ 
		| EInterface _
		| EFunction _ -> ()
		| _ -> error p)
	| true , false ->
		(match e with
		| EFunction _ 
		| EVars _
		| EBlock _ -> ()
		| _ -> error p)
	| _ , true ->
		let rec loop_val (v,p) = 
			match v with
			| EBinop (OpAssign,_,_)
			| EBinop (OpAssignOp _,_,_)
			| ECall _
			| EUnop (Increment,_,_)
			| EUnop (Decrement,_,_) -> 
				()
			| _ ->				
				error p
		in
		let rec loop (e,p) =
			match e with
			| EClass _ 
			| EImport _
			| EInterface _ ->
				error p
			| EFunction f ->
				(match f.fexpr with
				| None -> ()
				| Some e ->
					let oldf = ctx.in_fun in
					ctx.in_fun <- true;
					check_valid ctx e;
					ctx.in_fun <- oldf)
			| EBlock el -> List.iter loop el
			| EFor (el,_,_,e) -> List.iter loop (e::el)
			| EIf (_,e,e2) -> 
				loop e;
				(match e2 with None -> () | Some e2 -> loop e2)
			| ESwitch (_,el,eo) ->
				List.iter (fun (_,e) -> loop e) el;
				(match eo with None -> () | Some e -> loop e);
			| ETry (e,_,_,e2) -> 
				loop e;
				loop e2;
			| EWhile (_,e,_) ->
				loop e
			| EReturn _ 
			| EBreak
			| EContinue
			| EVars _ ->
				()
			| EVal v ->
				loop_val v
		in
		loop (e,p)

let vars_intersection v1 v2 =
	let h = Hashtbl.create 0 in
	List.iter (fun v -> Hashtbl.add h v ()) v1.vtodo;
	(* union *)
	let vtodo = List.fold_left (fun acc v ->
		if Hashtbl.mem h v then
			acc
		else
			v :: acc
	) v1.vtodo v2.vtodo in
	let h = Hashtbl.create 0 in
	List.iter (fun v -> Hashtbl.add h v ()) v1.vdone;
	(* inter *)
	let vdone = List.fold_left (fun acc v ->
		if Hashtbl.mem h v then
			v :: acc
		else
			acc
	) [] v2.vdone in
	{
		vtodo = vtodo;
		vdone = vdone;
	}



let rec check_val_vars vars = function
	| [] -> vars
	| (v,p) :: l ->
		let vars = (
			match v with
			| ELambda f ->
				vars
			| EArray (v1,v2) -> check_val_vars vars [v2;v1]
			| EArrayDecl vl -> check_val_vars vars vl
			| EBinop (OpAssign,(EConst (Ident name),_),v) ->
				let vars = check_val_vars vars [v] in
				if List.exists ((=) name) vars.vtodo then 
					{
						vdone = name :: vars.vdone;
						vtodo = List.filter (( <> ) name) vars.vtodo;
					}
				else begin
					let old_name = name in
					let name = "this." ^ name in
					if List.exists ((=) name) vars.vtodo && not (List.exists ((=) old_name) vars.vdone) then
					{
						vdone = name :: vars.vdone;
						vtodo = List.filter (( <> ) name) vars.vtodo;
					}
					else
						vars
				end
			| EBinop (OpAssign,(EField ((EConst (Ident "this"),_),name),_),v) ->
				let vars = check_val_vars vars [v] in
				let name = "this." ^ name in
				if List.exists ((=) name) vars.vtodo then
				{
					vdone = name :: vars.vdone;
					vtodo = List.filter (( <> ) name) vars.vtodo;
				}
				else
					vars
			| EBinop (_,v1,v2) -> check_val_vars vars [v2;v1]
			| EParenthesis v -> check_val_vars vars [v]
			| EObjDecl fl -> check_val_vars vars (List.map snd fl)
			| ECall (v,vl) -> check_val_vars vars (v :: vl)
			| ENew (_,_,vl) -> check_val_vars vars vl
			| EUnop (_,_,v) -> check_val_vars vars [v]
			| EQuestion (v1,v2,v3) -> 
				let vars = check_val_vars vars [v1] in
				let v1 = check_val_vars vars [v2] in
				let v2 = check_val_vars vars [v3] in
				vars_intersection v1 v2
			| EStatic _ ->
				vars
			| EField ((EConst (Ident "this"),_),name) ->
				let name = "this." ^ name in
				if List.exists (( = ) name) vars.vtodo then error (Not_initialized name) p;
				vars
			| EField (v,_) -> check_val_vars vars [v]
			| EConst c ->
				match c with				
				| Ident name ->
					if List.exists (( = ) name) vars.vtodo then error (Not_initialized name) p;
					vars
				| Int _
				| Float _
				| Name _
				| String _ ->
					vars
		) in
		check_val_vars vars l

let rec check_expr_vars vars = function
	| [] -> vars
	| (e,p) :: l ->
		let vars = (
			match e with
			| EClass _ 
			| EInterface _
			| EImport _
			| EFunction _ -> vars
			| EVars (_,vl) ->
				(* no need to remove from vdone since we don't allow local overriding *)
				let vtodo = List.fold_left (fun acc (v,_,ve) -> match ve with None -> v :: acc | Some _ -> acc) vars.vtodo vl in
				{
					vdone = vars.vdone;
					vtodo = vtodo;
				}
			| EBlock el ->
				check_expr_vars vars el
			| EFor (inits,conds,incrs,e) ->
				let vars = check_expr_vars vars inits in
				let vars = check_val_vars vars conds in
				ignore(check_expr_vars vars [e]);
				vars
			| EIf (v,e,e2) ->
				let vars = check_val_vars vars [v] in
				(match e2 with
				| None -> ignore(check_expr_vars vars [e]); vars
				| Some e2 ->
					let v1 = check_expr_vars vars [e] in
					let v2 = check_expr_vars vars [e2] in
					vars_intersection v1 v2);
			| EWhile (v,e,flag) ->
				(match flag with 
				| NormalWhile -> 
					let vars = check_val_vars vars [v] in
					ignore(check_expr_vars vars [e]);
					vars
				| DoWhile -> 
					let vars = check_expr_vars vars [e] in
					check_val_vars vars [v]
				);
			| ESwitch (v,el,eo) ->
				let vars = check_val_vars vars [v] in
				(match eo with
				| None -> List.iter (fun (_,e) -> ignore(check_expr_vars vars [e])) el; vars
				| Some de ->
					let vars2 = vars_intersection (check_expr_vars vars [de]) vars in
					List.fold_left (fun acc (_,e) -> vars_intersection acc (check_expr_vars vars [e])) vars2 el
				);
			| ETry (e,s,_,e2) ->
				let v1 = check_expr_vars vars [e] in
				let v2 = check_expr_vars vars [e2] in
				vars_intersection v1 v2
			| EReturn e ->
				(match e with
				| None -> vars
				| Some v -> check_val_vars vars [v]);
			| EBreak
			| EContinue ->
				vars
			| EVal v ->	
				check_val_vars vars [v]
		) in
		check_expr_vars vars l

let rec get_constructor name (e,p) =
	match e with
	| EVars _ -> None
	| EBlock el -> 
		let rec loop = function
			| [] -> None
			| e :: l ->
				match get_constructor name e with
				| None -> loop l
				| Some ec -> Some ec
		in
		loop el
	| EFunction f ->
		if f.fname = name then Some f else None
	| _ ->
		assert false

let check_val ctx v = ()

let rec check ?(frame=true) ctx (e,p) =
	check_valid ctx (e,p);
	match e with
	| EImport path ->
		()
	| EInterface (path,_,e) ->
		if ctx.in_class || ctx.in_fun then error Invalid_expression p;
		ctx.in_class <- true;
		check ctx e;
		ctx.in_class <- false;
	| EClass (path,_,HNative :: herits,e) ->
		if ctx.in_class || ctx.in_fun then error Invalid_expression p;
		let old_locals = ctx.locals in
		ctx.locals <- Hashtbl.create 0;
		ctx.in_class <- true;
		ctx.in_intrinsic <- true;
		check ctx e;
		ctx.in_class <- false;
		ctx.in_intrinsic <- false;
		ctx.locals <- old_locals
	| EClass (path,_,herits,e) ->
		if ctx.in_class || ctx.in_fun then error Invalid_expression p;
		ctx.in_class <- true;
		let f = new_frame ctx in
		check ctx ~frame:false e;
		clean_frame ctx f;
		ctx.in_class <- false;
	| EVars (stat,vl) ->
		if ctx.in_fun && stat = IsStatic then error Invalid_expression p;
		List.iter (fun (vname,_,vexpr) -> 
			match vexpr with
			| None -> 
				local ctx p vname (ctx.in_fun = false)
			| Some e -> 
				if not ctx.in_fun && stat <> IsStatic then error Invalid_expression (pos e);
				check_val ctx e;
				local ctx p vname false;
		) vl
	| EFunction f ->
		local ctx p f.fname false;
		let oldf = ctx.cur_fun in
		let fr = new_frame ctx in
		let hasreturn = ctx.has_return in
		ctx.cur_fun <- Some f;
		ctx.in_fun <- true;
		ctx.has_return <- false;
		List.iter (fun (arg,_) -> local ctx p arg false) f.fargs;
		(match f.fexpr with
		| None -> ()
		| Some e ->
			check ctx e;
			if ctx.has_return && not (check_returns [e]) then error Missing_return p;
			ignore(check_expr_vars { vtodo = []; vdone = List.map fst f.fargs } [e]);
		);
		clean_frame ctx fr;
		ctx.in_fun <- (oldf <> None);
		ctx.cur_fun <- oldf;
		ctx.has_return <- hasreturn;
	| EBlock el ->
		if frame then begin
			let f = new_frame ctx in
			List.iter (check ctx) el;
			clean_frame ctx f;
		end else
			List.iter (check ctx) el;
	| EFor (inits,conds,incrs,e) ->
		let f = new_frame ctx in 
		List.iter (check ctx) inits;
		List.iter (check_val ctx) conds;
		List.iter (check_val incrs) incrs;
		check ctx e;
		clean_frame ctx f;
	| EIf (v,e,e2) ->
		check_val ctx v;
		check ctx e;
		(match e2 with None -> () | Some e2 -> check ctx e2);
	| EWhile (v,e,flag) ->
		(match flag with 
		| NormalWhile -> check_val ctx v; check ctx e;
		| DoWhile -> check ctx e; check_val ctx v);
	| ESwitch (v,el,eo) ->
		check_val ctx v;
		List.iter (fun (_,e) -> check ctx e) el;
		(match eo with
		| None -> ()
		| Some e ->
			check ctx e);
	| EReturn e ->
		(match e with
		| None -> ()
		| Some v -> 
			check_val ctx v;
			ctx.has_return <- true);
	| ETry (e,s,_,e2) ->
		check ctx e;
		let f = new_frame ctx in
		local ctx p s false;
		check ctx e2;
		clean_frame ctx f
	| EBreak
	| EContinue ->
		()
	| EVal v ->	
		check_val ctx v
