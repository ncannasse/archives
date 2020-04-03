/* ************************************************************************ */
/*																			*/
/*	MTW File Format															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "mtw.h"

/* ------------------------------------------------------------------------ */

#define ISMIN(x) (ctag[x] >= 'a' && ctag[x] <= 'z')
#define ISMAJ(x) ((ctag[x] >= 'A' && ctag[x] <= 'Z') || (ctag[x] >= '0' && ctag[x] <= '9'))
#define IS_DATA() (ISMAJ(0) && ISMAJ(1) && ISMAJ(2) && ISMAJ(3))
#define IS_IDENT() (ISMIN(0) && ISMAJ(1) && ISMAJ(2) && ISMAJ(3))
#define IS_GROUP() (ISMAJ(0) && ISMIN(1) && ISMIN(2) && ISMIN(3))
#define IS_NUMBER() (ISMIN(0) && ISMIN(1) && ISMIN(2) && ISMIN(3))

class MTWFileIO : MTWReader, MTWWriter  {
public:
	FILE *f;
	MTWFileIO( FILE *f ) : f(f) {
	}
	bool read( void *buf, unsigned int size ) {
		while( size > 0 ) {
			int r = fread(buf,1,size,f);
			if( r <= 0 )
				return false;
			buf = (char*)buf + r;
			size -= r;
		}
		return true;
	}
	bool write( void *buf, unsigned int size ) {
		while( size > 0 ) {
			int r = fwrite(buf,1,size,f);
			if( r <= 0 )
				return false;
			buf = (char*)buf + r;
			size -= r;
		}
		return true;
	}
};

class MTWDataIO : MTWReader, MTWWriter  {
public:
	char *d; 
	unsigned int available;
	MTWDataIO( char *d, unsigned int available ) : d(d), available(available) {
	}
	bool read( void *buf, unsigned int size ) {
		if( size > available )
			return false;
		memcpy(buf,d,size);
		d += size;
		available -= size;
		return true;	
	}
	bool write( void *buf, unsigned int size ) {
		if( size > available )
			return false;
		memcpy(d,buf,size);
		d += size;
		available -= size;
		return true;
	}
};

MTW::MTW( TAG tag ) : tag(tag), size(0), data(NULL) {
	if( IS_GROUP() )
		data = new WList<MTW*>();
}

MTW::MTW( TAG tag, DWORD size, void *data ) : tag(tag), size(size), data(data) {
}

MTW::MTW( TAG tag, DWORD number ) : tag(tag), size(0), number(number) {
}

MTW::MTW( const MTW &t ) {
    tag = t.tag;
    size = t.size;
    if( t.IsGroup() ) {
        childs = new WList<MTW*>();
        FOREACH(MTW*, *t.childs)
            childs->Add(new MTW(*item));
        ENDFOR;
    }
    else if( t.IsNumber() )
        number = t.number;
    else if( t.IsData() ) {
        data = new char[size];
        memcpy(data, t.data, size);
    }
}

bool MTW::IsGroup() const {
	return IS_GROUP();
}

bool MTW::IsNumber() const {
	return IS_NUMBER();
}

bool MTW::IsData() const {
	return IS_DATA();
}

bool MTW::IsIdent() const {
	return IS_IDENT();
}

bool MTW::AddChild( MTW *t ) {
	if( !IS_GROUP() )
		return false;
	childs->Add(t);
	return true;
}

bool MTW::RemoveChild( MTW *t ) {
	if( !IS_GROUP() )
		return false;
	return childs->Remove(t);
}

MTW* MTW::Child( TAG t ) const {
	FOREACH(MTW*,*childs);
		if( item->tag == t )
			return item;
	ENDFOR;
	return NULL;
}

bool MTW::HasChild( TAG t ) const {
	return Child(t) != NULL;
}

WList<MTW*> MTW::Childs( TAG t ) const {
	if( t == 0 )
		return *childs;
	WList<MTW*> clist;
	FOREACH(MTW*,*childs);
		if( item->tag == t )
			clist.Add(item);
	ENDFOR;
	return clist;
}

MTW::~MTW() {
	if( IS_GROUP() ) {
		childs->Delete();
		delete childs;
	}
	else if( IS_DATA() )
		delete data;
}

MTW *MTW::Read( MTWReader *r ) {
	union {
		TAG tag;
		char ctag[4];
	};

	DWORD size;
	if( !r->read(&tag,4) )
		return NULL;

	if( IS_IDENT() )
		return new MTW(tag,0,NULL);

	if( !r->read(&size,4) )
		return NULL;

	if( IS_NUMBER() )
		return new MTW(tag,4,(void*)size);

	if( IS_GROUP() ) {
		WList<MTW*>* tlist = new WList<MTW*>();
		MTW *newt = new MTW(tag,size,tlist);
		while( size > 0 ) {
			MTW *t = Read(r);
			if( t == NULL ) {
				delete newt;
				return NULL;
			}
			size -= (t->IsGroup() || t->IsData())?(t->size + 8):(t->size + 4);
			tlist->Add(t);
			if( size < 0 ) {
				delete newt;
				return NULL;
			}
		}
		return newt;
	}

	if( IS_DATA() ) {
		char *data = new char[size];
		if( !r->read(data,size) ) {
			delete data;
			return false;
		}
		return new MTW(tag,size,data);
	}

	return NULL;
}

MTW* MTW::ReadFile( WString file ) {
	FILE *f = fopen(file.c_str(),"rb");
	if( f == NULL )
		return NULL;
	MTWFileIO r(f);
	MTW *data = Read((MTWReader*)&r);	
	fclose(f);
	return data;
}

MTW* MTW::ReadData( char *d, int length ) {
	MTWDataIO r(d, length);
	MTW *data = Read((MTWReader*)&r);
	return data;
}

bool MTW::Write( MTWWriter *r ) {
	r->write(&tag,4);
	if( IS_IDENT() )
		return true;
	if( IS_NUMBER() ) {
		r->write(&data,4);
		return true;
	}
	if( IS_GROUP() ) {
		r->write(&size,4);
		FOREACH(MTW*,*childs);
			if( !item->Write(r) )
				return false;
		ENDFOR;
		return true;
	}
	if( IS_DATA() ) {
		r->write(&size,4);
		r->write(data,size);
		return true;
	}
	return false;
}

bool MTW::WriteFile( WString file ) {
	FILE *f = fopen(file.c_str(),"wb");
	if( f == NULL )
		return false;
	MTWFileIO r(f);
	CalcSize();
	bool res = Write((MTWWriter*)&r);
	fclose(f);
	return res;
}

int MTW::CalcSize() {
	if( IS_IDENT() ) {
		size = 0;
		return 4;
	}
	if( IS_NUMBER() ) {
		size = 4;
		return 8;
	}
	if( IS_GROUP() ) {
		size = 0;
		FOREACH(MTW*,*childs);
			size += item->CalcSize();
		ENDFOR;
		return size + 8;
	}
	return size + 8;
}

WString MTW::ToString( TAG t ) {
	int i;
	WString s;
	for(i=0;i<4;i++) {
		char c = ((char*)&t)[i];
		if( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <='z') || (c >= '0' && c <= '9') )
			s += c;
		else
			s += wprintf("\\%.2X",((BYTE*)&t)[i]);
	}
	return s;
}

/* ************************************************************************ */