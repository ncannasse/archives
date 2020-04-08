open Type

let generate file t =
	let h = Hashtbl.create 0 in
	Typer.generate t (fun c ->
		List.iter (fun f -> 
			match f.cf_expr with
			| None -> ()
			| Some e ->
				Typetools.expr_iter (fun e ->
					match e.texpr with
					| TConst (TConstString s) -> Hashtbl.replace h s ()
					| _ -> ()
				) e		
		) c.c_class_fields
	);
	let ch = IO.output_channel (open_out file) in
	let l = Hashtbl.fold (fun s _ acc -> s :: acc) h [] in
	let l = List.sort (fun s1 s2 ->
		if String.length s1 = String.length s2 then
			compare s1 s2
		else
			String.length s1 - String.length s2
	) l in
	List.iter (fun s -> IO.nwrite ch s; IO.write ch '\n') l;
	IO.close_out ch

;;
let output = ref None in
Plugin.add
	[
		("-strings",Arg.String (fun f -> output := Some f),"<file> : print all the strings found in target file");
	]
	(fun t -> 
		match !output with
		| None -> ()
		| Some file -> generate file t
	)