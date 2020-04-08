class mail.Browser extends mail.MultiPart {

	static function parseString( str : String ) : mail.Browser {
		var o = new mail.Browser(null,false,"iso-8859-1");
		o.parse( str, "" );
		return o;
	}

	//////////

	function new(ctype : String, sp : bool, charset : String){
		super( ctype, sp, charset );
	}

	function newPart( ctype : String ) {
		var o = new mail.Browser( ctype, true, charset );
		this.addPart( o );
		return o;
	} 

	function getMainPart(){
		return getMainPartRec( 0, 0, 0 );
	}

	function getMainPartCharset( cs : String ){
		var r = getMainPart();
		if( cs != r.charset ){
			if( (String.startsWith(String.lowerCase(cs),"iso-8859-1")||String.startsWith(String.lowerCase(cs),"iso8859-1")) && String.lowerCase(r.charset) == "utf-8" ){
				r.content =  String.utf8Decode( r.content );
			}else if( (String.startsWith(String.lowerCase(r.charset),"iso-8859-1")||String.startsWith(String.lowerCase(r.charset),"iso8859-1")) && String.lowerCase(cs) == "utf-8" ){
				r.content =  String.utf8Encode( r.content );
			}
			r.charset = cs;
		}
		return r;
	}

	function getMainPartRec( level : int, priority : int, cpriority : int ) : {ctype_primary: String,ctype_secondary: String,charset: String,content: String}{
		var ctype = contentType.split("/");
		var ctype0 = ctype.at(0);
		var ctype1 = ctype.at(1);

		if( ctype0 != "multipart" || (level == 0 && parts._length == 0) ){
			if( level == 0 ) return mkBody();
			if( ctype1 == "html" ) return mkBody();
			if( ctype1 == "plain" && cpriority > 0 ) return mkBody();
		}else{
			if( level == 0 ){
				// multipart !
				// si c'est au premier niveau, c'est une boucle principale, avec priorité qui augmente
				do {
					do {
						var r = Std.cast(parts).find(fun(part:mail.Browser){
							return part.getMainPartRec( level + 1, priority, cpriority );
						});
						if( r != null ) return r;
						priority++;
					}while( priority <= 1 );
					cpriority++;
				}while( cpriority <= 1 );
			}else{
				// là c'est des boucles qui se déclanche si c'est ok
				if( ctype1 == "alternative" || priority > 0 ){
					return Std.cast(parts).find(fun(part:mail.Browser){
						return part.getMainPartRec( level + 1, priority, cpriority );
					});
				}
			}
		}
		return null;
	}

	function listAttachment( level : int ){
		var l = listAttachmentObjects( level );
		return l.map(fun(v){
			return {
				name: v.name, 
				id: v.id, 
				type: v.contentType
			};
		});
	}

	function listAttachmentObjects( level : int ) : List<mail.Browser> {
		var ctype = contentType.split("/");
		var ctype0 = ctype.at(0);
		var ctype1 = ctype.at(1);

		var ret = new List();
		if( ctype0 != "multipart" ){
			if( level != 0 && headers.exists("Content-Disposition") ){
				ret.add( this );
			}
		}else if( ctype1 != "alternative" ){
			Std.cast(parts).iter(fun(part:mail.Browser){
				var t = part.listAttachmentObjects( level + 1);
				if( t._length > 0 ){
					t.iter(fun(v){
						ret.add( v );
					});
				}
			});
		}
		return ret;
	}

	function getAttachment( i : String ) : {name: String,ctype: String,content: String }{
		if( id == i ){
			return {
				name: name,
				ctype: contentType,
				content: content
			};
		}
		return Std.cast(parts).find(fun(part:mail.Browser){
			return part.getAttachment( i );
		});
	}

	function getAttachmentByCid( cid : String ) : {name: String,ctype: String,content: String }{
		if( headers.exists("Content-Id") && String.trim(headers.get("Content-Id")) == "<"+cid+">" ){
			return {
				name: name,
				ctype: contentType,
				content: content
			};
		}
		return Std.cast(parts).find(fun(part:mail.Browser){
			return part.getAttachmentByCid( cid );
		});
	}
	
	function hasHeader( name ){
		name = mail.MultiPart.formatHeaderTitle( name );
		return headers.exists( name );
	}

	function getHeader( name, cs ){
		name = mail.MultiPart.formatHeaderTitle( name );
		if( !headers.exists( name ) ){
			return null;
		}

		var r = headers.get( name );
		
		if( cs != null && cs != charset ){
			if( (String.startsWith(String.lowerCase(cs),"iso-8859-1")||String.startsWith(String.lowerCase(cs),"iso8859-1")) && String.lowerCase(charset) == "utf-8" ){
				r =  String.utf8Decode( r );
			}else if( (String.startsWith(String.lowerCase(charset),"iso-8859-1")||String.startsWith(String.lowerCase(charset),"iso8859-1")) && String.lowerCase(cs) == "utf-8" ){
				r =  String.utf8Encode( r );
			}
		}
		return r;
	}

	function mkBody() : {ctype_primary: String,ctype_secondary: String,charset: String,content: String} {
		var ctype = contentType.split("/");

		return {
			ctype_primary: ctype.at(0),
			ctype_secondary: ctype.at(1),
			charset: charset,
			content: content
		};
	}

	function toString(  ){
		return toStringRec( 0 );
	}

	function toStringRec( level : int ) : String {
		var s = String.lpad("","\t",level);
		var s2 = String.lpad("","\t",level+1);
		var sb = new StringBuf();
		
		sb.add( s );
		sb.add("mail.Browser#");
		sb.add(id);
		sb.add("<");
		sb.add(contentType);
		sb.add(">");

		var sb2 = new StringBuf();

		if( headers.exists("From") ){
			sb2.add(s2);
			sb2.add("From: ");
			sb2.add(getHeader("From","utf-8"));
			sb2.add("\n");
		}

		if( headers.exists("To") ){
			sb2.add(s2);
			sb2.add("To: ");
			sb2.add(getHeader("To","utf-8"));
			sb2.add("\n");
		}

		if( headers.exists("Subject") ){
			sb2.add(s2);
			sb2.add("Subject: ");
			sb2.add(getHeader("Subject","utf-8"));
			sb2.add("\n");
		}

		if( headers.exists("Date") ){
			sb2.add(s2);
			sb2.add("Date: ");
			sb2.add(getHeader("Date","utf-8"));
			sb2.add("\n");
		}		

		if( name != null ){
			sb2.add(s2);
			sb2.add("Name: ");
			sb2.add(name);
			sb2.add("\n");
		}

		Std.cast(parts).iter(fun( part : mail.Browser ){
			sb2.add( part.toStringRec( level + 1) );
		});

		var t = sb2.toString();

		if( t._length > 0 ){
			sb.add(" [\n");
			sb.add(t);
			sb.add(s);
			sb.add("]");
		}
		
		sb.add("\n");
		return sb.toString();
	}

}
