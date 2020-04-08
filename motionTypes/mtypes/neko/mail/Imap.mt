class mail.Imap {
	var cnx : Socket;
	var count : int;
	var debugCnx : bool;

	static var REG_CRLF = new Regexp("\r?\n");
	function rmCRLF(s){
		return REG_CRLF.replaceAll(s, "");
	}
	
	function new(args: {host: String, port: int, user: String, pass: String}){
		debugCnx = false;
		count = 0;
		cnx = new Socket();
		connect( args.host, args.port );
		login( args.user, args.pass );
	}

	function command( command, args, r ){
		count++;
		var c = string(count);
		c = String.lpad(c,"A000",4);
		cnx.send( c+" "+command+" "+args+"\r\n" );
		if( debugCnx ) Std.print( "S: "+c+" "+command+" "+args+"\r\n" );

		if( !r ){
			return null;
		}
		return read(c);
	}

	static var REG_RESP = new Regexp("(OK|NO|BAD) (\\[([^\\]]+)\\] )?(([A-Z]{2,}) )? ?(.*)");
	function read( c ){
		var resp = new List();
		var sb : StringBuf = null;
		while( true ){
			var line = cnx.readLine();
			if( debugCnx ) Std.print("R: "+line);
			line = rmCRLF(line);

			if( c != null && String.msub(line,0,4) == c ){
				if( REG_RESP.match(String.msub(line,5,null)) ){
					if( sb != null ){
						resp.add( sb.toString() );
					}
					return {
						result: resp,
						success: REG_RESP.matched(1) == "OK",
						error: REG_RESP.matched(1),
						command: REG_RESP.matched(4),
						response: REG_RESP.matched(6),
						comment: REG_RESP.matched(3)
					};
				}else{
					Std.throw("Unknow response : "+line);
				}
			}else{
				if( String.startsWith(line,"* ") ){
					if( sb != null ){
						resp.add( sb.toString() );
					}
					sb = new StringBuf();
					sb.add( String.msub(line,2,null) );
				}else{
					if( sb != null ){
						sb.add( line+"\r\n" );
					}else{
						resp.add( line );
					}
				}
			}
		}
	}

	function connect( host : String, port : int ){
		try{
			cnx.connect( host, port );
		}catch( e ){
			Std.throw("Unable to connect to imap server "+host+" on port "+port);
		}
		cnx.readLine();
	}

	function login( user : String, pass : String ){	
		var r = command("LOGIN",user+" "+pass,true);
		if( !r.success ){
			Std.throw("Error on login: "+r.response);
		}
	}
	
	/////////
	//
	static var REG_EXISTS = new Regexp("^([0-9]+) EXISTS$");
	static var REG_RECENT = new Regexp("^([0-9]+) RECENT$");
	static var REG_UNSEEN = new Regexp("^OK \\[UNSEEN ([0-9]+)\\]");
	
	function select( mailbox : String ){
		var r = command("SELECT",mailbox,true);
		if( !r.success ) {
			Std.throw("Unable to select "+mailbox+": "+r.response);
		}
		
		var ret = {recent: 0,exists: 0,firstUnseen: null};
		r.result.iter(fun(v){
			if( REG_EXISTS.match(v) ){
				ret.exists = int(REG_EXISTS.matched(1));
			}else if( REG_UNSEEN.match(v) ){
				ret.firstUnseen = int(REG_UNSEEN.matched(1));
			}else if( REG_RECENT.match(v) ){
				ret.recent = int(REG_RECENT.matched(1));
			}
		});

		return ret;
	}

	static var REG_LIST_RESP = new Regexp("LIST \\(([ \\\\A-Za-z0-9]*)\\) \"\\.\" \"([^\"]+)\"");
	function mailboxes( pattern : String ){
		var r;
		if( pattern == null ){
			r = command("LIST","\".\" \"*\"",true);
		}else{
			r = command("LIST","\".\" \""+pattern+"\"",true);
		}
		if( !r.success ){
			Std.throw("Unable to list mailboxes (pattern: "+pattern+"): "+r.response);
		}

		var ret = new List();
		r.result.iter(fun(v){
			if( REG_LIST_RESP.match(v) ){	
				var name = REG_LIST_RESP.matched(2);
				var flags = REG_LIST_RESP.matched(1).split(" ");

				var t = {name: name,flags: null,hasChildren: null};

				t.flags = flags.filter(fun(v){
					if( v == "" ) return false;
					
					if( v == "\\HasNoChildren" ){
						t.hasChildren = false;
					}else if( v == "\\HasChildren" ){
						t.hasChildren = true;
					}
					return true;
				});

				ret.add(t);
			}
		});
		return ret;
	}

	function search( pattern : String ){
		if( pattern == null ) pattern = "ALL";
		var r = command("SEARCH",pattern,true);
		if( !r.success ){
			Std.throw("Unable to search messages (pattern: "+pattern+"): "+r.response);
		}

		var l = new List();

		r.result.iter(fun(v){
			if( String.startsWith(v,"SEARCH ") ){
				var t = String.msub(v,7,null).split(" ");
				t.iter(fun(i){
					l.add( int(i) );
				});
			}
		});

		return l;
	}

	function fetchSearch( pattern : String, markAsSeen : bool ){
		var r = search(pattern);
		if( r._length == 0 ) return new IHash();

		return fetchRange( r.join(","), markAsSeen );
	}

	function fetchOne( id : int, markAsSeen : bool ) {
		var r = fetchRange( string(id), markAsSeen );
		if( r.exists(id) ){
			return r.get(id);
		}
		Std.throw("fetchOne failed");
		return null;
	}

	static var REG_FETCH_BODY = new Regexp("([0-9]+) FETCH \\(BODY\\[\\] \\{([0-9]+)\\}");
	static var REG_FETCH_FLAGS = new Regexp("([0-9]+) FETCH \\(FLAGS \\(([ \\\\A-Za-z0-9]*)\\)\\)");
	static var REG_FETCH_END = new Regexp("^([A0-9]{4}) (OK|BAD|NO)");
	function fetchRange( range : String, markAsSeen : bool ){
		if( range == null ) return null;
		var section = "BODY[]"
		if( !markAsSeen ){
			section = "BODY.PEEK[]";
		}
		command("FETCH",range+" "+section,false);

		var ret = new IHash();
		while( true ){
			var l = cnx.readLine();
			if( REG_FETCH_BODY.match(l) ){
				var id = int(REG_FETCH_BODY.matched(1));
				var len = int(REG_FETCH_BODY.matched(2));
				var o = null;
				if( ret.exists(id) ){
					o = ret.get(id);
				}else{
					o = {body: null,flags: null};
					ret.set(id,o);
				}
				o.body = cnx.read( len );
				cnx.readLine();
			}else if( REG_FETCH_FLAGS.match(l) ){
				var id = int(REG_FETCH_FLAGS.matched(1));
				var flags = REG_FETCH_FLAGS.matched(2).split(" ");
				var o = null;
				if( ret.exists(id) ){
					o = ret.get(id);
				}else{
					o = {body: null,flags: null};
					ret.set(id,o);
				}
				Std.print("set flags of id :"+id+"\n");
				o.flags = flags;
			}else if( REG_FETCH_END.match(l) ){
				var resp = REG_FETCH_END.matched(2);
				if( resp == "OK" ){
					break;
				}else{
					Std.throw("Error fetching messages : "+l);
				}
			}else{
				Std.throw("Unknow response from fetch : "+l);
			}
		}

		return ret;
	}
}
