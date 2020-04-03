open Swf
open Printf
open IO

let printf = Printf.printf

type fsid = int

type mtv_seg =
	| MoveTo of int * int
	| LineTo of int * int
	| CurveTo of int * int * int * int

type mtv_shape = {
	fsid : fsid;
	ops : mtv_seg list;
}

type seg = {
	mutable x1 : int;
	mutable y1 : int;
	mutable x2 : int;
	mutable y2 : int;
	mutable ctrl : (int * int) option;
	fs : fsid;
}

type mtv_shapes = {
	mutable used : bool;
	shdata : mtv_shape list;
	shid : int;
}

type depth = int

type clip_op =
	| Place of depth * int * bool
	| Remove of depth
	| SetMatrix of depth * matrix
	| SetColor of depth * color_transform_alpha
	| Frame

type mtv_clip = {
	mutable clused : bool;
	mutable clexport : string option;
	cldata : clip_op list;
	clid : int;
}

type any_fill =
	| FsFill of shape_fill_style
	| FsLine of shape_line_style

type bmpdata = 
	| Lossless of bitmap_lossless
	| Lossless2 of bitmap_lossless
	| Jpeg of bitmap
	| JpegAlpha of bitmap_jpg3
	| JpegWithTable of bitmap

type bmp = {
	bmpdata : bmpdata;
	mutable bmpused : bool;
}

type fill = {
	filldata : any_fill;
	mutable fillused : bool;
}

type mtv_context = {
	fills : fill DynArray.t;
	mtv_shapes : (int,mtv_shapes) Hashtbl.t;
	mtv_clips : (int,mtv_clip) Hashtbl.t;
	bitmaps : (int, bmp) Hashtbl.t;
	mutable jpgtable : string option;
}

type context = {
	mutable x : int;
	mutable y : int;
	mutable fs0 : int;
	mutable fs1 : int;
	mutable ls : int;
	shapes : (fsid, seg list) Hashtbl.t;
	segs : (int * int * fsid, seg) Hashtbl.t;
}

let verbose = ref false

let signed v n =
	if v >= 1 lsl (n - 1) then
		- ((v lxor (1 lsl n - 1)) + 1)
	else
		v

let unsigned v n =
	if v < 0 then
		(- ( v + 1 )) lxor (1 lsl n - 1)
	else
		v

let mtv_context() =
	{
		fills = DynArray.create();
		mtv_shapes = Hashtbl.create 0;
		mtv_clips = Hashtbl.create 0;
		bitmaps = Hashtbl.create 0;
		jpgtable = None;
	}

let merge ctx fs ls =
	let fsi = 0 :: List.map (fun fs ->
		let fs = FsFill fs in
		try
			1 + DynArray.index_of (fun f -> f.filldata = fs) ctx.fills
		with
			Not_found -> 
				DynArray.add ctx.fills { filldata = fs; fillused = false };
				DynArray.length ctx.fills
	) fs in
	let lsi = 0 :: List.map (fun ls ->
		let ls = FsLine ls in
		try
			1 + DynArray.index_of (fun f -> f.filldata = ls) ctx.fills
		with
			Not_found -> 
				DynArray.add ctx.fills { filldata = ls; fillused = false };
				DynArray.length ctx.fills
	) ls in
	Array.of_list fsi , Array.of_list lsi

let make_color = function
	| ColorRGB c ->
		Int32.logor (Int32.of_int ((c.cr lsl 16) lor (c.cg lsl 8) lor c.cb)) (Int32.shift_left (Int32.of_int 0xFF) 24)
	| ColorRGBA c ->
		Int32.logor (Int32.of_int ((c.r lsl 16) lor (c.g lsl 8) lor c.b)) (Int32.shift_left (Int32.of_int c.a) 24)

let rec optimize = function
	| [] -> []
	| MoveTo (0,0) :: l -> optimize l
	| MoveTo (x1,y1) :: MoveTo (x2,y2) :: l -> optimize (MoveTo(x1+x2,y1+y2) :: l)
	| LineTo (x1,y1) :: LineTo (x2,y2) :: l when (y1 = 0 && y2 = 0) -> optimize ((LineTo (x1+x2,0)) :: l)
	| (LineTo (_,0) as x) :: l -> x :: optimize l
	| LineTo (x1,y1) :: LineTo (x2,y2) :: l when (float x1 /. float y1 = float x2 /. float y2) -> optimize ((LineTo (x1+x2,y1+y2)) :: l)
	| x :: l -> x :: optimize l

