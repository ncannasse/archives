open Mysql
open Printf

exception Error of string
exception ID_out of int
exception Invalid_version of (int * int)
exception Invalid_link_version of (int * int)

type pinfos = {
	pname : string;
	pparent : int;
	ptype : int;
	pver : int;
}

type pdata = {
	pdata : string;
	pdeps : int list;
}

type phandle = {
	pinf : mydata;
	mutable phandle : mysql;
	mutable pconnected : bool;
	rootid : int;
	pprj : string;
}

let handle ph = 
	if not ph.pconnected then ph.phandle <- Mysql.connect ph.pinf;
	ph.phandle

let error str = raise (Error str)

let connect ~host ~project ~user ~passwd =
	let infos = { host = host; port = 0; user = user; passwd = passwd; dbname = "dragoon3" } in
	let handle = Mysql.connect infos in
	let result = query handle (sprintf "SELECT root FROM project WHERE name = '%s'" project) in
		if (count result) = 0 then error ("No such project: "^project);
	let rootid = int (fetch result) "root" in
		{ pinf = infos; phandle = handle; pconnected = true; rootid = rootid; pprj = project }

let last_id = ref (-1)

let get_last_id () = !last_id

let get_root ph =
	ph.rootid

let rec check_domain ph id =
	if ph.rootid <> id then begin
		let h = handle ph in
		let result = query h (sprintf "SELECT parent FROM file WHERE id = '%d'" id) in
			if (count result) = 0 then raise (ID_out id);
			last_id := id;
			check_domain ph (int (fetch result) "parent");
	end

let get_infos ph id =
	check_domain ph id;
	let h = handle ph in
	let result = query h (sprintf "SELECT name,parent,type,version FROM file WHERE id='%d'" id) in
		if (count result) = 0 then error ("Unknown ID: "^(string_of_int id));
		let data = (fetch result) in
		{
			pname = string data "name";
			pparent = int data "parent";
			ptype = int data "type";
			pver = int data "version";
		}

let get_childs ph id =
	check_domain ph id;
	let h = handle ph in
	let result = query h (sprintf "SELECT id FROM file WHERE parent='%d'" id) in
		to_list (fun r -> int r "id") result

let get_deps ph id =
	[]

exception Return of int
let return n = raise (Return n)

let lock h = ignore( query h "LOCK TABLES file WRITE" )
let unlock h = ignore( query h "UNLOCK TABLES" )

