open Vfs

let t = (get_ftype "DIR")

let new_directory parent =	
	let d = Dialogs.create None "dir.new" in		
	if d#process then begin
		let name = (d#edit "dir.name")#get_caption in
		Dialogs.close d;
		ignore(create_file ~name:name ~parent:parent ~ftype:t);
	end else
		Dialogs.close d

let export id =
	match Osiris.select_directory "Export directory" with
	| None -> ()
	| Some dir ->		
		export_tree (dir^"/") id

;;
let data = {
	ft_name = "Directory";
	ft_t = t;
	ft_new = Some new_directory;
	ft_icon = Gfxres.bitmap "BMP_DIR";
	ft_select = None;
	ft_unselect = None;
	ft_show = None;
	ft_hide = None;
	ft_open = None;
	ft_close = None;
	ft_context = [("Export",export)];
	ft_delete = None;
	ft_hardlinked = None;	
	ft_export = None;
	ft_haschilds = true;
} in
	register data;
	Run.fast_register "Dir";
