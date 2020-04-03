type dll_handle
type dll_address

type 'a env

external new_env : tag:int -> len:int -> 'a env = "caml_obj_block"
external env_set : 'a env -> int -> 'a -> unit = "%obj_set_field"
external env_get : 'a env -> int -> 'a = "%obj_field"

exception File_not_found of string

let opens = ref []

let make_loader files =
	let exe_path = Filename.dirname Sys.argv.(0) in
	let paths = exe_path :: (List.map Filename.dirname files) in
	let paths = List.fold_left (fun acc path -> if List.exists (( = ) path) acc then acc else path :: acc) [] paths in
	let load f =
		let ch = open_in_bin f in
		opens := ch :: !opens;
		ch
	in
	(fun file ->
		try			
			load file , ""
		with
			Sys_error _ ->
				let f = ref None in
				if not (List.exists (fun path ->
						try
							f := Some (load (path ^ "/" ^ file), path ^ "/");
							true
						with
							Sys_error _ ->
								false
						) paths)
				then
					raise (File_not_found file)
				else
					match !f with
					| None -> assert false
					| Some ch -> ch
					
	)

let close_opens() =
	List.iter close_in !opens;
	opens := []

let loader = ref ((fun _ -> assert false) : string -> (in_channel * string))