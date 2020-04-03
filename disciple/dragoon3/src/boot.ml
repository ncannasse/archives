let crc_unit_list = [
  "Array",
    "\158\011\216\030\164\138\000\157\160\157\068\115\165\189\177\070";
  "Buffer",
    "\124\056\166\221\113\174\250\018\167\109\118\035\102\161\229\247";
  "Callback",
    "\152\230\194\252\081\086\100\176\111\168\107\151\111\037\036\087";
  "CamlinternalOO",
    "\084\223\188\021\029\215\026\215\038\093\028\051\023\154\124\035";
  "Char",
    "\128\143\233\117\077\096\050\063\039\007\121\125\016\127\001\060";
  "Digest",
    "\087\205\086\112\254\234\013\210\159\057\131\117\048\150\187\189";
  "Filename",
    "\048\166\105\038\146\171\204\131\168\121\096\109\105\072\015\093";
  "Gc",
    "\106\155\055\096\169\090\177\227\049\209\160\062\105\095\060\255";
  "Hashtbl",
    "\101\211\111\004\038\098\087\231\085\040\019\159\238\091\237\202";
  "Int32",
    "\232\121\248\055\061\088\228\031\183\251\227\197\018\033\018\244";
  "Int64",
    "\166\200\202\158\188\104\078\060\176\014\000\116\046\221\081\074";
  "Lexing",
    "\077\159\188\146\128\189\203\124\208\189\001\189\117\151\147\015";
  "List",
    "\250\181\051\086\126\133\240\185\172\061\123\021\011\099\096\073";
  "Marshal",
    "\027\106\109\121\205\023\192\131\049\159\090\228\227\155\232\219";
  "Obj",
    "\126\207\147\126\166\069\164\154\031\130\220\138\170\216\136\183";
  "Oo",
    "\193\248\241\202\052\096\032\104\118\083\045\151\192\177\166\006";
  "Parsing",
    "\010\144\146\163\102\020\019\179\013\096\042\143\236\084\021\122";
  "Printexc",
    "\017\108\120\213\076\135\255\075\132\094\012\113\233\053\054\010";
  "Printf",
    "\007\214\111\017\060\218\197\131\108\234\198\218\126\232\018\150";
  "Random",
    "\213\052\194\181\162\044\017\040\233\155\137\125\156\095\139\089";
  "String",
    "\196\170\241\209\057\188\096\008\144\003\060\130\099\017\249\116";
  "Sys",
    "\101\039\182\228\133\235\167\211\253\130\054\233\093\253\161\135";
  "Dynlink",
    "\012\190\252\072\137\184\251\103\191\117\121\169\064\161\203\117";
  "Unix",
    "\120\056\063\120\023\166\100\202\061\220\181\117\025\201\160\053";
  "Pervasives",
    "\063\227\193\060\125\178\249\118\067\094\125\140\228\109\015\239"
] in
try
	Dynlink.init();
	Dynlink.add_available_units crc_unit_list;
	Dynlink.add_interfaces [
		"Xml";
		"Mysql";
	]
	[".\\";".\\lib\\";".\\stdlib\\";"c:\\ocaml\\lib\\"];
	(Dynlink.allow_unsafe_modules true);
	(try (Dynlink.loadfile "c:\\ocaml\\lib\\win32.cma") with Sys_error _ -> Dynlink.loadfile "lib\\win32.cma");
	(Dynlink.loadfile "core.cma");
with
	Dynlink.Error e -> print_endline("Boot error : Dynlink("^(Dynlink.error_message e)^")")
	| error -> (print_endline ("Boot error: "^(Printexc.to_string error)))