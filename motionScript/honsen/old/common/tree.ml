
type 'a tree = {
	mutable tdata : 'a;
	mutable tparent : 'a tree option;
	mutable tchilds : 'a tree list;
	mutable tinfos : 'a t option;
}
and 'a t = {
	mutable tsort : ('a -> 'a -> int) option;
	mutable troot : 'a tree;
}

exception Recursive_insert

let sort t f = 
	t.tsort <- Some f;
	let rec loop t =
		List.iter loop t.tchilds;
		t.tchilds <- List.sort (fun t1 t2 -> f t1.tdata t2.tdata) t.tchilds
	in
	loop t.troot

let root t =
	t.troot

let rec rec_update tinf t =
	match t.tinfos with
	| Some tinf2 when tinf == tinf2 ->
		()
	| _ ->
		t.tinfos <- Some tinf;
		List.iter (rec_update tinf) t.tchilds;
		match tinf.tsort with
		| None -> ()
		| Some f ->
			t.tchilds <- List.sort (fun t1 t2 -> f t1.tdata t2.tdata) t.tchilds

let at t pos =
	let n = ref (-1) in
	let rec loop t =
		incr n;
		if !n = pos then
			Some t
		else
			let rec child_loop = function
				| [] -> None
				| x :: l ->
					match loop x with
					| None -> child_loop l
					| found -> found
			in
			child_loop t.tchilds
	in
	loop t.troot

let pos t =
	let rec loop t = 
		match t.tparent with
		| None -> t
		| Some p -> loop p
	in
	let n = ref (-1) in
	let rec loop2 ct =
		incr n;
		if t == ct then
			true
		else
			List.exists loop2 ct.tchilds
	in
	let root = loop t in
	ignore(loop2 root);
	!n

let data t =
	t.tdata

let set_data t data =
	t.tdata <- data;
	match t.tparent, t.tinfos with
	| Some t , Some { tsort = Some f } ->
		t.tchilds <- List.sort (fun t1 t2 -> f t1.tdata t2.tdata) t.tchilds
	| _ , _ ->
		()

let childs t =
	t.tchilds

let parent t =
	t.tparent

let remove t2 =
	match t2.tparent with
	| None -> ()
	| Some p ->
		p.tchilds <- List.filter (( != ) t2) p.tchilds;
		t2.tparent <- None

let insert ~item ~parent =
	let rec loop = function
		| None -> ()
		| Some t when t == item -> raise Recursive_insert
		| Some t -> loop t.tparent
	in
	loop (Some parent);
	remove item;
	item.tparent <- Some parent;
	match parent.tinfos with
	| Some ({ tsort = Some f } as tinf) ->
		rec_update tinf item;
		parent.tchilds <- List.sort (fun t1 t2 -> f t1.tdata t2.tdata) (item :: parent.tchilds)
	| Some tinf ->
		parent.tchilds <- item :: parent.tchilds;
		rec_update tinf item
	| None ->
		parent.tchilds <- item :: parent.tchilds

let item data childs =
	let	t = {
		tdata = data;
		tparent = None;
		tchilds = childs;
		tinfos = None;
	} in
	List.iter (fun c -> c.tparent <- Some t) childs;
	t

let create data =
	let rec t = {
		tsort = None;
		troot = {
			tdata = data;
			tparent = None;
			tchilds = [];
			tinfos = Some t;
		}
	} in
	t

let of_root r =
	let t = {
		tsort = None;
		troot = r;
	} in
	rec_update t r;
	t

let map f t =
	let rec loop p t =
		let nt = {
			tinfos = None;
			tdata = f t.tdata;
			tparent = p;
			tchilds = []
		} in
		nt.tchilds <- List.map (loop (Some nt)) t.tchilds;
		nt
	in
	let t = {
		tsort = None;
		troot = loop None t.troot;
	} in
	rec_update t t.troot;
	t
