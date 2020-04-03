(* module Data

 - CLIENT
 - Données permanantes du joueur durant la durée de sa connexion

*)
open Message
open Mtlib
open M3d
open Win32

type t = {
	myid : pid;
	font : Text.font;
}

let pdata = Global.empty "client data"

let init pid =
	let f = Text.create_font "Arial" 10 in	
	Global.set pdata {
		myid = pid;
		font = f;
	}

let close() =
	Global.undef pdata

let id() =
	(Global.get pdata).myid

let font() = 
	(Global.get pdata).font

let send msg =
	Server.send_msg (id()) msg