let build_shape ctx fsid =
	let shapes = Hashtbl.find_all ctx.shapes fsid in
	let freesegs = (let l = ref [] in Hashtbl.iter (fun _ s -> if s.fs = fsid then l := s :: !l) ctx.segs; !l) in
	let segs = List.concat (freesegs :: shapes) in
	let rec loop x y = function
		| [] -> []
		| s :: l ->
			let line = (match s.ctrl with
				| None ->
					LineTo (s.x2 - s.x1, s.y2 - s.y1)
				| Some (cx,cy) -> CurveTo (cx - s.x1, cy - s.y1,s.x2 - cx, s.y2 - cy)
			) in
			if s.x1 != x || s.y1 != y then
				MoveTo (s.x1 - x , s.y1 - y) :: line :: loop s.x2 s.y2 l
			else
				line :: loop s.x2 s.y2 l
	in
	let ops = loop 0 0 segs in
	{
		fsid = fsid;
		ops = (match optimize ops with (MoveTo _ :: _) as l -> l | l -> MoveTo (0,0) :: l)
	}


let add_seg ctx s =
	let rec loop x y acc =
		let s2 = Hashtbl.find ctx.segs (x,y,s.fs) in
		if s2 == s then 
			List.rev (s2 :: acc)
		else
			loop s2.x2 s2.y2 (s2 :: acc)
	in
	Hashtbl.add ctx.segs (s.x1,s.y1,s.fs) s;
	try
		let segs = loop s.x2 s.y2 [] in
		if !verbose then printf "  * shape %d segs fs %d found\n" (List.length segs) s.fs;
		List.iter (fun s -> Hashtbl.remove ctx.segs (s.x1,s.y1,s.fs)) segs;
		Hashtbl.add ctx.shapes s.fs segs;
	with
		Not_found -> ()
	
let line ?ctrl ctx dx dy =
	let x = ctx.x in
	let y = ctx.y in
	let tx = ctx.x + dx in
	let ty = ctx.y + dy in
	if ctx.fs0 > 0 then add_seg ctx { x1 = x; y1 = y; x2 = tx; y2 = ty; fs = ctx.fs0; ctrl = ctrl };
	if ctx.fs1 > 0 then add_seg ctx { x1 = tx; y1 = ty; x2 = x; y2 = y; fs = ctx.fs1; ctrl = ctrl };
	if ctx.ls > 0 then add_seg ctx { x1 = x; y1 = y; x2 = tx; y2 = ty; fs = ctx.ls; ctrl = ctrl };
	ctx.x <- tx;
	ctx.y <- ty

let curve ctx ax ay cx cy =
	line ~ctrl:(ctx.x+cx,ctx.y+cy) ctx (ax + cx) (ay + cy)

