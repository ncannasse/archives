/* ****************************************************************	*/
/*																	*/
/*	class WBuffer													*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard template buffer class								*/
/*																	*/
/*	class WCBuffer : a char buffer									*/
/*																	*/
/* ****************************************************************	*/
#ifndef WBUFFER_H
#define WBUFFER_H

template<class ITEM> class WBuffer {

public:
	
	WBuffer( int len = 0 )
		{
			if( len < 0 )
				len = 0;
			buf = new ITEM[len];
			length = len;
		}

	WBuffer( const ITEM *ibuf, int len )
		{
			if( len < 0 )
				len = 0;
			buf = new ITEM[len];
			length = len;
			memcpy(buf,ibuf,sizeof(ITEM)*len);
		}

	WBuffer( const WBuffer<ITEM> &cbuf )
		{
			length = cbuf.Length();
			buf = new ITEM[length];
			memcpy(buf,cbuf.Buffer(),sizeof(ITEM)*length);
		}

	int Length()
		{
			return length;
		}

	ITEM *Buffer()
		{
			return buf;
		}

	WBuffer<ITEM> *Duplicate()
		{
			return new WBuffer<ITEM>(*this);		
		}

	int Read( ITEM *ibuf, int len, int pos = 0 )
		{
			if( len > length )
				len = length;
			if( len+pos > length )
				len = length-pos;
			if( len <= 0 )
				return;
			memcpy( ibuf,buf+(pos*sizeof(ITEM)),sizeof(ITEM)*len);
			return len;
		}	

	int Read( WBuffer<ITEM> *ibuf, int len = -1, int pos = 0 )
		{
			if( len == -1 )
				len = ibuf->Length();
			if( len > length )
				len = length;
			if( len+pos > length )
				len = length-pos;
			if( len <= 0 )
				return;
			ibuf->Resize(len);
			memcpy( (void*)ibuf->Buffer(),buf+(pos*sizeof(ITEM)),sizeof(ITEM)*len);
			return len;
		}

	int Write( const ITEM *ibuf, int len, int pos = 0 )
		{		
			if( len <= 0 )
				return 0;
			if( len + pos > length ) {
				ITEM *obuf = buf;			
				buf = new ITEM[len+pos];				
				memcpy(buf,obuf,(pos <= length?pos:length)*sizeof(ITEM));
				length = len+pos;
				delete[] obuf;
			}
			memcpy(buf+pos*sizeof(ITEM),ibuf,len*sizeof(ITEM));
			return length;
		}

	int Write( const WBuffer<ITEM> *ibuf, int pos = 0 )
		{
			int len = ibuf->Length();
			if( pos <= 0 )
				pos = 0;
			if( len + pos > length )
			{
				ITEM *obuf = buf;			
				buf = new ITEM[len+pos];				
				memcpy(buf,obuf,(pos <= length?pos:length)*sizeof(ITEM));
				length = len+pos;
				delete[] obuf;
			}
			memcpy(buf+pos*sizeof(ITEM),ibuf->Buffer(),len*sizeof(ITEM));
			return length;
		}

	int Add( const ITEM i )
		{
			return Write(&i,1,length);
		}

	int Append( const ITEM *ibuf, int len )
		{
			return Write(ibuf,len,length);
		}

	int Append( const WBuffer<ITEM> *ibuf )
		{
			return Write(ibuf,length);
		}

	int Resize( int len, const ITEM *zero = NULL )
		{
			if( len < 0 )
				len = 0;
			if( len <= length )
				length = len;
			else
			{
				ITEM *nbuf = new ITEM[len];
				memcpy(nbuf,buf,length*sizeof(ITEM));
				if( zero != NULL )
				{
					int i;
					for(i=length;i<len;i++)
						nbuf[i] = *zero;
				}
				delete[] buf;
				buf = nbuf;				
				length = len;
			}
			return length;
		}

	int Delete( int pos = 0, int len = -1 )
		{
			if( pos > length )
				return length;
			if( pos < 0 )
				pos = 0;
			if( len < 0 )
				len = length;
			if( len + pos > length )
				len = length-pos;
			memcpy(buf+pos*sizeof(ITEM),buf+(pos+len)*sizeof(ITEM),(length-(pos+len))*sizeof(ITEM));
			length-=len;
			return length;
		}

	~WBuffer()
		{
			delete[] buf;
		}

private:

	ITEM	*buf;
	int		length;

};

typedef WBuffer<char>		WCBuffer;

#endif
/* ****************************************************************	*/
