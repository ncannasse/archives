class mtt2.Generator
{
	var out : StringBuf;

	var htmlBuf : StringBuf;

	function new()
	{
		out = new StringBuf();
		htmlBuf = null;
	}

	function toString() : String
	{
		flushHtml();
	
		var result = new StringBuf();
		result.add('
			String = $loader.String;
			iter = $loader.iter;
			buffer_new = $loader.loadprim("std@buffer_new", 0);
			buffer_add = $loader.loadprim("std@buffer_add", 2);
			buffer_string = $loader.loadprim("std@buffer_string", 1);
			string_split = $loader.loadprim("std@string_split", 2);

			replace = function( h, n, r ){
				var l = string_split(h, n);
				if (l[1] == null)
					return h;
				var res = buffer_new();
				buffer_add(res, l[0]);
				l = l[1];
				while (l != null){
					buffer_add(res, r);
					buffer_add(res, l[0]);
					l = l[1];
				}
				return buffer_string(res);
			}

			html_escape = function( data ){
				var t = $typeof(data);
				if (t == $tint)
					return data;
				if (t != $tstring)
					data = $string(data);
				if (data == "")
					return data;
				data = replace(data, "&", "&amp;");
				data = replace(data, "<", "&lt;");
				data = replace(data, ">", "&gt;");
				data = replace(data, "\\\"", "&quot;");
				return data;
			}

			is_true = function( data ){
				if (data == "") return false;
				return $istrue(data);
			}

			new_repeat = function( data ){
				var result = $new(null);
				result.data = data;
				result.index = 0-1;
				result.number = 0;
				result.first = true;
				result.last = false;
				result.odd = true;
				result.even = false;
				if (data.get_length != null) result.size = data.get_length();
				else if (data.size != null) result.size = data.size();
				else result.size = null;
				result.next = function(v){
					this.current = v;
					this.index = this.index + 1;
					this.first = this.index == 0;
					this.number = this.number + 1;
					this.last = (this.number == this.size);
					this.even = (this.number % 2) == 0;
					this.odd = (this.even == false);
				}
				return result;
			}

			new_output_buffer = function( parent ){
				var result = $new(null);
				result.parent = parent;
				result.buf = buffer_new();
				result.add = function(str){ return buffer_add(this.buf, str); }
				result.str = function(){ return buffer_string(this.buf); }
				return result;
			}

			new_context = function( parent, vars ){
				var result = $new(null);
				result.parent = parent;
				result.__isTemplateContext = true;
				if (vars == null){
					result.vars = $new(null);
				}
				else {
					result.vars = vars;
				}
				result.get = function( field ){
					if ($objfield(this.vars, field)) return $objget(this.vars, field);
					if (this.parent == null) return null;
					return this.parent.get(field);
				}
				result.set = function( field, v ){
					$objset(this.vars, field, v);
				}
				return result;
			}

			template = function( macro, params ){
				var __ctx = null;
				if (params.__isTemplateContext) {
					__ctx = new_context(params, null);
				}
				else {
					__ctx = new_context(null, params);
				}
				var __glb = __ctx;
				var __out = new_output_buffer(null);
				
//--- HERE COMES THE TEMPLATE CODE ---
');
		result.add(out.toString());
		result.add('//--- END OF TEMPLATE CODE ---
				return __out.str();
			}

			$exports.template = template;
		');
		return result.toString();
	}

	function writeHtml( str:String ) : void
	{
		if (htmlBuf == null){
			htmlBuf = new StringBuf();
		}
		htmlBuf.add(str);
	}

	function writeCode( str:String ) : void
	{
		if (htmlBuf != null){
			flushHtml();
		}
		out.add("__out.add("+str+");\n");
	}

	function writeEscapedCode( str:String ) : void
	{
		if (htmlBuf != null){
			flushHtml();
		}
		out.add("__out.add(html_escape("+str+"));\n");
	}

	function add( code:String ) : void
	{
		if (htmlBuf != null){
			flushHtml();
		}
		out.add(code);
	}

	static function hash( name:String ) : int 
	{
		return Std.neko("$hash(name.@s)");
	}

	function getVar( name:String ) : String
	{
		return "__ctx.get("+hash(name)+")";
	}

	function setVar( name:String, exp:String ) : void
	{
		add("__ctx.set("+hash(name)+", "+exp+");\n");
	}

	function flushHtml()
	{
		if (htmlBuf == null) return;
		var html = htmlBuf.toString();
		html = String.replace(html, "\\", "\\\\");
		html = String.replace(html, "\\'", "\\'");
		html = String.replace(html, "\"", "\\\"");
		html = String.replace(html, "\n", "\\n");
		out.add("__out.add(\"" + html + "\");\n");
		htmlBuf = null;
	}
}
