open Message
open Mtlib

type pdata = {
	pfun : client_message -> unit;
	pdeck : Deck.t;
}

let data = ( MBTree.empty() : (pid , pdata) MBTree.t )

let register pid pfun pdeck =
	MBTree.add data pid {
					pfun = pfun;
					pdeck = pdeck;
				}

let unregister pid = MBTree.remove data pid

let get_fun pid =
	try
		(MBTree.find data pid).pfun
	with
		Not_found -> raise (SError (EUnkPlayer pid))

let get_deck pid =
	try
		(MBTree.find data pid).pdeck
	with
		Not_found -> raise (SError (EUnkPlayer pid))


