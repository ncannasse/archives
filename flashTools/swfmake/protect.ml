open Win32
open ExtString

let computer_data() =
	let exe = Filename.basename Sys.argv.(0) in
	let d =
		(try Filename.chop_extension exe with _ -> exe) ,
		get_computer_name() ,
		get_user_name() ,
		()
	in
	Base64.str_encode (Extc.zip (Marshal.to_string d []))

let md5 d =
	Digest.to_hex (Digest.string d)

let get_key_data() =
	Base64.str_encode (Extc.zip (computer_data()))

let make_activation_key data =	
	prerr_endline (Extc.unzip (Base64.str_decode data))

;;
let activation_key = md5 (computer_data()) in
let key = (try md5 (String.strip (Std.input_file (Extc.executable_path() ^ "/license.key"))) with _ -> prerr_endline "No key found"; "") in
if key <> activation_key then begin
	Printf.eprintf "This software is not registered.\nPlease send the following activation key to Motion-Twin :\n---\n%s\n---\n" (get_key_data());
	exit 1;
end;
