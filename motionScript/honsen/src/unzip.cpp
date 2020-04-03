/* ************************************************************************ */
/*																			*/
/*	Honsen Runtime															*/
/*	Nicolas Cannasse														*/
/*	(c)2005 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "unzip.h"
#include "../common/mtw.h"
#include "../libs/zlib/zlib.h"

class ZLibReader : public MTWReader {
public:
	z_stream z;

	ZLibReader() {
		memset(&z,0,sizeof(z));
	}

	bool init( void *data, int size ) {
		z.avail_in = size;
		z.next_in = (Bytef*)data;
		return (inflateInit(&z) == Z_OK);
	}

	void close() {
		inflateEnd(&z);
	}

	bool read(void *buf, unsigned int size) {
		z.next_out = (Bytef*)buf;
		z.avail_out = size;
		inflate(&z,Z_SYNC_FLUSH);
		if( z.avail_out != 0 )
			return false;
		return true;
	}
};


char *unzip_data( char *data, int *len ) {
	return NULL;
}

MTW *unzip_mtw( MTW *mtw ) {
	if( mtw == NULL )
		return NULL;
	if( mtw->IsIdent() )
		return new MTW(mtw->tag);
	if( mtw->IsNumber() )
		return new MTW(mtw->tag,mtw->number);
	if( mtw->IsGroup() ) {
		MTW *m = new MTW(mtw->tag);
		FOREACH(MTW*,*mtw->childs)
			MTW *i = unzip_mtw(item);
			if( i == NULL ) {
				delete m;
				return NULL;
			}
			m->childs->Add(i);
		ENDFOR;
		return m;
	}
	if( mtw->tag == MAKETAG(HSGZ) ) {
		ZLibReader r;
		if( !r.init(mtw->data,mtw->size) )
			return NULL;
		MTW *m = MTW::Read(&r);
		r.close();
		MTW *mu = unzip_mtw(m);
		delete m;
		return mu;
	}
	if( mtw->IsData() ) {
		char *data2 = new char[mtw->size];
		memcpy(data2,mtw->data,mtw->size);
		return new MTW(mtw->tag,mtw->size,data2);
	}
	return NULL;
}

/* ************************************************************************ */