let rec create ~phandle ~name ~parent ~ftype =
	let h = handle phandle in
	(* Check that parent is in the project scope.
		If parent has been deleted, it will raise ID_out(-1)
		which will be catched by VFS to try to undelete parent *)
		check_domain phandle parent;
	try 
	(* Lock table to prevent multiple access *)
		lock h;
	(* Check if we already have a file with the
		same name and the same parent *)
		let result = query h (sprintf "SELECT id,type FROM file WHERE parent = '%d' AND name = '%s'" parent name) in
		if (count result) > 0 then begin
	(* Unlock table : we won't write this time *)
			unlock h;
	(* We suppose here that we can't have more than
		one file with the same name : the system
		is correct	*)
			let f = (fetch result) in 
	(* If the file has the same name+parent+type,
		then we ask the user if he wants to use the
		current file or create a new one using a
		different name. *)
	(* If the file type is different, we always
		create a file with a different name without
		notifying the user *)
			if (int f "type") = ftype && Osiris.question_box (sprintf "File '%s' already exists on server,\nrename to '_%s' ? ( NO = replace )" name name) then
				let id,_ = create ~phandle ~name:("_"^name) ~parent ~ftype in return id
			else
				return (int f "id");				
		end;
	(* Now we can try to create a new file
		and to get his ID.
		As the table is locked, we suppose that
		the SELECT will return us one unique result *)
		let result = query h (sprintf "INSERT INTO file(name,parent,type) VALUES ('%s','%d','%d')" name parent ftype) in
		if (affected result) = 0 then begin ignore( query h "UNLOCK TABLES" ); failwith ("Fail to create file "^name); end;
		let result = query h (sprintf "SELECT id FROM file WHERE parent = '%d' AND name = '%s'" parent name) in
		ignore( query h "UNLOCK TABLES");
		return (int (fetch result) "id");
	with
		Return id -> (id,name)

let rename ph id name =
	let h = handle ph in
	(* Check the scope of id - see create *)
		check_domain ph id;
	(* Retreive current file parent :
		we don't want the file to be moved
		between the query, so we're locking
		the table *)
		lock h;
		let result = query h (sprintf "SELECT parent FROM file WHERE id='%d'" id) in
	(* If the file is not found, then it
		is a client error *)
		if (count result) = 0 then begin unlock h; error ("No such file to rename: #"^(string_of_int id)); end;		
		let pid = int (fetch result) "parent" in
		let result = query h (sprintf "SELECT id,type FROM file WHERE parent = '%d' AND name = '%s'" pid name) in
	(* Get id+type for all the files with the same name
		under the current parent.
		If no file is found, then we rename and we check that
		the update has been done.
		If we got one file, then perhaps the current file
		has already been renamed, so we do nothing.
		If a different file has the same name, then we abort *)
		let b =(
			if (count result) = 0 then affected (query h (sprintf "UPDATE file SET name = '%s' WHERE id = '%d'" name id)) = 1
			else if (count result) = 1 && (int (fetch result) "id") = id then true (* already renamed *)
			else false)
		in
			unlock h;
			b

let move ph id parent =
	let h = handle ph in
	check_domain ph id;
	check_domain ph parent;
	(********TODO**** Check also that we don't create a cycle !! *)
	ignore( query h (sprintf "UPDATE file SET parent='%d' WHERE id='%d'" parent id) )

let delete ph id =
	let h = handle ph in
	(* Check the scope of id - see create *)
	try
		check_domain ph id;
	(* Update and check that it has been done 
		Error cases seems NULL *)
		let r = query h (sprintf "UPDATE file SET parent='-1' WHERE id='%d'" id) in
		if affected r <> 1 then error ("Fail to delete #"^(string_of_int id))
	with
	(* Ignore the exception if file already deleted ( no need to undelete ) *)
		ID_out (-1) -> ()

let undelete ~phandle ~id ~parent =
	let h = handle phandle in
	(* Check the scope of parent - see create *)
	(* We don't check the scope of 'id' because
		if 'id' is deleted then the file is out
		of scope *)
		check_domain phandle parent;
	ignore( query h (sprintf "UPDATE file SET parent='%d' WHERE id='%d'" parent id) )		

let update ~phandle ~id ~version ~deps ~data =
	let h = handle phandle in
	check_domain phandle id;
	ignore( query h (sprintf "LOCK TABLES file WRITE, data WRITE") );
	let r = query h (sprintf "SELECT version FROM file WHERE id = '%d'" id) in
	if count r <> 1 then begin unlock h; error ("No such file to update: #"^(string_of_int id)); end;
	let ver = int (fetch r) "version" in
	if ver <> version then begin unlock h; raise (Invalid_version (id,ver)); end;
	let data = escape data and deps = escape (Marshal.to_string deps []) in
	match count (query h (sprintf "SELECT version FROM data WHERE id = '%d' AND version = '%d' AND data = '%s' AND deps = '%s'" id ver data deps)) with
	| 0 ->
		ignore( query h (sprintf "INSERT INTO data(id,version,data,deps) VALUES ('%d','%d','%s','%s')" id (ver+1) data deps) );
		ignore( query h (sprintf "UPDATE file SET version='%d' WHERE id = '%d'" (ver+1) id) );
		unlock h;
		true
	| _ ->
		unlock h;
		false

let update_link ~phandle ~id ~version ~data =
	let h = handle phandle in
	check_domain phandle id;
	ignore( query h (sprintf "LOCK TABLES file WRITE, file as F1 WRITE, file as F2 WRITE, data WRITE") );
	let r = query h (sprintf "SELECT F2.id,F2.version FROM file as F1,file as F2 WHERE F1.id = '%d' AND F2.id = F1.hlink" id) in	
	if count r <> 1 then begin unlock h; error ("No such link to update: #"^(string_of_int id)); end;
	let result = fetch r in
	let ver = int result "version" in
	let lid = int result "id" in
	if ver <> version then begin unlock h; raise (Invalid_link_version (id,version)); end;
	ignore( query h (sprintf "INSERT INTO data(id,version,data) VALUES ('%d','%d','%s')" lid (ver+1) (escape data)) );
	ignore( query h (sprintf "UPDATE file SET version='%d' WHERE id = '%d'" (ver+1) lid) );
	unlock h


let checkout phandle id =
	let h = handle phandle in
	check_domain phandle id;
	let result = query h (sprintf "SELECT data,deps FROM data,file WHERE file.id = '%d' AND data.id = file.id AND data.version = file.version" id) in
	if count result <> 1 then None else
	let data = fetch result in
	Some { pdata = string data "data"; pdeps = Marshal.from_string (string data "deps") 0; }

let checkout_link phandle id =
	let h = handle phandle in
	check_domain phandle id;
	let q = (sprintf "SELECT data FROM data,file as F1, file as F2 WHERE F1.id = '%d' AND F2.id = F1.hlink AND data.id = F2.id AND data.version = F2.version" id) in
	let r = query h q in
	if count r <> 1 then None else Some (string (fetch r) "data")

let create_link phandle id name =
	let h = handle phandle in
	check_domain phandle id;
	let r = query h (sprintf "SELECT * FROM file WHERE hlink = '%d'" id) in
	if count r <> 0 then error ("The file #"^(string_of_int id)^" already have an hlink");
	let r = query h (sprintf "INSERT INTO file(name,parent,type,hlink) VALUES ('%s',0,-1,%d)" name id) in
	let r = query h (sprintf "SELECT id FROM file WHERE hlink = '%d'" id) in
	if count r <> 1 then error ("Failed to create hlink !");
	let linkid = int (fetch r) "id" in
	ignore( query h (sprintf "UPDATE file SET hlink = '%d' WHERE id = '%d'" linkid id) )

let get_link phandle id =
	let h = handle phandle in
	check_domain phandle id;
	let r = query h (sprintf "SELECT hlink FROM file WHERE id = '%d'" id) in
	if count r <> 1 then error ("The file #"^(string_of_int id)^" cannot be retreived");
	let linkid = int (fetch r) "hlink" in
	let result = query h (sprintf "SELECT name,parent,type,version FROM file WHERE id='%d'" linkid) in
	if (count result) = 0 then error ("Unknown HLink ID: "^(string_of_int linkid));
	let data = (fetch result) in
	{
		pname = string data "name";
		pparent = int data "parent";
		ptype = int data "type";
		pver = int data "version";
	}

let close ph =
	Mysql.close (handle ph);
	ph.pconnected <- false
;;
Run.fast_register "Protocol"