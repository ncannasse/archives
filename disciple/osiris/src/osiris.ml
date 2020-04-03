(********************************************************************************)
(*                                                                              *)
(* Osiris : OCaml Visual Components - Copyright (c)2002 Nicolas Cannasse		*)
(* version 1.2                                                                  *)
(*                                                                              *)
(* http://tech.motion-twin.com													*)
(* version 1.2                                                                  *)
(*                                                                              *)
(********************************************************************************)
(* Includes *)
open Win32

(********************************************************************************)
(* Interface *)

type handle = Win32.hwnd
type menuhandle = Win32.hmenu
type richcolor = Win32.color
type hres = {
	from : Win32.himagelist;
	index : int;
	data : Win32.handle;
}

type hresources = Win32.himagelist

type richeffect =
	| FE_AUTOCOLOR
	| FE_BOLD
	| FE_ITALIC
	| FE_STRIKEOUT
	| FE_UNDERLINE
	| FE_PROTECTED

type stringtype = ST_ALPHANUM | ST_SPACES | ST_OTHER
type show_window_state = Maximize | Minimize | Normal
type align = AlNone | AlLeft | AlRight | AlTop | AlBottom | AlClient | AlCenter
type border = BdNone | BdSingle | BdNormal
type question_result = QR_YES | QR_NO | QR_CANCEL

type sel = {
	mutable smin : int;
	mutable smax : int
}

type keys = {
	k_del : int;
	k_enter : int;
	k_esc : int;
	k_fun : int
}

type richformat = {
	rf_effects : richeffect list;
	rf_height : int option;
	rf_color : richcolor option;
	rf_fontface : string option;
}

type event = (unit -> unit) -> unit
type event_bool = (unit -> bool) -> unit
type event_key = (int -> unit) -> unit

class type font =
  object
	method face : string -> unit
	method size : int -> unit
	method bold : bool -> unit
	method italic : bool -> unit
	(* color : GDI low-level *)
  end

class type mouse_events =
  object
	method on_click : event
	method on_dblclick : event
	method on_rdblclick : event
	method on_mdblclick : event
	method on_ldown : event
	method on_rdown : event
	method on_mdown : event
	method on_lup : event
	method on_rup : event
	method on_mup : event
	method on_mousemove : (int -> int -> unit) -> unit
  end

class type key_events =
  object	
	method on_key_down : event_key
	method on_key_up : event_key
  end

class type virtual component =
  object
	method handle : handle
	method parent : container option
	method visible : bool -> unit
	method is_visible : bool
	method enable : bool -> unit
	method is_enabled : bool
	method caption : string -> unit
	method get_caption : string
	method x : int -> unit
	method get_x : int
	method y : int -> unit
	method get_y : int
	method width : int -> unit
	method get_width : int
	method height : int -> unit
	method get_height : int
	method align : align -> unit
	method get_align : align
	method component : component
	method destroy : unit
	method is_destroyed : bool
	method font : font
	method focus : unit

	method set_custom : (unit -> unit) -> unit
	method call_custom : unit
  end
and
 virtual container =
  object
	inherit component
	method span : int -> unit
	method space : int -> unit
	method container : container
  end

class type menu =
  object
	method handle : menuhandle
	method destroy : unit
	method popup : unit
  end

class type menuitem =
  object
	method caption : string -> unit
	method get_caption : string
	method parent : menu
	method enable : bool -> unit
	method is_enabled : bool
	method check : bool -> unit
	method is_checked : bool
	method break : bool -> unit
	method is_break : bool
	method submenu : menu option -> unit
	method get_submenu : menu option	
	method on_click : event
  end

class type window =
  object
	inherit container
	inherit mouse_events
	inherit key_events

	method process : bool -> bool
	method loop : unit
	method close : unit

	method state : show_window_state -> unit
	method get_state : show_window_state
	method on_top : bool -> unit

	method menu : menu	
	method on_closequery : event_bool
	method on_destroy : event
	method on_resize : event
	method on_move : event
  end

class type panel =
  object
	inherit container
	inherit mouse_events
	inherit key_events

	method clear : unit
	method border : border -> unit
	method get_border : border
  end

class type button =
  object
	inherit component
	inherit mouse_events
  end

class type ['a] anylist =
  object
	inherit component
	inherit mouse_events
	inherit key_events

	method list : 'a list
	method set_list : 'a list -> unit
	method printer : ('a -> string) -> unit
	method equal : ('a -> 'a -> bool) -> unit
	method sort : ('a -> 'a -> int) option -> unit

	method redraw : unit

	method selected : 'a option
	method set_selected : 'a option -> unit

	method on_selchange : event
  end

class type ['a] listbox =
  object
	inherit ['a] anylist
	method multiselect : bool -> unit
	method is_multiselect : bool
	method top_index : int
	method set_top_index : int -> unit
	method selection : 'a list
	method set_selection : 'a list -> unit 
  end

class type ['a] combobox =
  object
	inherit ['a] anylist
  end

class type virtual toggle =
  object
	inherit component
	inherit mouse_events

	method state : bool -> unit
	method get_state : bool
	method on_statechange : event
	method toggle : toggle
  end

class type radiobutton =
  object
	inherit toggle
  end

class type checkbox =
  object
	inherit toggle
  end

class type label =
  object
	inherit component
	inherit mouse_events
	inherit key_events
  end

class type edit =
  object
	inherit component
	inherit mouse_events
	inherit key_events

	method on_change : event
	method undo : unit	
  end

class type splitter =
  object
	method pos : int
	method set_pos : int -> unit
	method on_move : event
  end

class type richedit =
  object
	inherit component

	method tabsize : int -> unit
	method wordwrap : bool -> unit
	method selection : sel -> unit
	method get_selection : sel
	method get_tabsize : int
	method get_wordwrap : bool

	method line_from_char : int -> int
	method char_from_line : int -> int
	method set_format : bool -> richformat -> unit
	method get_format : bool -> richformat
	method get_text : sel -> string

	method colorize : (stringtype -> string -> int -> richcolor option) -> sel option -> unit

	method on_change : event

	method undo : unit
	method redo : unit
  end


class type resources =
  object
	method add_source : string -> unit

	method get_icon : string -> hres
	method get_bitmap : string -> hres

	method handle : hresources
  end

class type ['a] treeview =
  object
	inherit component
	inherit mouse_events
	inherit key_events

	method tree : 'a option
	method set_tree : 'a option -> unit
	method printer : ('a -> string * hres option * 'a list) -> unit
	method equal : ('a -> 'a -> bool) -> unit
	method sort : ('a -> 'a -> int) option -> unit

	method redraw : unit

	method icons : resources option
	method set_icons : resources option -> unit
	method on_selchange : event

	method selected : 'a option
	method set_selected : 'a option -> unit
	method hittest : int -> int -> 'a option
	method tree_parent : 'a -> 'a option

  end

(********************************************************************************)
(********************************************************************************)
(********************************************************************************)
(* General *)

let no_event _ = ()
let no_event_2 _ _ = ()
let event_true _ = true
let event_false _ = false
let mouse_x = ref 0
let mouse_y = ref 0

let raised_exn = ref None
let global_id_count = ref 1
let alloc_id () = let id = !global_id_count in incr global_id_count; id

let normal_cursor = get_cursor()
let horizontal_cursor = load_cursor ~inst:null_handle ~name:(resource_name_of_standard IDC_SIZENS)
let vertical_cursor = load_cursor ~inst:null_handle ~name:(resource_name_of_standard IDC_SIZEWE)

let ( += ) a b = a := !a + b
let ( -= ) a b = a := !a - b

let set_window_handlers wnd processors =
	let base = get_window_proc wnd in
	let basesafe = Wp_function (fun ~wnd ~msg -> try apply_window_proc ~proc:base ~wnd ~msg with Win32.Error _ -> Int32.zero) in
	let proc = subclassing_window_proc basesafe processors in
	ignore(set_window_proc wnd proc)

let cnv_show s =
	match s with
	| Minimize -> SW_SHOWMINIMIZED
	| Maximize -> SW_SHOWMAXIMIZED
	| Normal -> SW_SHOWNORMAL

let style_of_border b =
	match b with
	| BdNone -> []
	| BdSingle -> [WS_BORDER]
	| BdNormal -> [WS_BORDER;WS_EX_CLIENTEDGE]

(********************************************************************************)
(* Fonts *)

class virtual ifont_listener =
  object
	method virtual font_refresh : hfont -> unit
  end

class ifont (parent:ifont_listener) =
  object(this)
	val mutable handle = null_handle
	val mutable face = "MS Sans Serif"
	val mutable size = 8
	val mutable bold = false
	val mutable italic = false

	method private alloc_font =
		if not (is_null_handle handle) then delete_object handle;
		let info = {
			lf_height = (-size);
			lf_width = 0;
			lf_escapement = 0;
			lf_orientation = 0;
			lf_weight = if bold then fw_bold else fw_normal;
			lf_italic = italic;
			lf_underline = false;
			lf_strike_out = false;
			lf_char_set = Default_charset;
			lf_out_precision = OUT_DEFAULT_PRECIS;
			lf_clip_precision = [];
			lf_quality = DEFAULT_QUALITY;
			lf_pitch = DEFAULT_PITCH;
			lf_family = FF_DONTCARE;
			lf_face_name = face
		} in
		create_font info;

	method refresh =
		handle <- this#alloc_font;
		parent#font_refresh handle;
	
	initializer
		this#refresh

	method face s = face <- s; this#refresh
	method size t = size <- t; this#refresh
	method bold b = bold <- b; this#refresh
	method italic b = italic <- b; this#refresh
  end

(********************************************************************************)
(* Menus *)

class imenuitem (parent:imenu) =
  object(this)
	val mutable submenu = None
	val mutable id = 0
	val mutable on_click = no_event
	val mutable break = false

	initializer
		id <- alloc_id();
		let infos = {
		    smii_item = Some {
				mi_contents = Mic_string "";
				mi_options = []; 
			};
			smii_state = None;
			smii_id = Some id;
			smii_submenu = None;
			smii_check_marks = None;
			smii_data = None
		} in
		insert_menu_item this#parent#handle id MF_BYCOMMAND infos;
		this#parent#add_item (this:>imenuitem)

	method private get_infos = 
		get_menu_item_info ~menu:this#parent#handle ~item:id ~typ:MF_BYCOMMAND ~max_length:100

	method private set_infos infos =
		set_menu_item_info this#parent#handle id MF_BYCOMMAND infos

	method enable b =
		let enablefunc = enable_menu_item ~menu:this#parent#handle ~item:id ~typ:MF_BYCOMMAND in
		enablefunc (if b then MF_ENABLED else MF_GRAYED)

	method is_enabled =
		let infos = this#get_infos in
		match infos.gmii_state.mis_enable with
		| MF_ENABLED -> true
		| _ -> false
				 
	method check b =
		check_menu_item ~menu:this#parent#handle ~item:id ~typ:MF_BYCOMMAND ~check:b

	method is_checked =
		let infos = this#get_infos in
		infos.gmii_state.mis_checked
	
	method break b =
		break <- b;		
		let infos = {
			smii_item = Some {
				mi_contents = if b then Mic_separator else Mic_string this#get_caption; 
				mi_options = [];
			};
			smii_state = None;
			smii_id = None;
			smii_submenu = None;
			smii_check_marks = None;
			smii_data = None
		} in
		this#set_infos infos

	method is_break = break

	method submenu (menu:menu option) =
		submenu <- (match menu with
			| None -> None 
			| Some m -> Some (Obj.magic m:imenu)); (* Downcast menu to imenu : we use imenu for friend calls *)
		let handle = match submenu with
			| None -> None
			| Some m -> Some m#handle in			
		let infos = {
		    smii_item = None;
			smii_state = None;
			smii_id = None;
			smii_submenu = handle;
			smii_check_marks = None;
			smii_data = None
		} in
		this#set_infos infos

	method get_submenu = submenu

	method id = id
	method activate = on_click ()

	method caption str =
		let infos = {
		    smii_item = Some {
				mi_contents = Mic_string str;
				mi_options = [];
			};
			smii_state = None;
			smii_id = None;
			smii_submenu = None;
			smii_check_marks = None;
			smii_data = None;
		} in
		this#set_infos infos

	method get_caption =
		let infos = this#get_infos in
		match infos.gmii_item.mi_contents with
		| Mic_string str -> str
		| _ -> ""

	method process sid =
		if id = sid then begin this#activate; true; end else
		match submenu with
		| None -> false
		| Some m -> try m#process sid with Not_found -> false

	method parent = parent
	method on_click f = on_click <- f

  end
and imenu canpop handle =
  object(this)

    val mutable childs = Hashtbl.create 0

	method add_item (item:imenuitem) =
		Hashtbl.add childs item#id item

	method process id =		
		let found = ref 0 in
		Hashtbl.iter (fun key item -> if item#process id then found := 1) childs;
		if !found = 0 then raise Not_found;
		true;

	method destroy =
		destroy_menu ~menu:handle
				
	method popup =
		if not canpop then failwith "Cannot popup a window menu";
		let m = track_popup_menu ~menu:handle ~options:[] ~pt:{x= !mouse_x;y= !mouse_y;} ~wnd:(get_active_window()) ~rect:None in
		match m with
		| None -> ()
		| Some id -> ignore(this#process id)

	method handle = handle
  end

let new_menuitem parent =
	(* Downcast menu to imenu : we use imenu for friend calls *)
	(new imenuitem (Obj.magic parent:imenu) : imenuitem :> menuitem)

let new_imenu canpop = new imenu canpop (Win32.create_menu())
let new_menu() = (new_imenu true : imenu :> menu)

(********************************************************************************)
(* Events *)
(*------------------------------------------------------------------------------*)
(*-- Mouse --*)

class imouse_events =
  object
	val mutable on_click = no_event
	val mutable on_dblclick = no_event
	val mutable on_rdblclick = no_event
	val mutable on_mdblclick = no_event
	val mutable on_ldown = no_event
	val mutable on_mdown = no_event
	val mutable on_rdown = no_event
	val mutable on_lup = no_event
	val mutable on_mup = no_event
	val mutable on_rup = no_event
	val mutable on_mousemove = (fun _ _ -> ())

	method on_click func =	on_click <- func
	method on_dblclick func = on_dblclick <- func
	method on_rdblclick func = on_rdblclick <- func
	method on_mdblclick func = on_mdblclick <- func
	method on_ldown func = on_ldown <- func
	method on_mdown func = on_mdown <- func
	method on_rdown func = on_rdown <- func
	method on_lup func = on_lup <- func
	method on_mup func = on_mup <- func
	method on_rup func = on_rup <- func
	method on_mousemove func = on_mousemove <- func

	method private mouse_handle msg (wparam:int) (lparam:int32) =
			 if msg = wm_lbuttondown then begin on_ldown(); true; end
		else if msg = wm_mbuttondown then begin on_mdown(); true; end
		else if msg = wm_rbuttondown then begin on_rdown(); true; end
		else if msg = wm_lbuttonup then begin on_lup(); on_click(); true; end
		else if msg = wm_mbuttonup then begin on_mup(); true; end
		else if msg = wm_rbuttonup then begin on_rup(); true; end
		else if msg = wm_lbuttondblclk then begin on_dblclick(); true; end
		else if msg = wm_mbuttondblclk then begin on_mdblclick(); true; end
		else if msg = wm_rbuttondblclk then begin on_rdblclick(); true; end
		else if msg = wm_mousemove then begin on_mousemove wparam (Int32.to_int lparam); true end
		else false
  end

(*------------------------------------------------------------------------------*)
(*-- Keys --*)

class ikey_events =
  object
	val mutable on_key_down = no_event
	val mutable on_key_up = no_event

	method on_key_down func = on_key_down <- func
	method on_key_up func = on_key_up <- func
	
	method private key_handle msg (wparam:int) (lparam:int32) =
			 if msg = wm_keydown then begin on_key_down wparam; true end
		else if msg = wm_keyup then begin on_key_up wparam; true end
		else false
  end

(********************************************************************************)
(* Component Funcs *)

let current_component = ref None

let get_component hwnd =
	current_component := None;
	do_window_action hwnd;
	match !current_component with
	| Some c -> c;
	| None -> assert false


(********************************************************************************)
(* Handlers *)

let protect f p =
	(try f p with e -> raised_exn := Some e);
	message_not_handled

let protect_handled f p =
	(try f p with e -> raised_exn := Some e);
	message_handled

let def_handler classname event wparam lparam =
		 if event = wm_destroy then ()
	else if event = wm_move then ()
	else if event = wm_size then ()
	else begin
		Printf.printf "%s Event : 0x%.4X (%d,%d)\n" classname event wparam (Int32.to_int lparam);
		flush stdout
	end

let pass wnd msg =
	(get_component wnd)#handle_event msg.mc_id
		(Int32.to_int msg.mc_wparam) msg.mc_lparam
	
let mouse_pass ~wnd ~msg ~modifiers ~pt =
	protect (pass wnd) msg

let window_handlers = [
	on_wm_destroy (fun ~wnd ~msg ->
		(get_component wnd)#do_destroy;
		message_not_handled
	);
	on_wm_command (fun ~wnd ~msg ~notify_code ~id ~ctrl ->
		protect (fun () ->
			if is_null_hwnd ctrl then
				(get_component wnd)#handle_event wm_command notify_code (Int32.of_int id)
			else begin
				current_component := None;
				do_window_action ctrl;
				(match !current_component with
				| Some c -> c;
				| None -> (get_component wnd))#handle_event wm_command notify_code msg.mc_lparam;
			end) ();
	);
    on_wm_size (fun ~wnd ~msg ~typ ~size ->
		(get_component wnd)#on_move_resize;
		protect (pass wnd) msg;
	);
	on_wm_move (fun ~wnd ~msg ~pt ->
		(get_component wnd)#on_move_resize;
		protect (pass wnd) msg;		
	);
	on_wm_close (fun ~wnd ~msg -> protect_handled (pass wnd) msg);
	on_wm_keydown (fun ~wnd ~msg ~key ~data -> protect (pass wnd) msg);
	on_wm_keyup (fun ~wnd ~msg ~key ~data -> protect (pass wnd) msg);
	on_wm_mousemove (fun ~wnd ~msg ~modifiers ~pt ->
		let p = client_to_screen wnd pt in
		mouse_x := p.x;
		mouse_y := p.y;
		protect (fun () -> (get_component wnd)#handle_event wm_mousemove pt.x (Int32.of_int pt.y)) ();
	);

	on_wm_notify (fun ~wnd ~msg ~id ~hdr ->
		protect (fun () ->
			let ctrl = hdr.nmh_from_wnd in
			let notify_code = hdr.nmh_code in
			if is_null_hwnd ctrl then
				(get_component wnd)#handle_event wm_notify notify_code (Int32.of_int id)
			else begin
				current_component := None;
				do_window_action ctrl;
				(match !current_component with
				| Some c -> c;
				| None -> (get_component wnd))#handle_event wm_notify notify_code msg.mc_lparam;
			end) ()
	);

	on_wm_lbuttondown mouse_pass;
	on_wm_mbuttondown mouse_pass;
	on_wm_rbuttondown mouse_pass;
	on_wm_lbuttonup mouse_pass;
	on_wm_mbuttonup mouse_pass;
	on_wm_rbuttonup mouse_pass;
	on_wm_lbuttondblclk mouse_pass;
	on_wm_mbuttondblclk mouse_pass;
	on_wm_rbuttondblclk mouse_pass;
]

(********************************************************************************)
(* Component *)

class virtual icomponent =
  object(this)
	inherit ifont_listener

	val mutable font = None
	val mutable handle = null_hwnd
	val mutable align = AlNone
	val mutable parent = (None : icontainer option)
	val mutable custom = no_event
	val mutable lastcaption = "" (* prevent garbage collect *)
	val mutable classname = ""

	val mutable style = Int64.zero
	val mutable ex_style = Int64.zero
	val mutable visible = true

	method set_custom func = custom <- func
	method call_custom = custom()

	method handle = handle

	method parent =
		match parent with
		| None -> None
		| Some p -> Some (p:>container)
	
	method visible b =
		visible <- b;
		ignore( show_window ~wnd:handle ~show:(if b then SW_SHOW else SW_HIDE) );
		this#update_parent;

	method enable b =
		ignore (enable_window ~wnd:handle ~enable:b);
		this#update_parent;

	method caption v =
		lastcaption <- v;
		set_window_text ~wnd:handle ~text:v;
		this#update_parent;

	method align a = 
		align <- a;
		this#update_parent;

	method is_visible = visible
	method is_enabled = is_window_enabled ~wnd:handle
	method get_caption = get_window_text ~wnd:handle

	method x v = this#set_client_pos v this#get_y this#get_width this#get_height
	method y v = this#set_client_pos this#get_x v this#get_width this#get_height
	method width v = this#set_client_pos this#get_x this#get_y v this#get_height
	method height v = this#set_client_pos this#get_x this#get_y this#get_width v

	method get_x = 
		let r = get_window_rect ~wnd:handle in
		let p = (match parent with
			| None -> { x = r.left; y = r.top }
			| Some p -> screen_to_client ~wnd:p#handle ~pt:{ x = r.left; y = r.top })
		in
		p.x

	method get_y =
		let r = get_window_rect ~wnd:handle in
		let p = (match parent with
			| None -> { x = r.left; y = r.top }
			| Some p -> screen_to_client ~wnd:p#handle ~pt:{ x = r.left; y = r.top })
		in
		p.y

	method get_width = this#get_full_width
	method get_height = this#get_full_height

	method get_align = align
	method component = (this :> component)
	method icomponent = (this :> icomponent)

	method font = 
		match font with
		| None -> assert false
		| Some f -> (f :> font)

	method focus =
		ignore(set_focus ~wnd:handle);

	method destroy =
		destroy_window ~wnd:handle;

	method is_destroyed =
		is_null_hwnd handle

	(* ----- internal ,  hidden ---------- *)

	method get_full_width =
		let r = get_window_rect ~wnd:handle in
		r.right - r.left

	method get_full_height =
		let r = get_window_rect ~wnd:handle in
		r.bottom - r.top

	method get_client_width =
		let r = get_client_rect ~wnd:handle in
		r.right

	method get_client_height =
		let r = get_client_rect ~wnd:handle in
		r.bottom

	method on_move_resize = ()

	method set_full_pos x y w h =
		move_window ~wnd:handle ~pt:{x=x;y=y;} ~size:{cx=w;cy=h;} ~repaint:true;

	method set_client_pos x y w h =
		this#set_full_pos x y w h

	method update_parent =
		match parent with
		| None -> this#update
		| Some p -> p#update_parent

	method do_destroy =
		handle <- null_hwnd;
		match parent with
		| None -> ()
		| Some p -> p#del_item this#component

	method private init_component width height cname pa pstyle =
		(match parent with
		| None -> ()
		| Some p -> p#del_item this#component);
		parent <- (match pa with
			| None -> None (* Downcast container to icontainer *)
			| Some p -> Some (Obj.magic p : icontainer));
		(match parent with
		| None -> ()
		| Some p -> p#add_item this#component);
		style <- pstyle;
		classname <- cname;
		handle <- create_window {
			cs_class_name = An_string classname;
			cs_window_name = "";
			cs_style = Int64.logor style (style_of_option WS_VISIBLE);
			cs_position = Some { x = 0; y = 0; };
			cs_size = Some { cx = width; cy = height; };
			cs_parent = (match parent with None -> null_hwnd | Some p -> p#handle);
			cs_menu = null_handle;
			cs_child_id = 0;
			cs_instance = null_handle;
			cs_param = null_pointer;
		};
		set_window_handlers handle window_handlers;
		set_window_action ~wnd:handle ~action:(fun () -> current_component := Some (this:>icomponent));
		font <- Some (new ifont (this:>ifont_listener))

	method private reset_component =
		let enable = this#is_enabled in
		let _visible = this#is_visible in
		let x = this#get_x and y = this#get_y
		and width = this#get_full_width
		and height = this#get_full_height in
		(* prevent logical removing from parent children list when destroy event received *)
		let oldp = parent in
		parent <- None;
		this#destroy;
		parent <- oldp;
		handle <- create_window {
			cs_class_name = An_string classname;
			cs_window_name = lastcaption;
			cs_style = if _visible then Int64.logor style (style_of_option WS_VISIBLE) else style;
			cs_position = Some { x = x; y = y; };
			cs_size = Some { cx = width; cy = height; };
			cs_parent = (match parent with None -> null_hwnd | Some p -> p#handle);
			cs_menu = null_handle;
			cs_child_id = 0;
			cs_instance = null_handle;
			cs_param = null_pointer;
		};
		set_window_handlers handle window_handlers;
		set_window_action ~wnd:handle ~action:(fun () -> current_component := Some (this:>icomponent));
		visible <- _visible;
		this#enable enable;
		match font with
		| None -> assert false
		| Some f -> f#refresh;

	method private add_style s =
		style <- Int64.logor style s;

	method private remove_style s =
		style <- Int64.logand style (Int64.sub Int64.max_int s)

	method private update_style =
		let style = (if visible then Int64.logor style (style_of_option WS_VISIBLE) else style) in
		ignore( set_window_style ~wnd:handle ~style );
		let w = this#get_full_width and h = this#get_full_height in
		set_window_pos  {
		    wp_hwnd = handle;
			wp_z_order = Zo_notopmost;
			wp_position = { x=0; y=0; };
			wp_size = { cx=0; cy=0; };
			wp_options = [SWP_NOMOVE;SWP_NOSIZE;SWP_NOZORDER;SWP_FRAMECHANGED]
		};
		this#set_full_pos this#get_x this#get_y w h

	method font_refresh h =
		send ~wnd:handle ~issuer:issue_wm_setfont ~font:h ~redraw:true;

	method update = ()

	method virtual handle_event : int -> int -> int32 -> unit

end

(********************************************************************************)
(* Container *)

and virtual icontainer =
  object(this)
	inherit icomponent as super

	val items = ref []
	val mutable span = 0
	val mutable space = 0

	method add_item item =
		items := !items @ [(Obj.magic item : icomponent)]
				
	method del_item item =
		let rec remove = function
			| [] -> []
			| x :: l when (x :> component) = item -> l
			| x :: l -> x :: (remove l)
		in
		items := remove !items
	
	method span v = span <- v
	method space v = space <- v

	method clear =
		List.iter (fun it -> it#destroy) !items

	(* containers size is client area size *) 

	method get_width = this#get_client_width
	method get_height = this#get_client_height
	
	method set_full_pos x y w h =
		super#set_full_pos x y w h;
		this#update_items

	method set_client_pos x y width height =
		let rc = get_client_rect ~wnd:handle in
		let rw = get_window_rect ~wnd:handle in
		let w = (rw.right - rw.left) - rc.right + width in
		let h = (rw.bottom - rw.top) - rc.bottom + height in
		this#set_full_pos x y w h

	(* ------- *) 

	method previous (p:icomponent) =
		let prev = ref p in
		if not (List.exists (fun x -> if x == p then true else begin prev := x; false; end) !items) then None else if !prev == p then None else Some !prev

	method update_items =
		let width = this#get_client_width and height = this#get_client_height in
		let curw = ref (width-span*2) and curh = ref (height-span*2)
		and curx = ref span and cury = ref span	in
		List.iter (fun it ->
			if not it#is_visible then it#update else
			match it#get_align with
			| AlNone -> it#update
			| AlLeft ->
				let itw = it#get_full_width in
				it#set_full_pos !curx !cury itw !curh;
				if !curw >= itw+space then begin curx += (itw+space); curw -= (itw+space); end
				else begin curx += (!curw+space); curw := 0; end
			| AlRight ->
				let itw = it#get_full_width in
				it#set_full_pos (!curw + !curx - itw) !cury itw !curh;
				if !curw >= itw+space then begin curw -= (itw+space); end
				else begin curw := 0; end
			| AlTop ->
				let ith = it#get_full_height in
				it#set_full_pos !curx !cury !curw ith;
				if !curh >= ith+space then begin cury += (ith+space); curh -= (ith+space); end
				else begin curh := 0; end
			| AlBottom ->
				let ith = it#get_full_height in
				it#set_full_pos !curx (!curh + !cury - ith) !curw ith;
				if !curh >= ith+space then begin curh -= (ith+space); end
				else begin curh := 0; end
			| AlClient ->
				it#set_full_pos !curx !cury !curw !curh;
				curw := 0;
				curh := 0;
			| AlCenter ->
				let ith = it#get_full_height in
				let itw = it#get_full_width in
				let dx = (!curw-itw)/2 in
				let dy = (!curh-ith)/2 in
				it#set_full_pos (!curx+dx) (!cury+dy) itw ith;
		) !items

	method container = (this :> container)

  end

(********************************************************************************)
(* Window *)

let wnd_class = ref false

class iwindow =
  object(this)
	inherit icontainer
	inherit imouse_events
	inherit ikey_events
	
	val menu = new_imenu false
	val mutable on_closequery = event_true
	val mutable on_destroy = no_event
	val mutable on_resize = no_event
	val mutable on_move = no_event

	(* other applications can hide us, we are thus not
	   relying on Osiris internal flag but on Win32API
	   call. *)
	method is_visible = is_window_visible ~wnd:handle

	method on_move_resize = this#update

    method update =
		this#update_items;
		ignore( update_window ~wnd:handle );
		draw_menu_bar ~wnd:handle;

	initializer
		let classname = "stdwndclass" in
		if not !wnd_class then begin
			let wc = {
				wc_class_name = An_string classname;
				wc_wnd_proc = Wp_function def_window_proc;
				wc_style = [ CS_VREDRAW; CS_HREDRAW ];
				wc_instance = null_handle;
				wc_icon = null_handle;
				wc_cursor = null_handle;
				wc_background = Cb_sys_color COLOR_APPWORKSPACE;
				wc_menu_name = None;
				wc_icon_sm = null_handle;
				wc_cls_extra = 0;
				wc_wnd_extra = 0
			} in
			ignore (register_class ~wc);
			wnd_class := true;
		end;
		this#init_component 200 300 classname None
			(style_of_options [WS_OVERLAPPEDWINDOW;WS_CLIPCHILDREN]);		
		this#visible false;
		ignore (set_menu ~wnd:handle ~menu:menu#handle);		
	
	method menu = (menu :> menu)

	method process block = 
		if block || peek_message ~wnd:None ~msg_filter:None ~options:[PM_NOREMOVE] <> None then begin
			let (ok, msg) = get_message ~wnd:None ~msg_filter:None in
			if ok then begin
				ignore (translate_message ~msg);
				ignore (dispatch_message ~msg);
				(match !raised_exn with
				| None -> ()
				| Some e ->
					raised_exn := None;
					raise e);
				not(is_null_hwnd handle);
			end else false
		end else true

	method loop =
		while this#process true do
			();
		done

	method close =
		if on_closequery() then destroy_window ~wnd:handle

	method handle_event event wparam lparam =
			 if event = wm_command then ignore(menu#process (Int32.to_int lparam))
		else if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_move then on_move()
		else if event = wm_size then on_resize()
		else if event = wm_destroy then on_destroy()
		else if event = wm_close then this#close
		else def_handler "Window" event wparam lparam;

	method state s =
		ignore (show_window ~wnd:handle ~show:(cnv_show s));

	method get_state =
		let p = get_window_placement ~wnd:handle in
		match p.wpl_show_cmd with
		| SW_SHOWMAXIMIZED -> Maximize
		| SW_SHOWMINIMIZED -> Minimize
		| _ -> Normal

	method on_top b =
		set_window_pos {
		    wp_hwnd = handle;
			wp_z_order = (if b then Zo_topmost else Zo_notopmost);
			wp_position = { x=0; y=0; };
			wp_size = { cx=0; cy=0; };
			wp_options = [SWP_NOMOVE;SWP_NOSIZE]
		};

	method on_destroy func = on_destroy <- func
	method on_closequery func = on_closequery <- func
	method on_move func = on_move <- func
	method on_resize func = on_resize <- func
  end

let new_window () = (new iwindow : iwindow :> window)

(********************************************************************************)
(* Panel *)

class ipanel pa =
  object(this)
	inherit icontainer
	inherit imouse_events
	inherit ikey_events
	
	val mutable border = BdNormal

	initializer
		this#init_component 200 200 "STATIC" (Some pa)
			(static_style_of_options (WS_CHILD :: (style_of_border border),[SS_CENTER;SS_NOTIFY]));

	method update =
		this#update_items

	method border b =
		this#remove_style (style_of_options (style_of_border border));
		this#add_style (style_of_options (style_of_border b));
		border <- b;		
		this#update_style;

	method get_border = border

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = stn_clicked then ()
		else if event = wm_command && wparam = stn_dblclk then ()
		else if event = wm_command && wparam = stn_enable then ()
		else if event = wm_command && wparam = stn_disable then ()
		else def_handler "Panel" event wparam lparam;
	
  end

let new_panel parent = (new ipanel parent : ipanel :> panel)

(********************************************************************************)
(* Splitter *)

class isplitter pa =
  object(this)
	inherit icomponent
	val mutable horizontal = true
	val mutable moving = false
	val mutable move_pos = 0
	val mutable move_capture = null_hwnd
	val mutable on_move = (no_event : unit -> unit)

	initializer
		this#init_component 4 4 "STATIC" (Some pa)
			(static_style_of_options ([WS_CHILD;WS_BORDER;WS_EX_CLIENTEDGE],[SS_NOTIFY]));

	method get_full_width = 6
	method get_full_height = 6

	method get_align =
		match parent with
		| None -> assert false
		| Some p ->
			match p#previous this#icomponent with
			| None -> horizontal <- false; AlLeft
			| Some c ->
				match c#get_align with
				| AlLeft | AlRight -> horizontal <- false; c#get_align
				| AlTop  | AlBottom -> horizontal <- true; c#get_align
				| AlNone | AlCenter | AlClient -> horizontal <- false; AlLeft

	method update = ()

	method on_move f =
		on_move <- f

	method pos =
		if horizontal then this#get_y else this#get_x
	
	method set_pos newpos =
		if this#pos = newpos then 
			()
		else match parent with
		| None -> assert false
		| Some p ->
			match p#previous this#icomponent with
			| None -> ()
			| Some c when horizontal ->
				c#set_full_pos c#get_x c#get_y c#get_full_width (c#get_full_height - this#pos + newpos);
				this#update_parent	
			| Some c ->
				c#set_full_pos c#get_x c#get_y (c#get_full_width - this#pos + newpos) c#get_full_height;
				this#update_parent

	method handle_event event wparam lparam =
			 if event = wm_lbuttondown then begin moving <- true; move_capture <- set_capture handle; move_pos <- (if horizontal then this#get_y - !mouse_y else this#get_x - !mouse_x); end
		else if event = wm_mbuttondown then ()
		else if event = wm_rbuttondown then ()
		else if event = wm_lbuttonup then begin moving <- false; ignore(set_capture move_capture); end
		else if event = wm_mbuttonup then ()
		else if event = wm_rbuttonup then ()
		else if event = wm_lbuttondblclk then ()
		else if event = wm_mbuttondblclk then ()
		else if event = wm_rbuttondblclk then ()
		else if event = wm_mousemove then begin
			ignore(set_cursor (if horizontal then horizontal_cursor else vertical_cursor));
			if moving then this#set_pos (if horizontal then move_pos + !mouse_y else move_pos + !mouse_x);
		end
		else if event = wm_command && wparam = stn_clicked then ()
		else if event = wm_command && wparam = stn_dblclk then ()
		else if event = wm_command && wparam = stn_enable then ()
		else if event = wm_command && wparam = stn_disable then ()
		else def_handler "Splitter" event wparam lparam

  end

let new_splitter parent = (new isplitter parent : isplitter :> splitter)

(********************************************************************************)
(* Listbox *)

class ['a] ilistbox pa =
  object(this) 
	inherit icomponent
	inherit imouse_events
	inherit ikey_events

	val mutable on_selchange = no_event
	val mutable printer = (fun _ -> "<no printer>")
	val mutable curlist = ([] : 'a list)
	val mutable curitems = [||]
	val mutable curstrings = [||]
	val mutable equal = (=)
	val mutable sort = None
	val mutable mselect = false

	initializer
		this#init_component 150 200 "LISTBOX" (Some pa)
			(listbox_style_of_options ([WS_CHILD;WS_BORDER;WS_VSCROLL;WS_HSCROLL;WS_EX_CLIENTEDGE],
										[LBS_NOTIFY;LBS_HASSTRINGS;LBS_NOINTEGRALHEIGHT;LBS_WANTKEYBOARDINPUT]));
		this#redraw;

	method list = curlist

	method set_list l =
		curlist <- l;
		this#redraw

	method printer f =
		printer <- f;
		this#redraw

	method equal f =
		equal <- f;

	method sort f =
		sort <- f;
		this#redraw

	method multiselect b =
		mselect <- b;
		(if b then this#add_style else this#remove_style) (listbox_style_of_option LBS_MULTIPLESEL);
		this#reset_component

	method is_multiselect = mselect

	method update = ()

	method selected =
		if mselect then
			None 
		else 
			match send ~wnd:handle ~issuer:issue_lb_getcursel with
			| None -> None
			| Some n -> Some curitems.(n)

	method selection =
		let selitems = send ~wnd:handle ~issuer:issue_lb_getselitems ~max_count:10000 in
		let rec loop acc n =
			if n = -1 then acc else loop (curitems.(Array.unsafe_get selitems n) :: acc) (n-1)
		in
		loop [] (Array.length selitems - 1)

	method set_selection s =
		if mselect then begin
			this#set_selected None;
			List.iter (fun n -> this#set_selected (Some n)) s;
		end

	method private index_of p =
		let max = Array.length curitems in
		let rec loop n =
			if n = max then raise Not_found;
			if equal p (Array.unsafe_get curitems n) then n else loop (n+1);
		in
		loop 0

	method top_index =
		send ~wnd:handle ~issuer:issue_lb_gettopindex

	method set_top_index s =
		try
			send ~wnd:handle ~issuer:issue_lb_settopindex ~index:s
		with
			List_box_error _ -> ()

	method set_selected s =
		try
			match mselect, s with
			| true , None -> send ~wnd:handle ~issuer:issue_lb_setsel ~index:(-1) ~select:false
			| false, None -> send ~wnd:handle ~issuer:issue_lb_setcursel ~index:(-1)
			| true, Some p -> 
				let p = this#index_of p in
				send ~wnd:handle ~issuer:issue_lb_setsel ~index:p ~select:true
			| false, Some p ->
				let p = this#index_of p in
				send ~wnd:handle ~issuer:issue_lb_setcursel ~index:p
		with
			| List_box_error _ -> ()

	method redraw =
		let index = this#selected in
		let top = this#top_index in
		ignore(send ~wnd:handle ~issuer:issue_lb_resetcontent);
		curitems <- Array.of_list (match sort with None -> curlist | Some f -> List.sort f curlist);
		curstrings <- Array.map printer curitems;
		Array.iter (fun x -> ignore (send ~wnd:handle ~issuer:issue_lb_addstring ~str:x)) curstrings;
		this#set_selected index;
		this#set_top_index top

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = lbn_setfocus then ()
		else if event = wm_command && wparam = lbn_killfocus then ()
		else if event = wm_command && wparam = lbn_dblclk then ()
		else if event = wm_command && wparam = lbn_selcancel then ()
		else if event = wm_command && wparam = lbn_selchange then on_selchange()		
		else def_handler "ListBox" event wparam lparam;

	method on_selchange func = on_selchange <- func

  end

let new_listbox parent = (new ilistbox parent : 'a ilistbox :> 'a listbox)

(********************************************************************************)
(* ComboBox *)

class ['a] icombobox pa =
  object(this) 
	inherit icomponent as super
	inherit imouse_events
	inherit ikey_events
	val mutable on_selchange = no_event
	val mutable printer = (fun _ -> "<no printer>")
	val mutable curlist = ([] : 'a list)
	val mutable curitems = [||]
	val mutable curstrings = [||]
	val mutable equal = (=)
	val mutable sort = None
	val mutable drop = false

	initializer
		this#init_component 80 18 "COMBOBOX" (Some pa)
			(combobox_style_of_options ([WS_CHILD;WS_BORDER;WS_VSCROLL],[CBS_HASSTRINGS;CBS_DROPDOWNLIST]));
		this#redraw;

	method list = curlist

	method set_list l =
		curlist <- l;
		this#redraw

	method printer f =
		printer <- f;
		this#redraw

	method equal f =
		equal <- f;

	method sort f =
		sort <- f;
		this#redraw

	method update = ()

	method selected =
		match send ~wnd:handle ~issuer:issue_cb_getcursel with
		| None -> None
		| Some n -> Some curitems.(n)

	method private index_of p =
		let max = Array.length curitems in
		let rec loop n =
			if n = max then raise Not_found;
			if equal p (Array.unsafe_get curitems n) then n else loop (n+1);
		in
		loop 0

	method set_selected s =
		try
			match s with
			| None -> send ~wnd:handle ~issuer:issue_cb_setcursel ~index:(-1)
			| Some p ->
				let p = this#index_of p in
				send ~wnd:handle ~issuer:issue_cb_setcursel ~index:p
		with
			| Combo_box_error _ -> ()

	method redraw =
		let index = this#selected in
		ignore(send ~wnd:handle ~issuer:issue_cb_resetcontent);
		curitems <- Array.of_list (match sort with None -> curlist | Some f -> List.sort f curlist);
		curstrings <- Array.map printer curitems;
		Array.iter (fun x -> ignore (send ~wnd:handle ~issuer:issue_cb_addstring ~str:x)) curstrings;
		this#set_selected index;

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = cbn_setfocus then ()
		else if event = wm_command && wparam = cbn_killfocus then ()
		else if event = wm_command && wparam = cbn_dropdown then begin drop <- true; this#height 100; end
		else if event = wm_command && wparam = cbn_closeup then begin drop <- false; this#update_parent; end
		else if event = wm_command && wparam = cbn_selendcancel then ()
		else if event = wm_command && wparam = cbn_selchange then () 
		else if event = wm_command && wparam = cbn_selendok then on_selchange()
		else def_handler "ComboBox" event wparam lparam;

	method on_selchange func = on_selchange <- func

	method set_full_pos x y w h =
		if not drop then super#set_full_pos x y w h

  end

let new_combobox parent = (new icombobox parent : 'a icombobox :> 'a combobox)

(********************************************************************************)
(* Button *)

class ibutton pa =
  object(this)
	inherit icomponent
	inherit imouse_events
	inherit ikey_events

	initializer
		this#init_component 75 25 "BUTTON" (Some pa)
			(button_style_of_options ([WS_CHILD], [BS_PUSHBUTTON]));

	method update = ()
	
	method handle_event event wparam lparam =		
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = bn_clicked then ()
		else def_handler "Button" event wparam lparam;

	method activate = on_click()
  end

let new_button parent = (new ibutton parent : ibutton :> button)

(********************************************************************************)
(* Toggles *)

class virtual itoggle pa pstyle =
  object(this)
	inherit icomponent
	inherit imouse_events
	inherit ikey_events
	val mutable on_statechange = no_event 

	initializer
		this#init_component 75 25 "BUTTON" (Some pa)
			(button_style_of_options ([WS_CHILD], BS_NOTIFY::pstyle));

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = bn_clicked then on_statechange()
		else if event = wm_command && wparam = bn_dblclk then ()
		else if event = wm_command && wparam = bn_setfocus then ()
		else if event = wm_command && wparam = bn_killfocus then ()
		else def_handler "Toggle" event wparam lparam;

	method state b =		
		let s = if b then BST_CHECKED else BST_UNCHECKED in
		send ~wnd:handle ~issuer:issue_bm_setcheck ~state:s;		

	method get_state =
		let s = send ~wnd:handle ~issuer:issue_bm_getcheck in 
		match s with
		| BST_CHECKED -> true
		| _ -> false

	method on_statechange func = on_statechange <- func
	method update = ()

	method toggle = (this :> toggle)
  end

class iradiobutton pa =
  object
	inherit itoggle pa [BS_AUTORADIOBUTTON]
  end

class icheckbox pa =
  object
	inherit itoggle pa [BS_AUTOCHECKBOX]
  end

let new_radiobutton parent = (new iradiobutton parent : iradiobutton :> radiobutton)
let new_checkbox parent = (new icheckbox parent : iradiobutton :> checkbox)

(********************************************************************************)
(* Label *)

class ilabel pa =
  object(this)
	inherit icomponent
	inherit imouse_events
	inherit ikey_events

	initializer
		this#init_component 100 15 "STATIC" (Some pa)
			(static_style_of_options ([WS_CHILD],[SS_LEFT;SS_NOTIFY]));

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = stn_clicked then ()
		else if event = wm_command && wparam = stn_dblclk then ()
		else if event = wm_command && wparam = stn_enable then ()
		else if event = wm_command && wparam = stn_disable then ()
		else def_handler "Label" event wparam lparam;
	
  end

let new_label parent = (new ilabel parent : ilabel :> label)

(********************************************************************************)
(* Edit *)

class iedit pa =
  object(this)
	inherit icomponent
	inherit imouse_events
	inherit ikey_events
	val mutable on_change = no_event

	initializer
		this#init_component 80 22 "EDIT" (Some pa)
			(edit_style_of_options ([WS_CHILD;WS_BORDER;WS_EX_CLIENTEDGE],[ES_LEFT;ES_AUTOHSCROLL]));		

	method update = ()

	method on_change func = on_change <- func
	method undo =
		ignore (send ~wnd:handle ~issuer:issue_em_undo)

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = en_change then on_change()
		else if event = wm_command && wparam = en_setfocus then ()
		else if event = wm_command && wparam = en_killfocus then ()
		else if event = wm_command && wparam = en_update then ()
		else def_handler "Edit" event wparam lparam;
  end

let new_edit parent = (new iedit parent : iedit :> edit)

(********************************************************************************)
(* RichEdit *)
(*
class irichedit pa =
  object(this)
	inherit icomponent
	inherit imouse_events
	inherit ikey_events	
	val mutable wordwrap = false
	val mutable tabsize = 4
	val mutable on_change = no_event

	initializer
		this#init_component 180 220 "RICHEDIT20A" (Some pa)
			(edit_style_of_options ([WS_CHILD;WS_BORDER;WS_EX_CLIENTEDGE;WS_HSCROLL;WS_VSCROLL],[ES_MULTILINE;ES_AUTOHSCROLL]));	
		this#wordwrap wordwrap;
		this#tabsize tabsize;

	method update = ()

	method undo = ignore( send ~wnd:handle ~issuer:issue_em_undo );
	method redo = ignore( send ~wnd:handle ~issuer:issue_em_redo );
	method selection s = ignore( send ~wnd:handle ~issuer:issue_em_exsetsel ~range:{cpMin=s.smin;cpMax=s.smax} );

	method line_from_char pos = send ~wnd:handle ~issuer:issue_em_exlinefromchar ~pos
	method char_from_line line = send ~wnd:handle ~issuer:issue_em_lineindex ~line

	method get_wordwrap = wordwrap
	method get_tabsize = tabsize
	method get_selection = let s = send ~wnd:handle ~issuer:issue_em_exgetsel in {smin=s.cpMin;smax=s.cpMax}
	method get_text s =	send ~wnd:handle ~issuer:issue_em_gettextrange ~range:{cpMin=s.smin;cpMax=s.smax}	

	method tabsize t =
		tabsize <- t;
		let s = ref 0 in
		let f = {
			pf_numbering = None;
			pf_startindent = None;
			pf_rightindent = None;
			pf_offset = None;
			pf_alignment = None;
			pf_tabs = Some (List.map (fun _ -> incr s; !s*t*120) (Array.to_list (Array.make 32 0)));
		} in
		ignore( send ~wnd:handle ~issuer:issue_em_setparaformat ~pformat:f );

	method wordwrap b =
		wordwrap <- b;
		ignore( send ~wnd:handle ~issuer:issue_em_settargetdevice ~target:null_handle ~linewidth:(if b then 0 else 1));	

	method set_format b format =
		let cnv_effect e = (
			match e with
			| FE_AUTOCOLOR -> CFE_AUTOCOLOR
			| FE_BOLD -> CFE_BOLD
			| FE_ITALIC -> CFE_ITALIC
			| FE_STRIKEOUT -> CFE_STRIKEOUT
			| FE_UNDERLINE -> CFE_UNDERLINE
			| FE_PROTECTED -> CFE_PROTECTED
		) in
		let f = {
			cf_effects = List.map cnv_effect format.rf_effects;
			cf_height = format.rf_height;
			cf_offset = None;
			cf_textcolor = format.rf_color;
			cf_charset = Default_charset;
			cf_pitch = DEFAULT_PITCH;
			cf_family = FF_DONTCARE;
			cf_facename = format.rf_fontface;
		} in
		ignore( send ~wnd:handle ~issuer:issue_em_setcharformat ~style:(if b then SCF_SELECTION else SCF_ALL) ~format:f );

	method get_format b =
		let cnv_effect e = (
			match e with
			| CFE_AUTOCOLOR -> FE_AUTOCOLOR
			| CFE_BOLD -> FE_BOLD
			| CFE_ITALIC -> FE_ITALIC
			| CFE_STRIKEOUT -> FE_STRIKEOUT
			| CFE_UNDERLINE -> FE_UNDERLINE
			| CFE_PROTECTED -> FE_PROTECTED
		) in
		let f = send ~wnd:handle ~issuer:issue_em_getcharformat ~selection:b in
		{
			rf_effects = List.map cnv_effect f.cf_effects;
			rf_height = f.cf_height;
			rf_color = f.cf_textcolor;
			rf_fontface = f.cf_facename;
		}

	method colorize (func : stringtype -> string -> int -> color option) sel =
		let s = (match sel with
			| None -> {cpMin=0;cpMax=(-1);};
			| Some range -> {cpMin=range.smin;cpMax=range.smax;})
		in
		richedit_colorize ~func ~wnd:handle ~range:s;

	method on_change func = on_change <- func

	method handle_event event wparam lparam =
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_command && wparam = en_setfocus then ()
		else if event = wm_command && wparam = en_killfocus then ()
		else if event = wm_command && wparam = en_update then on_change()		
		else def_handler "Richedit" event wparam lparam;
  end

let new_richedit parent = (new irichedit parent : irichedit :> richedit)
*)
(********************************************************************************)
(* Icon Res *)

class iresources width height =
  object
	val mutable libs = []
	val imagelist = image_list_create ~size:{cx=width;cy=height} ~color_type:ILC_COLOR32 ~mask:false ~initial:1 ~grow:0

	method handle = imagelist

	method add_source name =
		try
			let h = load_library_ex name LOAD_LIBRARY_AS_DATAFILE in
			libs <- h::libs;
		with _ ->
			try
				let h = load_library_ex (Filename.dirname Sys.argv.(0) ^ "/" ^ name) LOAD_LIBRARY_AS_DATAFILE in
				libs <- h :: libs;
			with _ ->
				failwith ("Failed to load resource file : "^name)

	method get_icon name =
		try
			let h = ref None in
			ignore (List.find (fun lib -> try h := Some (load_icon lib (Rn_string name)); true; with Win32.Error _ -> false) libs);
			(match !h with
			| None -> assert false;
			| Some ico ->
				let index = image_list_add_icon imagelist ico in
				{ index = index; from = imagelist; data = ico; })
		with
			Not_found -> failwith ("No such icon resource "^name)

	method get_bitmap name =
		try
			let h = ref None in
			ignore (List.find (fun lib -> try h := Some (load_bitmap lib (Rn_string name)); true; with Win32.Error _ -> false) libs);
			(match !h with
			| None -> assert false;
			| Some bmp ->				
				let index = image_list_add imagelist bmp null_handle in
				{ index = index; from = imagelist; data = bmp; })
		with
			Not_found -> failwith ("No such bmp resource "^name)

  end

let new_resources ~width ~height = (new iresources width height : iresources :> resources)

(********************************************************************************)
(* TreeView *)

class ['a] itreeview pa =
  object(this)
	inherit icomponent
	inherit imouse_events
	inherit ikey_events

	val mutable printer = (fun _ -> "<no printer>",None,[])
	val mutable equal = (=)
	val mutable sort = None
	val mutable curtree = (None : 'a option)
	val mutable curitems = ([||] : 'a array)
	val mutable curtreeitems = [||]
	val mutable cursel = (None : 'a option)
	val mutable icons = (None : resources option)
	val mutable on_selchange = no_event

	initializer
		this#init_component 150 200 wc_treeview (Some pa)
			(treeview_style_of_options ([WS_CHILD;WS_BORDER;WS_EX_CLIENTEDGE;WS_HSCROLL;WS_VSCROLL],[TVS_HASBUTTONS;TVS_HASLINES;TVS_LINESATROOT;TVS_SHOWSELALWAYS;TVS_DISABLEDRAGDROP]));

	method update = ()

	method tree = curtree

	method tree_parent (x : 'a) =
		(None : 'a option)(*** TODO ***)

	method set_tree t =
		curtree <- t;
		this#redraw

	method printer f =
		printer <- f;
		this#redraw

	method equal f =
		equal <- f

	method sort f =
		sort <- f;
		this#redraw

	method private tree_of_item it = 
		let max = Array.length curitems in
		let rec loop n =
			if n = max then raise Not_found else if equal it (Array.unsafe_get curitems n) then Array.unsafe_get curtreeitems n else loop (n+1)
		in
		loop 0

	method private item_of_tree t =
		let max = Array.length curitems in
		let rec loop n =
			if n = max then None else if eq_htreeitem t (Array.unsafe_get curtreeitems n) then Some (Array.unsafe_get curitems n) else loop (n+1)
		in	
		loop 0

	method redraw =
		let sel = this#selected in
		ignore(send ~wnd:handle ~issuer:issue_wm_setredraw ~redraw:false);
		ignore(send ~wnd:handle ~issuer:issue_tvm_deleteitem ~item:root_htreeitem);
		let items = ref [] in
		let treeitems = ref [] in
		let rec loop parent t =
			let str , icon, childs = printer t in
			let h = send ~wnd:handle ~issuer:issue_tvm_insertitem ~parent ~location:Tvil_first
				~tvitem:{
					tvi_item = None;
					tvi_children = None;
					tvi_image = (match icon with None -> None | Some res -> Some (Ccii_index res.index));
					tvi_selected_image = (match icon with None -> None | Some res -> Some (Ccii_index res.index));
					tvi_state = Some {
						tvis_bold = Some false;
						tvis_cut = Some false;
						tvis_drophilited = Some false;
						tvis_expanded = Some true;
						tvis_expandedonce = Some false;
						tvis_selected = Some false;
						tvis_expandpartial = None;
						tvis_overlay = None;
						tvis_state_image = None;
					};
					tvi_text = (Some (Cct_text str));
				    tvi_lparam = None;
					tvi_integral = None;
				}
			in
			items := t :: !items;
			treeitems := h :: !treeitems;
			List.iter (loop h) (match sort with None -> childs | Some f -> List.sort f childs);
		in
		(match curtree with
		| None -> ()
		| Some t -> loop root_htreeitem t);
		curitems <- Array.of_list (List.rev !items);
		curtreeitems <- Array.of_list (List.rev !treeitems);
		ignore(send ~wnd:handle ~issuer:issue_wm_setredraw ~redraw:true);
		try
			this#set_selected sel
		with 
			Not_found -> ()

	method icons = icons

	method set_icons i =
		icons <- i;
		ignore(send ~wnd:handle ~issuer:issue_tvm_setimagelist ~which:TVSIL_NORMAL
			~iml:(match icons with
				| None -> null_handle
				| Some h -> h#handle));

	method on_selchange func = on_selchange <- func

	method handle_event event wparam lparam =
		if event = wm_lbuttondown then ignore(set_capture handle)
		else if event = wm_lbuttonup then release_capture();
			 if this#mouse_handle event wparam lparam then ()
		else if this#key_handle event wparam lparam then ()
		else if event = wm_notify && wparam = nm_setcursor then ()
		else if event = wm_notify && wparam = nm_customdraw then ()
		else if event = wm_notify && wparam = nm_setfocus then ()
		else if event = wm_notify && wparam = nm_killfocus then ()
		else if event = wm_notify && wparam = nm_dblclk then ()
		else if event = wm_notify && wparam = nm_click then ()
		else if event = wm_notify && wparam = nm_rclick then ()
		else if event = wm_notify && wparam = tvn_selchanging then ()
		else if event = wm_notify && wparam = tvn_beginrdrag then ()
		else if event = wm_notify && wparam = tvn_itemexpanding then ()
		else if event = wm_notify && wparam = tvn_itemexpanded then ()
		else if event = wm_notify && wparam = tvn_deleteitem then ()
		else if event = wm_notify && wparam = tvn_keydown then ()
		else if event = wm_notify && wparam = -530(*tooltip*) then ()
		else if event = wm_notify && wparam = -521(*tooltip*) then ()
		else if event = wm_notify && wparam = -522(*tooltip*) then ()
		else if event = wm_notify && wparam = tvn_selchanged then begin			
			cursel <- (
			match (unpack_nmtreeview_delete { mc_id = event; mc_wparam = Int32.of_int wparam; mc_lparam = lparam }).nmtvd_item.tvi_item with
				| None -> None; 
				| Some h -> this#item_of_tree h);
			on_selchange();
		end
		else if event = wm_notify && wparam = tvn_begindrag then ()
		else def_handler "TreeView" event wparam lparam;

	method selected = cursel

	method set_selected o =
		let titem = (match o with None -> null_htreeitem | Some i -> this#tree_of_item i) in
		ignore(send ~wnd:handle ~issuer:issue_tvm_selectitem ~item:titem ~action:Tvsa_select)

	method hittest x y =
		let pos = screen_to_client handle {x=x;y=y} in
		let h = send ~wnd:handle ~issuer:issue_tvm_hittest ~pt:pos in
		match h with
		| Tvhtr_onitem (t,_) -> this#item_of_tree t
		| _ -> None

  end

let new_treeview parent = (new itreeview parent : 'a itreeview :> 'a treeview)

(********************************************************************************)
(* API *)

let message_box msg =
	ignore(Win32.message_box ~wnd:null_hwnd ~text:msg ~caption:"Message:" ~options:[MB_OK;MB_TOPMOST])

let error_box msg =
	ignore(Win32.message_box ~wnd:null_hwnd ~text:msg ~caption:"Error:" ~options:[MB_OK;MB_TOPMOST;MB_ICONERROR])

let question_box msg =
	let i = Win32.message_box ~wnd:null_hwnd ~text:msg ~caption:"Question:" ~options:[MB_YESNO;MB_TOPMOST;MB_ICONQUESTION] in
	IDYES = i

let question_cancel_box msg =
	let i = Win32.message_box ~wnd:null_hwnd ~text:msg ~caption:"Question:" ~options:[MB_YESNOCANCEL;MB_TOPMOST;MB_ICONQUESTION] in
	match i with
	| IDYES -> QR_YES
	| IDNO -> QR_NO
	| _ -> QR_CANCEL

let mouse_pos() = (!mouse_x,!mouse_y)

let exit_application() = (post_quit_message ~exit_code:Int32.zero)

let keys = { k_del = 46; k_enter = 13; k_esc = 27; k_fun = 111; }

let make_richcolor ~red ~green ~blue = Clr_rgb {
	red = red;
	green = green;
	blue = blue;
}

let open_file exts =
	match get_open_file_name {
		ofn_owner = null_hwnd;
		ofn_filter = Some (List.map (fun (n,l) -> (n^" ("^(String.concat ";" l)^")",String.concat ";" l)) exts);
		ofn_filter_index = 0;
		ofn_initial_dir = Some (get_current_directory());
		ofn_title = Some "Osiris Open dialog";
		ofn_flags = [OFN_FILEMUSTEXIST;OFN_HIDEREADONLY;OFN_LONGNAMES;OFN_NOCHANGEDIR;OFN_PATHMUSTEXIST];
		ofn_def_ext = None;
		ofn_custom_filter = None;
		ofn_file = "";
		ofn_readonly = false;
		ofn_template = Cdt_none;
		ofn_hook = None;
	} with
	| None -> None
	| Some f -> Some f.ofnr_file

let create_file ~default ~exts =
	match get_save_file_name {
		ofn_owner = null_hwnd;
		ofn_filter = Some (List.map (fun (n,l) -> (n,String.concat ";" l)) exts);
		ofn_filter_index = 0;
		ofn_file = default;
		ofn_initial_dir = Some (get_current_directory());
		ofn_title = Some "Osiris Create dialog";
		ofn_flags = [OFN_NOCHANGEDIR;OFN_PATHMUSTEXIST;OFN_OVERWRITEPROMPT];
		ofn_def_ext = None;
		ofn_custom_filter = None;
		ofn_readonly = false;
		ofn_template = Cdt_none;
		ofn_hook = None;
	} with
	| None -> None
	| Some f -> Some f.ofnr_file	

let select_directory ~title =
	match sh_browse_for_folder {
		bri_owner = null_hwnd;
		bri_title = title;
		bri_flags = [BIF_DONTGOBELOWDOMAIN;BIF_RETURNONLYFSDIRS;BIF_RETURNFSANCESTORS;BIF_NEWDIALOGSTYLE];
	} with
	| None -> None
	| Some (str,_) -> Some str

;;
ignore( load_library "riched32.dll" );
init_common_controls();

(********************************************************************************)