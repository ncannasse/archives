(* module PGame

 - CLIENT
 - Partie en Cours

*)
open Message
open Mtlib
open Selection
open M3d
open Board
open ICard
open Card

module Keyb = Input.Keyb

let is_down = function
	| Input.Up -> false
	| _ -> true

type game_message =
	| GDraw of pid * int
	| GYouDraw of icid list
	| GInvoke of invoke_data
	| GPlay of icid
	| GEvent of (event * int)
	| GMulliganQuery
	| GPlayerMulligan of pid
	| GPlayEvent of event
	| GMoveCreature of (icid * Maths.polygon)

type t = {
	gid : gid;
	gcreator : pid;
	gname : string;
	mutable grunning : bool;
	gplayers : pid MList.t;
	gpdata : PPlayer.t MList.t;
	ghand : PHand.t;
	gboard : PBoard.t;
	gdeck : Sprite.t;
	gqueue : PEvent.t EQueue.t;
	mutable glastevent : PEvent.t option;
	mutable gwaiting : bool;
	mutable gmessages : game_message MList.t;
}

let pgame = Global.empty "Current Client Game"

exception No_current_pgame
exception No_such_player of pid

(* ******************************** INFORMATIONS ************************************** *)

let board g =
	PBoard.board g.gboard

let get() =
	try
		Global.get pgame
	with
		Global_not_initialized _ -> raise No_current_pgame

let player pid =
	let g = get() in
	try
		MList.find (fun p -> PPlayer.id p = pid) g.gpdata
	with
		Not_found -> raise (No_such_player pid)

let gid() =
	let g = get() in g.gid

(* ******************************** INITIALISATION ************************************** *)

let start_create () =
	Data.send (SCreateGame { gcd_name = "test game";})

let create data =
	if data.gi_creator = Data.id() then begin
		let s = Sprite.attach Constfiles.fCardHiddenFace Constants.card_bg_zone in
		Sprite.size s Constants.card_size;
		Sprite.pos s Constants.deck_pos;		
		let g = {
			gid = data.gi_id;
			gcreator = data.gi_creator;
			gname = data.gi_name;
			grunning = false;
			gplayers = MList.from_list [data.gi_creator];
			gpdata = MList.empty();
			ghand = PHand.empty();
			gboard = PBoard.empty Constants.board_width Constants.board_height;
			gqueue = EQueue.empty();
			glastevent = None;
			gwaiting = false;
			gdeck = s;
			gmessages = MList.empty();
		} in
		Global.set pgame g;
	end

let init data =
	let g = get() in
	MList.copy_list g.gpdata (List.map PPlayer.create data.gid_players)

let run() =
	let g = get() in
	g.grunning <- true;
	Data.send (SRunGame g.gid)

let close() =
	match Global.opt pgame with
	| None -> ()
	| Some g -> 
		PHand.close g.ghand;
		PBoard.close g.gboard;
		Sprite.detach g.gdeck;
		MList.iter PPlayer.close g.gpdata;
		EQueue.iter (fun (e,_) -> PEvent.close e) g.gqueue;
		(match g.glastevent with None -> () | Some e -> PEvent.close e);
		Global.undef pgame

(* ******************************** GLOBALS ************************************** *)

let nop = (fun () -> ())

let pause g t f =
	g.gwaiting <- true;
	Events.add_delayed (fun () -> g.gwaiting <- false; f()) t

let command_calc g =
	Board.command_calc (PBoard.board g.gboard)

let can_use_card g c card =
	match card.ctype with
	| Creature cdata ->
		is_captain (Creature.card (PCreature.data c)) && (
			let cur,max = command_calc g in
			cur + cdata.clevel <= max)

(* ******************************** PHASES ************************************** *)

