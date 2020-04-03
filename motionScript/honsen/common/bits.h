/* ************************************************************************ */
/*																			*/
/*	Bits Reading															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

class Bits {
public:
	unsigned char *data;
	unsigned int acc;
	int size;
	int nbits;

	Bits( void *data, int size ) : data((unsigned char*)data), size((size > 0)?size:0) {
		acc = 0;
		nbits = 0;
	}

	int Read( int n ) {
		if( nbits >= n ) {
			int c = nbits - n;
			int k = (acc >> c) & ((1 << n) - 1);
			nbits = c;
			return k;
		}
		if( size == 0 )
			return -1;
		int k = *data;
		data++;
		size--;
		if( nbits >= 24 ) {
			if( n >= 32 )
				return -1;
			int c = 8 + nbits - n;
			int d = acc & ((1 << nbits) - 1);
			d = (d << (8 - c)) | (k >> c);
			acc = k;
			nbits = c;
			return d;
		}
		acc = (acc << 8) | k;
		nbits += 8;
		return Read(n);
	}

	static int Signed( int v, int nbits ) {
		if( v >= (1 << (nbits - 1)) )
			return -((v ^ ((1 << nbits) - 1)) + 1);
		return v;
	}

};

/* ************************************************************************ */
