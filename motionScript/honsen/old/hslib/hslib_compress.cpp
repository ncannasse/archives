/* ************************************************************************ */
/*																			*/
/*	Honsen Lib																*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
typedef struct HWND__ *HWND;
#include <memory.h>
#include <stdio.h>
#include "zlib/zlib.h"
#include "hslib.h"
#include "../common/wlist.h"

namespace HSLib {


const int LEVEL_BITS[11][2] = {
	// density * precision
	{ 3 , 15 },
	{ 3 , 10 },
	{ 3 , 7 },
	{ 4 , 7 },
	{ 4 , 6 },
	{ 5 , 6 },
	{ 6 , 6 },
	{ 6 , 5 },
	{ 7 , 5 },
	{ 8 , 4 },
	{ 10 , 3 }
};

#define DENSITY_BITS	LEVEL_BITS[data->level][0]
#define PRECISION_BITS	LEVEL_BITS[data->level][1]

enum SUBTYPE {
	NOSUB = 0,
	XSUB = 1,
	YSUB = 2,
	ZSUB = 3
};

/* ------------------------------------------------------------------------ */

struct Bits {
	unsigned char *data;
	unsigned int bits;
	int datasize;
	int nbits;

	static int NBits( unsigned int v ) {
		int n = 0;
		while( v > 0 ) {
			v >>= 1;
			n++;
		}
		return n;
	}

	Bits( void *data, int size ) : data((unsigned char*)data), datasize(size) {
		bits = 0;
		nbits = 0;
	}

	void Flush() {
		if( nbits > 0 )
			Write(8-nbits,0);
	}

	bool Write( int nb, unsigned int v ) {
#ifdef _DEBUG
		if( nb < 32 && v >= (unsigned int)(1 << nb) )
			return false;
#endif
		if( nb == 32 )
			bits = 0;
		else
			bits <<= nb;
		bits |= v;
		nbits += nb;
		while( nbits >= 8 ) {
			if( datasize <= 0 )
				return false;
			nbits -= 8;
			*data = (unsigned char)(bits >> nbits);
			data++;
			datasize--;
		}
		return true;
	}

	int Read( int nb ) {
		while( nbits < nb ) {
			if( datasize <= 0 )
				return -1;
			bits <<= 8;
			bits |= *data;
			nbits += 8;
			data++;
			datasize--;
		}
		nbits -= nb;
		if( nb == 32 )
			return bits >> nbits;
		else
			return (bits >> nbits) & ((1 << nb) - 1);
	}

};

/* ------------------------------------------------------------------------ */

struct Block {
	FLOAT x,y,z;
	FLOAT xsize,ysize,zsize;
	WList<VECTOR*> elts;

	FLOAT xmin, xmax;
	FLOAT ymin, ymax;
	FLOAT zmin, zmax;

	SUBTYPE subtype;
	Block *sub1;
	Block *sub2;

	Block(FLOAT x, FLOAT y, FLOAT z, FLOAT xsize, FLOAT ysize, FLOAT zsize) : x(x), y(y), z(z), xsize(xsize), ysize(ysize), zsize(zsize) {
		sub1 = NULL;
		sub2 = NULL;
		subtype = NOSUB;
	}

	Block *Add( VECTOR *v ) {
		switch( subtype ) {
		case XSUB:
			return ((v->x < (x + xsize/2))?sub1:sub2)->Add(v);
		case YSUB:
			return ((v->y < (y + ysize/2))?sub1:sub2)->Add(v);
		case ZSUB:
			return ((v->z < (z + zsize/2))?sub1:sub2)->Add(v);
		default:
			if( elts.Empty() ) {
				xmin = v->x;
				xmax = xmin;
				ymin = v->y;
				ymax = ymin;
				zmin = v->z;
				zmax = zmin;
			} else {
				if( v->x < xmin )
					xmin = v->x;
				else if( v->x > xmax )
					xmax = v->x;
				if( v->y < ymin )
					ymin = v->y;
				else if( v->y > ymax )
					ymax = v->y;
				if( v->z < zmin )
					zmin = v->z;
				else if( v->z > zmax )
					zmax = v->z;
			}
			elts.Add(v);
			return this;
		}
	}

	void CreateSubs() {
		switch( subtype ) {
		case XSUB:
			sub1 = new Block(x,y,z,xsize/2,ysize,zsize);
			sub2 = new Block(x+xsize/2,y,z,xsize/2,ysize,zsize);
			break;
		case YSUB:
			sub1 = new Block(x,y,z,xsize,ysize/2,zsize);
			sub2 = new Block(x,y+ysize/2,z,xsize,ysize/2,zsize);
			break;
		case ZSUB:
			sub1 = new Block(x,y,z,xsize,ysize,zsize/2);
			sub2 = new Block(x,y,z+zsize/2,xsize,ysize,zsize/2);
			break;
		}
	}