let invoke_at_phase g card fretry =
	let ic = PCard.icard card in
	let gfx = Inst.attach Constfiles.fSelectionModel in
	let size = (match (PCard.card card).ctype with Creature c -> c.csize) in
	let ref_valid = ref None in
	Inst.visible gfx false;
	Inst.color gfx Color.zero;
	FX.src_blend (Inst gfx) Blend_SrcAlpha;
	FX.dst_blend (Inst gfx) Blend_InvSrcAlpha;
	Inst.texture gfx Constfiles.fSelCursorInvoke;
	PHand.filter g.ghand (fun c -> c <> ic);
	PHand.enable g.ghand false;
	let sdone() =
		Inst.detach gfx;
		Mouse.show true;
	in
	let confirm pos =
		PHand.filter g.ghand fall;
		PHand.enable g.ghand false;
		raise (SMessage (SInvoke { id_card = ic.cid; id_pos = pos; }));
	in
	Selection.add {
		fs_text = "Select invocation place";
		fs_self_validate = false;
		fs_test = (fun() ->	
			match PHand.selected g.ghand with
			| Some _ ->
				Inst.visible gfx false;
				Inst.color gfx Color.zero;
				ref_valid := None;
				Mouse.show true;
				false
			| None ->
				let px,py = Render.projection (Mouse.pos()) 0. in
				match Board.on_board (board g) (px,py) size with
				| POutBoard ->
					Inst.visible gfx false;
					Inst.color gfx Color.zero;
					ref_valid := None;
					Mouse.show true;					
					false
				| x ->
					let fsize = itof size in
					Inst.pos gfx (Matrix.trs { x = px; y = py; z = 0.2 } 
											{ x = 0.; y = 0.; z = Render.time() *. 5.; }
											{ x = fsize /.50.; y = fsize /.50.; z = fsize /.50.; }
								);
					let valid = (x = PCollide []) in
					if !ref_valid <> Some valid then begin
						Inst.fade gfx (Color.alpha (if valid then Color.full else Color.rgb 0xFF0000) 128) 0.15;
						ref_valid := Some valid;
					end;
					Inst.visible gfx true;
					Mouse.show false;
					valid
			);
		fs_cancel = Some (fun () -> sdone(); fretry());
		fs_ok = (fun () -> sdone(); confirm (Render.projection (Mouse.pos()) 0.));
	}

let rec invoke_phase g ffilter =
	Mouse.show true;
	PHand.cfilter g.ghand ffilter;
	PHand.enable g.ghand true;
	Selection.add {
		fs_text = "Choose creature to invoke";
		fs_self_validate = false;
		fs_test = (fun () ->
			match PHand.selected g.ghand with
			| Some c when not (PCard.filtered c) -> true
			| _ -> false);
		fs_cancel = None;
		fs_ok = (fun () ->
			match PHand.selected g.ghand with
			| None -> assert false
			| Some c ->
				invoke_at_phase g c (fun () -> invoke_phase g ffilter)
		);
	}

let use_card g c card fretry =
	let icard = PCard.icard card in
	PHand.filter g.ghand (fun c -> c <> icard);
	PHand.enable g.ghand false;
	match (PCard.card card).ctype with
	| Creature _ ->	
		invoke_at_phase g card fretry

let draw_poly p =
	let rec aux col = function
		| a::b::p ->
			Render.draw_line { x = fst a; y = snd a; z = 1. } { x = fst b; y = snd b; z = 1. } (Color.rgb (if col then 0xFFFFFF else 0));
			aux (not col) (b::p)
		| _ -> ()
	in
	aux true p

let rec play_creature_phase g c =
	let cc = PCreature.data c in
	let origin = Creature.pos cc in
	let pos = ref origin in
	let poslist = MList.from_list [!pos] in
	let size = Creature.size cc in
	let cid = PCreature.id c in
	let ang = ref (PCreature.angle c) in
	let speed = itof (Creature.infos cc).cmovespeed in	
	let selected_card = ref None in
	(** TODO "center camera" *)
	PCreature.active_actions c;
	PHand.cfilter g.ghand (fnot (can_use_card g c));
	PHand.enable g.ghand true;
	Mouse.show true;
	Selection.add {	
		fs_text = "Play that creature";
		fs_self_validate = true;
		fs_cancel = None;
		fs_test = (fun () ->
			let mouse = (match PHand.selected g.ghand with
						| Some c when not (PCard.filtered c) ->
							if Mouse.click() then begin
								selected_card := Some c;
								Selection.validate();
								raise Run.Click_handled;
							end;
							true							
						| _ ->
							false) in
			if is_down (Keyb.state Keyb.left) then begin
				ang -=. 1. *. Render.elapsed_time();
				PCreature.update_pos c !pos !ang;
			end;
			if is_down (Keyb.state Keyb.right) then begin
				ang +=. 1. *. Render.elapsed_time();
				PCreature.update_pos c !pos !ang;
			end;
			if is_down (Keyb.state Keyb.up) then begin
				let x,y = !pos in
				let etime = Render.elapsed_time() in
				let mspeed = speed *. etime in
				let p = x +. (sin !ang) *. mspeed , y -. (cos !ang) *. mspeed in				
				let move =
					(match Board.on_board (PBoard.board g.gboard) p size with
					| PCollide [] -> true
					| PCollide [c] when Creature.id c = cid -> true
					| _ -> false) 
					in
				let poly = Maths.calc_polygon (List.rev (p::(MList.to_list poslist))) 1. in				
				if move && not (Board.is_valid_path (PBoard.board g.gboard) origin poly size cid) then begin
					Render.clear_print();
					Render.print (Printf.sprintf "#(%g, %g)" (fst p) (snd p));
					List.iter (fun p -> Render.print (Printf.sprintf "+(%g, %g)" (fst p) (snd p))) poly;
					failwith "Move imprecision failure";
				end;
				match move , Maths.polygon_length poly < itof (Creature.infos cc).cmovement with
				| false, _ -> 
					PCreature.play_anim c PCreature.AWait
				| true, false ->
					PCreature.play_anim c PCreature.AWait;
					Selection.validate();
					raise Run.Click_handled;
				| true, true ->
					pos := p;
					MList.push poslist p;					
					PCreature.update_pos c !pos !ang;
					PCreature.play_anim c PCreature.AWalk;
					(** TODO "CAMERA FOLLOW" *)
					(** TODO "DISPLAY ATTACK ICON" *)
			end else
				PCreature.play_anim c PCreature.AWait;
			mouse
		);
		fs_ok = (fun () ->
			match !selected_card with
			| Some card ->
				use_card g c card (fun () -> play_creature_phase g c)
			| None ->
				let poly = Maths.calc_polygon (List.rev (MList.to_list poslist)) 1. in
				Data.send (SMoveCreature (cid,poly))
				(** TODO "OR ATTACK" *)
		);
	}

