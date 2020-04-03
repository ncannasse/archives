(********************************************************************************)
(* Includes *)

open Osiris
open Xml

(********************************************************************************)
(* Interface *)

class type xobj =
  object
	method component : string -> component
	method component : string -> component
	method container : string -> container
	method menu : string -> menu
	method menuitem : string -> menuitem
	method window : string -> window
	method panel : string -> panel
	method button : string -> button
	method listbox : string -> listbox
	method combobox : string -> combobox
	method toggle : string -> toggle
	method radiobutton : string -> radiobutton
	method checkbox : string -> checkbox
	method label : string -> label
	method edit : string -> edit
	method richedit : string -> richedit
	method treeview : string -> treeview

	method process : bool
	method destroy : unit
	method self : component
  end

(********************************************************************************)
(* Types *)

type xparent =
	| XMenu of menu
	| XWindow of window
	| XPanel of panel

type xany =
	| Menu of menu
	| Menuitem of menuitem
	| Window of window
	| Panel of panel
	| Button of button
	| Listbox of listbox
	| Combobox of combobox
	| Radiobutton of radiobutton
	| Checkbox of checkbox
	| Label of label
	| Edit of edit
	| Richedit of richedit
	| Treeview of treeview
	| Group of xparent
	| Noaccess
	| Action

type vobj =	xml * xany

type vnames = (string,xany) Hashtbl.t

exception Conversion
exception Ok
exception Cancel

(********************************************************************************)
(* vobj Functions *)

let x_xml o =
	let x,_ = o in x

let x_any o =
	let _,x = o in x

(********************************************************************************)
(* xparent Functions *)

let p_menu p =
	match p with
		XMenu m -> m
		| XWindow w -> w#menu
		| _ -> raise Not_found

let p_container p =
	match p with
		XWindow w -> w#container
		| XPanel p -> p#container
		| _ -> raise Not_found

(********************************************************************************)
(* xany Functions *)

let a_action any =
	match any with
	| Action -> ()
	| _ -> raise Not_found

let a_component any =
	match any with
	| Window c -> c#component
	| Panel c -> c#component
	| Button c -> c#component
	| Listbox c -> c#component
	| Combobox c -> c#component
	| Radiobutton c -> c#component
	| Checkbox c -> c#component
	| Label c -> c#component
	| Edit c -> c#component
	| Richedit c -> c#component
	| Treeview c -> c#component
	| _ -> raise Not_found

let a_container any =
	match any with
	| Window c -> c#container
	| Panel c -> c#container
	| _ -> raise Not_found

let a_menuitem any =
	match any with
	| Menuitem i -> i
	| _ -> raise Not_found

let a_window any =
	match any with
	| Window w -> w
	| _ -> raise Not_found

let a_menu any =
	match any with
	| Menu m -> m
	| _ -> raise Not_found

let a_panel any =
	match any with
	| Panel m -> m
	| _ -> raise Not_found

let a_button any =
	match any with
	| Button m -> m
	| _ -> raise Not_found

let a_listbox any =
	match any with
	| Listbox m -> m
	| _ -> raise Not_found

let a_combobox any =
	match any with
	| Combobox m -> m
	| _ -> raise Not_found

let a_radiobutton any =
	match any with
	| Radiobutton m -> m
	| _ -> raise Not_found

let a_checkbox any =
	match any with
	| Checkbox m -> m
	| _ -> raise Not_found

let a_toggle any =
	match any with
	| Checkbox t -> t#toggle
	| Radiobutton t -> t#toggle
	| _ -> raise Not_found

let a_label any =
	match any with
	| Label m -> m
	| _ -> raise Not_found

let a_edit any =
	match any with
	| Edit m -> m
	| _ -> raise Not_found

let a_richedit any = 
	match any with
	| Richedit m -> m
	| _ -> raise Not_found

let a_treeview any =
	match any with
	| Treeview t -> t
	| _ -> raise Not_found

let a_parent any =
	match any with
	| Window w -> XWindow w
	| Panel p -> XPanel p
	| Menu m -> XMenu m
	| Group g -> g
	| _ -> raise Not_found


(********************************************************************************)
(* xobj class *)

let query names name func =
	let f =	try (Hashtbl.find names name) with Not_found -> failwith ("XGui query unknown component : "^name) in
	try func f with	Not_found -> failwith ("XGui query component of invalid type : "^name)

