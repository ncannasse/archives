/* ************************************************************************ */
/*																			*/
/*	M3D-SE																	*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include "../common/wstring.h"
#include "mdx.h"
#include "../common/wmem.h"
#include "../common/wlist.h"

#define M3DDEBUG(msg)		M3D::DebugMessage(c,msg,__FILE__,__LINE__)
#define M3DERROR(call,msg)	(((c->lasterror = (call)) == D3D_OK)?false:(M3D::DebugMessage(c,msg##" #"+itos(c->lasterror-MAKE_D3DHRESULT(0)),__FILE__,__LINE__),true))
#define FLAG(value,flag)	(((value) & (flag)) == (flag))
#define MAKETAG(x)			((#x[3]<<24) | (#x[2]<<16) | (#x[1]<<8) | #x[0])

typedef DWORD TAG;
class MTW;

namespace M3D {

	struct Context;

	void Display( Context *c, WString msg );
	void DebugMessage( Context *c, WString msg, const char *file, int line );
	MATRIX *GetMatrixProj( Context *c );
	void SetMatrixProj( Context *c, MATRIX *m );
	void SetCamera( Context *c, MATRIX *m );
	void SetWorld( Context *c, MATRIX *m );
	MATRIX *GetWorld( Context *c );
	void SetLightDir( Context *c, VECTOR *m );
	VECTOR *GetLightDir( Context *c );
	void SetLightColor( Context *c, DWORD m );
	DWORD GetLightColor( Context *c );
	void SetAmbient( Context *c, DWORD m );
	DWORD GetAmbient( Context *c );

	template <class T> class Variable {
	private:
		T data;
		bool valid;
	public:
		Variable() : valid(false) { }
		Variable( T data ) : data(data), valid(false) { }
		
		T Get() { return data; }
		T*Ref() { return &data; }

		void Set( const T &copy ) { data = copy; valid = false; }
		void Validate() { valid = true; }
		void Invalidate() { valid = false; }
		bool IsValid() { return valid; }
	};

	class Mesh;

	class Anim {
		struct KeyFrame;
		KeyFrame *keys;
		KeyFrame *next_keys;
		MTW *adata;
		void UpdateKeyFrames();
		static void CleanRef( void *ref );
	public:
		Mesh *parent;
		WString name;
		int nframes;
		double speed;
		double time;
		bool loop;
		bool play;


		Anim( Mesh *m );
		~Anim();

		bool SetData( MTW *data );
		MTW *GetData();

		void *script_ref;
		void UpdateTime();
		void TimeCoef2( void **f1, void **f2, void **f3, FLOAT *coef );		
	};

	class Mesh {
		struct Data;
		Data *data;
		static void CleanRef( void *ref );
	public:
		Mesh( Context *c, Mesh *parent, MTW *objtag );
		~Mesh();

		// variables
		bool visible;
		MATRIX pos;
		DWORD color;
		double alpha;

		Variable<MATRIX> abs_pos;
		Variable<COLOR> abs_color;

		void PosModified();
		void ColorAlphaModified();
		void UpdatePos();
		void UpdateColor();

		MTW *GetTexture();
		void SetTexture( MTW *t );

		// pipeline
		bool AddRenderer( WString name, bool is_static );
		bool RemoveRenderer( WString name );
		void ClearRenderers();
		void SetShader( WString code );
		WString GetShader();

		bool Validate();
		void Invalidate();
		bool IsValid();
		bool HasTag( TAG t );
		int Render( bool is_alpha );

		// childs
		Mesh *parent;
		WList<Mesh*> childs;

		Context *context;
		void *script_ref;

		void AddAnim( Anim *a );
		void RemoveAnim( Anim *a );

		static int RenderAll( Context *c, bool is_alpha );
		static void ValidateAll( Context *c );
		static void InvalidateAll( Context *c );
		static bool Prepare( MTW *m );
		static int MakeId( const char *name );
		static void *GetData( VECTOR4 *vtmp, void *o, int id, int *size );
	};

};

/* ************************************************************************ */
