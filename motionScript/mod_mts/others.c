#include <value.h>

value std_url_decode( value v ) {	
	if( !val_is_string(v) )
		return NULLVAL;
	{
		int pin = 0;
		int pout = 0;
		const char *in = val_string(v);
		int len = val_strlen(v);
		value v2 = alloc_raw_string(len);
		char *out = (char*)val_string(v2);
		while( len-- > 0 ) {
			char c = in[pin++];
			if( c == '+' )
				c = ' ';
			else if( c == '%' ) {
				int p1, p2;
				if( len < 2 )
					break;
				p1 = in[pin++];
				p2 = in[pin++];
				len -= 2;
				if( p1 >= '0' && p1 <= '9' )
					p1 -= '0';
				else if( p1 >= 'a' && p1 <= 'f' )
					p1 -= 'a' - 10;
				else if( p1 >= 'A' && p1 <= 'F' )
					p1 -= 'A' - 10;
				else
					continue;
				if( p2 >= '0' && p2 <= '9' )
					p2 -= '0';
				else if( p2 >= 'a' && p2 <= 'f' )
					p2 -= 'a' - 10;
				else if( p2 >= 'A' && p2 <= 'F' )
					p2 -= 'A' - 10;
				else
					continue;
				c = (char)((unsigned char)((p1 << 4) + p2));
			}
			out[pout++] = c;
		}
		out[pout] = 0;
		val_strlen(v2) = pout;		
		return v2;
	}
}

value std_url_encode( value v ) {
	if( !val_is_string(v) )
		return NULLVAL;
	{
		int pin = 0;
		int pout = 0;
		const unsigned char *in = (const unsigned char*)val_string(v);
		int len = val_strlen(v);
		value v2 = alloc_raw_string(len * 3);
		unsigned char *out = (unsigned char*)val_string(v2);
		while( len-- > 0 ) {
			unsigned char c = in[pin++];
			if( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' )
				out[pout++] = c;
			else {
				out[pout++] = '%';
				out[pout++] = '0' + (c >> 4);
				out[pout++] = '0' + (c & 0xF);
			}
		}
		out[pout] = 0;
		val_strlen(v2) = pout;
		return v2;
	}
}

DEFINE_PRIM(std_url_encode,1);
DEFINE_PRIM(std_url_decode,1);

