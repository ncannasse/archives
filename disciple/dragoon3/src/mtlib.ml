(* **************************************************************************** *)
(*																				*)
(*		MTLib , Motion-Twin Ocaml Standard Library								*)
(*		(c)2002 Nicolas Cannasse												*)
(*		(c)2002 Motion Twin														*)
(*																				*)
(* **************************************************************************** *)
(* Globals *)

exception Global_not_initialized of string

module Global =
  struct

	type 'a t = ('a option ref) * string

	let empty name = (ref None,name)
	let name g = let _,name = g in name
	let set g letue = let curv,name = g in curv := Some letue
	let get g =		let curv,name = g in 
		match !curv with
		| None -> raise (Global_not_initialized name)
		| Some letue -> letue

	let undef g = let curv,_ = g in	curv := None

	let isdef g =
		let curv,_ = g in
		!curv = None

	let opt g =
		let curv,_ = g in
		!curv
  end

(* **************************************************************************** *)
(* Mutable List *)

exception Invalid_index of int
exception Invalid_operation

module MList =
  struct

	type 'a t = ('a list) ref

	let empty () = ref []
	let isempty x =
		match !x with
		| [] -> true
		| _ -> false

	let from_list l = ref l
	let to_list ml = !ml
	let copy ml l = ml := !l
	let copy_list ml l = ml := l
	

	let add ml item = ml := !ml@[item]
	let push ml item = ml := item::!ml

	let clear ml = ml := []

	let length ml = List.length !ml
	let hd ml = try List.hd !ml with _ -> raise Invalid_operation
	let tl ml = try List.tl !ml with _ -> raise Invalid_operation
	let iter f ml = List.iter f !ml
	let map f ml = ref (List.map f !ml)
	let find f ml = List.find f !ml
	let findex f ml exn = try List.find f !ml with _ -> raise exn
	let exists f ml = List.exists f !ml
	let sort func ml = ml := List.sort func !ml

	let last ml = try List.hd (List.rev !ml) with _ -> raise Invalid_operation

	let index_of ml item =
		let index = ref (-1) in
		List.find (fun it -> incr index; it = item; ) !ml;
		!index

	let index_of_with func ml =
		let index = ref (-1) in
		List.find (fun it -> incr index; func it; ) !ml;
		!index

	let at_index ml pos =
		try
			List.nth !ml pos
		with
			_ -> raise (Invalid_index pos)

	let set ml pos newitem =
		let p = ref (-1) in
		ml := List.map (fun item -> incr p; if !p = pos then newitem else item) !ml;
		if !p < pos || pos < 0 then raise (Invalid_index pos)

	let remove ml item =
		let rec del_aux = function			
			| x::l when x = item -> l
			| x::l -> x::(del_aux l)
			| [] -> raise Not_found
		in
			ml := del_aux !ml

	let remove_if f ml =
		let rec del_aux = function
			| x::l when (f x) -> l
			| x::l -> x::(del_aux l)
			| [] -> raise Not_found
		in
			ml := del_aux !ml
	
	let remove_at_index ml pos =
		let p = ref (-1) in
		let rec del_aux = function			
			| x::l -> incr p; if !p = pos then l else x::(del_aux l)
			| [] -> raise (Invalid_index pos)
		in
			ml := del_aux !ml

	let add_sort func ml item =
		let rec add_aux l =
			match l with
			| x::lnext ->
				let r = func x item in
				if r < 0 then item::l else x::(add_aux lnext)
			| [] -> [item]
		in
			ml := add_aux !ml

	let pop ml =
		match !ml with
		| [] -> raise Invalid_operation
		| e::l -> ml := l; e

	let npop ml n =		
		let rec pop_aux l n =
			if n = 0 then begin
				ml := l;
				[]
			end else
				match l with
				| [] -> raise Invalid_operation
				| x::l -> x::(pop_aux l (n-1))
		in
		pop_aux !ml n

	let shuffle ml =
		let a = Array.of_list !ml in
		let len = Array.length a in	
		for i = 0 to len-1 do
			let p = (Random.int (len-i))+i in
			let tmp = a.(p) in
			a.(p) <- a.(i);
			a.(i) <- tmp;
		done;
		ml := Array.to_list a

  end

(* **************************************************************************** *)
(* Extended Strings *)

module MString =
  struct

	let rsplit_char str ch =
		let p = String.rindex str ch in
			(String.sub str 0 p,String.sub str (p+1) ((String.length str)-p-1))

	let split_char str ch =
		let p = String.index str ch in
			(String.sub str 0 p,String.sub str (p+1) ((String.length str)-p-1))

	let find_sub str sub =
		let l = String.length sub in
		let found = ref None in
		(try
			for i = 0 to (String.length str)-l do
				if (String.sub str i l) = sub then begin
					found := Some i;
					raise Exit;
				end;
			done;		
		with Exit -> ());
		match !found with
		| None -> raise Not_found
		| Some p -> p

	let split str sep =			
		let p = find_sub str sep in
		let l = String.length sep in
			(String.sub str 0 p,String.sub str (p+l) ((String.length str)-p-l))

	let lchop str =
		let l = String.length str in
		match l with
		| 0 | 1 -> ""
		| _ ->
			String.sub str 1 (l-1)

	let rchop str =
		let l = String.length str in
		match l with
		| 0 | 1 -> ""
		| _ ->
			String.sub str 0 (l-1)

	let rec unconcat str sep =
		try
			let found,next = split str sep in
			found::(unconcat next sep)
		with
			Not_found ->
				match str with
				| "" -> []
				| _ -> [str]

  end