	void Divide( int max ) {		
		if( subtype != NOSUB )
			return;

		if( elts.Length() >= max ) {
			FLOAT xd = xmax - xmin;
			FLOAT yd = ymax - ymin;
			FLOAT zd = zmax - zmin;

			if( xd >= yd && xd >= zd )
				subtype = XSUB;
			else if( yd >= xd && yd >= zd )
				subtype = YSUB;
			else
				subtype = ZSUB;
			CreateSubs();

			FOREACH(VECTOR*,elts);
				Add(item);
			ENDFOR;

			elts.Clean();

			sub1->Divide(max);
			sub2->Divide(max);
		}
	}

	~Block() {
		delete sub1;
		delete sub2;
	}
};

/* ------------------------------------------------------------------------ */

struct CData {
	int tbl_nbits;
	int tbl_size;
	int level;
	VECTOR *tbl_base;
};

/* ------------------------------------------------------------------------ */

bool CompressBlock( Block *b, Bits *out, CData *data ) {
	if( !out->Write(2,b->subtype) )
		return false;
	if( b->subtype == NOSUB ) {
		if( !out->Write(DENSITY_BITS,b->elts.Length()) )
			return false;		
		FOREACH(VECTOR*,b->elts);
			FLOAT dx = (item->x - b->x)/b->xsize;
			FLOAT dy = (item->y - b->y)/b->ysize;
			FLOAT dz = (item->z - b->z)/b->zsize;
			if( !out->Write(data->tbl_nbits,(int)(item - data->tbl_base)) )
				return false;
			if( !out->Write(PRECISION_BITS, (int)(dx * (1 << PRECISION_BITS)) ) )
				return false;
			if( !out->Write(PRECISION_BITS, (int)(dy * (1 << PRECISION_BITS)) ) )
				return false;
			if( !out->Write(PRECISION_BITS, (int)(dz * (1 << PRECISION_BITS)) ) )
				return false;
		ENDFOR;
	} else {
		if( !CompressBlock(b->sub1,out,data) )
			return false;
		if( !CompressBlock(b->sub2,out,data) )
			return false;
	}
	return true;
}

/* ------------------------------------------------------------------------ */

bool Compress( int level, void *output, int *outlen, VECTOR *input, int inlen ) {
	if( inlen == 0 ) {
		*outlen = 0;
		return true;
	}

	if( level <= 0 )
		level = 0;
	else if( level >= 10 )
		level = 10;

	FLOAT xmin = input[0].x;
	FLOAT ymin = input[0].y;
	FLOAT zmin = input[0].z;
	FLOAT xmax = xmin, ymax = ymin, zmax = zmin;
	
	int i;
	for(i=1;i<inlen;i++) {
		VECTOR *v = &input[i];
		if( v->x < xmin )
			xmin = v->x;
		else if( v->x > xmax )
			xmax = v->x;
		if( v->y < ymin )
			ymin = v->y;
		else if( v->y > ymax )
			ymax = v->y;
		if( v->z < zmin )
			zmin = v->z;
		else if( v->z > zmax )
			zmax = v->z;
	}

	xmax += 0.01f;
	ymax += 0.01f;
	zmax += 0.01f;

	Block root(xmin,ymin,zmin,xmax-xmin,ymax-ymin,zmax-zmin);
	for(i=0;i<inlen;i++) {
		Block *b = root.Add(&input[i]);
		b->Divide(1 << LEVEL_BITS[level][0]);
	}

	Bits bits(output,*outlen);
	bits.Write(16,inlen);
	bits.Write(32,*(DWORD*)&xmin);
	bits.Write(32,*(DWORD*)&xmax);
	bits.Write(32,*(DWORD*)&ymin);
	bits.Write(32,*(DWORD*)&ymax);
	bits.Write(32,*(DWORD*)&zmin);
	bits.Write(32,*(DWORD*)&zmax);
	bits.Write(4,level);

	CData d;
	d.tbl_base = input;
	d.tbl_size = inlen;
	d.tbl_nbits = Bits::NBits(inlen);
	d.level = level;

	if( !CompressBlock(&root,&bits,&d) )
		return false;
	bits.Flush();
	*outlen -= bits.datasize;

	return true;
}

