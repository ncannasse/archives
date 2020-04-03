open Osiris
open Log

let gfx = new_resources 16 16
;;
gfx#add_source "d3gfx"

let not_loaded = gfx#add_bitmap "NOTLOADED"

let add_source s = gfx#add_source s

let bitmap str = 
	try
		gfx#add_bitmap str
	with
		Failure msg -> log WARNING msg; not_loaded

let icon str = 
	try
		gfx#add_icon str
	with
		Failure msg -> log WARNING msg; not_loaded
;;
Run.fast_register "Gfxres";