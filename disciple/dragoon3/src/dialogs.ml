open Xml

type dialog = Xgui.xobj

let dialog_filename = "dialogs.xml"
let dialogs = ref (try Xml.parse_file dialog_filename with Xml.Error e -> 
	failwith ("Invalid dialog file "^dialog_filename^" : "^(Xml.error e)))

let rec find x name =
	try
		ignore(List.find (fun (att,aval) -> att = "name" && aval = name) (Xml.attribs x));
		x
	with
		Not_found ->			
			let found = ref None in
			(List.iter
				(fun x ->
					try
						found := Some(find x name)
					with Not_found -> ()
				)
				(Xml.children x));
			match !found with
			| None -> raise Not_found
			| Some p -> p

let find_dialog str =
	try
		find !dialogs str;
	with
		Not_found -> failwith ("No such dialog: "^str)

let create parent str =
	Xgui.create parent (find_dialog str)

let create_in ~obj ~parent ~name =
	Xgui.create_in obj parent (find_dialog name)	

let get str =
	find_dialog str

let close d =
	d#destroy

let reload () =
	dialogs := Xml.parse_file dialog_filename
;;
Run.fast_register "Dialogs";