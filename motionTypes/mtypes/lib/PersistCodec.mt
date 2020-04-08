
class PersistCodec {

	/*
		TYPE-BITS =

		- number
			int 	00
			float	010
			NaN		0110
			+Inf	01110
			-Inf	01111

		- array		100
		- object	101
		- boolean
			true	1101
			false	1100

		- string	1110

		- undefined 1111

		DATA =

		- int
			normal	NN [XX|XX|XX]  : 2 bits + 2-6 bits
			medium	1100 + 16 bits
			big		1101 + 32 bits
			negatif 111 + int

		- float
			as a string with :
			string length	5
			character		4
				number 0-9
				dot     10
				plus    11
				minus   12
				exp     13

		- array
			element	0
			jump	10 + int
			end		11

		- object
			key
				0 + index
				10 + string
			end
				11

		- string
			extended	11
			ASCII		10
			B64			0
			len			int
			chars		 7/8


	*/

	var obfu_mode : bool;


	var bc : BitCodec;
	var fields : Hash<int>;
	var fieldtbl : Array<String>;
	var nfields : int;
	var next_field_bits : int;
	var nfields_bits : int;
	var cache : Array<void>;
	var result : void;
	var fast : bool;
	var crc : bool;

	function new() {
		obfu_mode = false;
		crc = false;
	}

	function encode_array(a) {
		var i;
		var njumps = 0;
		for(i=0;i<a.length;i++) {
			if( a[i] == null )
				njumps++;
			else {
				if( njumps > 0 ) {
					bc.write(2,2)
					encode_int(njumps);
					njumps = 0;
				}
				bc.write(1,0);
				do_encode(a[i]);
			}
		}
		bc.write(2,3);
	}

	function decode_array() {
		var a = new Array();
		downcast(a).pos = 0;
		cache.unshift(Std.cast(a));
		return Std.cast(a);
	}

	function decode_array_item(a) {
		var elt = (bc.read(1) == 0);
		if( elt )
			a[a.pos++] = do_decode();
		else {
			var exit = (bc.read(1) == 1);
			if( exit )
				return false;
			downcast(a).pos += decode_int();
		}
		return true;
	}

	function decode_array_fast() {
		var a = new Array();
		var pos = 0;
		while( true ) {
			var elt = (bc.read(1) == 0);
			if( elt )
				a[pos++] = do_decode();
			else {
				var exit = (bc.read(1) == 1);
				if( exit )
					break;
				pos += decode_int();
			}
			if( bc.error_flag )
				break;
		}
		return Std.cast(a);
	}

	function encode_string(s) {
		var is_b64 = true;
		var is_ascii = true;
		var i;
		for(i=0;i<s.length;i++) {
			if( s.charCodeAt(i) > 127 ) {
				is_b64 = false;
				is_ascii = false;
				break;
			} else if( is_b64 ) {
				var c = s.charAt(i);
				if( BitCodec.d64(c) == null )
					is_b64 = false;
			}
	 	}
	 	encode_int(s.length);
		if( is_b64 ) {
			bc.write(1,0);
			for(i=0;i<s.length;i++)
				bc.write(6,BitCodec.d64(s.charAt(i)));
 		} else {
			bc.write(2,is_ascii?2:3);
			for(i=0;i<s.length;i++)
				bc.write(is_ascii?7:8,s.charCodeAt(i));
		}
	}

	function decode_string() {
		var len = decode_int();
		var is_b64 = (bc.read(1) == 0);
		var s = "";
		var i;
		if( is_b64 ) {
			for(i=0;i<len;i++)
				s += BitCodec.c64(bc.read(6));
		} else {
			var is_ascii = (bc.read(1) == 0);
			for(i=0;i<len;i++)
				s += BitCodec.chr(bc.read(is_ascii?7:8));
		}
		return Std.cast(s);
	}

	function encode_object(o) {
		var me = this;
		downcast(Std).forin(o,fun(k) { me.encode_object_field(k,Std.cast(o)[Std.cast(k)]) });
		bc.write(2,3);
	}