bool UncompressBlock( Block *b, Bits *input, CData *data ) {
	b->subtype = (SUBTYPE)input->Read(2);
	if( b->subtype == -1 )
		return false;
	b->CreateSubs();
	if( b->subtype == NOSUB ) {
		int i;
		int nvects = input->Read(DENSITY_BITS);
		if( nvects == -1 )
			return false;
		for(i=0;i<nvects;i++) {
			int p = input->Read(data->tbl_nbits);
			int dx = input->Read(PRECISION_BITS);
			int dy = input->Read(PRECISION_BITS);
			int dz = input->Read(PRECISION_BITS);
			if( dx == -1 || dy == -1 || dz == -1 || p == -1 || p >= data->tbl_size )
				return false;
			VECTOR* v = data->tbl_base + p;
			v->x = (FLOAT)(b->x + (dx * 1.0 / (1 << PRECISION_BITS)) * b->xsize);
			v->y = (FLOAT)(b->y + (dy * 1.0 / (1 << PRECISION_BITS)) * b->ysize);
			v->z = (FLOAT)(b->z + (dz * 1.0 / (1 << PRECISION_BITS)) * b->zsize);			
		}
	} else {
		if( !UncompressBlock(b->sub1,input,data) )
			return false;
		if( !UncompressBlock(b->sub2,input,data) )
			return false;
	}
	return true;
}

int UncompressHeader( void *input, int len ) {
	Bits bits(input,len);
	return bits.Read(16);
}

bool Uncompress( VECTOR *output, int *outlen, void *input, int len ) {
	Bits bits(input,len);
	DWORD nvects = bits.Read(16);
	DWORD xmin = bits.Read(32);
	DWORD xmax = bits.Read(32);
	DWORD ymin = bits.Read(32);
	DWORD ymax = bits.Read(32);
	DWORD zmin = bits.Read(32);
	DWORD zmax = bits.Read(32);
	int level = bits.Read(4);

	if( nvects == -1 || xmin == -1 || ymin == -1 || zmin == -1 || xmax == -1 || ymax == -1 || zmax == -1 || level == -1 )
		return false;

	if( *outlen < (int)nvects || level > 10 )
		return false;

	Block root(
		*(FLOAT*)&xmin,
		*(FLOAT*)&ymin,
		*(FLOAT*)&zmin,
		*(FLOAT*)&xmax - *(FLOAT*)&xmin,
		*(FLOAT*)&ymax - *(FLOAT*)&ymin,
		*(FLOAT*)&zmax - *(FLOAT*)&zmin
	);

	CData d;
	d.tbl_size = nvects;
	d.tbl_base = output;
	d.tbl_nbits = Bits::NBits(nvects);
	d.level = level;
	if( !UncompressBlock(&root,&bits,&d) )
		return false;
	*outlen = nvects;
	return true;
}

bool Unzip( void *output, int *outlen, void *input, int inlen ) {
	return( uncompress((Bytef*)output,(uLongf*)outlen,(Bytef*)input,inlen) == Z_OK );
}


/* ------------------------------------------------------------------------ */
}; // namespace

/* ------------------------------------------------------------------------ */

extern "C" {

#define ERROR ((-1) << 1) | 1

__declspec(dllexport) int caml_hslib_compress(int cmp, char *input, int inlen, char *out, int outlen) {
	cmp >>= 1;
	inlen >>= 1;
	outlen >>= 1;
	int rest = inlen % sizeof(VECTOR);

	if( rest != 0 ) {
		char *inp2 = new char[inlen + sizeof(VECTOR)-rest];
		memcpy(inp2,input,inlen);
		memset(inp2+inlen,0,sizeof(VECTOR)-rest);
		input = inp2;
		inlen += sizeof(VECTOR)-rest;
	}

	if( !HSLib::Compress(cmp,out,&outlen,(VECTOR*)input,inlen/sizeof(VECTOR)) ) {
		if( rest != 0 )
			delete input;
		return ERROR;
	}
	if( rest != 0 )
		delete input;
	return (outlen << 1) | 1;
}

__declspec(dllexport) int caml_hslib_zip(char *input, int inlen, char *out, int outlen) {
	inlen >>= 1;
	outlen >>= 1;
	outlen -= 4;
	if( compress((Bytef*)(out+4),(uLongf*)&outlen,(Bytef*)input,inlen) != Z_OK )
		return ERROR;	
	outlen += 4;
	*(DWORD*)out = inlen;
	return (outlen << 1) | 1;
}

};

/* ************************************************************************ */
