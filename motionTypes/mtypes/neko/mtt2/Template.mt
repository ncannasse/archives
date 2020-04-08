class mtt2.Template 
{
	/** Enable production mode (no file check). */
	static var PRODUCTION = false;
	/** Destination of .neko and .n files */
	static var TMP_DIR = "/tmp/";
	/** Templates repository */
	static var REPOSITORY = null;

	static function nekoId( path:String ) : String
	{
		var rpath = path;
		var temp = path;
		if (temp.charAt(0) == "/") temp = temp.sub(1, temp._length-1);
		temp = String.replace(temp, "/", "__");
		temp = String.replace(temp, "\\", "__");
		temp = String.replace(temp, ":", "__");
		temp = String.replace(temp, "____", "__");
		return temp;
	}

	static function nekoBin( path:String ) : String
	{
		return TMP_DIR + nekoId(path) + ".n";
	}

	static function nekoSrc( path:String ) : String
	{
		return TMP_DIR + nekoId(path) + ".neko";
	}

	static function fromFile( path:String ) : {} -> String
	{
		if (PRODUCTION){
			return loadTemplate(nekoBin(path));
		}

		var f = new File(nekoBin(path));
		if (f.exists()){
			var macroStamp  = mtt2.Preprocessor.macroFileStamp;
			var sourceStamp = mtime(REPOSITORY+"/"+path);
			var stamp       = mtime(nekoBin(path));
			if ( Int32.compare(stamp,sourceStamp) >= 0 && (macroStamp == null || Int32.compare(macroStamp,stamp) < 0) ){
				return loadTemplate(nekoBin(path));
			}
		}
		var content = new File(REPOSITORY+"/"+path).contents();
		return fromString(content, nekoId(path));
	}
	
	static function fromString( src:String, id:String ) : {} -> String
	{
		if (id == null){
			id = md5(src);
		}
		src = mtt2.Preprocessor.process(src);
		
		var path = nekoSrc(id);
		var x = null;
		try {
			x = new Xml(src);
		}
		catch (e){
			var error = {
				message:"Error in "+id+"\n"+Std.neko("String.new($string(e))"),
				source:src
			};
			Std.throw(error);
		}
		
		var p = new mtt2.Parser();
		var s = p.parse(x);

		s = "// generated from " + id + "\n//" + src.split("\n").join("//") + "\n" + s;

		var f = new File(path);
		f.create();
		f.write(s);
		f.close();

		var r = Sys.command("nekoc -o "+TMP_DIR+" "+path+" 2> "+TMP_DIR+"/nekoc.out");
		if (r != 0){
			var fout = new File(TMP_DIR+"/nekoc.out");
			if (fout.exists()){
				Std.throw("nekoc compilation of "+path+" failed ("+r+") : "+fout.contents());
			}
			else {
				Std.throw("nekoc compilation of "+path+" failed ("+r+") -- no nekoc.out available");
			}
		}

		return loadTemplate(nekoBin(id));
	}

	static function loadTemplate( nPath:String ) : {} -> String
	{
		return Std.neko('
			var oldCache = $loader.cache;
			$loader.cache = $new(oldCache);
			$loader.String = String;
			$loader.iter = function(loop, fnc){ loop.iter(fnc); }
			var code = $loader.loadmodule(nPath.@s, $loader);
			$loader.cache = oldCache;
			var wrap = function(context){
				var wrapCache = $loader.cache;
				$loader.cache = $new(wrapCache);
				var macro = function(path){
					if (mtt2_Template.PRODUCTION == false){
						mtt2_Template.fromFile(String.new(path));
					}
					return $loader.loadmodule(mtt2_Template.nekoBin(String.new(path)).@s, $loader);
				}
				var result = String.new(code.template(macro, context));
				$loader.cache = wrapCache;
				return result;
			}
			wrap;
		');
	}

	static function md5( s:String ) : String
	{
		return Std.neko('
			var val = make_md5(s.@s);
			String.new(base_encode(val, "0123456789abcdef"));
		');
	}

	static var __init = Std.neko('
		make_md5 = $loader.loadprim("std@make_md5", 1);
	');

	static function mtime(file:String) : Int32
	{
		return Std.neko('
			sys_stat = $loader.loadprim("std@sys_stat",1);
			var stat = sys_stat(file.@s);
			stat.mtime;
		');
	}

	/*
	var func : {} -> String;
	function new( src:String, file:String ) { func = fromString(src, file); }
	function toString(ctx:{}) : String { return func(ctx); }
	*/
}
