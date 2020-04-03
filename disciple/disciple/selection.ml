(* module Selection

 - CLIENT
 - permet de gérer des selections diverses par l'utilisateur

*)
open Mtlib
open M3d
open Mouse

type t = {
	fs_text : string;
	fs_test : unit -> bool;
	fs_ok : unit -> unit;
	fs_cancel : (unit -> unit) option;
	fs_self_validate : bool;
}

type cursor =
	| Normal
	| Select
	| Cancel

let sellist = MList.empty()

let add t = 
	MList.push sellist t

let validate() =
	try
		(MList.pop sellist).fs_ok()
	with
		Invalid_operation -> failwith "Validating a not defined selection"

let cancel() =
	try
		let f = (MList.at_index sellist 0).fs_cancel in
		match f with
		| None -> ()
		| Some f -> ignore(MList.pop sellist); f()		
	with
		Invalid_operation -> failwith "Canceling a not defined selection"

let exit() =
	try
		ignore (MList.pop sellist);
	with
		Invalid_operation -> failwith "Exiting a not defined selection"

let close() =
	MList.iter (fun s -> match s.fs_cancel with | None -> () | Some f -> f()) sellist;
	MList.clear sellist

let process() = 
	match MList.to_list sellist with
	| [] -> Mouse.set_cursor CRNormal		
	| s::_ ->
		(if Mouse.rclick() then cancel() else
		match s.fs_test() with
		| true ->
			Mouse.set_cursor CRSelect;
			if not s.fs_self_validate && Mouse.click() then begin
				validate();
				raise Run.Click_handled;
			end
		| false ->
			Mouse.set_cursor CRCancel;
			if not s.fs_self_validate && Mouse.click() then begin
				cancel();
				raise Run.Click_handled;
			end);		
			


