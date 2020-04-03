class Std {

	static function neko( s : String ) : 'a {
		return Std.neko(s);
	}

	static function print( x : 'a ) : void {
		Std.neko("$print(x)");
	}

	static var rnd__ : Random;

	static function random( x : int ) : int {
		if( rnd__ == null )
			rnd__ = new Random();
		return rnd__.int(x);
	}

	static function rnd() : float {
		if( rnd__ == null )
			rnd__ = new Random();
		return rnd__.float();
	}

	static function parseFloat( s : String ) : float {
		return Std.neko("$float(s.@s)");
	}

	static function serialize( x : 'a ) : String {
		return Std.neko("String.new(serialize(x))");
	}

	static function unserialize( x : String ) : 'a {
		return Std.neko("unserialize(x.@s,$loader)");
	}

	static function cast( x : 'a ) : 'b {
		return Std.neko("x");
	}

	static function throw( e : 'a ) : void {
		Std.neko("$throw(e)");
	}

	static function rethrow( e : 'a ) : void {
		Std.neko("$rethrow(e)");
	}

	static function fieldId( x : String ) : int {
		return Std.neko("$hash(x.@s)");
	}

	static function fieldName( x : int ) : String {
		return Std.neko("String.new($field(x))");
	}

	static function fieldCall( o : {}, id : int, params : Array<'a> ) : 'b {
		return Std.neko("$objcall(o,id,params.@a)");
	}

	static function field( o : {}, f : int ) : 'a {
		return Std.neko("$objget(o,f)");
	}

	static function fields( o : {} ) : List<int> {
		return Std.neko("
			var a = $objfields(o);
			var l = $asize(a);
			var i = 0;
			var li = List.new();
			while( i < l ) {
				li.add(a[i]);
				i = i + 1;
			}
			li;
		");
	}

	static function prototype( o : {} ) : {} {
		return Std.neko("$objgetproto(o)");
	}

	static function setField( o : {}, f : int, v : 'a ) : void {
		Std.neko("$objset(o,f,v)");
	}

	static function haveField( o : {} , f : int ) : bool {
		return Std.neko("$objfield(o,f)");
	}

	static function typeof( o : 'a ) : int {
		return Std.neko("$typeof(o)");
	}

	static function is( o : {}, cclass : {} ) : bool {
		return Std.neko("$typeof(o) == $tobject && $pcompare($objgetproto(o),cclass.prototype) == 0");
	}

	static var types : {
		tnull : int,
		tint : int,
		tfloat : int,
		tstring : int,
		tbool : int,
		tobject : int,
		tarray : int,
		tfunction : int,
		tabstract : int
	} = Std.neko("
		{
			tnull => $tnull,
			tint => $tint,
			tfloat => $tfloat,
			tbool => $tbool,
			tstring => $tstring,
			tobject => $tobject,
			tarray => $tarray,
			tfunction => $tfunction,
			tabstract => $tabstract
		}
	");

	static var __init = Std.neko('
		serialize = $loader.loadprim("std@serialize",1);
		unserialize = $loader.loadprim("std@unserialize",2);
		__int = function(s) {
			if( $typeof(s) == $tobject )
				s = s.__string();
			return $int(s);
		}
	');
}
