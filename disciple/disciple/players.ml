(* module Players

 - SERVER
 - Données sur l'ensemble des joueurs connectés

*)
open Message
open Mtlib

type t = {
	pid : pid;
	pfun : client_message -> unit;
	pdeck : Deck.t;
	mutable pgame : Games.t option;
}

let players = MBTree.empty()

let print x =
	print_endline x;
	flush stdout

let create f = 		
	let pid = alloc_pid() in
	let deck = Deck.empty() in
	SData.register pid f deck;
	let pdata = {
		pid = pid;
		pfun = f;
		pgame = None;
		pdeck = deck;
	} in
	MBTree.add players pid pdata;
	pid

let get pid =	
	try
		MBTree.find players pid
	with
		Not_found -> raise (SError (EUnkPlayer pid))

let load_deck pid cards =	
	let p = get pid in
	Deck.fill p.pdeck cards pid
	
let get_deck pid =
	let p = get pid in
	p.pdeck

let send pid msg =
	let p = get pid in
	try
		p.pfun msg
	with
		e -> Run.process_exc "Client" e

let send_all msg =
	MBTree.iter (fun _ pdata -> (try pdata.pfun msg with e -> Run.process_exc "Client" e)) players

let create_game pid data =
	let p = get pid in
	match p.pgame with
	| Some _ -> raise (SError (EWarning "A game is already joined"))
	| None ->
		if Deck.is_empty p.pdeck then raise (SError ENoDeckSelected);
		let g = Games.create pid data in
		p.pgame <- Some g;
		g

let join_game pid g =
	let p = get pid in
	match p.pgame with
	| None -> 
		if Deck.is_empty p.pdeck then raise (SError ENoDeckSelected);
		p.pgame <- Some g
	| Some _ -> raise (SError (EWarning "A game is already joined"))

let leave_game pid =
	let p = get pid in
	match p.pgame with
	| Some _ -> p.pgame <- None
	| None  -> raise (SError ENoGameJoined)

let game pid =
	let p = get pid in
	match p.pgame with
	| None -> raise (SError ENoGameJoined)
	| Some g -> Games.data g