let process_shape mtv_ctx (s : shape) =
	let ctx = {
		x = 0;
		y = 0;
		shapes = Hashtbl.create 0;
		fs0 = 0;
		fs1 = 0;
		ls = 0;
		segs = Hashtbl.create 0;
	} in
	let fills = DynArray.create() in
	let use_fill fs =
		if fs <> 0 then
		try
			ignore(DynArray.index_of ((=)fs) fills);
		with
			Not_found -> DynArray.add fills fs
	in
	let fstbl , lstbl = merge mtv_ctx s.sh_style.sws_fill_styles s.sh_style.sws_line_styles in
	let fstbl , lstbl = ref fstbl , ref lstbl in
	if !verbose then printf "Shape %d fs %d ls\n" (Array.length !fstbl - 1) (Array.length !lstbl - 1);
	List.iter (fun r ->
		match r with
		| SRStyleChange r ->
			begin
			(match r.scsr_move with
			| None -> ()
			| Some (n,px,py) ->
				let px = signed px n in
				let py = signed py n in
				if !verbose then printf "  moveto %d %d\n" px py;
				ctx.x <- px;
				ctx.y <- py;
			);
			(match r.scsr_fs0 with
			| None -> ()
			| Some i -> 
				if !verbose then printf "  fs %d\n" i;
				let fs = (!fstbl).(i) in
				use_fill fs;
				ctx.fs0 <- fs);
			(match r.scsr_fs1 with
			| None -> ()
			| Some i -> 
				if !verbose then printf "  fs1 %d\n" i;
				let fs = (!fstbl).(i) in
				use_fill fs;
				ctx.fs1 <- fs);
			(match r.scsr_ls with
			| None -> ()
			| Some i -> 
				if !verbose then printf "  ls %d\n" i;
				let ls = (!lstbl).(i) in
				use_fill ls;
				ctx.ls <- ls);
			(match r.scsr_new_styles with
			| None -> ()
			| Some st ->
				if !verbose then printf "  change styles (%d %d)\n" (List.length st.sns_fill_styles) (List.length st.sns_line_styles);
				let fst , lst = merge mtv_ctx st.sns_fill_styles st.sns_line_styles in
				fstbl := fst;
				lstbl := lst); 
			end;
		| SRCurvedEdge r ->
			let ax = signed r.scer_ax r.scer_nbits in
			let ay = signed r.scer_ay r.scer_nbits in
			let cx = signed r.scer_cx r.scer_nbits in
			let cy = signed r.scer_cy r.scer_nbits in
			if !verbose then printf "  curve %d,%d\n" (ax + cx) (ay + cy);
			curve ctx ax ay cx cy
		| SRStraightEdge r ->
			let dx = (match fst r.sser_line with None -> 0 | Some d -> signed d r.sser_nbits) in
			let dy = (match snd r.sser_line with None -> 0 | Some d -> signed d r.sser_nbits) in
			if !verbose then printf "  line %d,%d\n" dx dy;
			line ctx dx dy;
	) s.sh_style.sws_records.srs_records;
	if !verbose then printf "%d total fill styles\n" (DynArray.length mtv_ctx.fills);
	let shapes = ref [] in
	for i = 0 to DynArray.length fills - 1 do
		let fs = DynArray.get fills i in
		let sh = build_shape ctx fs in
		if sh.ops <> [] then begin
			if !verbose then printf "shape %d ops fs %d\n" (List.length sh.ops) fs;
			(DynArray.get mtv_ctx.fills (fs - 1)).fillused <- true;
			shapes := sh :: !shapes
		end;
	done;
	Hashtbl.add mtv_ctx.mtv_shapes s.sh_id { used = false; shdata = List.rev !shapes; shid = s.sh_id }

let process_clip ctx mc =
	let ops = ref [] in
	let action a =
		ops := a :: !ops
	in
	let is_shape id =
		if Hashtbl.mem ctx.mtv_shapes id then
			true
		else if Hashtbl.mem ctx.mtv_clips id then
			false
		else
			failwith ("Id not found " ^ string_of_int id)
	in
	List.iter (fun t ->
		match t.tdata with
		| TShowFrame ->
			action Frame
		| TRemoveObject2 i ->
			let d = i - 1 in
			if d < 0 then assert false;
			action (Remove d)
		| TPlaceObject2 pl ->
			let d = pl.po_depth - 1 in
			if d < 0 then assert false;
			(match pl.po_cid with
			| None -> ()
			| Some id -> 
				action (Place (d,id,is_shape id)));
			(match pl.po_matrix with
			| None -> ()
			| Some m -> action (SetMatrix (d,m)));
			(match pl.po_color with
			| None -> ()
			| Some c -> 
				action (SetColor (d,c)));
(*/*		(match pl.po_inst_name with
			| None -> ()
			| Some n -> action (SetName (pl.po_depth,n)));
			(match pl.po_clip_depth with
			| None -> ()
			| Some d -> action (SetClip (pl.po_depth,d))); */*)
		| TDoAction _ ->
			()
		| TFrameLabel s ->
			()
		| t ->
			let t = Obj.repr t in
			let i , s = if Obj.is_int t then Obj.magic t , 0 else Obj.tag t , Obj.size t in
			failwith ("Unknown MC tag " ^ string_of_int i ^ "," ^ string_of_int s)
	) mc.c_tags;
	Hashtbl.add ctx.mtv_clips mc.c_id { clused = false; clexport = None; cldata = List.rev !ops; clid = mc.c_id }

let calc_nbits v =
	let k = ref 0 in
	let v = ref (abs v) in
	while !v > 0 do
		incr k;
		v := !v lsr 1
	done;
	!k