class ixobj (obj:xany) (names:vnames) =
  object(this)

	method menu n = query names n a_menu
	method menuitem n = query names n a_menuitem
	method window n = query names n a_window
	method panel n = query names n a_panel
	method button n = query names n a_button
	method listbox n = query names n a_listbox
	method combobox n = query names n a_combobox
	method toggle n = query names n a_toggle
	method radiobutton n = query names n a_radiobutton
	method checkbox n = query names n a_checkbox
	method label n = query names n a_label
	method edit n = query names n a_edit
	method richedit n = query names n a_richedit
	method treeview n = query names n a_treeview

	method component n = query names n a_component
	method container n = query names n a_container
	method any n = query names n (fun x -> x)

	method process =
		try
			let wnd = (a_window obj) in
			while wnd#process true do () done;
			false;
		with
			Ok -> true
			| Cancel -> false
			| Not_found -> false
	
	method names = names

	method destroy =
		try			
			let c =	a_component obj in
			if not(c#is_destroyed) then c#destroy;
		with
			Not_found -> (a_menu obj)#destroy

	method self =
		try
			(a_component obj)
		with
			Not_found -> failwith "This XOBJ is not a component : it has no method self"

  end

let null_xobj = (new ixobj Noaccess (Hashtbl.create 0) :> xobj)

(********************************************************************************)
(* xml Functions *)

let is_child_component x = 
	match Xml.tag x with
	"window" -> false
	| _ -> true

let name x =
	(Xml.tag x)^"("^(
		try
			(Xml.attrib x "name")
		with
			No_attribute _ -> "<unamed>"
	)^")"

(********************************************************************************)
(* attribs Functions *)

let v_int v =
	try
		(int_of_string v)
	with
		Failure _ -> raise Conversion

let v_bool v =
	match (String.lowercase v) with
	| "true" -> true
	| "false" -> false
	| _ -> raise Conversion

let v_align v =
	match (String.lowercase v) with
	| "top" -> AlTop
	| "bottom" -> AlBottom
	| "left" -> AlLeft
	| "right" -> AlRight
	| "center" -> AlCenter
	| "client" -> AlClient
	| _ -> raise Conversion

let v_wstate v =
	match (String.lowercase v) with
	| "maximize" -> Maximize
	| "minimize" -> Minimize
	| "normal" -> Normal
	| _ -> raise Conversion

let v_border v =
	match (String.lowercase v) with
	| "no" -> BdNone
	| "single" -> BdSingle
	| "normal" -> BdNormal
	| _ -> raise Conversion

(********************************************************************************)
(* events Functions *)	

let no_events s = raise Not_found

let events_func = ref no_events

let events func = events_func := func

let v_eval names v =
	try
		Hashtbl.find names v
	with Not_found -> failwith ("Invalid reference to object "^v)

let	v_event_parse (names: (string,xany) Hashtbl.t) (obj:vobj) v =
	try 
		let pos = (String.index v '#') in
		let oname = (String.sub v 0 pos) in
		let action = (String.sub v (pos+1) ((String.length v)-pos-1)) in
		(v_eval names oname,action)
	with
		Not_found -> ((x_any obj),v)

let v_event names obj v =
	let target,action = v_event_parse names obj v in
	match action with	
	| "destroy" -> let c = (a_component target) in (fun () -> c#destroy)
	| "close" -> let c = (a_window target) in (fun () -> c#close)
	| "cancel" -> (fun () -> raise Cancel)
	| "ok" -> (fun () -> raise Ok)
	| _ -> if (String.get v 0) = '%' then begin 
					let event = (String.sub v 1 ((String.length v)-1)) in
					try
						!events_func event
					with
						Not_found -> failwith ("Custom Event not found : "^event);
				end else raise Not_found

let mtry funcs v =
	let f = (List.find (fun f -> try (f v); true with Not_found -> false) funcs) in ()

let init_event names obj att aval =
	let v_event = v_event names in
	let any = (x_any obj) in	
	match att with
	| "on_click" -> mtry [
		(fun f -> (a_menuitem any)#on_click f);
		(fun f -> (a_window any)#on_click f);
		(fun f -> (a_button any)#on_click f);
		(fun f -> (a_listbox any)#on_click f);
		(fun f -> (a_combobox any)#on_click f);
		(fun f -> (a_toggle any)#on_click f);
		(fun f -> (a_label any)#on_click f);
		(fun f -> (a_edit any)#on_click f);
		] (v_event obj aval);
	| "on_keybreturn" -> 
		let double = (fun f k -> if k = keys.k_enter then f()) in
		mtry [
		(fun f -> (a_window any)#on_key_down (double f) );
		(fun f -> (a_panel any)#on_key_down (double f) );
		(fun f -> (a_listbox any)#on_key_down (double f) );
		(fun f -> (a_combobox any)#on_key_down (double f) );
		(fun f -> (a_label any)#on_key_down (double f) );
		(fun f -> (a_edit any)#on_key_down (double f) );
		] (v_event obj aval);
	| "on_keybesc" -> 
		let double = (fun f k -> if k = keys.k_esc then f()) in
		mtry [
		(fun f -> (a_window any)#on_key_up (double f) );
		(fun f -> (a_panel any)#on_key_up (double f) );
		(fun f -> (a_listbox any)#on_key_up (double f) );
		(fun f -> (a_combobox any)#on_key_up (double f) );
		(fun f -> (a_label any)#on_key_up (double f) );
		(fun f -> (a_edit any)#on_key_up (double f) );
		] (v_event obj aval);
	| _ -> raise Not_found

(********************************************************************************)
(* xgui Functions *)	
		
let init_attrib names x any att aval =
	let v_eval = v_eval names in
	try
	match att with
		| "name" -> (Hashtbl.add names aval any)
		| "x" -> (a_component any)#x (v_int aval)
		| "y" -> (a_component any)#y (v_int aval)
		| "width" -> (a_component any)#width (v_int aval)
		| "height" -> (a_component any)#height (v_int aval)
		| "visible" -> (a_component any)#visible (v_bool aval)
		| "enable" -> mtry [(fun x -> (a_component any)#enable x);(fun x -> (a_menuitem any)#enable x)] (v_bool aval)
		| "caption" -> mtry [(fun x -> (a_component any)#caption x);(fun x -> (a_menuitem any)#caption x)] aval
		| "align" -> (a_component any)#align (v_align aval)
		| "check" -> (a_menuitem any)#check (v_bool aval)
		| "submenu" -> (a_menuitem any)#submenu (Some (a_menu (v_eval aval)))
		| "span" -> (a_container any)#span (v_int aval)
		| "space" -> (a_container any)#space (v_int aval)
		| "state" -> (try (a_window any)#state (v_wstate aval); with Conversion -> (a_toggle any)#state (v_bool aval))
		| "focus" -> (a_action any); (a_component (v_eval aval))#focus
		| "border" -> (a_panel any)#border (v_border aval); (a_panel any)#visible true;
		| "multiselect" -> (a_listbox any)#multiselect (v_bool aval);
		| "bold" -> (a_component any)#font#bold (v_bool aval);
		| "italic" -> (a_component any)#font#italic (v_bool aval);
		| "fontsize" -> (a_component any)#font#size (v_int aval);
		| "fontface" -> (a_component any)#font#face aval;
		| "tabsize" -> (a_richedit any)#tabsize (v_int aval)
		(* stringlist for combo/listbox *)
		| _ -> if (String.length att) > 3 && String.sub att 0 3 = "on_" then init_event names (x,any) att aval else raise Not_found
	with
		Not_found -> failwith ("XGui "^(name x)^" got an invalid attribute "^att)
		| Conversion -> failwith ("XGui "^(name x)^" : "^aval^" is an invalid attribute value for "^att)		

let rec do_create names x parent =
	match parent with
		None -> create_parent names x
		| Some p -> create_child names x p
and
	init_object names x any =
		List.iter (fun (att,aval) -> init_attrib names x any att aval) (Xml.attribs x);
		let obj = (x,any) in
			(List.iter (fun c -> ignore(do_create names c (Some obj))) (Xml.children x));
			obj
and
	create_parent names x =
		let any =
			match Xml.tag x with
			| "window" -> Window (new_window())
			| "menu" -> Menu (new_menu())
			| _ -> failwith ("XGui cannot create "^(name x)^" because it requires a parent") in
		(init_object names x any);
and
	create_child names x p =
		if not(is_child_component x) then failwith ("XGui cannot create "^(name x)^" because it cannot have a parent");
		let parent = (try a_parent (x_any p) with Not_found -> failwith ("XGui "^(name (x_xml p))^" cannot be a parent")) in
		let any =
		try
			match Xml.tag x with
			| "menuitem" -> Menuitem (new_menuitem (p_menu parent))
			| "panel" -> Panel (new_panel (p_container parent))
			| "button" -> Button (new_button (p_container parent))
			| "listbox" -> Listbox (new_listbox (p_container parent))
			| "combobox" -> Combobox (new_combobox (p_container parent))
			| "radiobutton" -> Radiobutton (new_radiobutton (p_container parent))
			| "checkbox" -> Checkbox (new_checkbox (p_container parent))
			| "label" -> Label (new_label (p_container parent))
			| "edit" -> Edit (new_edit (p_container parent))
			| "richedit" -> Richedit (new_richedit (p_container parent))
			| "treeview" -> Treeview (new_treeview (p_container parent))
			| "menu" -> Menu (new_menu())
			| "action" -> Action
			| "group" -> Group parent
			| "menubreak" -> let m = new_menuitem (p_menu parent) in m#break true; Noaccess

			| name -> failwith ("Unknown XGui identifier "^name)
		with
			Not_found -> failwith ("XGui "^(name x)^" has an invalid parent "^(name (x_xml p)))
		in
		(init_object names x any)

let conv_parent p =
	match p with
		| None -> None
		| Some p ->
			Some (match p with
			| XPanel p -> ( Element("panel",[],[]), Panel p)
			| XWindow w -> ( Element("window",[],[]), Window w)
			| XMenu m -> ( Element("menu",[],[]), Menu m))

let create parent x =
	let names = (Hashtbl.create 0) in
	let xparent = conv_parent parent in
	let obj = do_create names x xparent in
	let classobj = new ixobj (x_any obj) names in
	(try
		let c = a_component (x_any obj) in
		c#visible true;
	with
		Not_found -> ());
	(classobj:>xobj)		

let create_in x name xml =
	let realx = (Obj.magic x : ixobj) in
	let xparent = try a_parent (realx#any name) with Not_found -> failwith ("Component "^name^" cannot be a parent !") in
		ignore(do_create realx#names xml (conv_parent (Some xparent)))

(********************************************************************************)