	function encode_object_field(k,d) {
		if( downcast(Std)._typeof(d) != "function" && d != null ) {
			if( obfu_mode && k.charAt(0) == "$" )
				k = k.substring(1);
			if( fields.get(k) != null ) {
				bc.write(1,0);
				bc.write(nfields_bits,fields.get(k));
			} else {
				fields.set(k,nfields++);
				if( nfields >= next_field_bits ) {
					nfields_bits++;
					next_field_bits *= 2;
				}
				bc.write(2,2);
				encode_string(k);
			}
			do_encode(d);
		}
	}

	function decode_object_fast() {
		var o = downcast({});
		while( true ) {
			var k;
			var is_field_index = (bc.read(1) == 0);
			if( is_field_index )
				k = fieldtbl[bc.read(nfields_bits)];
			else {
				var is_end = (bc.read(1) == 1);
				if( is_end )
					break;
				k = decode_string();
				if( obfu_mode && k.charAt(0) != "$" )
					k = "$"+k;
				fieldtbl[nfields++] = k;
				if( nfields >= next_field_bits ) {
					nfields_bits++;
					next_field_bits *= 2;
				}
			}
			o[Std.cast(k)] = do_decode();
			if( bc.error_flag )
				break;
		}
		return Std.cast(o);
	}


	function decode_object() {
		var o = Std.cast({});
		cache.unshift(o);
		return Std.cast(o);
	}

	function decode_object_field(o) {
		var k;
		var is_field_index = (bc.read(1) == 0);

		if( is_field_index )
			k = fieldtbl[bc.read(nfields_bits)];
		else {
			var is_end = (bc.read(1) == 1);
			if( is_end )
				return false;
			k = decode_string();
			if( obfu_mode && k.charAt(0) != "$" )
				k = "$"+k;
			fieldtbl[nfields++] = k;
			if( nfields >= next_field_bits ) {
				nfields_bits++;
				next_field_bits *= 2;
			}
		}
		o[Std.cast(k)] = do_decode();
		return true;
	}

	function encode_int(o : int) {
		if( o < 0 ) {
			bc.write(3,7);
			encode_int(-o);
		} else if( o < 4 ) {
			bc.write(2,0);
			bc.write(2,o);
		} else if( o < 16 ) {
			bc.write(2,1);
			bc.write(4,o);
		} else if( o < 64 ) {
			bc.write(2,2);
			bc.write(6,o);
		} else if( o < 65536 ) {
			bc.write(4,12);
			bc.write(16,o);
		} else {
			bc.write(4,13);
			bc.write(16,o & 0xFFFF);
			bc.write(16,(o >> 16) & 0xFFFF);
		}
	}

