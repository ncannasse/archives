
type tag =
	| TData
	| TGroup
	| TIdent
	| TNumber

type t =
	| Data of string * string
	| Group of string * t list
	| Ident of string
	| Number of string * int32

type file = t list

let rec calc_size = function
	| Data (_,data) -> 4 + 4 + String.length data
	| Group (_,l) -> List.fold_left calc_sizes (4 + 4) l
	| Ident _ -> 4
	| Number (_,_) -> 4 + 4
and
	calc_sizes sz it = sz + calc_size it

let size = calc_size

exception Invalid_tag of string

let is_upper = function
	| 'A'..'Z' | '0'..'9' -> true
	| _ -> false

let is_lower = function
	| 'a'..'z' -> true
	| _ -> false

let type_tag tag = 
	if is_upper tag.[0] && is_upper tag.[1] && is_upper tag.[2] && is_upper tag.[3] then
		TData
	else if is_upper tag.[0] && is_lower tag.[1] && is_lower tag.[2] && is_lower tag.[3] then
		TGroup
	else if is_lower tag.[0] && is_upper tag.[1] && is_upper tag.[2] && is_upper tag.[3] then
		TIdent
	else if is_lower tag.[0] && is_lower tag.[1] && is_lower tag.[2] && is_lower tag.[3] then
		TNumber
	else
		failwith ("Invalid tag "^tag)

let tag = function
	| Data (t,_)
	| Group (t,_)
	| Ident t
	| Number (t,_) ->
		t

let rec write ch = function
	| Data (tag,data) ->
		if String.length tag <> 4 then raise (Invalid_tag tag);
		if type_tag tag <> TData then raise (Invalid_tag tag);
		IO.nwrite ch tag;
		IO.write_i32 ch (String.length data);
		IO.nwrite ch data
	| Group (tag,l) ->
		if String.length tag <> 4 then raise (Invalid_tag tag);
		if type_tag tag <> TGroup then raise (Invalid_tag tag);
		IO.nwrite ch tag;
		IO.write_i32 ch (List.fold_left calc_sizes 0 l);
		List.iter (write ch) l
	| Ident tag ->
		if String.length tag <> 4 then raise (Invalid_tag tag);
		if type_tag tag <> TIdent then raise (Invalid_tag tag);
		IO.nwrite ch tag;
	| Number (tag,v) ->
		if String.length tag <> 4 then raise (Invalid_tag tag);
		if type_tag tag <> TNumber then raise (Invalid_tag tag);
		IO.nwrite ch tag;
		IO.write_real_i32 ch v

let read_tag ch =
	IO.nread ch 4

let rec read ch =
	let t = read_tag ch in
	match type_tag t with
	| TGroup ->
		let size = ref (IO.read_i32 ch) in
		let datas = ref [] in
		while !size > 0 do
			let x = read ch in
			datas := x :: !datas;
			size := !size - calc_size x;
		done;
		Group (t,List.rev !datas)
	| TData ->
		let size = IO.read_i32 ch in
		Data (t,IO.nread ch size)
	| TIdent ->
		Ident t
	| TNumber ->
		Number (t,IO.read_real_i32 ch)

let get_data tag it =
	match it with
	| Group (_,items) ->
		(match List.find (function Data(t,_) -> t = tag | _ -> false) items with
		| Data (_,data) -> data
		| _ -> assert false);
	| _ ->
		raise Not_found

let get_data_rec tag it =
	let found_data = ref "" in
	let rec loop = function
		| Data (t,data) when t = tag -> found_data := data; true
		| Group (_,items) -> List.exists loop items
		| Data _
		| Ident _ 
		| Number _ ->
			false
	in
	match loop it with
	| false -> raise Not_found
	| true -> !found_data
