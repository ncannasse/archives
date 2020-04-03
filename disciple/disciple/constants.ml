(* module Constants

 - COMMON
 - Constantes générales du jeu

*)

open Run
open Mtlib
open M3d

let symbols_t = Vfs.get_ftype "_SY"

(*-- GAME PLAY --*)

let min_deck_count = 40
let max_deck_count = 40
let starting_hand_count = 7

let draw_time = 150

let board_width = 1000.
let board_height = 1000.

let creature_move_penalty = 40

(*-- GENERAL DISPLAY --*)

let wh_ratio = 800. /. 600.

let deck_pos = { x = -0.84; y = 0.7; z = 0.01 }

(*-- MOUSE DISPLAY --*)

let select_cursor_zone = { tx = 0.  /. 128.; ty = 0.; tx2 = 24. /. 128.; ty2 = 24. /. 32. }
let normal_cursor_zone = { tx = 25. /. 128.; ty = 0.; tx2 = 41. /. 128.; ty2 = 25. /. 32. }
let cancel_cursor_zone = { tx = 42. /. 128.; ty = 0.; tx2 = 66. /. 128.; ty2 = 24. /. 32. }

let normal_mouse_size = ( 0.0156 *. 2., 0.0244 *. 2. *. wh_ratio )
let select_mouse_size = ( 0.0234 *. 2., 0.0234 *. 2. *. wh_ratio )
let cancel_mouse_size = select_mouse_size

(*--- CARD DISPLAY ---*)

let card_width = 189.
let card_height = 256.
let art_width = 128.
let art_height = 128.
let art_scale = 1.2
let card_screen_width = 0.23

let art_delta_x = 0.01
let art_delta_y = -0.065
let text_delta_x = 0.
let text_delta_y = 0.13

let card_bg_zone = { tx = 0.; ty = 0.; tx2 = card_width /. card_height; ty2 = 1. }
let card_size = ( card_screen_width, (card_screen_width *. card_height /. card_width)*. wh_ratio )
let art_size = 	( (fst card_size) *. art_width *. art_scale /. card_width ,
				(snd card_size) *. art_height *. art_scale /. card_height )

(*---- EVENT BAR DISPLAY ---*)

let event_size = (fst art_size /. 2., snd art_size /. 2.)
let events_pos = { x = 1.5; y = -0.9; z = 0.01 }

let current_event_pos = (-0.9,-0.7)
let first_event_pos = (-0.9,-0.9)

;;
Run.fast_register "Constants"
