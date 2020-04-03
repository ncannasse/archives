(* module Client

 - CLIENT
 - Application Cliente

*)
open Message
open Mtlib
open Osiris
open M3d
open Run
open ICard
open Printf
open PGame

exception Debug_error of string

let fullscreen = false
let contre_plongee = true
let vsync = true
let show_infos = false

let main_wnd = Global.empty "Main Window"
let debug_error msg = raise (Debug_error msg)

let print x =
	let sid = (try pid_str (Data.id()) with Global_not_initialized _ -> "C") in
	Printf.printf "%s> " sid;
	let r = Printf.printf x in
	flush stdout;
	r

let error_to_string e =
	match e with
	| EWarning msg -> "Warning - "^msg
	| EUnkGame gid -> "Unknown game #"^(gid_str gid)
	| EUnkPlayer pid -> "Unknown player #"^(pid_str pid)
	| ENoGameRights gid -> "No rights on game #"^(gid_str gid)
	| EInvalidDeck msg -> "The deck you submited is invalid ("^msg^")"
	| ECardNotOwned icid -> "You're using one card wich is not yours : "^(Vfs.name_of_file (ICard.load (Data.id()) icid).ccard)^" (#"^(icid_str icid)^")"
	| ECardNotInHand icid -> "You're using one card wich is not in your hand : "^(Vfs.name_of_file (ICard.load (Data.id()) icid).ccard)^" (#"^(icid_str icid)^")"
	| ENoDeckSelected -> "Please Select a Deck first"
	| ENoGameJoined -> "You're not currently in any game"
	| EGameIsNotRunning gid -> "The game #"^(gid_str gid)^" is not running"
	| EActionNotAvailable msg -> "The action \""^msg^"\" is not available"
	| EInvalidPosition -> "Invalid position"
	| EInvalidCreature cid -> "Invalid creature #"^(icid_str cid)
	| ECreatureNotControled cid -> "You don't have the control of creature #"^(icid_str cid)

let evt_to_string = function
	| EInvokeCaptains -> "Invoke captains"
	| EDrawPhase -> "Draw Phase"
	| EPlayCreature id -> "Play Creature #"^(icid_str id)

let on_message m =
	try
		match m with
		| CConnected id -> 
			Data.init id;
			print "Connected\n";
		| CGames games ->
			CGames.refresh games;
			print "%d games found\n" (CGames.count())
		| CGameCreated g ->
			print "Game %s sucessfully created\n" g.gi_name;
			CGames.create g;
			PGame.create g;
		| CGameInfos g ->
			CGames.set_infos g;
		| CGameStarted gid ->
			CGames.start gid;
		| CGameInit data ->
			if PGame.gid() <> data.gid_id then debug_error "Received Invalid Init Data";
			print "Game #%s init data received\n" (gid_str data.gid_id);
			PGame.init data;
		| CPlayerDraw (pid,ncards) -> 
			print "Player %s is drawing %d cards\n" (pid_str pid) ncards;
			PGame.message (GDraw (pid,ncards))
		| CDrawCards cards ->
			print "You're drawing %d cards\n" (List.length cards);
			PGame.message (GYouDraw cards)
		| CInvoke data ->
			print "Creature #%s is invocated\n" (icid_str data.id_card);
			PGame.message (GInvoke data)
		| CPlayCard cid ->
			print "Card played #%s\n" (icid_str cid);
			PGame.message (GPlay cid)
		| CMulliganQuery ->
			print "Mulligan query\n";
			PGame.message GMulliganQuery;
		| CPlayerTookMulligan pid ->
			print "Player #%s took a mulligan\n" (pid_str pid);
			PGame.message (GPlayerMulligan pid);
		| CError e ->
			print "ERROR: %s\n" (error_to_string e)
		| CEmitEvent (e,t) ->
			print "New event %s added @%d\n" (evt_to_string e) t;
			PGame.message (GEvent (e,t));
		| CPlayEvent e ->
			print "Running event %s\n" (evt_to_string e);
			PGame.message (GPlayEvent e)
		| CMoveCreature (cid,pos) ->
			print "Moving creature %s\n" (icid_str cid);
			PGame.message (GMoveCreature (cid,pos));
	with
		| CGames.No_such_game gid -> print "WARN: Unknown Game #%s\n" (gid_str gid)


(* CLIENT VIRTUAL BEHAVIOR *)

let message_hook m =
	on_message m;
	match m with
	| CConnected _ ->
		let deck = ICard.load_deck (Vfs.get_fid "default deck") (Data.id()) in
		Data.send (SSelectDeck deck);
	| CGames _ ->
		if CGames.count() = 0 then PGame.start_create();
	| CGameCreated g ->
		if PGame.gid() <> g.gi_id then debug_error "Created and Invalid Game";
		PGame.run();
	| _ -> ()

(***************************)

let store = ( MList.empty() : Inst.t MList.t)

let init() =
	Callback.register "logfunc" (fun msg -> Log.log Log.ERROR msg);	
	print "Init Disciple...\n";
	Random.self_init();
	Card.load_dbase();
	ICard.load_dbase();
	let wnd = new_window() in	
	Global.set main_wnd wnd;
	wnd#width 800;
	wnd#height 600;
	wnd#state Maximize;
	wnd#caption "Disciple Online - (c)2002 Motion-Twin";
	wnd#visible true;
	Render.set_option (OFullScreen fullscreen);
	if not (M3d.init wnd#handle vsync) then failwith "Failed to Init M3D";
	M3d.Render.set_option (M3d.OShowInfos show_infos);
	Mouse.init wnd#handle;	
	Input.Keyb.bind (fun () -> raise Exit_application) [Input.Keyb.key "Esc"];	
	if contre_plongee then begin
		Camera.set_pos { x = 0.; y = -600.; z = 470. };
		Camera.set_target { x = 0.; y = 0.; z = 70. };
	end else begin
		Camera.set_pos { x = 0.; y = -100.; z = 400. };
		Camera.set_target { x = 0.; y = 0.; z = 70. };
	end;
	Render.start();
	Render.update_time();
(*	let id = Vfs.get_fid "Creatures/Arpenteur/arpenteur.mtw" in	
	for i=0 to 50 do
		let i = Inst.attach id in
		Inst.anim i (Some "attente");
		Inst.time i (Random.float 1.0);
		Inst.pos i (Matrix.trans { x = Random.float 500. -. 250.; y = Random.float 500. -. 250.; z = 0. });
		MList.add store i;
	done;
*)
	Server.connect message_hook

let main() =
	let wnd = Global.get main_wnd in
	if not (wnd#process false) then raise Exit_application;
	try
		M3d.handle_events();
		Mouse.process();
		Selection.process();
		PGame.process();
	with
		| Click_handled -> ()
		| SMessage msg -> Data.send msg

let close() =	
	print "Closing Disciple...\n";
	Selection.close();
	Mouse.close();
	PGame.close();
	Data.close();
	Input.close();
	M3d.close();
	let wnd = Global.get main_wnd in
	Global.undef main_wnd;
	if not wnd#is_destroyed then wnd#destroy

;;
Run.register {
	m_name = "Disciple";
	m_init = Some init;
	m_main = Some main;
	m_close = Some close;
};
