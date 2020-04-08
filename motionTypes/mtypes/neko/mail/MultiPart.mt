class mail.MultiPart {

	static var REG_HEADER = new Regexp("^([a-zA-Z0-9_\\-]+):(.*)$");
	static var REG_HEADER_DECODE = new Regexp("^(.*?)=\\?([^\\?]+)\\?(Q|B|q|b)\\?([^?]*)\\?=(.*?)$");
	static var REG_START_TAB = new Regexp("^(\t| )+");
	static var REG_QP_LB = new Regexp("=\\r?\\n");
	static var REG_QP = new Regexp("=([A-Fa-f0-9]{1,2})");

	static var REG_CRLF_END = new Regexp("(\r?\n)$");
	
	static function chunkSplit( str:String, length:int, sep:String ){
		var ret = "";
		while( str._length > length ){
			ret += str.sub(0,length) + sep;
			str = str.sub(length,str._length - length);
		}
		return ret + str;
	}

	static function splitLines( str : String ) : List<String> {
		return str.split("\n").map(fun(l){
			if( String.msub(l,-1,1) == "\r" ){
				return l.sub(0,l._length-1);
			}else{
				return l;
			}
		});
	}

	static function encodeBase64( content : String , crlf : String ){
		return String.rtrim(chunkSplit(String.baseEncode( content, String.BASE64MIME ), 76, crlf)) + "==";
	}

	static function decodeBase64( content : String, crlf ){
		return String.baseDecode( String.replace(String.replace(content,crlf,""),"=",""), String.BASE64MIME );
	}

	static function encodeQuotedPrintable( content : String, crlf : String ) : String{
		var rs : List<String> = new List();
		var lines = splitLines( content );
		
		lines.iter(fun(ln:String){
			var len = ln._length;
			var line = "";
			for( var i=0 ; i<len ; i++ ){
				var c = ln.charAt(i);
				var o = c.at(0);
				if( o == 9 ){
				}else if( o < 16 ){
					c = "=0" + String.baseEncode(c,"0123456789ABCDEF");
				}else if( o == 61 || o < 32 || o > 126 ){
					c = "=" + String.baseEncode(c,"0123456789ABCDEF");
				}

				// space at the end of line
				if( i == len - 1 ){
					if( o == 32 ){
						c = "=20";
					}else if( o == 9 ){
						c = "=09";
					}
				}

				// soft line breaks
				var ll = line._length;
				var cl = c._length;
				if( ll + cl >= 76 && (i != len -1 || ll + cl != 76) ){
					rs.add(line + "=");
					line = "";
				}
				line += c;
			}
			rs.add(line);
		});

		return rs.join(crlf);
	}

	static function decodeQuotedPrintable( str : String ){
		var s = Regexp.newOptions("=([0-9A-F]{2})","i");
		str = s.replaceFun(str,fun(r){
			return String.baseDecode(String.upperCase(r.matched(1)),"0123456789ABCDEF");
		});
		str = new Regexp("=\r?\n").replaceAll(str,"");
		return str;
	}

	static function headerQpEncode( ostr : String, initSize : int, charset : String ){
		var str = removeCRLF(ostr);
		
		var len = str._length;
		var quotedStr : List<String> = new List();
		var line = "";
		var useQuoted = false;
		for( var i=0;i<len;i++ ){
			var c = str.charAt(i);
			var o = c.at(0);
			
			if( o == 9 ){
			}else if( o < 16 ){
				useQuoted = true;
				c = "=0" + String.baseEncode(c,"0123456789ABCDEF");
			}else if( o == 61 || o == 58 || o == 63 || o == 95 || o == 34 ){
				c = "=" + String.baseEncode(c,"0123456789ABCDEF");
			}else if( o < 32 && o > 126 ){
				useQuoted = true;
				c = "=" + String.baseEncode(c,"0123456789ABCDEF");
			}else if( o == 32 ){
				c = "_";
			}

			// max line length = 76 - 17 ( =?iso-8859-1?Q?...?= ) => 59 - initSize
			var max : int;
			if( quotedStr._length == 0 ){
				max = 69 - charset._length - initSize;
			}else{
				max = 69 - charset._length;
			}
			if( line._length + c._length >= max ){
				quotedStr.add(line);
				line = '';
			}
			line += c;
		}
		quotedStr.add(line);

		if( !useQuoted ){
			return ostr;
		}else{
			return "=?"+charset+"?Q?"+quotedStr.join("?=\r\n\t=?"+charset+"?Q?")+"?=";
		}
	}

	
	static function headerDecode( str : String, charsetOut : String ){
		while( REG_HEADER_DECODE.match(str) ){
			var charset = String.trim(String.lowerCase(REG_HEADER_DECODE.matched(2)));
			var encoding = String.trim(String.lowerCase(REG_HEADER_DECODE.matched(3)));
			var encoded = String.trim(REG_HEADER_DECODE.matched(4));

			var start = REG_HEADER_DECODE.matched(1);
			var end = REG_HEADER_DECODE.matched(5);

			if( encoding == "q" ){
				encoded = decodeQuotedPrintable(String.replace(encoded,"_"," "));
			}else if( encoding == "b" ){
				encoded = decodeBase64(encoded,"\r\n");
			}else{
				Std.throw( "mail.MultiPart.headerDecode: Unknow transfer-encoding: "+encoding );
			}

			if( charset != charsetOut ){
				if( (String.startsWith(String.lowerCase(charsetOut),"iso-8859-1")||String.startsWith(String.lowerCase(charsetOut),"iso8859-1")) && charset == "utf-8" ){
					encoded =  String.utf8Decode( encoded );
				}else if( (String.startsWith(charset,"iso-8859-1")||String.startsWith(charset,"iso8859-1")) && String.lowerCase(charsetOut) == "utf-8" ){
					encoded =  String.utf8Encode( encoded );
				}
			}

			str = start + encoded + end;
		}

		return str;
	}

	static function removeCRLF( str ){
		return String.replace(String.replace(str,"\n",""),"\r","");
	}

	static function formatHeaderTitle( str : String ) : String {
		str = String.trim( str );
		if( String.lowerCase(str) == "mime-version" ) return "MIME-Version";

		var arr = str.split("-");
		return arr.map(fun(v:String){
			return String.ucfirst(String.lowerCase(v));
		}).join("-");
	}

	static function parseString( str : String ) : mail.MultiPart {
		var o = new mail.MultiPart(null,false,"iso-8859-1");
		o.parse( str, "" );
		return o;
	}

	static function parseFile( f : String ) : mail.MultiPart {
		var str = new File(f).contents();
		return parseString( str );
	}
	
	static var headerOrder = [
		"Return-Path","Received","Date","From","Subject","Sender","To"
		"Cc","Bcc","Content-Type","X-Mailer","X-Originating-IP","X-Originating-User"
	];


	static function countInitTab( str : String ) : int {
		if( REG_START_TAB.match(str) ){
			return REG_START_TAB.matched(0)._length;
		}else{
			return 0;
		}
	}

	static function randomEight(){
		var s = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		var ret = "";
		for( var i=0; i<8; i++ ){
			ret += s.charAt(Std.random(s._length));
		}
		return ret;
	}

	static function pregQuote( str : String ){
		str = String.replace(str,"\\","\\\\");
		str = String.replace(str,".","\\.");
		str = String.replace(str,"+","\\+");
		str = String.replace(str,"*","\\*");
		str = String.replace(str,"?","\\?");
		str = String.replace(str,"^","\\^");
		str = String.replace(str,")","\\)");
		str = String.replace(str,"(","\\(");
		str = String.replace(str,"[","\\[");
		str = String.replace(str,"]","\\]");
		str = String.replace(str,"{","\\{");
		str = String.replace(str,"}","\\}");
		str = String.replace(str,"=","\\=");
		str = String.replace(str,"!","\\!");
		str = String.replace(str,"<","\\<");
		str = String.replace(str,">","\\>");
		str = String.replace(str,"|","\\|");
		str = String.replace(str,":","\\:");
		str = String.replace(str,"$","\\$");
		str = String.replace(str,"/","\\/");
			
		return str;
	}
	
	//////////////////
	
	var content : String;
	var parts : List<mail.MultiPart>;
	var headers : Hash<String>;
	var contentType : String;
	var charset : String;
	var boundary : String;
	var name : String;
	var id : String;
	var subPart : bool;

	function new( ctype : String, sp : bool, charset : String ){
		this.contentType = ctype;
		this.subPart = sp;
		this.charset = charset;

		content = "";
		
		parts = new List();
		headers = new Hash();
	}

	function getContentType(){
		return contentType;
	}

	function addPart( part : mail.MultiPart ){
		parts.push( part );
	}

	function newPart( ctype ){
		var o = new mail.MultiPart( ctype, true, charset );
		this.addPart( o );
		return o;
	}

	function setContent( c : String ){
		addHeader( "Content-Transfer-Encoding", "quoted-printable", false );
		content = c;
	}

	function setContentFromFile(filename,type){
		var f = new File(filename);
		var a = filename.split("/");
		name = a.pop();
		content = f.contents();
		contentType = type;
		addHeader("Content-Type",type+"; name=\""+name+"\"",true);
		addHeader("Content-Disposition","attachment; filename=\""+name+"\"",true);
		addHeader("Content-Transfer-Encoding","base64",true);
	}
	
	function addHeader( name : String, content : String, force : bool ){
		if( !force && headers.exists(name) ) return;

		headers.set(name,content);
	}

	function addDate( d : Date, force : bool ){
		addHeader("Date",d.format("%a, %e %b %Y %H:%M:%S %z"),force);
	}

	function addContentId(){
		if( headers.exists("Content-Id") ) return String.msub( headers.get("Content-Id"),1,-1 );

		var rs = randomEight()+"."+randomEight();

		addHeader("Content-Id","<"+rs+">",true);

		return rs;
	}

	function htmlUseContentId(p:Hash<String>){
		var me = this;
		p.iter(fun(filename,cid){
			me.content = String.replace(me.content,filename,"cid:"+cid);
		});
	}

	function htmlRemoveTab(){
		content = REG_START_TAB.replace(content,"");
	}

	function get( crlf : String ) : String {
		var boundary : String = "";

		if( parts._length > 0 ){
			if( String.lowerCase(String.left(contentType,10)) != "multipart/" ){
				contentType = "multipart/mixed";
			}

			if( boundary._length == 0 ){
				boundary = "----=" + randomEight() + "_" + randomEight() + "." + randomEight();
			}

			addHeader("Content-Type",contentType+"; charset=\""+charset+"\";"+crlf+"\tboundary=\""+boundary+"\"",true);
		}else{
			addHeader("Content-Type",contentType+"; charset=\""+charset+"\"",false);
		}

		if( !subPart ){
			addHeader("MIME-Version","1.0",true);
			addHeader("X-Mailer","MotionTypes Mailer",true);
		}

		var ret = {str: ""};
		
		// copy headers
		var myHeaders = new Hash();
		headers.iter(fun(k,v){
			myHeaders.set(k,v);
		});

		var charset = charset;
		
		// Put standard headers
		headerOrder.iter(fun(p){
			if( myHeaders.exists(p) ){
				ret.str += p+": "+headerQpEncode(myHeaders.get(p),p._length,charset)+crlf;
				myHeaders.remove(p);
			}
		});
		
		// Put other headers
		myHeaders.iter(fun(p,v){
			ret.str += p+": "+headerQpEncode(v,p._length,charset)+crlf;
		});

		ret.str += crlf;

		// Add content
		if( content._length > 0 ){
			if( headers.get("Content-Transfer-Encoding") == "base64" ){
				ret.str += encodeBase64(content,crlf) + crlf;
				
			}else if( headers.get("Content-Transfer-Encoding") == "quoted-printable" ){
				ret.str += encodeQuotedPrintable(content,crlf) + crlf;
				
			}else{
				ret.str += content + crlf;
			}
		}
		
		// Add parts
		if( parts._length > 0 ){
			var pcp = parts.map(fun(v){ return v });
			
			if( contentType == "multipart/alternative" ){
				// text/plain first
				pcp.iter(fun(v:mail.MultiPart){
					if( v.contentType == "text/plain" ){
						ret.str += "--" + boundary + crlf + v.get(crlf);
						pcp.remove(v);
					}
				});
				
				// then text/html
				pcp.iter(fun(v){
					if( v.contentType == "text/html" ){
						ret.str += "--" + boundary + crlf + v.get(crlf);
						pcp.remove(v);
					}
				});
			}

			pcp.iter(fun(v){
					ret.str += "--" + boundary + crlf + v.get(crlf);
			});
			
		}

		return ret.str;
	}


	function parse( str:String, id : String ){
		if( id != "" ) subPart = true;
		this.id = id;

		var me = this;
		var arr = splitLines(str);
		var notSoBasicHeader : List<String> = new List();

		{
			
			var o = {inHead: true,buf: new StringBuf()};
			
			arr.iter(fun(ln){
				if( !o.inHead ){
					o.buf.add( removeCRLF(ln) );
					o.buf.add("\r\n");
				}else{
					if( String.trim(ln)._length == 0 ){
						o.inHead = false;
						notSoBasicHeader.add( o.buf.toString() );
						o.buf = new StringBuf();
					}else{
						var nbTab = countInitTab(ln);
						if( nbTab > 0 ){
							o.buf.add( removeCRLF(String.msub(ln,nbTab,null)) );
						}else{
							notSoBasicHeader.add( o.buf.toString() );
							o.buf = new StringBuf();
							o.buf.add( removeCRLF(ln) );
						}
					}
				}
			});
			content = o.buf.toString();
		}

		
		notSoBasicHeader.iter(fun(ln){
			if( REG_HEADER.match(ln) ){
				me.headers.set(formatHeaderTitle(REG_HEADER.matched(1)),String.trim(REG_HEADER.matched(2)));
			}
		});

		var ctype0 = "text";
		var ctype1 = "plain";

		// parse contentType
		var hctype = parseMultiHeader("Content-Type");
		if( hctype != null ){
			var t = hctype.value.split("/");
			ctype0 = String.lowerCase(String.trim(t.at(0)));
			ctype1 = String.lowerCase(String.trim(t.at(1)));

			if( hctype.params.exists("charset") ){
				charset = hctype.params.get("charset");
			}

			if( hctype.params.exists("boundary") ){
				boundary = hctype.params.get("boundary");
			}

			if( hctype.params.exists("name") ){
				name = hctype.params.get("name");
			}
		}
		
		contentType = ctype0+"/"+ctype1;
		
		headers.iter(fun(k,v){
			me.headers.set(k,headerDecode(v,me.charset));
		});


		if( ctype0 == "multipart" ){
			if( boundary == null || boundary._length == 0 ){
				contentType = "text/plain";
				ctype0 = "text";
				ctype1 = "plain";
			}else{
				this.splitContent();
			}
		}

		if( headers.exists("Content-Transfer-Encoding") ){
			var cte = String.lowerCase( headers.get("Content-Transfer-Encoding"); );
			if( cte == "quoted-printable" ){
				content = decodeQuotedPrintable( content );
			}else if( cte == "base64" ){
				content = decodeBase64( content,"\r\n" );
			}
		}

		var cdispo = parseMultiHeader("Content-Disposition");
		if( cdispo != null && cdispo.params.exists("filename") ){
			name = cdispo.params.get("filename");
		}
	}

	static var REG_MHEADER = new Regexp("^([^;]+)(.*?)$");
	static var REG_PARAM1 = new Regexp("^; *([a-zA-Z]+)=\"(([^\"]|\\\\\")+)\"");
	static var REG_PARAM2 = new Regexp("^; *([a-zA-Z]+)=([^;]+)");
	function parseMultiHeader( headerName ){
		if( !headers.exists( headerName ) ){
			return null;
		}

		var h = headers.get(headerName);

		var ret = {value: null, params: new Hash()};
		if( REG_MHEADER.match(h) ){
			ret.value = String.trim( REG_MHEADER.matched(1) );

			var params = REG_MHEADER.matched(2);
			while( params._length > 0 ){
				params = String.ltrim( params );
				
				if( REG_PARAM1.match( params ) ){
					var k = String.lowerCase(String.trim(REG_PARAM1.matched(1)));
					var v = REG_PARAM1.matched(2);
					ret.params.set( k, v ); 
					params = String.msub(params,REG_PARAM1.matched(0)._length,null);
				}else if( REG_PARAM2.match( params ) ){
					var k = String.lowerCase(String.trim(REG_PARAM2.matched(1)));
					var v = String.trim(REG_PARAM2.matched(2));
					ret.params.set( k, v );
					params = String.msub(params,REG_PARAM2.matched(0)._length,null);
				}else{
					break;
				}
			}
		}else{
			ret.value = h;
		}
		return ret;

	}

	function splitContent(){
		var bound = pregQuote(boundary);
		var regStr = "(.*?)--"+bound+"(.*)--"+bound+"--";
		var reg = Regexp.newOptions(regStr,"s");

		if( reg.match( content ) ){
			content = reg.matched(1);

			var tmp = reg.matched(2).split("--"+boundary);

			var myId = ((id == null || id._length == 0) ? "" : (id + "."));
			var o = {i: 0}
			var me = this;
			tmp.iter(fun(str){
				o.i++;
				if( REG_CRLF_END.match(str) ){
					str = String.msub(str,0,-REG_CRLF_END.matched(1)._length);
				}
				
				var p = me.newPart("text/plain");
				p.parse( String.trim(str), myId+o.i );
			});
		}
	}
}