	function decode_int() : 'a {
		var nbits = bc.read(2);
		if( nbits == 3 ) {
			var is_neg = (bc.read(1) == 1);
			if( is_neg )
				return Std.cast( -decode_int() );
			var is_big = (bc.read(1) == 1);
			if( is_big ) {
				var n = bc.read(16);
				var n2 = bc.read(16);
				return Std.cast( n | (n2 << 16) );
			} else
				return Std.cast( bc.read(16) );
		}
		var i = bc.read((nbits+1)*2);
		return Std.cast(i);
	}

	function encode_float(o) {
		var s = string(o);
		var l = s.length;
		var i;
		bc.write(5,l);
		for(i=0;i<l;i++) {
			var c = s.charCodeAt(i);
			if( c >= 48 && c <= 58 ) // 0 - 9
				bc.write(4,c-48);
			else if( c == 46 ) // '.'
				bc.write(4,10);
			else if( c == 43 ) // '+'
				bc.write(4,11);
			else if( c == 45 ) // '-'
				bc.write(4,12);
			else // 'e'
				bc.write(4,13);
		}
	}

	function decode_float() {
		var l = bc.read(5);
		var i;
		var s = "";
		for(i=0;i<l;i++) {
			var k = bc.read(4);
			if( k < 10 )
				k += 48;
			else switch( k ) {
			case 10: k = 46; break;
			case 11: k = 43; break;
			case 12: k = 45; break;
			default: k = 101; break;
			}
			s += String.fromCharCode(k);
		}
		return Std.getGlobal("parseFloat")(s);
	}

	function do_encode(o : 'a) : bool {
		if( o == null )
			bc.write(4,15);
		else if( downcast(Std)._instanceof(o,Array) ) {
			bc.write(3,4);
			encode_array(Std.cast(o));
		} else switch( downcast(Std)._typeof(o) ) {
		case "string":
			bc.write(4,14);
			encode_string(Std.cast(o));
			break;
		case "number":
			var n = Std.cast(o);
			if( Std.isNaN(n) )
				bc.write(4,6);
			else if( n == downcast(Std).infinity )
				bc.write(5,14);
			else if( n == -downcast(Std).infinity )
				bc.write(5,15);
			else if( int(n) == n ) {
				bc.write(2,0);
				encode_int(n);
			} else {
				bc.write(3,2);
				encode_float(n);
			}
			break;
		case "boolean":
			if( Std.cast(o) == true )
				bc.write(4,13);
			else
				bc.write(4,12);
			break;
		default:
			bc.write(3,5);
			encode_object(Std.cast(o));
			break;
		}
		return true;
	}

	function do_decode() : 'a {
		var is_number = (bc.read(1) == 0);
		if( is_number ) {
			var is_float = (bc.read(1) == 1);
			if( is_float ) {
				var is_special = (bc.read(1) == 1);
				if( is_special ) {
					var is_infinity = (bc.read(1) == 1);
					if( is_infinity ) {
						var is_negative = (bc.read(1) == 1);
						if( is_negative )
							return Std.cast(-downcast(Std).infinity);
						else
							return downcast(Std).infinity;
						return null;
					} else
						return Std.cast(0 * null); // NaN
				} else
					return decode_float();
			} else
				return decode_int();
		}
		var is_array_obj = (bc.read(1) == 0);
		if( is_array_obj ) {
			var is_obj = (bc.read(1) == 1);
			if( is_obj )
				return (fast?decode_object_fast():decode_object());
			else
				return (fast?decode_array_fast():decode_array());
		}
		var tflag = bc.read(2);
		if( tflag == 0 )
			return Std.cast(false);
		else if( tflag == 1 )
			return Std.cast(true);
		else if( tflag == 2 )
			return decode_string();
		else
			return null;
	}

	function encodeInit( o ) {
		fast = false;
		bc = new BitCodec();
		fields = new Hash();
		nfields = 0;
		next_field_bits = 1;
		nfields_bits = 0;
		cache = new Array();
		cache.push(Std.cast(o));
 	}

 	function encodeLoop() {
		if( cache.length == 0 )
			return true;
		do_encode(cache.shift());
		return false;
	}

	function encodeEnd() {
		var s = bc.toString();
		if( crc )
			s += bc.crcStr();
		return s;
	}

	function encode(o) {
		encodeInit(o);
		fast = true;
		while( encodeLoop() ) {
		}
		return encodeEnd();
	}

	function progress() {
		return bc.in_pos * 100 / bc.data.length;
	}

 	function decodeInit( data ) {
		fast = false;
		bc = new BitCodec();
		bc.setData(data);
		fieldtbl = new Array();
		nfields = 0;
		next_field_bits = 1;
		nfields_bits = 0;
		cache = new Array();
		result = null;
	}

	function decodeLoop() {
		if( cache.length == 0 )
			result = do_decode();
		else {
			var o = cache[0];
			if( downcast(Std)._instanceof(o,Array) ) {
				if( !decode_array_item(Std.cast(o)) )
					cache.shift();
			} else {
				if( !decode_object_field(Std.cast(o)) ) {
					Std.deleteField(Std.cast(o),"pos");
					cache.shift();
				}
			}
		}
		if( bc.error_flag ) {
			result = null;
			return false;
		}
		return (cache.length != 0);
	}

	function decodeEnd() {
		if( crc ) {
			var s = bc.crcStr();
			var s2 = bc.data.substr(bc.in_pos,4);
			if( s != s2 )
				return null;
		}
		return Std.cast(result);
	}

	function decode( data ) {
		decodeInit(data);
		//fast = true;
		while( decodeLoop() ) {
		}
		return decodeEnd();
	}

}
