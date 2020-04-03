let magic = Obj.magic 

open Intf
open Xml
open Mtlib
open W3d

(************************************************)
(**********         INTERFACE         ***********)
(************************************************)

class type interface =
  object
   	method add_component : string -> component -> unit 
  	method add_panel : string -> panel -> unit 
	method add_button : string -> adv_button -> unit 
	method get_component : string -> component
	method get_panel : string -> panel 
	method get_button : string -> adv_button 
	method destroy : unit -> unit
  end


  
(************************************************)
(**********        I INTERFACE        ***********)
(************************************************)

class iinterface =
  object

	val table_panel = Hashtbl.create 0
	val table_button = Hashtbl.create 0
	val table_component = Hashtbl.create 0

	method add_component (name:string) (comp:component) = 
		try 
			ignore(Hashtbl.find table_component name);
			failwith ("same component " ^ name ^ " found in interface")
		with Not_found -> 
			Hashtbl.add table_component name comp
		

	method add_panel (name:string) (pan : panel) = 
		Hashtbl.add table_panel name pan
			
	method add_button (name:string) (but : adv_button)= 
		Hashtbl.add table_button name but

	method get_component name =
		try
			Hashtbl.find table_component name
		with Not_found -> failwith ("Component " ^ name ^ " not found in interface")

	method get_panel name =
		try
			Hashtbl.find table_panel name
		with Not_found -> failwith ("Panel " ^ name ^ " not found in interface")
			
	method get_button name =
		try 
			Hashtbl.find table_button name
		with Not_found -> failwith ("Button " ^ name ^ " not found in interface")
		
	method destroy () =
		Hashtbl.iter
		( fun name obj ->
			obj#destroy();
			Hashtbl.remove table_panel name 
		)
		table_panel;
		Hashtbl.iter
		( fun name obj ->
			obj#destroy();
			Hashtbl.remove table_button name
		)
		table_button
  end


(************************************************)
(**********       DO_WITH_XML       *************)
(************************************************)

