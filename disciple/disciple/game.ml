(* module Game

 - SERVER
 - Données sur une partie en cours

*)
open Message
open ICard
open Card
open Mtlib

type gstate =
	| NoAction
	| AWaitMulligan of pid
	| APutCaptains
	| AEvent of event

type t = {
	gid : gid;
	gplayers : Player.t list;
	gboard : Board.t;
	gevents : event EQueue.t;
	mutable gstate : gstate;
	mutable gnextstate : unit -> gstate;
}

let player g pid =
	try
		List.find (fun p -> Player.id p = pid) g.gplayers
	with
		Not_found -> raise (SError (EUnkPlayer pid))

let send g msg =
	List.iter (fun p -> Player.send p msg) g.gplayers

let emit g evt time =
	EQueue.insert g.gevents evt time;
	send g (CEmitEvent (evt,time))

let draw_phase g =
	let rec aux prev cur =
		match cur with
		| [] -> ()
		| p::next ->
			Player.draw p 1;
			let msg = CPlayerDraw (Player.id p,1) in
			List.iter (fun p' -> Player.send p' msg) (prev@next);
			aux (p::prev) next
	in
	aux [] g.gplayers


let rec next_action g =
	let rec process_next_event g =
		try
			let e = EQueue.pop g.gevents in
			send g (CPlayEvent e);
			(match e with
			| EInvokeCaptains -> e
			| EDrawPhase ->
				draw_phase g;
				emit g EDrawPhase Constants.draw_time;
				process_next_event g
			| EPlayCreature id -> e
			);
		with
			_ -> failwith "process_next_event"
	in
	g.gstate <- g.gnextstate();
	match g.gstate with
	| NoAction -> ()
	| AWaitMulligan pid -> Player.send (player g pid) CMulliganQuery
	| APutCaptains ->
		g.gnextstate <- (fun () -> AEvent (process_next_event g));
		ignore(process_next_event g)
	| AEvent _ -> ()

let no_action aname =	
	raise (SError (EActionNotAvailable aname))

let create gid players =
	let players = MList.from_list players in
	MList.shuffle players;
	let players = MList.to_list players in
	let p = ref (-1) in
	let players_data = List.map Player.create players in
	let g = {
		gid = gid;
		gplayers = players_data;
		gstate = NoAction;
		gboard = Board.empty Constants.board_width Constants.board_height;
		gevents = EQueue.empty();
		gnextstate = (fun () ->
			incr p;
			try
				AWaitMulligan (List.nth players !p)
			with _ ->
				APutCaptains
		);
	} in	
	send g (CGameInit {
		gid_id = gid;
		gid_players = List.map (fun p -> {
				gip_pid = Player.id p;
				gip_deck_count = Player.deck_count p;
		}) g.gplayers;
	});
	let init_hand p =
		let n = Constants.starting_hand_count in
		Player.draw p n;
		List.iter (fun p' -> if p <> p' then Player.send p' (CPlayerDraw (Player.id p,n))) g.gplayers
	in
	List.iter init_hand g.gplayers;	
	g

let start g =
	emit g EInvokeCaptains 0;
	emit g EDrawPhase Constants.draw_time;
	next_action g

let mulligan g pid b =
	match g.gstate with
	| AWaitMulligan curp when curp = pid ->
		if not b then next_action g else begin
			let p = player g pid in
			send g (CPlayerTookMulligan pid);
			if not (Player.mulligan p) then next_action g else Player.send p CMulliganQuery
		end
	| _ -> no_action "mulligan"

let emit_creature ?(base=0) g c coef =
	let time = base + ftoi (itof (Creature.infos c).cspeed *. coef) in
	emit g (EPlayCreature (Creature.id c)) time

let invoke g pid data = 
	let p = player g pid in
	let icard = ICard.load pid data.id_card in
	let do_invoke() =
		try
			if not(Player.has_card p icard) then raise (SError (ECardNotInHand data.id_card));
			let c = Board.invoke g.gboard icard data.id_pos in
			Player.add_creature p c;
			Player.play p icard;
			send g (CPlayCard data.id_card);
			send g (CInvoke data);
			emit_creature g c 1.;
		with
			Board.Invalid_position -> raise (SError EInvalidPosition)
	in
	try
		match g.gstate with
		| APutCaptains when Card.is_captain (Card.find icard.ccard) ->
			if Player.has_captain p then no_action "invoke@1";
			do_invoke();
			(try
				ignore(List.find (fnot Player.has_captain) g.gplayers)
			with
				Not_found -> next_action g)
		| AEvent (EPlayCreature cid) -> 				
			let ccapt = Board.creature g.gboard cid in
			if not (Card.is_captain (Creature.card ccapt)) then no_action "invoke@2";
			let cmin,cmax = Board.command_calc g.gboard in
			let c = Card.find icard.ccard in
			(match c.ctype with
			| Creature cdata ->
				if cmin + cdata.clevel > cmax then no_action "invoke@3";
				do_invoke();
				emit_creature g ccapt 1.;
				next_action g;				
			)
		| _ -> no_action "invoke@4"
	with
		Board.No_such_creature cid -> raise (SError (EInvalidCreature cid))

let move g pid cid path =	
	let p = player g pid in
	try
		let c = Board.creature g.gboard cid in
		if Creature.controler c <> pid then raise (SError (ECreatureNotControled cid));
		match g.gstate with
		| AEvent (EPlayCreature cid) ->
			let size = Creature.size c in
			let pos = Creature.pos c in
			if Maths.polygon_start path <> pos then raise (SError EInvalidPosition);
			if not (Board.is_valid_path g.gboard pos path size cid) then raise (SError EInvalidPosition);			
			let dist = Maths.polygon_length path in
			let movement = itof (Creature.infos c).cmovement in
			if dist > movement then raise (SError EInvalidPosition);
			let coef = dist /. movement in
			Creature.move_to c (Maths.polygon_end path);
			send g (CMoveCreature (cid,Maths.polygon_next path));
			emit_creature ~base:Constants.creature_move_penalty g c coef;
			next_action g;
		| _ -> no_action "move"
	with
		Board.No_such_creature cid -> raise (SError (EInvalidCreature cid))

