open Vfs
open Card
open Mtlib
open M3d

let ddeck_t = Vfs.get_ftype "DDK"

let new_card pid =
	let d = Dialogs.create None "dcard.new" in
	let ct = d#combobox "dcard.type" in
	let cmodel = d#combobox "dcard.model" in
	ct#listprint (fun() -> ctype_strings);
	ct#set_selected (Some 0);
	cmodel#listprint (fun() -> List.map name_of_file (get_group M3dview.mtw_t));
	if d#process then begin
		let name = (d#edit "dcard.name")#get_caption in
		let dtype = ct#selected in
		let msel = cmodel#selected in
		Dialogs.close d;
		match dtype with
			| None -> failwith "You must select a Card Type"
			| Some pos ->
				let mid = (match msel with
					| None -> failwith "You must select a Model"
					| Some pos -> List.nth (get_group M3dview.mtw_t) pos 
				) in
				let id = Vfs.create_file ~name:name ~parent:pid ~ftype:Card.ftype in
				ignore(add_dep id mid);
				Card.save id ("",(match List.nth ctype_strings pos with
					| "Creature" ->
						Creature {
							clevel = 1;
							cessence = EIce;
							clife = 1;
							cmovement = 100;
							cspeed = 100;
							csize = 50;
							cmodel = mid;
							canimbcl = "attente_boucle";
							canimmove = "marche";
							cmovespeed = 100;
							ccapacities = [];
						}
					| _ -> assert false
				));				
				Document.dopen id;
	end else
		Dialogs.close d

let do_save_card x id confirm =
	let invalid_anims = MList.empty() in
	let get_string str = (x#component str)#get_caption in
	let get_bool str =
		(x#toggle str)#get_state in
	let get_int str =
		try
			let i = stoi (get_string str) in
			if i < 0 then raise (Failure "int_of_string");
			i
		with
			Failure "int_of_string" -> failwith ("Invalid integer value for "^str)
	in
	match Card.load id with
	| script, Creature data ->
		let newdata = {
			clevel = get_int "level";
			clife = get_int "life";
			cmovement = get_int "movement";
			csize = get_int "size";
			cessence = (match (x#combobox "essence")#selected with None -> assert false | Some e -> (Obj.magic e : essence));
			cspeed = get_int "speed";
			cmodel = data.cmodel;
			canimbcl = get_string "animbcl";
			canimmove = get_string "animmove";
			cmovespeed = get_int "movespeed";
			ccapacities = Dparser.capacities Dparser_lex.tags (Lexing.from_string (get_string "capacities"));
		} in
		match data <> newdata || script <> get_string "capacities", confirm with
		| false , _ -> ()
		| true, true ->
			let doclose = Osiris.question_box "Close without saving ?" in
			if not doclose then raise Run.Exception_no_display;
		| true, false ->
			let i = Inst.attach newdata.cmodel in
			let anims = Inst.anims i in
			Inst.detach i;
			Gc.full_major(); (* clean resources *)
			let hasanim str = List.exists ((=) str) anims in
			if not (hasanim newdata.canimbcl) then MList.add invalid_anims newdata.canimbcl;
			if not (hasanim newdata.canimmove) then MList.add invalid_anims newdata.canimmove;
			List.iter (fun c ->
				match c with
				| XAttack a when not(hasanim a.aanim) -> MList.add invalid_anims a.aanim
				| _ -> ()) newdata.ccapacities;
			if MList.length invalid_anims > 0 then Osiris.message_box ("Warning ! Some anims haven't been found in the model : "^(String.concat ", " (MList.to_list invalid_anims)));
			Card.save id (get_string "capacities",(Creature newdata))

let open_card id =	
	match Card.load id with
	| script, Creature data ->
		Document.create id (Dialogs.get "dcard.edit.creature") (fun x ->
			let init_string str value = (x#component str)#caption value in
			let	init_int str value = init_string str (itos value) in
			let init_bool str value = (x#toggle str)#state value in
			init_int "level" data.clevel;
			init_int "life" data.clife;
			(x#combobox "essence")#listprint (fun () -> essence_strings );
			(x#combobox "essence")#set_selected (Some (Obj.magic data.cessence : int));
			init_int "movement" data.cmovement;
			init_int "speed" data.cspeed;
			init_int "size" data.csize;
			init_string "model" (name_of_file data.cmodel);
			init_string "animbcl" data.canimbcl;
			init_string "animmove" data.canimmove;
			init_int "movespeed" data.cmovespeed;
			init_string "capacities" script;
			(x#button "save")#on_click (fun () -> do_save_card x id false)
		)

let close_card id =	
	let x = Document.get id in
	do_save_card x id true


let new_deck pid =
	let d = Dialogs.create None "ddeck.new" in
	if d#process then begin
		let name = (d#edit "ddeck.name")#get_caption in
		Dialogs.close d;
		let id = Vfs.create_file ~name:name ~parent:pid ~ftype:ddeck_t in
		Vfs.write id []
	end else
		Dialogs.close d

let rec count_cards = function
	| [] -> []
	| x::l ->
		let same,others = List.partition ((=) x) l in
		let count = 1 + List.length same in
		(x,count)::(count_cards others)

let opened_decks = Hashtbl.create 0

let get_deck id =
	try
		Hashtbl.find opened_decks id
	with
		Not_found ->
			let deck = List.map (fun dep -> get_dep id dep) (read id) in
			let deck = List.sort (fun f f' -> compare (name_of_file f) (name_of_file f')) deck in
			Hashtbl.replace opened_decks id deck;
			deck

let set_deck id deck =
	let deck = List.sort (fun f f' -> compare (name_of_file f) (name_of_file f')) deck in
	Hashtbl.replace opened_decks id deck

let rec open_deck id =
	let deck = get_deck id in
	let cards = count_cards deck in
	let card_display (card,n) =
		let name = name_of_file card in
		Printf.sprintf "<panel border='no' align='top' height='20'><label align='left' caption='%d' bold='true' width='20'/><label align='left' caption='%s' width='200'/><button name='del%s' caption='DELETE' align='left'/></panel>" n name name
	in
	let xdata = "<panel border='no' span='10' space='10'>"^
		"<listbox name='cards' align='left' width='200'/>"^
		(String.concat "" (List.map card_display cards))^
		"<button name='save' align='center' caption='SAVE'/></panel>" in
	Document.create id (Xml.parse_string xdata) (fun x ->
		let l = x#listbox "cards" in
		l#listprint (fun () -> List.map name_of_file (get_group Card.ftype));
		l#on_dblclick (fun () ->
			match l#selected with
			| None -> ()
			| Some p ->
				let card = List.nth (get_group Card.ftype) p in
				Document.close id;
				set_deck id (card::deck);
				open_deck id);
		let card_bind_delete (card,_) =
			let name = name_of_file card in
			(x#button ("del"^name))#on_click (fun () ->
				let deck = MList.from_list deck in
				MList.remove deck card;
				Document.close id;
				set_deck id (MList.to_list deck);
				open_deck id);
		in
		List.iter card_bind_delete cards;
		(x#button "save")#on_click (fun () ->
			clear_deps id;
			let deck = List.map (fun cid -> add_dep id cid) (get_deck id) in
			write id deck)
	)
		

let close_deck id =
	Hashtbl.remove opened_decks id

;;
Vfs.register {
	ft_name = "Disciple Card";
	ft_t = Card.ftype;	
	ft_icon = Gfxres.bitmap "BMP_DISCARD";
	ft_new = Some new_card;
	ft_select = None;
	ft_unselect = None;
	ft_open = Some open_card;
	ft_show = None;
	ft_hide = None;
	ft_close = Some close_card;
	ft_context = [];
	ft_delete = None;
	ft_hardlinked = None;
	ft_haschilds = false;
	ft_export = None;
};

Vfs.register {
	ft_name = "Disciple Deck";
	ft_t = ddeck_t;
	ft_icon = Gfxres.bitmap "BMP_DISDECK";
	ft_new = Some new_deck;
	ft_select = None;
	ft_unselect = None;
	ft_open = Some open_deck;
	ft_show = None;
	ft_hide = None;
	ft_close = Some close_deck;
	ft_context = [];
	ft_delete = None;
	ft_hardlinked = None;
	ft_haschilds = false;
	ft_export = None;
};

Run.fast_register "Disciple Cards";