let segs_to_data ops =
	let ch = IO.output_string() in
	let b = IO.output_bits ch in
	let write = IO.write_bits b in
	let swrite n v = write n (unsigned v n) in
	let nbits = ref 0 in
	List.iter (function
		| LineTo (x,y)
		| MoveTo (x,y) -> nbits := max !nbits (calc_nbits (max (abs x) (abs y)))
		| CurveTo (cx,cy,x,y) -> nbits := max !nbits (calc_nbits (max (max (abs x) (abs y)) (max (abs cx) (abs cy))))
	) ops;
	let nbits = !nbits + 1 in
	write 16 (List.length ops);
	write 5 nbits;
	List.iter (function
		| CurveTo (cx,cy,x,y) -> write 1 1; swrite nbits cx; swrite nbits cy; swrite nbits x; swrite nbits y
		| MoveTo (x,y) -> write 2 0; swrite nbits x; swrite nbits y
		| LineTo (x,y) -> write 2 1; swrite nbits x; swrite nbits y
	) ops;
	IO.flush_bits b;
	IO.close_out ch

let shape_data_to_mtw sh =
	Mtw.Group ("Form",
		[Mtw.Number ("fsnb",Int32.of_int sh.fsid);Mtw.Data ("DATA", segs_to_data sh.ops)]
	)

let shape_to_mtw sh =
	Mtw.Group ("Shap",
		Mtw.Number ("shid",Int32.of_int sh.shid) :: List.map shape_data_to_mtw sh.shdata
	)

let kind = [| 0;0;0;0;0;0 |]

let matrix_part_nbits m = 
	m.m_nbits

let write_matrix write m =
	let write_matrix_part p =
		let nbits = matrix_part_nbits p in
		write 5 nbits;		
		write nbits p.mx;
		write nbits p.my;
	in
	(match m.scale with
	| None ->
		write 1 0
	| Some s ->
		write 1 1;
		write_matrix_part s
	);
	(match m.rotate with
	| None ->
		write 1 0
	| Some r ->
		write 1 1;
		write_matrix_part r);
	write_matrix_part m.trans

let cxa_nbits c =
	c.cxa_nbits

let write_cxa write c =
	let nbits = cxa_nbits c in
	match c.cxa_add , c.cxa_mult with
	| None , None ->
		write 2 0;
	| Some c , None -> 
		write 2 2;
		write 4 nbits;
		List.iter (write nbits) [c.r;c.g;c.b;c.a];
	| None , Some c -> 
		write 2 1;
		write 4 nbits;
		List.iter (write nbits) [c.r;c.g;c.b;c.a];
	| Some c1 , Some c2 -> 
		write 2 3;
		write 4 nbits;
		List.iter (write nbits) [c2.r;c2.g;c2.b;c2.a;c1.r;c1.g;c1.b;c1.a]

let clip_to_data ops =
	let ch = IO.output_string() in
	let b = IO.output_bits ch in
	let write n x = IO.write_bits b n x in
	let nbits = ref 0 in
	let inbits = ref 0 in
	List.iter (function
		| Place (d,i,_) ->
			if d < 0 then assert false;
			nbits := max !nbits (calc_nbits d);
			inbits := max !inbits (calc_nbits i);
		| Remove d
		| SetMatrix (d,_)
		| SetColor (d,_) ->
			if d < 0 then assert false;
			nbits := max !nbits (calc_nbits d)
		| Frame ->
			()
	) ops;
	let nbits = !nbits in
	let inbits = !inbits in
	write 5 nbits;
	write 5 inbits;
	List.iter (function
		| Place (d,i,_) ->
			write 4 1;
			write nbits d;
			write inbits i
		| Remove d ->
			write 4 2;
			write nbits d
		| SetMatrix (d,m) ->
			write 1 1;
			write nbits d;
			write_matrix write m
		| SetColor (d,c) ->
			write 4 3;
			write nbits d;
			write_cxa write c
		| Frame ->
			write 2 1
	) ops;
	write 4 0;
	IO.flush_bits b;
	IO.close_out ch

let clip_to_mtw cl =
	let d = Mtw.Data ("DATA" , clip_to_data cl.cldata) in
	Mtw.Group ("Clip",
		Mtw.Number ("clid",Int32.of_int cl.clid) ::
		(match cl.clexport with
		| None -> [d]
		| Some e -> [Mtw.Data("RSNM",e ^ "\000");d])
	)

