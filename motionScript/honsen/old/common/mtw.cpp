/* ************************************************************************ */
/*																			*/
/*	MTW File Format															*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */

#include "mtw.h"

/* ------------------------------------------------------------------------ */

#define ISOTHER(x) (ctag[x] >= '0' && ctag[x] <= '9')
#define ISMIN(x) ((ctag[x] >= 'a' && ctag[x] <= 'z') || ISOTHER(x))
#define ISMAJ(x) ((ctag[x] >= 'A' && ctag[x] <= 'Z') || ISOTHER(x))
#define IS_IDENT() (ISMIN(0) && ISMAJ(1) && ISMAJ(2) && ISMAJ(3))
#define IS_GROUP() (ISMAJ(0) && ISMIN(1) && ISMIN(2) && ISMIN(3))
#define IS_NUMBER() (ISMIN(0) && ISMIN(1) && ISMIN(2) && ISMIN(3))

const TAG MTW2 = MAKETAG(MTW2);
const TAG Root = MAKETAG(Root);

class MTWFileIO : MTWReader, MTWWriter  {
public:
	FILE *f;
	MTWFileIO( FILE *f ) : f(f) {
	}

	bool read( void *buf, int size ) {
		return fread(buf,1,size,f) == (unsigned int)size;
	}

	bool write( void *buf, int size ) {
		return fwrite(buf,1,size,f) == (unsigned int)size;
	}
};

class MTWDataIO : MTWReader, MTWWriter  {
public:
	char *d; 
	DWORD position;
	DWORD length;
	MTWDataIO( void *d, DWORD length ) 
	: d((char *)d), position(0), length(length) {
	}

	bool read( void *buf, int size ) {
		if(position == length)
			position = 0;
		/*
		for(int i = 0; i < size; i++) {
			if(position >= length)
				return false;
			((char *)buf)[i] = d[position++];
		}
		*/
		memcpy(buf, d+position,size);
		position+=size;
		return true;	
	}

	bool write( void *buf, int size ) {
		return false;
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

MTW::MTW( const MTW &t )
{
    tag = t.tag;
    size = t.size;

    if( t.IsGroup() ) {
        childs = new WList<MTW*>();
        FOREACH(MTW*, *t.childs)
            childs->Add(new MTW(*item));
        ENDFOR;
    }
    else if(t.IsNumber())
        number = t.number;
    else
    {
        data = new char[size];
        memcpy(data, t.data, size);
    }
}


bool
MTW::IsGroup() const {
	return IS_GROUP();
}

bool
MTW::IsNumber() const {
	return IS_NUMBER();
}

bool
MTW::AddChild( MTW *t ) {
	if( !IS_GROUP() )
		return false;
	childs->Add(t);
	return true;
}

bool
MTW::RemoveChild( MTW *t ) {
	if( !IS_GROUP() )
		return false;
	return childs->Remove(t);
}

MTW*
MTW::Child( TAG t ) const {
	FOREACH(MTW*,*childs);
		if( item->tag == t )
			return item;
	ENDFOR;
	return NULL;
}

bool
MTW::HasChild( TAG t ) const {
	return Child(t) != NULL;
}

WList<MTW*>
MTW::Childs( TAG t ) const {
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
	else if( !IS_NUMBER() && !IS_IDENT() )
		delete data;
}

bool TagRead( MTW *t, MTWReader *r, int *remains ) {
	union {
		TAG tag;
		char ctag[4];
	};

	DWORD size;
	*remains -= 4;
	if( *remains < 0 )
		return false;
	if( !r->read(&tag,4) )
		return false;

	if( IS_IDENT() )
		size = 0;
	else {
		*remains -= 4;
		if( *remains < 0 )
			return false;
		if( !r->read(&size,4) )
			return false;
	}

	if( IS_GROUP() ) {
		WList<MTW*>* tlist = new WList<MTW*>();
		MTW *newt = new MTW(tag,size,tlist);
		int fpos = (*remains)-size;
		t->childs->Add(newt);
		while( *remains > fpos )
			if( !TagRead(newt,r,remains) )
				return false;
		return true;
	}

	if( IS_NUMBER() ) {
		t->childs->Add( new MTW(tag,4,(void*)size) );
		return true;
	}

	// IDENT
	if( size == 0 ) {
		t->childs->Add( new MTW(tag,0,NULL) );
		return true;
	}

	if( (int)size > *remains )
		return false;
	*remains -= size;
	char *data = new char[size];
	t->childs->Add( new MTW(tag,size,data) );
	if( !r->read(data,size) )
		return false;
	return true;
}

MTW*
MTW::ReadFile( WString file ) {
	FILE *f = fopen(file.c_str(),"rb");
	if( f == NULL )
		return NULL;
	MTWFileIO r(f);
	MTW *data = Read((MTWReader*)&r);	
	fclose(f);
	return data;
}

MTW*
MTW::ReadData( void *d, DWORD length ) {
	MTWDataIO r(d, length);
	MTW *data = Read((MTWReader*)&r);	
	return data;
}

MTW *
MTW::Read( MTWReader *r ) {
	TAG header;
	if( !r->read(&header,4) )
		return NULL;
	if( header != MTW2 )		
		return NULL;	
	int remains;
	if( !r->read(&remains,4) )
		return NULL;
	WList<MTW*> *tags = new WList<MTW*>();
	MTW *maintag = new MTW(Root,remains,tags);
	while( remains > 0 ) {
		if( !TagRead(maintag,r,&remains) ) {
			delete maintag;
			return NULL;
		}
	}
	if( remains != 0 ) {
		delete maintag;
		return NULL;
	}	
	return maintag;
}

static void WriteTag( MTW *t, FILE *f ) {
	union {
		TAG tag;
		char ctag[4];
	};
	tag = t->tag;
	fwrite(&t->tag,1,4,f);
	if( IS_IDENT() )
		return;
	if( IS_NUMBER() ) {
		fwrite(&t->data,1,4,f);
		return;
	}
	if( IS_GROUP() ) {
		int pos = ftell(f);
		fwrite(&t->size,1,4,f);
		FOREACH(MTW*,*t->childs);
			WriteTag(item,f);
		ENDFOR;
		DWORD size = ftell(f)-pos-4;
		if( t->size != size ) {
			fseek(f,pos,SEEK_SET);
			fwrite(&size,1,4,f);
			fseek(f,0,SEEK_END);
			t->size = size;
		}
		return;
	}
    fwrite(&t->size,1,4,f);
    fwrite(t->data,1,t->size,f);
}

bool
MTW::WriteFile( WString file ) {
	if( tag != Root )
		return false;
	FILE *f = fopen(file.c_str(),"wb");
	if( f == NULL )
		return false;
	WriteTag(this,f);
	fseek(f,0,SEEK_SET);
	fwrite(&MTW2,1,4,f);
	fclose(f);
	return true;
}

WString
MTW::ToString( TAG t ) {
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