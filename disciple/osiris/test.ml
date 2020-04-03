open Printf
open Osiris

let log s = 
	prerr_string "*> ";
	prerr_endline s

let process w =
	for i = 1 to 1000 do
		w#process false;
	done

;;
let w = new_window() in
w#caption "Test";
w#visible true;
let p = new_panel w in
p#x 10;
p#y 10;
let p = new_panel p in
p#width 100;
p#height 100;
p#border BdSingle;

printf "%d x %d\n" p#get_width p#get_height;
flush stdout;

let p2 = new_button p in
let a = ref false in 
p2#on_click (fun () -> a := not !a; p#align (if !a then AlCenter else AlNone)
;			 
printf "%d x %d\n" p#get_width p#get_height;
flush stdout;
			 
);
p2#width 100;
p2#height 100;
p2#caption "button";
for i = 1 to 11 do
	p2#width p2#get_width;
	flush stderr;
done;

process w;


w#loop