let rec do_with_xml xml_list (panel:panel option) interface =
	match xml_list with
		| head::tail -> 
			( 
			do_with_xml tail panel interface;
			try
				match head.x_name with 
					| "panel" ->
						(
						let pan = new_panel (panel) 
			{ f_texture = Vfs.get_fid (get_attrib "face" head) ; f_rect = { tx=(float_of_string (get_attrib "facex1" head)) ;ty=(float_of_string (get_attrib "facey1" head)) ; tx2=(float_of_string (get_attrib "facex2" head)) ; ty2=(float_of_string (get_attrib "facey2" head));}; }
			((float_of_string (get_attrib "posx" head)),(float_of_string (get_attrib "posy" head)))
			((float_of_string (get_attrib "sizex" head)),(float_of_string (get_attrib "sizey" head)))
			(get_attrib "name" head) in
			interface#add_component  (get_attrib "name" head) (pan :>component);
					
						do_with_xml head.x_childs (Some pan) interface;
						)
					| "adv_button" ->
						(
						let but = new_adv_button (panel) 
			{ f_texture = Vfs.get_fid (get_attrib "face1" head) ; f_rect = { tx=(float_of_string (get_attrib "facex11" head)) ;ty=(float_of_string (get_attrib "facey11" head)) ; tx2=(float_of_string (get_attrib "facex12" head)) ; ty2=(float_of_string (get_attrib "facey12" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face2" head) ; f_rect = { tx=(float_of_string (get_attrib "facex21" head)) ;ty=(float_of_string (get_attrib "facey21" head)) ; tx2=(float_of_string (get_attrib "facex22" head)) ; ty2=(float_of_string (get_attrib "facey22" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face3" head) ; f_rect = { tx=(float_of_string (get_attrib "facex31" head)) ;ty=(float_of_string (get_attrib "facey31" head)) ; tx2=(float_of_string (get_attrib "facex32" head)) ; ty2=(float_of_string (get_attrib "facey32" head));}; } 
			((float_of_string (get_attrib "posx" head)),(float_of_string (get_attrib "posy" head)))
			((float_of_string (get_attrib "sizex" head)),(float_of_string (get_attrib "sizey" head))) 
			true (get_attrib "name" head) in
			interface#add_component (get_attrib "name" head) (but:>component);
						)
					| "button" -> 
					(
						let but = new_button (panel) 
			{ f_texture = Vfs.get_fid (get_attrib "face" head) ; f_rect = { tx=(float_of_string (get_attrib "facex11" head)) ;ty=(float_of_string (get_attrib "facey11" head)) ; tx2=(float_of_string (get_attrib "facex12" head)) ; ty2=(float_of_string (get_attrib "facey12" head));}; } 
			((float_of_string (get_attrib "posx" head)),(float_of_string (get_attrib "posy" head)))
			((float_of_string (get_attrib "sizex" head)),(float_of_string (get_attrib "sizey" head))) 
			true (get_attrib "name" head) in
			interface#add_component (get_attrib "name" head) (but:>component);
					)
					| "progress_bar" -> 
					(
						let progress = new_progress_bar (panel) 
			{ f_texture = Vfs.get_fid (get_attrib "face1" head) ; f_rect = { tx=(float_of_string (get_attrib "facex11" head)) ;ty=(float_of_string (get_attrib "facey11" head)) ; tx2=(float_of_string (get_attrib "facex12" head)) ; ty2=(float_of_string (get_attrib "facey12" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face2" head) ; f_rect = { tx=(float_of_string (get_attrib "facex11" head)) ;ty=(float_of_string (get_attrib "facey11" head)) ; tx2=(float_of_string (get_attrib "facex12" head)) ; ty2=(float_of_string (get_attrib "facey12" head));}; } 
			((float_of_string (get_attrib "posx" head)),(float_of_string (get_attrib "posy" head)))
			((float_of_string (get_attrib "sizex" head)),(float_of_string (get_attrib "sizey" head))) 
			(get_attrib "name" head) in
			interface#add_component (get_attrib "name" head) (progress:>component);
					)

					| "scroll_bar" -> 

						(
						let scroll = new_scroll_bar (panel) 
			{ f_texture = Vfs.get_fid (get_attrib "face1" head) ; f_rect = { tx=(float_of_string (get_attrib "facex11" head)) ;ty=(float_of_string (get_attrib "facey11" head)) ; tx2=(float_of_string (get_attrib "facex12" head)) ; ty2=(float_of_string (get_attrib "facey12" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face2" head) ; f_rect = { tx=(float_of_string (get_attrib "facex21" head)) ;ty=(float_of_string (get_attrib "facey21" head)) ; tx2=(float_of_string (get_attrib "facex22" head)) ; ty2=(float_of_string (get_attrib "facey22" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face3" head) ; f_rect = { tx=(float_of_string (get_attrib "facex31" head)) ;ty=(float_of_string (get_attrib "facey31" head)) ; tx2=(float_of_string (get_attrib "facex32" head)) ; ty2=(float_of_string (get_attrib "facey32" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face4" head) ; f_rect = { tx=(float_of_string (get_attrib "facex41" head)) ;ty=(float_of_string (get_attrib "facey41" head)) ; tx2=(float_of_string (get_attrib "facex42" head)) ; ty2=(float_of_string (get_attrib "facey42" head));}; } 
			((float_of_string (get_attrib "posx" head)),(float_of_string (get_attrib "posy" head)))
			((float_of_string (get_attrib "sizex" head)),(float_of_string (get_attrib "sizey" head))) 
			(get_attrib "name" head) 
			(int_of_string (get_attrib "nbpos" head))
			(int_of_string (get_attrib "orientation" head))	in
			interface#add_component (get_attrib "name" head) (scroll:>component);
						)

					| "checkbox" -> 
						(
					let check = new_checkbox (panel) 	
			{ f_texture = Vfs.get_fid (get_attrib "face1" head) ; f_rect = { tx=(float_of_string (get_attrib "facex11" head)) ;ty=(float_of_string (get_attrib "facey11" head)) ; tx2=(float_of_string (get_attrib "facex12" head)) ; ty2=(float_of_string (get_attrib "facey12" head));}; } 
			{ f_texture = Vfs.get_fid (get_attrib "face2" head) ; f_rect = { tx=(float_of_string (get_attrib "facex21" head)) ;ty=(float_of_string (get_attrib "facey21" head)) ; tx2=(float_of_string (get_attrib "facex22" head)) ; ty2=(float_of_string (get_attrib "facey22" head));}; } 
			((float_of_string (get_attrib "posx" head)),(float_of_string (get_attrib "posy" head)))
			((float_of_string (get_attrib "sizex" head)),(float_of_string (get_attrib "sizey" head))) 
			true (get_attrib "name" head) in
			interface#add_component (get_attrib "name" head) (check:>component);
						
					)
					| _ -> failwith ("Interface XML tag " ^ head.x_name ^ " unknown");
			with
				Not_found -> failwith ("Missing attribute in interface " ^ (Xml.to_string head));
			)
		| [] -> ()

(*
val new_checkbox : parent:(panel option) -> face_normale:face -> face_rollover:face -> face_click:face -> position:pos -> taille:size -> visible:bool -> nom:string -> checkbox
*)


(************************************************)
(**********     CREATE_INTERFACE    *************)
(************************************************)

let create_interface xml panel =
	let inter = new iinterface in
	do_with_xml xml.x_childs panel (inter :> interface);
	(inter :> interface)