(* ******************************** EVENTS ************************************** *)

let message e =
	let g = get() in
	MList.add g.gmessages e

let update_queue g =	
	EQueue.iter (fun (e,t) -> PEvent.update e t) g.gqueue

let process_next_event g e =	
	let evt = (try EQueue.pop g.gqueue with _ -> failwith "PGame.process_event") in
	(match g.glastevent with
	| None -> ()
	| Some e -> PEvent.destroy e);
	PEvent.select evt;
	g.glastevent <- Some evt;
	if PEvent.evt evt <> e then failwith "Event desynchronization !";
	update_queue g;	
	match e with
	| EInvokeCaptains ->
		invoke_phase g (fnot is_captain);
	| EDrawPhase ->
		pause g 0.5 nop
	| EPlayCreature id ->
		let owner = ICard.owner id in
		let c = PBoard.creature g.gboard id in
		if owner = Data.id() then
			play_creature_phase g c
		else begin
			PHand.filter g.ghand fall;
			PCreature.filter c (Color.alpha (Color.rgb 0xFF0000) 128)
		end

let process_next_message g =
	let pause = pause g in
	if not g.gwaiting then begin
		try			
			match MList.pop g.gmessages with
			| GDraw (pid,ncards) ->
				let p = player pid in
				PPlayer.draw p ncards
			| GYouDraw cards ->
				PHand.enable g.ghand false;
				PHand.add g.ghand (List.map ICard.find cards);
				(match MList.to_list g.gmessages with
				 | (GEvent(EInvokeCaptains,_))::_ -> ()
				 | _ -> pause 2. (fun () -> PHand.filter g.ghand fall));
			| GInvoke data ->
				let c = ICard.find data.id_card in
				PBoard.invoke g.gboard c data.id_pos;
			| GPlay cardid ->
				let owner = ICard.owner cardid in
				let card = ICard.find cardid in
				if owner = Data.id() then begin
					PHand.play g.ghand card;
					pause 0.2 nop
				end else
					PPlayer.play (player owner) card
			| GEvent (event,time) ->
				EQueue.insert g.gqueue (PEvent.emit event time) time;	
				update_queue g;
				pause 0.3 nop
			| GMulliganQuery ->
				(** TODO "display mulligan dialog, enter Selection" **)
				let do_mulligan = false in
				if do_mulligan then PHand.clear g.ghand;
				Data.send (SMulligan do_mulligan)
			| GPlayerMulligan pid ->
				PPlayer.mulligan (player pid)
			| GPlayEvent event ->
				process_next_event g event
			| GMoveCreature (cid,pos) ->
				let c = PBoard.creature g.gboard cid in
				let owner = (ICard.owner cid = Data.id()) in
				g.gwaiting <- true;
				PCreature.move_to ~play:(not owner) c pos (fun () -> g.gwaiting <- false);
		with
			Invalid_operation -> ()
	end


(* ******************************** PROCESS ************************************** *)

let process() =
	match Global.opt pgame with
	| None -> ()
	| Some g ->
		try
			PBoard.process g.gboard;
			process_next_message g;
		with
			| PBoard.Select_creature _ ->
				(** TODO "Display informations about the creature - card etc..." **)
				()

(* ***************************************************************************** *)