let matrix_to_data m =
	let ch = IO.output_string() in
	let b = IO.output_bits ch in
	write_matrix (fun n x -> IO.write_bits b n x) m;
	IO.flush_bits b;
	IO.close_out ch

let gradient_to_mtw mat grad gstyle =
	let grad = (match grad with GradientRGB l -> List.map (fun (i,c) -> i , ColorRGB c) l | GradientRGBA l -> List.map (fun (i,c) -> i , ColorRGBA c) l)  in
	[ 
		Mtw.Data (gstyle, matrix_to_data mat); 
		Mtw.Group ("Cols",
			List.map (fun (i,c) -> Mtw.Number ("colr", make_color c)) grad
		);
		Mtw.Group ("Vals",
			List.map (fun (i,c) -> Mtw.Number ("cval", Int32.of_int i)) grad
		);
	]

let fill_style_to_mtw ctx id f =
	Mtw.Group ("Fill",
		Mtw.Number ("fsid",Int32.of_int (id + 1)) ::
		match f with
		| FsFill f ->
			(match f with
			| SFSSolid c -> [ Mtw.Number ("colr",make_color (ColorRGB c)) ]
			| SFSSolid3 c -> [ Mtw.Number ("colr",make_color (ColorRGBA c)) ]
			| SFSLinearGradient (mat,grad) -> gradient_to_mtw mat grad "LMAT"
			| SFSRadialGradient (mat,grad) -> gradient_to_mtw mat grad "RMAT"
			| SFSBitmap bmp ->
				try
					let b = Hashtbl.find ctx.bitmaps bmp.sfb_cid in
					b.bmpused <- true;
					[ Mtw.Number ("ibmp", Int32.of_int bmp.sfb_cid); Mtw.Data ("BMAT", matrix_to_data bmp.sfb_mpos); ]
				with
					Not_found -> failwith ("Unknown bitmap id " ^ string_of_int bmp.sfb_cid))
		| FsLine l ->
			[ Mtw.Number ("colr", make_color l.sls_color); Mtw.Number ("wdth",Int32.of_int l.sls_width) ]
	)

let bitmap_to_mtw ctx id b =
	let lossless b alpha f =
		let nbits , ncols = (if b.bll_format = 3 then 8 , int_of_char b.bll_data.[0] + 1 else if b.bll_format = 5 then 32 , 0 else failwith "Unsupported Bitmap format") in
		let w = b.bll_width in
		let h = b.bll_height in
		let data = Extc.unzip (if nbits = 8 then String.sub b.bll_data 1 (String.length b.bll_data - 1) else b.bll_data) in
		Mtw.Group ("Bimp",
			Mtw.Number ("bmid", Int32.of_int id ) ::
			Mtw.Number ("wdth", Int32.of_int w ) ::
			Mtw.Number ("hght", Int32.of_int h ) ::
			if nbits = 8 then begin
				let palet = 
					(if alpha then
						String.sub data 0 (ncols * 4)
					else
						let buf = Buffer.create 0 in
						for i = 0 to ncols-1 do
							Buffer.add_substring buf data (i*3) 3;
							Buffer.add_char buf '\255';
						done;
						Buffer.contents buf)
				in
				let buf = Buffer.create 0 in
				let width2 = ((w + 3) / 4) * 4 in 
				for i = 0 to ncols - 1 do
					let tmp = palet.[i*4] in
					palet.[i*4] <- palet.[i*4+2];
					palet.[i*4+2] <- tmp;
				done;
				for i = 0 to h - 1 do
					Buffer.add_substring buf data (ncols * (if alpha then 4 else 3) + i * width2) w
				done;
				Mtw.Data ("PALT", palet ^ String.make ((256 - ncols) * 4) '\000') ::
				Mtw.Data ("DATA", Buffer.contents buf) ::
				[]
			end else
				f data (w * h)
		)
	in
	match b with
	| Lossless b -> 
		lossless b false (fun data size ->
			let data = (if String.length data = size * 3 then begin
				let buf = Buffer.create 0 in
				for i = 0 to size - 1 do
					Buffer.add_char buf data.[i * 3 + 3];
					Buffer.add_char buf data.[i * 3 + 2];
					Buffer.add_char buf data.[i * 3 + 1];
				done;
				Buffer.contents buf
			end else 
				let buf = Buffer.create 0 in
				for i = 0 to size - 1 do
					Buffer.add_char buf data.[i * 4 + 3];
					Buffer.add_char buf data.[i * 4 + 2];
					Buffer.add_char buf data.[i * 4 + 1];
				done;
				Buffer.contents buf
			) in
			Mtw.Data ("DATA", data) ::
			[]
		)
	| Lossless2 b -> 
		lossless b true (fun data size -> 
			for i = 0 to size - 1 do
				let tmp = data.[i*4] in
				data.[i*4] <- data.[i*4+3];
				data.[i*4+3] <- tmp;
				let tmp = data.[i*4+1] in
				data.[i*4+1] <- data.[i*4+2];
				data.[i*4+2] <- tmp;
			done;
			Mtw.Ident "rGBA" ::
			Mtw.Data ("DATA", data) ::
			[]
		)
	| Jpeg _
	| JpegAlpha _
	| JpegWithTable _ ->
		prerr_endline "Warning : JPEG Bitmaps not supported";
		Mtw.Ident "jPEG"

