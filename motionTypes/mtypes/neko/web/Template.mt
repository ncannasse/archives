class Template {

	static var COMPRESS = false;
	static var CLEAN_FREQUENCY = 50;
	static var BASEDIR = "";
	static var NO_CACHE = false;
	static var CACHE_TIMEOUT = new Date("00:02:00");

	static var splitter = new Regexp("::[A-Za-z0-9_ ()&|!+=/><*.-]+::");
	static var macros = new Regexp("(\\$\\$[A-Za-z0-9_-]+)\\(");
	static var blanks = new Regexp("[\n\r\t ]+");
	static var expr_op = new Regexp("[!+=/><*.&|-]+");
	static var expr_var = new Regexp("[A-Za-z0-9_]+");
	static var expr_par = new Regexp("[()]");
	static var expr_number = new Regexp("[0-9]+");
	static var hcache : Hash<{ time : Date, t : Template }> = new Hash();
	static var cache_hits = 0;

	static var __ensure_order = TemplateOp.OP_NORMAL;
	static var globals : Hash<'a> = new Hash();

	var execute : TemplateOp -> void;
	var locals_stack : List<{}>;
	var locals : {};
	var params : Hash<'a>;
	var text : List<TemplateOp>;
	var buf : StringBuf;

	function new( data ) {
		params = new Hash();
		locals_stack = new List();

		var tokens = parseTokens(data);

		// make ops
		text = new List();
		while( true ) {
			var t = parse(tokens);
			if( t == null )
				break;
			text.add(t);
		}
		execute = callback(this,_execute);
	}

	static function chop(s : String) {
		if( s == "" )
			return s;
		while( true ) {
			var c = s.charAt(0);
			if( c != " " && c != "\n" && c != "\r" && c != "\t" )
				break;
			s = s.sub(1,s._length-1);
		}
		while( true ) {
			var c = s.charAt(s._length-1);
			if( c != " " && c != "\n" && c != "\r" && c != "\t" )
				break;
			s = s.sub(0,s._length-1);
		}
		return s;
	}

	function parseTokens( data : String ) : List<{ p : String, s : String }> {
		// parse tokens
		var tokens = new List();
		var p1 = null;
		var p2 = null;
		if( splitter.match(data) )
			p1 = splitter.matchedPos(0);
		if( macros.match(data) )
			p2 = macros.matchedPos(0);
		while( p1 != null || p2 != null ) {
			if( p1 != null && (p2 == null || p1.pos < p2.pos) ) {
				var param = data.sub(p1.pos + 2,p1.len - 4);
				tokens.add({ p : param, s : data.sub(0,p1.pos) });
				data = data.sub(p1.pos + p1.len,null);
				if( p2 != null )
					p2.pos -= p1.pos + p1.len;
				if( splitter.match(data) )
					p1 = splitter.matchedPos(0);
				else
					p1 = null;
			} else {
				var parp = p2.pos + p2.len;
				var npar = 1;
				while( npar > 0 ) {
					var c = data.at(parp);
					if( c == 40 )
						npar++;
					else if( c == 41 )
						npar--;
					else if( c == null )
						Std.throw("Unclosed macro parenthesis");
					parp++;
				}
				var l : List<String> = data.sub(p2.pos+p2.len,parp-p2.pos-p2.len-1).split(",").map(fun(s) { return s.split("#comma#").join(",") });
				if( l._length == 1 && l.first() == "" )
					l = new List();
				else
					l = l.map(chop);
				tokens.add({
					p : macros.matched(1),
					l : Std.cast(l).map(parseTokens),
					s : data.sub(0,p2.pos)
				});
				data = data.sub(parp,null);
				if( p1 != null ) {
					p1.pos -= parp;
					if( p1.pos < 0 ) {
						if( splitter.match(data) )
							p1 = splitter.matchedPos(0);
						else
							p1 = null;
					}
				}
				if( macros.match(data) )
					p2 = macros.matchedPos(0);
				else
					p2 = null;
			}
		}
		tokens.add({ p : null, s : data });
		return tokens;
	}

	function parseExprTokens( data : String ) : List<{ kind : int, s : String }> {
		var l = new List();
		var p = [null,null,null];
		var e = [expr_op,expr_var,expr_par];
		if( expr_op.match(data) )
			p[0] = expr_op.matchedPos(0);
		if( expr_var.match(data) )
			p[1] = expr_var.matchedPos(0);
		if( expr_par.match(data) )
			p[2] = expr_par.matchedPos(0);
		var big = 100000000;
		while( p[0] != null || p[1] != null || p[2] != null ) {
			var p0m = (p[0] == null)?big:p[0].pos;
			var p1m = (p[1] == null)?big:p[1].pos;
			var p2m = (p[2] == null)?big:p[2].pos;
			var k, i;
			if( p0m < p1m && p0m < p2m ) {
				i = 0;
				k = TemplateOp.TK_OP;
			} else if( p1m < p2m ) {
				i = 1;
				k = TemplateOp.TK_VAR;
			} else {
				i = 2;
				k = (expr_par.matched(0) == "(")?TemplateOp.TK_POPEN:TemplateOp.TK_PCLOSE;
			}
			l.add({ kind : k, s : data.sub(p[i].pos,p[i].len) });
			var delta = p[i].pos + p[i].len;
			data = data.sub(delta,null);
			if( p[0] != null ) p[0].pos -= delta;
			if( p[1] != null ) p[1].pos -= delta;
			if( p[2] != null ) p[2].pos -= delta;
			if( e[i].match(data) )
				p[i] = e[i].matchedPos(0);
			else
				p[i] = null;
		}
		return l;
	}

	function parseExpr( tokens : List<{ kind : int, s : String }> ) : void -> 'a {
		var tok = tokens.pop();
		switch( tok.kind ) {
		case TemplateOp.TK_POPEN:
			var e1 = parseExpr(tokens);
			if( e1 == null )
				return null;
			var op = tokens.pop();
			if( op.kind != TemplateOp.TK_OP ) {
				if( op.kind == TemplateOp.TK_PCLOSE )
					return Std.cast(e1);
				return null;
			}
			if( op.s == "." ) {
				var ee1 = e1;
				var t = tokens.pop();
				if( t.kind != TemplateOp.TK_VAR )
					return null;
				if( tokens.pop().kind != TemplateOp.TK_PCLOSE )
					return null;
				var f : int = Std.fieldId(t.s);
				return fun() {
					return Std.cast(Std.field(Std.cast(ee1()),f));
				};
			}
			var e2 = parseExpr(tokens);
			if( e2 == null )
				return null;
			if( tokens.pop().kind != TemplateOp.TK_PCLOSE )
				return null;
			switch( op.s ) {
			case "==":
				return fun() { return Std.cast(e1() == e2()) };
			case "!=":
				return fun() { return Std.cast(e1() != e2()) };
			case ">":
				return fun() { return Std.cast(e1() > e2()) };
			case "<":
				return fun() { return Std.cast(e1() < e2()) };
			case ">=":
				return fun() { return Std.cast(e1() >= e2()) };
			case "<=":
				return fun() { return Std.cast(e1() <= e2()) };
			case "+":
				return fun() { return Std.cast(e1() + e2()) };
			case "-":
				return fun() { return Std.cast(Std.cast(e1()) - Std.cast(e2())) };
			case "*":
				return fun() { return Std.cast(Std.cast(e1()) * Std.cast(e2())) };
			case "/":
				return fun() { return Std.cast(Std.cast(e1()) / Std.cast(e2())) };
			case "&&":
				return fun() { return Std.cast(Std.cast(e1()) && Std.cast(e2())) };
			case "||":
				return fun() { return Std.cast(Std.cast(e1()) || Std.cast(e2())) };
			default:
				Std.print("Unknown op '"+op.s+"'<br/>");
				return null;
			}
		case TemplateOp.TK_OP:
			switch( tok.s ) {
			case "!":
				var e = parseExpr(tokens);
				return fun() { return Std.cast(! Std.cast(e()) ) };
			default:
				return null;
			}
		case TemplateOp.TK_VAR:
			if( expr_number.exactMatch(tok.s) ) {
				var n = int(tok.s);
				return fun() { return Std.cast(n) };
			}
			return Std.cast(callback(this,resolveId,tok.s,Std.fieldId(tok.s)));
		}
		return null;
	}

	function parse(tokens : List<{ p : String, s : String }> ) : TemplateOp {
		var tk = tokens.pop();
		if( tk == null )
			return null;

		var p = tk.p;
		var op = TemplateOp.OP_NORMAL;
		var e = null;
		var subs = null;
		var elses = null;
		var plen = (p == null)?null:p._length;
		var isif = (plen > 3 && p.sub(0,3) == "if ");

		if( isif || (plen > 7 && p.sub(0,7) == "elseif ") ) {
			p = isif?p.sub(3,p._length - 3):p.sub(7,p._length - 7);
			op = isif?TemplateOp.OP_IF:TemplateOp.OP_ELSEIF;
			subs = new List();
			var t = null;
			while( true ) {
				t = parse(tokens);
				if( t == null )
					break;
				if( t.op == TemplateOp.OP_ELSEIF ) {
					subs.add({
						op : TemplateOp.OP_NORMAL,
						expr : null,
						fid : null,
						str : t.str,
						subs : null,
						elses : null
					});
					t.str = "";
					break;
				}
				subs.add(t);
				if( t.op == TemplateOp.OP_END || t.op == TemplateOp.OP_ELSE )
					break;
			}
			if( t.op == TemplateOp.OP_ELSEIF ) {
				elses = new List();
				elses.push(t);
			}
			if( t.op == TemplateOp.OP_ELSE ) {
				elses = new List();
				while( true ) {
					t = parse(tokens);
					if( t == null )
						break;
					elses.add(t);
					if( t.op == TemplateOp.OP_END )
						break;
				}
			}
		} else if( plen > 8 && p.sub(0,8) == "foreach " ) {
			p = p.sub(8,p._length - 8);
			op = TemplateOp.OP_FOREACH;
			subs = new List();
			while( true ) {
				var t = parse(tokens);
				if( t == null )
					break;
				subs.add(t);
				if( t.op == TemplateOp.OP_END )
					break;
			}
		} else if( p == "end" ) {
			p = null;
			op = TemplateOp.OP_END;
		} else if( p == "else" ) {
			p = null;
			op = TemplateOp.OP_ELSE;
		} else if( plen > 2 && p.sub(0,2) == "$$" ) {
			var p2 = p.sub(2,null);
			e = fun() { return p2; };
			p = null;
			op = TemplateOp.OP_MACRO;
			var me = this;
			subs = Std.cast(tk).l.map(fun(tl) {
				var l = new List();
				while( true ) {
					var t = me.parse(tl);
					if( t == null )
						break;
					l.add(t);
				}
				return l;
			});
		}
		if( p != null ) {
			var l = parseExprTokens(p);
			e = parseExpr(l);
			if( l._length > 0 || e == null )
				Std.throw("Invalid template expression : '" +p+"'");
		}
		return { op : op, expr : e, str : tk.s, subs : subs, elses : elses };
	}

	function set( p : String, v : 'a ) {
		params.set(p,Std.cast(v));
	}

	function resolve( n : String ) {
		return resolveId(n,Std.fieldId(n));
	}

	function resolveId( n : String, fid : int ) {
		var p = Std.field(locals,fid);
		if( p == null ) {
			p = params.get(n);
			if( p == null ) {
				p = locals_stack.find(fun(o) { return Std.field(o,fid) });
				if( p == null )
					p = globals.get(n);
			}
		}
		return p;
	}

	function _execute( t : TemplateOp ) {
		buf.add(t.str);
		if( t.expr != null ) {
			var p = t.expr();
			switch( t.op ) {
			case TemplateOp.OP_NORMAL:
				buf.add(p);
				break;
			case TemplateOp.OP_ELSEIF:
			case TemplateOp.OP_IF:
				if( Std.cast(p) != null && Std.cast(p) != false )
					t.subs.iter(execute);
				else if( t.elses != null )
					t.elses.iter(execute);
				break;
			case TemplateOp.OP_FOREACH:
				var pa : List<{}> = Std.cast(p);
				if( pa == null )
					break;
				if( Std.neko("$typeof(pa) != $tobject ") ) {
					buf.add("NOT-A-LIST");
					break;
				}
				var me = this;
				var i = 0;				
				var l = pa._length;
				var old_cur = params.get("cur");
				var old_i = params.get("i");
				locals_stack.push(locals);
				pa.iter(fun(h) {
					me.params.set("cur",Std.cast(h));
					me.params.set("i",i++);
					me.locals = h;
					t.subs.iter(me.execute);
				});
				params.set("cur",old_cur);
				params.set("i",old_i);
				locals = locals_stack.pop();
				break;
			case TemplateOp.OP_MACRO:
				var me = this;
				var oldc;
				var pa : List<String> = Std.cast(t.subs).map(fun(l) {
					var oldbuf = me.buf;
					me.buf = new StringBuf();
					l.iter(me.execute);
					var b = me.buf.toString();
					me.buf = oldbuf;
					return b;
				});
				oldc = TemplateMacros.current;
				TemplateMacros.current = this;
				try {
					var r = Std.fieldCall(TemplateMacros,Std.fieldId(p),pa.toArray());
					buf.add(r);					
				} catch( e ) {
					Std.print("Failed to call macro "+p+" with "+pa._length+" arguments.<br>");
				}
				TemplateMacros.current = oldc;
				break;
			}
		}
	}

	function begin() {
		params = new Hash();
		locals = null;
		locals_stack = new List();
	}

	function toString() {
		buf = new StringBuf();
		text.iter(execute);
		var s = buf.toString();
		buf = null;
		return s;
	}

	function __string() {
		return Std.field(toString(),Std.fieldId("@s"));
	}

	function print() {
		buf = new StringBuf();
		text.iter(execute);
		Std.print(buf.toString());
		buf = null;
	}

	static function setGlobal( k : String, v : 'a ) {
		globals.set(k,Std.cast(v));
	}

	static function file(f : String) : Template {
		var data = new File(BASEDIR+f).contents();
		if( COMPRESS )
			data = blanks.replaceAll(data," ");
		return new Template(data);
	}

	static function cache(f : String) : Template {
		if( NO_CACHE )
			return file(f);
		var c = hcache.get(f);
		var d = Date.now();
		if( c == null ) {
			c = { t : file(f), time : null, f : f };
			hcache.set(f,c);
		}
		c.time = d;
		cache_hits++;
		if( cache_hits % CLEAN_FREQUENCY == 0 ) {
			var limit = d.sub(CACHE_TIMEOUT);
			hcache.iter(fun(f,ca) {
				if( limit > ca.time )
					hcache.remove(f);
			});
		}
		return c.t;
	}

}