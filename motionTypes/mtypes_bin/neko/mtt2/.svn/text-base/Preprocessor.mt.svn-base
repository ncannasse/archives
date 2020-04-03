class mtt2.Preprocessor
{
	static var r_if = new Regexp("::if (.*?)::");
	static var r_elseif = new Regexp("::elseif (.*?)::");
	static var r_else = new Regexp("::else *?::");
	static var r_foreach = new Regexp("::foreach (.*?)::");
	static var r_fill = new Regexp("::fill (.*?)::");
	static var r_use = new Regexp("::use (.*?)::");
	static var r_set = new Regexp("::set (.*?)::");

	static var r_cond = new Regexp("::cond (.*?)::");
	static var r_repeat = new Regexp("::repeat (.*?)::");
	static var r_attr = new Regexp("::attr (.*?)::");
	static var r_cdata = new Regexp("<!\\[CDATA\\[([^\\0]*?)]]>");
	static var r_comment = new Regexp("<!--([^\\0]*?)-->");
	static var r_macroCall = new Regexp("\\$\\$([a-zA-Z0-9_]+)\\(");

	static var macros : Hash<mtt2.Macro> = new Hash();
	static var macroFileStamp : Int32;

	static function process( str:String ) : String
	{
		var res = expandMacros(str);
		res = escapeCdata1(res);
		res = res.split("::else::").join("</mt><mt mt:else=\"\">");
		res = res.split("::end::").join("</mt>");
		while (r_if.match(res)){
			res = res.split(r_if.matched(0)).join("<mt mt:if=\""+quote(r_if.matched(1))+"\">");
		}
		while (r_elseif.match(res)){
			res = res.split(r_elseif.matched(0)).join("</mt><mt mt:elseif=\""+quote(r_elseif.matched(1))+"\">");
		}
		while (r_foreach.match(res)){
			res = res.split(r_foreach.matched(0)).join("<mt mt:foreach=\""+quote(r_foreach.matched(1))+"\">");
		}
		while (r_fill.match(res)){
			res = res.split(r_fill.matched(0)).join("<mt mt:fill=\""+quote(r_fill.matched(1))+"\">");
		}
		while (r_set.match(res)){
			res = res.split(r_set.matched(0)).join("<mt mt:set=\""+quote(r_set.matched(1))+"\"/>");
		}
		while (r_use.match(res)){
			res = res.split(r_use.matched(0)).join("<mt mt:use=\""+quote(r_use.matched(1))+"\">");
		}
		res = escapeComments(res);

		while (r_cond.match(res)){
			res = res.split(r_cond.matched(0)).join("mt:if=\""+quote(r_cond.matched(1))+"\"");
		}
		while (r_repeat.match(res)){
			res = res.split(r_repeat.matched(0)).join("mt:foreach=\""+quote(r_repeat.matched(1))+"\"");
		}
		while (r_attr.match(res)){
			res = res.split(r_attr.matched(0)).join("mt:attr=\""+quote(r_attr.matched(1))+"\"");
		}
		res = unescapeCdata1(res);
		return "<mt>" + res + "</mt>";
	}

	static function expandMacros( str:String ) : String
	{
		var res = str;
		while (r_macroCall.match(res)){
			var macroName = r_macroCall.matched(1);
			var macroCallPos = r_macroCall.matchedPos(1);
			var i = macroCallPos.pos + macroCallPos.len + 1;
			var args = new Array();
			var nargs = 0;
			var startArg = i;
			var endArg = -1;
			var oAccos = 0;
			var oParas = 0;
			var end = 0;
			var forceArg = false;
			while (i < res._length){
				var c = res.charAt(i);
				if (c == "("){
					if (oAccos == 0){
						oParas++;
						++i;
						continue;
					}
				}
				if (c == ")" && oParas > 0){
					oParas--;
					++i;
					continue;
				}
				if (c == "{"){
					if (oAccos == 0){ 
						startArg = i+1;
						forceArg = true; 
					}
					oAccos++;
				}
				if (c == "}"){
					if (oAccos > 0){ 
						oAccos--;
						if (oAccos == 0){ endArg = i; }
					}
				}
				if (oAccos == 0 && oParas == 0 && (c == "," || c == ")")){
					if (endArg == -1){ endArg = i; }
					var p = res.sub(startArg, endArg - startArg);
					p = String.trim(p);
					if (p._length > 0 || forceArg){
						args[ nargs ] = p;
						nargs++;
					}
					startArg = i+1;
					endArg = -1;
					forceArg = false;
				}
				if (oAccos == 0 && (c == ")")){
					end = i+1;
					break;
				}
				++i;
			}
			var mcr = macros.get(macroName);
			if (mcr == null){
				Std.throw("Unknown macro "+macroName);
			}
			var src = res.sub(macroCallPos.pos - 2, end - macroCallPos.pos + 2);
			res = res.split(src).join(mcr.expand(args));
		}
		return res;
	}

	static function registerMacroFile( path:String ) : void
	{
		var f = new File(path);
		if (!f.exists()){
			Std.throw("Macro file "+path+" does not exists");
		}
		macroFileStamp = mtt2.Template.mtime(path);
		registerMacros(f.contents());
	}

	static function registerMacros( str:String ) : void
	{
		var src = str;
		var rFind = new Regexp("<macro\\s+name=['\"](.*?)['\"]\\s*?>([^\\0]*?)</macro>");
		while (rFind.match(src)){
			var pos = rFind.matchedPos(0);
			var name = rFind.matched(1);
			var content = rFind.matched(2);
			var macro = new mtt2.Macro(name, content);
			mtt2.Preprocessor.macros.set( macro.name, macro );
			var end = pos.pos + pos.len;
			src = src.sub(end, src._length - end);
		}
	}

	static function quote( str:String ) : String
	{
		return str.split("&").join("&amp;").split("\"").join("&quot;");
	}

	static function escapeCdata1( str:String ) : String
	{
		var res = str;
		while (r_cdata.match(res)){
			var pos   = r_cdata.matchedPos(0);
			var cdata = r_cdata.matched(1);
			cdata = cdata.split("&").join("&amp;").split("<").join("&lt;").split(">").join("&gt;");
			res = res.sub(0, pos.pos) + "<![CDATAX[" + cdata + "]]>" + res.sub(pos.pos+pos.len, res._length-(pos.pos+pos.len));
		}
		return res;
	}

	static function escapeComments( str:String ) : String
	{
		var res = str;
		while (r_comment.match(res)){
			var pos   = r_comment.matchedPos(0);
			var comment = r_comment.matched(1);
			comment = comment.split("&").join("&amp;").split("<").join("&lt;").split(">").join("&gt;");
			res = res.sub(0, pos.pos) + "<![COMMENT[" + comment + "]]C>" + res.sub(pos.pos+pos.len, res._length-(pos.pos+pos.len));
		}
		return res;
	}

	static function unescapeCdata1( str:String ) : String
	{
		var res = str.split("<![CDATAX[").join("<![CDATA[");
		//res = res.split("<![COMMENT[").join("<!--").split("]]C>").join("-->");
		res = escapeCdata1(res);
		res = res.split("<![CDATAX[").join("X![CDATA[").split("]]>").join("]]X");
		res = res.split("<![COMMENT[").join("X!--").split("]]C>").join("]]C");
		return res;
	}
}
