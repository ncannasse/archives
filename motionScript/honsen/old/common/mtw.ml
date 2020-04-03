
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

let rec write_mtw ch = function
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
		List.iter (write_mtw ch) l
	| Ident tag ->
		if String.length tag <> 4 then raise (Invalid_tag tag);
		if type_tag tag <> TIdent then raise (Invalid_tag tag);
		IO.nwrite ch tag;
	| Number (tag,v) ->
		if String.length tag <> 4 then raise (Invalid_tag tag);
		if type_tag tag <> TNumber then raise (Invalid_tag tag);
		IO.nwrite ch tag;
		IO.write_real_i32 ch v

let write ch items =
	IO.nwrite ch "MTW2";
	let sz = List.fold_left calc_sizes 0 items in
	IO.write_i32 ch sz;
	List.iter (write_mtw ch) items

let read_tag ch =
	IO.nread ch 4

let rec read_mtw ch =
	let t = read_tag ch in
	match type_tag t with
	| TGroup ->
		let size = ref (IO.read_i32 ch) in
		let datas = ref [] in
		while !size > 0 do
			let x = read_mtw ch in
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

let read ch = 
	let h = read_tag ch in
	if h <> "MTW2" then raise Exit;
	let size = ref (IO.read_i32 ch) in
	let data = ref [] in
	while !size > 0 do
		let x = read_mtw ch in
		data := x :: !data;
		size := !size - calc_size x;
	done;
	if !size <> 0 then raise Exit;
	List.rev !data

let rec get_data tag it =
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