let process_tag ctx t =
	match t.tdata with
	| TShape s
	| TShape2 s
	| TShape3 s ->
		process_shape ctx s
	| TMorphShape _ ->
		failwith "Morph shape not supported"
	| TClip mc ->
		process_clip ctx mc
	| TExport el ->
		List.iter (fun e ->
			let c = (try Hashtbl.find ctx.mtv_clips e.exp_id with Not_found -> assert false) in
			c.clexport <- Some e.exp_name
		) el;
	| TBitsLossless b -> 
		Hashtbl.add ctx.bitmaps b.bll_id { bmpdata = Lossless b; bmpused = false }
	| TBitsLossless2 b ->
		Hashtbl.add ctx.bitmaps b.bll_id { bmpdata = Lossless2 b; bmpused = false }
	| TBitsJPEG b ->
		Hashtbl.add ctx.bitmaps b.bmp_id { bmpdata = JpegWithTable b; bmpused = false }
	| TBitsJPEG2 b ->
		Hashtbl.add ctx.bitmaps b.bmp_id { bmpdata = Jpeg b; bmpused = false }
	| TBitsJPEG3 b ->
		Hashtbl.add ctx.bitmaps b.jp3_id { bmpdata = JpegAlpha b; bmpused = false }
	| TJPEGTables t ->
		ctx.jpgtable <- Some t
	| _ ->
		()

let process (headers,tags) res id =
	let ctx = mtv_context() in
	List.iter (process_tag ctx) tags;
	let clips = ref [] in
	let shapes = ref [] in
	let loop_shape s =
		if s.used then
			()
		else begin
			s.used <- true;
			shapes := s :: !shapes;
		end;
	in
	let rec loop_clip c =
		if c.clused then
			()
		else begin
			c.clused <- true;
			clips := c :: !clips;
			List.iter (function
				| Place (_,id,is_shape) -> 
					if is_shape then
						loop_shape (Hashtbl.find ctx.mtv_shapes id)
					else
						loop_clip (Hashtbl.find ctx.mtv_clips id)
				| Frame
				| Remove _
				| SetMatrix _
				| SetColor _ ->
					()
			) c.cldata;
		end;
	in
	Hashtbl.iter (fun _ c -> if c.clexport <> None then loop_clip c) ctx.mtv_clips;
	let fsl = ref [] in
	DynArray.iteri (fun i fs ->
		if fs.fillused then fsl := fill_style_to_mtw ctx i fs.filldata :: !fsl
	) ctx.fills;
	let bmp = ref [] in
	Hashtbl.iter (fun i b ->
		if b.bmpused then bmp := bitmap_to_mtw ctx i b.bmpdata :: !bmp
	) ctx.bitmaps;
	let fsl = List.rev !fsl in
	let bmp = List.rev !bmp in
	let shapes = List.map shape_to_mtw (List.rev !shapes) in
	let clips = List.map clip_to_mtw (List.rev !clips) in
	Mtw.Group ("Mtvd", Mtw.Data ("RSNM",res ^ "\000") :: Mtw.Number("mtvi",Int32.of_int id) :: bmp @ fsl @ shapes @ clips)

;;
SwfParser.init SwfZip.inflate SwfZip.deflate;