let ( =& ) a b = String.lowercase a = String.lowercase b

let itos = string_of_int
let ftos = string_of_float
let print = prerr_endline
let ftoi = int_of_float
let itof = float_of_int
let stoi = int_of_string
let stof = float_of_string

(* **************************************************************************** *)

type 'a tree =
	| Empty
	| Tree of ('a * ('a tree) ref * ('a tree) ref)

exception Already_Exists

module MBTree =
  struct

	type ('a,'b) t = (('a * 'b) tree) ref
	
	let empty() = ref Empty

	let rec add t key item =
		match !t with
		| Empty -> t := Tree((key,item),ref Empty,ref Empty)
		| Tree((k,_),tleft,tright) ->
			let r = compare k key in
				if r = 0 then raise Already_Exists else
				if r > 0 then add tleft key item else add tright key item

	let rec length t =
		match !t with
		| Empty -> 0
		| Tree(_,tl,tr) -> 1+(length tl)+(length tr)

	let rec depth t =
		match !t with
		| Empty -> 0
		| Tree(_,tl,tr) -> 1+(max (depth tl) (depth tr))

	let rec find t key =
		match !t with
		| Empty -> raise Not_found
		| Tree((k,i),tleft,tright) ->
			let r = compare k key in
			if r = 0 then i else
			if r > 0 then find tleft key else find tright key

	let rec remove t key =
		let p = ref None in
		let rec remove_aux t =
			match !t with
			| Empty -> raise Not_found
			| Tree((k,i),tleft,tright) ->
				let r = compare k key in
				if r = 0 then t else begin
					p := Some !t;
					if r > 0 then remove_aux tleft else remove_aux tright
				end
			in
		let found = remove_aux t in
			match !found with
			| Empty -> assert false
			| Tree((k,i),fleft,fright) ->
				let attach subtree = 
					(match !p with
					| None -> t := subtree
					| Some Empty -> assert false
					| Some (Tree(_,pleft,pright)) -> 
						if found == pleft then
							pleft := subtree
						else
							pright := subtree) in
				if !fright = Empty then attach !fleft
				else begin
					let lp = ref None in
					let rec mostleft_aux t =
						match !t with
						| Empty -> assert false
						| Tree(_,left,_) -> 
							if !left = Empty then t else begin
								lp := Some !t;
								mostleft_aux left;
							end
					in
						let mostleft = mostleft_aux fright in
						let mdata,mleft,mright =
							(match !mostleft with
							| Empty -> assert false
							| Tree infos -> infos) in
						(match !lp with
						| None ->
							attach (Tree(mdata,fleft,mright))
						| Some Empty -> assert false
						| Some (Tree(_,lpleft,lpright)) ->
							attach (Tree(mdata,fleft,fright));
							lpleft := !mright;
						)
				end
			

	let rec to_list t =
		match !t with
		| Empty -> []
		| Tree(data,tleft,tright) -> (to_list tleft)@(data::(to_list tright))

	let optimize t =
		let l = ref (to_list t) in
		let len = List.length !l in
		let rec opt_list len =
			if len = 0 then
				Empty 
			else if len = 1 then begin
				let t = Tree(List.hd !l,ref Empty,ref Empty) in
				l := List.tl !l;
				t
			end else begin
				let left = opt_list (len/2) in
				let item = List.hd !l in
				l := List.tl !l;
				let right = opt_list ((len-1)/2) in
				Tree(item,ref left,ref right)
			end in
			t := opt_list len	

	let rec iter func t =
		match !t with
		| Empty -> ()
		| Tree((key,item),left,right) ->
			iter func left;
			func key item;
			iter func right

	let rec find_p func t =
		match !t with
		| Empty -> raise Not_found
		| Tree((key,item),left,right) ->
			if func key item then (key,item)
			else
				(try
					find_p func left;
				with
					Not_found -> find_p func right)

	let copy bto from = bto := !from

	let clear t = t := Empty

  end

(* **************************************************************************** *)
(* Misc *)

let read_all_file f =
	let total = ref "" 
	and str = String.create 1024
	and len = ref 0 in
	try
		while true do
			len := input f str 0 1024;
			if !len = 0 then raise Exit;
			if !len <> 1024 then
				total := (!total)^(String.sub str 0 !len)
			else
				total := (!total)^str;
		done;
		!total;
	with Exit -> !total

let fnot f x = not(f x)
let fall _ = true

let fabs x = if x < 0. then x *. -1. else x

let ( += ) r x = r := !r + x
let ( -= ) r x = r := !r - x
let ( +=. ) r x = r := !r +. x
let ( -=. ) r x = r := !r -. x

(* **************************************************************************** *)
