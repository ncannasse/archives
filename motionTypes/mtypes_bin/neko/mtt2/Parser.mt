class mtt2.Parser
{
	static var REGEX_EXP = new Regexp("^([a-zA-Z][A-Za-z0-9_]{0,})[ \n\r\t]+(.*?)$");
	static var REGEX_DXP = new Regexp("::([^:][^\\0]*?)::");

	static var MT = "mt";
	static var MT_IF = Std.fieldId("mt:if");
	static var MT_ELSEIF = Std.fieldId("mt:elseif");
	static var MT_ELSE = Std.fieldId("mt:else");
	static var MT_CONTENT = Std.fieldId("mt:content");
	static var MT_REPLACE = Std.fieldId("mt:replace");
	static var MT_FOREACH = Std.fieldId("mt:foreach");
	static var MT_ATTRIBUTES = Std.fieldId("mt:attr");
	static var MT_SET = Std.fieldId("mt:set");
	static var MT_FILL = Std.fieldId("mt:fill");
	static var MT_OMIT_TAG = Std.fieldId("mt:omit-tag");
	static var MT_MACRO = Std.fieldId("mt:macro");
	static var MT_USE = Std.fieldId("mt:use");
	static var XHTML_EMPTY = ["area","base","basefont","br","col","frame","hr","img","input","isindex","link","meta","param"];
	static var XHTML_ATTRS = ["compact","nowrap","ismap","declare","noshade","checked","disabled","readonly","multiple","selected","noresize","defer"];
	static var R_CDATA = new Regexp("X!\\[CDATA\\[([^\\0]*?)\\]\\]X");
	static var R_COMMENT = new Regexp("X!--([^\\0]*?)\\]\\]C");

	var out : mtt2.Generator;

	function new()
	{
		out = new mtt2.Generator();
	}

	function parse( xml:Xml ) : String
	{
		parseNode(xml);
		return out.toString();
	}

	function parseNode( xml:Xml )
	{
		if (xml.node == null && xml.children != null && xml.children._length > 0){
			var self = this;
			xml.children.iter(fun(child){
				self.parseNode(child);
			});
			return;
		}
		if (xml.text != null && R_CDATA.match(xml.text)){
			parseCDATA(xml);
			return;
		}
		if (xml.text != null && R_COMMENT.match(xml.text)){
			parseComment(xml);
			return;
		}
		if (xml.cdata || xml.node == null){
			echoString(xml.toString());
			return;
		}

		var mtSet = extractAttribute(xml, MT_SET);
		if (mtSet != null){
			var parts = mtSet.split("=").toArray();
			var dest = String.trim(parts[0]);
			var exp = parseExpression( String.trim(parts[1]) );
			out.setVar(dest, "("+exp+")");
			return;
		}

		var mtUse = extractAttribute(xml, MT_USE);
		if (mtUse != null){
			var f = mtt2.Template.fromFile(mtUse); // ensure template is parsed (beware of cycle)
			out.add("tmp = "+out.getVar("__content__"));
			out.add("__out = new_output_buffer(__out);\n");
			parseNode(xml);
			out.setVar("__content__", "__out.str()");
			out.add("__out = __out.parent;\n");
			out.add("mcr = macro(\""+mtUse+"\");\n");
			out.add("__out.add(mcr.template(macro, __ctx));\n");
			out.setVar("__content__", "tmp");
			return;
		}
		
		var mtFill = extractAttribute(xml, MT_FILL);
		if (mtFill != null){
			out.add("__out = new_output_buffer(__out);\n");
			parseNode(xml);
			out.setVar(String.trim(mtFill), "String.new(__out.str())");
			out.add("__out = __out.parent;\n");
			return;
		}
		
		var mtIf = extractAttribute(xml, MT_IF);
		if (mtIf != null){
			out.add("if (is_true("+parseExpression(mtIf)+")){\n");
			parseNode(xml);
			out.add("}\n");
			return;
		}
		
		var mtElseIf = extractAttribute(xml, MT_ELSEIF);
		if (mtElseIf != null){
			out.add("else if (is_true("+parseExpression(mtElseIf)+")){\n");
			parseNode(xml);
			out.add("}\n");
			return;
		}
		
		var mtElse = extractAttribute(xml, MT_ELSE);
		if (mtElse != null){
			out.add("else {\n");
			parseNode(xml);
			out.add("}\n");
			return;
		}

		var mtForeach = extractAttribute(xml, MT_FOREACH);
		if (mtForeach != null){
			var o = extractExpressionTarget(mtForeach);		
			out.add("var loop = "+parseExpression(o.exp)+";\n");
			out.add("__ctx.vars.repeat_"+o.target+" = new_repeat(loop);\n");
			out.add("iter(loop, function(__item){\n");
			out.add("__ctx.vars.repeat_"+o.target+".next(__item);\n");
			out.setVar(o.target, "__item");
			parseNode(xml);
			out.add("});\n");
			return;
		}

		var mtReplace = extractAttribute(xml, MT_REPLACE);
		if (mtReplace != null){
			echoExpression(mtReplace);
			return;
		}

		var mtOmitTag = extractAttribute(xml, MT_OMIT_TAG);
		if (mtOmitTag == null && xml.node == MT){
			mtOmitTag = "true";
		}
		
		var mtAttributes = extractAttribute(xml, MT_ATTRIBUTES);
		var mtContent = extractAttribute(xml, MT_CONTENT);

		var hasContent = (mtContent != null || xml.children._length > 0);

		var xhtmlEmpty = isXHTMLEmptyTag(xml.node);
		if (hasContent && xhtmlEmpty){
			hasContent = false;
		}
		if (!hasContent && !xhtmlEmpty){
			hasContent = true;
		}
		
		if (mtOmitTag == null){
			out.writeHtml("<"+xml.node);
			if (mtAttributes == null){
				attributesToString(xml.att);
			}
			else {
				doMtAttributes(mtAttributes, xml.att);
			}
			if (hasContent){ 
				out.writeHtml(">");
			}
			else {
				out.writeHtml("/>");
				return;
			}
		}
				
		
		if (mtContent != null){
			echoExpression(mtContent);
		}
		else {
			var self = this;
			xml.children.iter(fun(child){
				self.parseNode(child);
			});
		}
		
		if (mtOmitTag == null && hasContent){
			out.writeHtml("</" + xml.node + ">");
		}
	}

	function doMtAttributes( att:String, attributes:{} ) : void
	{
		var overwritten = new Hash();
		var parts = splitExpression(att).toArray();
		for (var i=0; i<parts._length; ++i){
			var x = String.trim(parts[i]);
			var o = extractExpressionTarget(x);
			var exp = parseExpression(o.exp);
			if (isBooleanAttribute(o.target)){
				out.add("if ("+exp+"){\n");
				out.add("__out.add(\" "+o.target+"=\\\""+o.target+"\\\"\");\n");
				out.add("}");
			}
			else {
				out.add("var value = "+exp+";\n");
				out.add("if (value != false && value != null){\n");
				out.add("__out.add(\" "+o.target+"=\\\"\");\n");
				out.add("__out.add(value);\n");
				out.add("__out.add(\"\\\"\");\n");
				out.add("}");
			}
			overwritten.set(o.target, true);
		}

		var self = this;
		Std.fields(attributes).iter(fun(field){
			var attName = Std.fieldName(field);
			if (!overwritten.exists(attName)){
				var attVal = Std.field(attributes, field);
				if (attVal != null){ // mt attributes
					self.out.add("__out.add(\" "+attName+"=\\\"\");\n");
					self.echoString(attVal);
					self.out.add("__out.add(\"\\\"\");\n");
				}
			}
		});
	}

	function extractAttribute( xml:Xml, id:int ) : String
	{
		var res = Std.field(xml.att, id);
		if (res == null){
			return null;
		}
		Std.setField(xml.att, id, null);
		return String.trim(res.split("&quot;").join("\"").split("&amp;").join("&"));
	}

	function splitExpression( exp:String ) : List<String>
	{
		var result = new List();
		var start = 0;
		var len = exp._length;
		for (var i=0; i<len; ++i){
			if (exp.charAt(i) == ";"){
				if (exp.charAt(i+1) == ";"){
					++i;
				}
				else {
					result.push(exp.sub(start, i-start));
					start = i+1;
				}
			}
		}
		if (start < len){
			result.push(exp.sub(start, len-start));
		}
		return result;
	}

	function extractExpressionTarget( exp:String ) : { target:String, exp:String }
	{
		if (REGEX_EXP.match(exp)){
			return {target:REGEX_EXP.matched(1), exp:REGEX_EXP.matched(2)};
		}
		return {target:null, exp:exp};
	}

	function parseExpression( exp:String ) : String
	{
		var result = new StringBuf();
	
		var r_num = new Regexp("[0-9]+");
		var r_digit = new Regexp("[0-9.]+");
		var r_var = new Regexp("[\\$a-zA-Z0-9_]+");
		var r_dot = new Regexp("[.]");
		var r_white = new Regexp("[ \t\n\r]");
		var r_op = new Regexp("[!+-/*<>=&|%]+");
		var r_par = new Regexp("[()]");

		var states = { none:0, string:1, dstring:2, variable:3, num:4, member:5 };
		var str = String.trim(exp);
		var state = states.none;
		var mark = 0;
		var getter = false;
		var len = str._length;
		for (var i=0; i<=len; ++i){
			var skip = false;
			var c = (i == len) ? "\n" : str.charAt(i);
			var n = (i+1 >= len) ? "\n" : str.charAt(i+1);
			switch (state){
				case states.none:
					if (r_num.match(c)){
						state = states.num;
					}
					else if (c == "\""){
						result.add("String.new(");
						state = states.dstring;
					}
					else if (c == "'"){
						result.add("String.new(\"");
						state = states.string;
						skip = true;
						break;
					}
					else if (c == "."){
						state = states.member;
					}
					else if (r_op.match(c)){
						if (c == '!'){
							if (n == "="){
							}
							else {
								result.add("false == ");
								skip = true;
								break;
							}
						}
					}
					else if (c == "("){
						var end = findEndOfBracket(str, i);
						var sub = str.sub(i+1, end-i);
						result.add("(");
						result.add(parseExpression(sub));
						//result.add(")");
						i = end;
						skip = true;
						break;
					}
					else if (r_var.match(c)){
						state = states.variable;
						mark = i;
					}
					break;
					
				case states.string:
					if (c == "\\" && n == "'"){ 
						result.add("'");
						skip = true;
						++i; 
					}
					else if (c == "\""){
						result.add("\\");
					}						
					else if (c == "'"){
						state = states.none;
						result.add("\")");
						//++i;
						skip = true;
						break;
					}
					break;
					
				case states.dstring:
					if (c == "\\" && n == "'"){
						result.add("\\'");
						skip = true;
						++i;
					}
					else if (c == "\\" && n == "\""){
						++i;
					}
					else if (c == "\""){
						state = states.none;
						result.add("\")");
						//++i;
						skip = true;
						break;
					}
					break;

				case states.variable:
					if (r_var.match(c)){
					}
					else if (c == "."){
						var variable = str.sub(mark, i-mark);
						if (variable == "repeat"){
							result.add("__ctx.vars.repeat_");
							state = states.member;
							skip = true;
							break;
						}
						result.add(out.getVar(variable));
						state = states.member;
						if (i < len && str.charAt(i+1) == "_"){
							result.add(".get");
							getter = true;
							skip = true;
							break;
						}
					}
					else {
						var variable = str.sub(mark, i-mark);
						if (isExpressionKeyword(variable)){
							result.add(variable);
						}
						else {									
							result.add(out.getVar(variable));
						}
						state = states.none;
					}
					break;

				case states.num:
					if (r_digit.match(c)){
					}
					else {
						state = states.none;
					}
					break;
					
				case states.member:
					if (r_var.match(c)){
					}
					else if (c == "("){
						if (getter){
							result.add("()");
							getter = false;
						}
						var end = findEndOfBracket(str, i);
						var sub = str.sub(i+1, end-i);
						var argStr = splitArguments(sub).toArray();
						for (var j=0; j<argStr._length; ++j){
							argStr[j] = parseExpression(argStr[j]);
						}
						result.add("(");
						result.add(argStr.join(","));
						result.add(")");
						i = end+1; skip = true;
						/*
						if (i < len && str.charAt(i) == "."){
							state = states.member;
						}
						else {
							state = states.none; // test a.foo().bar
						}
						*/
					}
					else if (c == "."){
						if (getter){
							result.add("()");
							getter = false;
						}
						if (i < len && str.charAt(i+1) == "_"){
							getter = true;
							result.add(".get");
							skip = true;
							break;
						}
					}
					else {
						if (getter){
							result.add("()");
							getter = false;
						}
						state = states.none;
						if (i != len){
							i--;
						}
						skip = true;
						break;
					}
					break;
			}
			if (!skip && i < len && state != states.variable){
				result.add(str.charAt(i));
			}
		}
		return result.toString();
	}

	static function findEndOfBracket( str:String, pos:int ) : int 
	{
		var len = str._length;
		var ctopen = 0;
		for (var i = pos+1; i < len; ++i){
			var c = str.charAt(i);
			if (c == "("){ 
				ctopen++; 
			}
			else if (c == ")"){
				if (ctopen == 0){
					return i-1;
				}
				else {
					ctopen--;
				}
			}
		}
		return -1;
	}

	function attributesToString( att:{} ) : void
	{
		if (att == null){ return; }
		var self = this;
		Std.fields(att).iter(fun(fid){
			var value = string( Std.field(att, fid) );
			if (value == null){
				return;
			}
			self.out.writeHtml(" "+Std.fieldName(fid));
			self.out.writeHtml("=\"");
			self.echoString( String.replace(value, "\"", "&quot;") );
			// self.out.writeHtml( String.replace(value, "\"", "&quot;") );
			self.out.writeHtml("\"");
		});
	}

	static function isBooleanAttribute( attName:String ) : bool
	{
		try {
			XHTML_ATTRS.iter(fun(f){ if (f == attName) { Std.throw("found"); } });
		}
		catch (found) {
			return true;
		}
		return false;
	}

	static function isXHTMLEmptyTag( tag:String ) : bool
	{
		try {
			XHTML_EMPTY.iter(fun(f){ if (f == tag) { Std.throw("found"); } });
		}
		catch (found) {
			return true;
		}
		return false;
	}

	static function splitArguments( str:String ) : List<String>
	{
		var res = new List();
		var arg = "";
		var len = str._length;
		var cto = 0;
		for (var i = 0; i < len; ++i){
			var c = str.charAt(i);
			if (c == "("){ 
				cto++; 
			}
			else if (c == ")"){ 
				cto--; 
			}
			if (c == "," && cto == 0) {
				res.add(String.trim(arg));
				arg = "";
			}
			else {
				arg += c;
			}
		}
		if (arg != ""){
			res.add(String.trim(arg));
		}
		return res;
	}

	static function isExpressionKeyword( varName:String ) : bool
	{
		return varName == "true" || varName == "false" || varName == "null";
	}

	function echoExpression( exp:String ) : void
	{
		exp = String.trim(exp);
		if (String.find(exp, "raw ", 0) == 0){
			out.writeCode(parseExpression(exp.sub(4, exp._length-4)));
		}
		else {
			out.writeEscapedCode(parseExpression(exp));
		}
	}

	function echoString( str:String ) : void
	{
		var source = str;
		while (REGEX_DXP.match(source)){
			var pos = REGEX_DXP.matchedPos(0);
			if (pos.pos > 0){
				out.writeHtml(source.sub(0,pos.pos));
			}
			var sub = REGEX_DXP.matchedPos(1);
			echoExpression(source.sub(sub.pos, sub.len));
			source = source.sub(pos.pos + pos.len, source._length - pos.pos - pos.len);
		}
		if (source._length > 0){
			out.writeHtml(source);
		}
	}

	function parseCDATA( xml:Xml )
	{
		var src = xml.text;
		while (R_CDATA.match(src)){
			var pos = R_CDATA.matchedPos(0);
			if (pos.pos != 0){
				echoString(src.sub(0, pos.pos));
			}
			out.writeHtml("<![CDATA[\n");
			var cdataSrc = R_CDATA.matched(1);
			cdataSrc = mtt2.Utils.htmldecode(cdataSrc);
			cdataSrc = "<mt>" + cdataSrc + "</mt>";
			
			var cdataxml = null;
			try {
				cdataxml = new Xml(cdataSrc);
				if (cdataxml == null){
					Std.throw("Unable to parse CDATA content");
				}
			}
			catch (e){
				var error = {
					error : e,
					xmlsource : cdataSrc,
					cdatasource : R_CDATA.matched(1)
				};
				Std.throw(error);					
			}
			restoreCDATAHtmlEncoding(cdataxml);
			parseNode(cdataxml);
			out.writeHtml("]]>");
			src = src.sub(pos.pos + pos.len, src._length - (pos.pos + pos.len));
		}
		if (src._length > 0){
			echoString(src);
		}
	}

	static function restoreCDATAHtmlEncoding( xml:Xml ) : void
	{
		if (xml.text != null && xml.text != ""){
			xml.text = mtt2.Utils.htmldecode(xml.text);
			return;
		}
		xml.children.iter( fun(x){ restoreCDATAHtmlEncoding(x); } );
	}
	
	function parseComment( xml:Xml )
	{
		var src = xml.text;
		while (R_COMMENT.match(src)){
			var pos = R_COMMENT.matchedPos(0);
			if (pos.pos != 0){
				echoString(src.sub(0, pos.pos));
			}
			out.writeHtml("<!--");
			out.writeHtml(mtt2.Utils.htmldecode(R_COMMENT.matched(1)));
			out.writeHtml("-->");
			src = src.sub(pos.pos + pos.len, src._length - (pos.pos + pos.len));
		}
		if (src._length > 0){
			echoString(src);
		}
	}
}
