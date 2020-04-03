class mtt2.Macro 
{
	static var R_PARAMS = new Regexp("^([a-zA-Z0-9_]+)\\((.*?)\\)$");
	static var R_NOTVAR = new Regexp("[^a-zA-Z0-9_.]");
	static var R_NUM = new Regexp("^[0-9.]+$");
	static var R_VAR = new Regexp("^::(.*?)::$");

	var name : String;
	var params : Array<String>;
	var source : String;

	function new( n:String, c:String )
	{
		params = new Array();
		name = n;
		if (R_PARAMS.match(name)){
			name = R_PARAMS.matched(1);	
			var self = this;
			var list = R_PARAMS.matched(2).split(",").toArray();
			for (var i = 0; i < list._length; ++i){
				params[i] = String.trim(list[i]);
			}
		}
		source = c;
	}

	function expand( p:Array<String> ) : String
	{
		if (params._length != p._length){
			Std.throw("macro "+name+" takes "+params._length+" arguments"+params.toString()+"\ngot: "+p.toString());
		}
		var res = source;
		for (var i = 0; i < params._length; ++i){
			var replace = new Regexp("::\\s*?"+params[i]+"\\s*?::");
			res = replace.replaceAll(res, p[i]);
			var isNum = R_NUM.match(p[i]);
			var isVar = (R_VAR.match(p[i]) && String.find(R_VAR.matched(1), "::", 0) == null);
			var regexp = new Regexp("^(::.*[^a-zA-Z0-9_.])("+ params[i] +")([^a-zA-Z0-9_.].*::)$");
			var pos = 0;
			while ((pos = String.find(res, "::", pos)) != null){
				var end = String.find(res, "::", pos+2);
				if (end == null){
					Std.print(res);
					Std.throw("Unable to find matching ::");
				}
				var exp = res.sub(pos+2, end-pos-2);
				var rep = replaceArgumentInExpression(exp, params[i], p[i], isVar, isNum);
				res = res.sub(0, pos+2) + rep + res.sub(end, res._length - end);
				pos = end - exp._length + rep._length + 2;
			}
		}
		return res;
	}

	static function replaceArgumentInExpression( exp:String, paramName:String, value:String, isVar:bool, isNum:bool ) : String
	{	
		var repl = null;
		if (isNum) repl = value;
		else if (isVar) repl = "(" + value.sub(2, value._length-4) + ")";
		else repl = stringArgumentToExpressionCompound(value);
		
		var res = exp;
		var pos = String.find(res, paramName, 0);
		while (pos != null){
			var end = pos + paramName._length;
			var before = (pos > 0) ? res.charAt(pos-1) : " ";
			var after = (end < res._length) ? res.charAt(end) : " ";
			if (R_NOTVAR.match(before) && (after == "." || R_NOTVAR.match(after))){
				res = res.sub(0, pos) + repl + res.sub(end, res._length-end);
				end = end - paramName._length + repl._length;
			}
			pos = String.find(res, paramName, end+1);
		}
		return res;
	}

	static function xmlToString( xml:Xml ) : String
	{
		if (xml.node == null){
			return xml.text;
		}
		var res = new StringBuf();
		res.add("<");
		res.add(xml.node);
		var fields = Std.fields(xml.att).toArray();
		for (var i = 0; i < fields._length; ++i){
			res.add(" ");
			res.add(Std.fieldName(fields[i]));
			res.add("=\"");
			res.add(mtt2.Utils.htmlencode(Std.field(xml.att, fields[i])));
			res.add("\"");
		}
		if (xml.children._length > 0){
			res.add(">");
			xml.children.iter(fun(x){ res.add(xmlToString(x)); });
			res.add("</");
			res.add(xml.node);
			res.add(">");
		}
		else {
			res.add("/>");
		}
		return res.toString();
	}

	static function stringArgumentToExpressionCompound( str:String ) : String
	{
		var res = String.replace(str,'\'',"\\\'");
		var pos = 0;
		while ( (pos = String.find(res, "::", pos)) != null ){
			var end = String.find(res, "::", pos+2);
			if (end == null){
				Std.throw("Malformed expression '"+str+"'");
			}
			var left = res.sub(0, pos);
			var data = res.sub(pos+2, end-pos-2);
			var right = res.sub(end+2, res._length-end-2);
			res = left + "\'+" + data + "+\'" + right;
			pos = end + 2;
		}
		return "(\'" + res + "\')";
	}
}
