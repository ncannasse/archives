#load "str.cma"
#load "extLib.cma"
open ExtLib

let header = input_file "header.t.html"
let menu = input_file "menu.t.html"
let footer = input_file "footer.t.html"

let extract_params f s =
	try
		let l = String.length f + 1 in
		let p = String.find s (f ^ "(") in
		let sbefore = String.sub s 0 p in
		let pend = String.index_from s p ')' + 1 in
		let sheader = String.sub s (p + l) (pend - p - l - 1) in
		let safter = String.sub s pend (String.length s - pend) in
		Some (String.nsplit sheader ",") , sbefore , safter
	with
		Not_found | Invalid_string -> None , "" , s

let rec mts_colorize s =
	let colorize s = 
		let kwds = ["import";"function";"new";"if";"for";"while";"else";"break";"return";"switch";"null";"true";"false";"class";"var";"this";"extends";"super"] in
		List.fold_left (fun s k ->
			let sl = String.nsplit s k in
			String.join ("<font color=\"blue\">" ^ k ^ "</font>") sl
		) s kwds;
	in
	let s = String.join "    " (String.nsplit s "\t") in
	try
		let before, after = String.split s "<mts>" in
		let code , after = String.split after "</mts>" in
		before ^ "<pre class=\"mts\">" ^ colorize code ^ "</pre>" ^ mts_colorize after
	with
		Invalid_string -> s

let generate name =
	let ch = IO.output_channel (open_out ("release/" ^ name ^ ".html")) in
	IO.nwrite ch header;
	let s = input_file (name ^ ".d.html") in
	let headers , s1, s2 = extract_params "$HEADER" s in
	let s = s1 ^ s2 in
	(match headers with 
	| None | Some [] -> ()
	| Some l ->
		match List.rev l with
		| [] -> assert false
		| h :: l ->	
			List.iter (fun h ->
				match String.nsplit h "-->" with
				| [title ; url] ->
					let url = (if url = "/" then "index.html" else url) in
					IO.nwrite ch ("<a href=\"" ^ url ^ "\">" ^ title ^ "</a>|")
				| _ -> failwith ("Invalid header " ^ h)
			) (List.rev l);
			match String.nsplit h "-->" with
			| [title ; url] -> IO.nwrite ch ("<a class=\"highlight\" href=\"" ^ url ^ "\">" ^ title ^ "</a>")
			| _ -> failwith ("Invalid header " ^ h)
	);
	IO.nwrite ch "</div>";
	IO.nwrite ch "<div id=\"content\">";
	IO.nwrite ch menu;
	IO.nwrite ch (mts_colorize s);
	IO.nwrite ch "</div>";
	IO.nwrite ch footer;
	IO.close_out ch

;;
let files = Sys.readdir (Sys.getcwd()) in
Array.iter (fun f ->
	match String.nsplit f "." with
	| name :: "d" :: "html" :: [] ->
		generate name
	| _ ->
		()
) files