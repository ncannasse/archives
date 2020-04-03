/* ************************************************************************ */
/*																			*/
/*	M3D-SE SDK Definitions													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once
#include "mdx.h"
#include "../common/mtw.h"
#include "../common/wstring.h"

namespace M3D {

	struct Context;
	typedef DWORD TAG;

	enum TAG_FORMAT {
		TF_UNKNOWN,// not registered or not defined
		TF_FLOAT1, // x -> ( x, 0, 0, 1 )
		TF_FLOAT2, // x,y ->( x, y, 0, 1 )
		TF_FLOAT3, // x,y,z -> ( x, y, z, 1 )
		TF_FLOAT4, // x,y,z,w -> ( x, y, z, w )
		TF_COLOR,  // ARGB DWORD -> ( a, r, g, b )
		TF_SHORT2, // XXYY DWORD  -> ( x, y, 0, 1 )
		TF_SHORT4, // XXYYZZWW QWORD -> ( x, y, z, w )
		//TF_UBYTE4, // ABGR DWORD -> ( a, r, g, b ) not supported on GForce3
	};

	namespace TagFormat {
		void RegisterFormat( Context *c, TAG t, TAG_FORMAT fmt );
		void RegisterStride( Context *c, TAG t, DWORD stride );
		TAG_FORMAT GetFormat( Context *c, TAG t );
		int GetStride( Context *c, TAG t );		
		int StrideOfFormat( TAG_FORMAT fmt );
	};

	class Pipeline {
	public:
		virtual void SetLastError( WString msg ) = 0;
		virtual bool Read( TAG t ) = 0;
		virtual bool Write( TAG t ) = 0;
		virtual bool Delete( TAG t ) = 0;
	};

	template <class T> class WriteStream;

	class StreamPipeline {
	protected:
		virtual const void * QueryRead( TAG t, int *size ) = 0;
	public:
		const void* Read( TAG t, int *size = NULL ) { return QueryRead(t,size); }
		virtual WriteStream<BYTE> QueryWrite( TAG t ) = 0;
		virtual int NVerts() = 0;
		virtual int NFaces() = 0;
		virtual bool TimeCoef( TAG t, void **keyf1, void **keyf2, FLOAT *time ) = 0;
		virtual bool TimeCoef2( TAG t, void **keyf1, void **keyf2, void **keyf3, FLOAT *time ) = 0;
	};

	template <class T> class WriteStream {
	private:
		char *data;
		int stride;
	public:
		WriteStream( StreamPipeline *sp, TAG t ) { *this = *(WriteStream<T>*)&sp->QueryWrite(t); }
		WriteStream( char *data, int stride ) : data(data), stride(stride) { }
		void Set( const T &item ) { *(T*)data = item; }
		void Copy( const T *from, int n ) { int p; for(p=0;p<n;p++) { *(T*)data = from[p]; data += stride; } }
		void Next() { data += stride; }
		inline T& operator []( int index ) { return *(T*)(data + stride*index); }
	};


	class Mesh;

	class Renderer {
	public:
		virtual ~Renderer() { }
		virtual const char *Name() = 0;
		virtual bool Validate( Pipeline *p ) = 0;
		virtual bool Render( StreamPipeline *s ) = 0;

		static void Register( Context *c, Renderer* (*create)(Mesh*) );
		static Renderer *New( const char *name, Mesh *obj );		
	};

	class ShaderConst {
	public:
		virtual ~ShaderConst() { }
		virtual const char *Name() = 0;
		virtual int Size() = 0;
		virtual void *Data( Context *c ) = 0;

#		define SHADERCONST(name,size) const char *Name() { return #name; } int Size() { return size; }

		static void Register( Context *c, ShaderConst *(*create)(Mesh*) );
		static ShaderConst *New( const char *name, Mesh *obj );		
	};

};

/* ************************************************************************ */
