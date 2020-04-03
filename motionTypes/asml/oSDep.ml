let exe_name() = 
	let exe = Win32.get_module_file_name None in
	if Filename.basename exe = "ocamlrun.exe" then Sys.argv.(0) else exe