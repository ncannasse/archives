/* **************************************************************************** */
/*																				*/
/*   M3D 3D render																*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#pragma once
#ifdef EXPORTER
#pragma warning( disable : 4530 )
#endif
#include <mdx.h>
#include <wstring.h>
#include <wiostream.h>
#ifndef EXPORTER
#include <wmem.h>
#endif
#include <wlist.h>
#include <warray.h>

#define M3DCHECK(code)		M3D::CheckResult(code,__FILE__,__LINE__)
#define M3DERROR(code,msg) !M3D::CheckResult(code,__FILE__,__LINE__,(WString)(msg))
#define M3DTRACE(msg)		M3D::Log((WString)(msg))
#define M3DRELEASE(x)		if( x ) { x->Release(); x = NULL; }

/* **************************************************************************** */
namespace M3D {

/* ---------------------------------------------------------------------------- */

template<class T> class MFlagMod {
	T item;
	bool f_modified; /*** Some problem could occur because the flag is not atomic */
public:
	MFlagMod( const T& def ) : item(def) { f_modified = false; }
	T* Get() { return &item; }
	void Set( const T& i ) { item = i; f_modified = true; }
	bool IsModified() { return f_modified; }
	void Invalidate() { f_modified = true; }
	void ClearModified() { f_modified = false; }
};

typedef MFlagMod<VECTOR> MVec;
typedef MFlagMod<MATRIX> MMat;
typedef MFlagMod<VECTOR4> MVec4;

/* ---------------------------------------------------------------------------- */
	
	void SetConfig( HWND target, bool fullscreen );
	
	bool Init( bool vsync );	
	bool Render();
	void UpdateTime();
	void SetMaterial( MATERIAL *mat, TEXTURE tex );		
	float SecTo3DTime( FLOAT t );
	void Print( WString msg );
	void Log( WString msg );
	void Close();

	VECTOR2 Project( FLOAT x, FLOAT y, FLOAT target_z );
	bool CheckResult( HRESULT h, const char *file, int line, WString msg = "" );

	extern const MATRIX IDMATRIX;
	extern MMat transform;
	extern MVec4 dirlight;
	extern MVec4 calclight;
	extern MATRIX ttransform;
	extern DEVICE device;
	extern FLOAT curtime;
	extern FLOAT elapsed_time;
	extern DWORD bgcolor;
	extern bool show_infos;
	extern bool do_shading;

/* ---------------------------------------------------------------------------- */

	namespace Camera {
		extern MVec pos;
		extern MVec target;
		extern MMat view;
		void Reset();
		void Update();
		void Zoom( FLOAT f );
		void Goto( VECTOR p, VECTOR t, FLOAT time );
		void Stop();
	};

/* ---------------------------------------------------------------------------- */

	namespace Infos {
		void DrawBox( DWORD color, VECTOR *bmin, VECTOR *bmax, MATRIX *m );
		void DrawLine( DWORD color, VECTOR *a, VECTOR *b );
		void Render();
	};

/* ---------------------------------------------------------------------------- */

	namespace Shader {
		bool Add( WString name, const char *code_asm, DWORD *decl );
		bool Set( WString name );
		void Clean();
	};

/* ---------------------------------------------------------------------------- */

	namespace FX {

		struct FXParam {
			WString name;
			enum {
				PINT,
				PFLOAT,
				PMATRIX,
				PVECTOR,
				PBOOL,
				PMATREF,
				PVECREF
			} type;
			void *data;
		};

		bool LoadLib( WString name );
		void Names( WList<WString> *names );
		bool Play( WString name, WList<FXParam*> *params );
		void Render();
		void Close();

		class Gfx {
		public:
			TEXTUREOP fx_color;
			TEXTUREOP fx_alpha;
			BLEND fx_src;
			BLEND fx_dst;
			Gfx();
			void SetupDevice( bool alpha );
		};

		namespace Lerp {
			void VLerp( VECTOR *out, VECTOR *src, VECTOR *dst, FLOAT time);
			void V2Lerp( MVec *out,VECTOR *src, VECTOR *dst, FLOAT time);
			void CLerp( DWORD *out, DWORD *src, DWORD *dst, FLOAT time );
			void MLerp( MMat *out, MATRIX *src, MATRIX *dst, FLOAT time);
			
			void Cancel( void *out );
			void Process();
		};

	};

/* ---------------------------------------------------------------------------- */

	namespace Text {

		struct Font;

		class TData {
			int ref;
		public:
			TData( int ref );
			int Ref();
			void SetPos( VECTOR *v );
			void SetDest( FLOAT x, FLOAT y, FLOAT time );
			void SetString( const char *str );
			void SetColor( DWORD color );
			void SetCenter( bool c );
		};

		Font* CreateFont( int height, const char *face, bool bold );
		void FreeFont( Font *f );
		TData Create( Font *f );
		void Remove( int tref );		
		void Close();
		int Render();		
	};

/* ---------------------------------------------------------------------------- */

	namespace Events {

		enum EVENT {
			E_ENDANIM,
			E_CALLFUNC
		};

		void Init();
		void Close();

		void Send( EVENT e, void *obj );
		void Post( EVENT e, void *obj );
		void Delayed( FLOAT time, EVENT e, void *obj );
		void Flush();		
		void FlushUrgent();
		void Process();

		bool Get( EVENT *e, void **obj );
		void Next();
	};

/* ---------------------------------------------------------------------------- */

class Any {
protected:
	WString _name;
public:
	enum MTYPE {
		MFILE = 1,
		MMESH,
		MMATERIAL,
		MINST,
		MSPRITE,
		MFLAGS,
		MPATH
	};

	Any();
	Any( WString name );
	virtual ~Any();
	WString name();

	virtual bool export( WOStream *out );
	virtual bool import( WIStream *in );
	virtual MTYPE type() = 0;	
};

/* ---------------------------------------------------------------------------- */

class Material : public Any {
public:
	Material();
	COLORVALUE diffuse;
	COLORVALUE specular;
	FLOAT strength;
	FLOAT alpha;
	WString textname;
	bool export( WOStream *out );
	bool import( WIStream *in );
	MTYPE type();

	static TEXTURE LoadTexture( WIStream *data );
	static void FreeTexture( TEXTURE t);
};

/* ---------------------------------------------------------------------------- */

class Mesh;
class Path;

/* ---------------------------------------------------------------------------- */

class Inst : public Any, public FX::Gfx {
	struct Data;
	Data *data;
	int anim;
	MMat worldpos;
	MMat transpos;
	MMat position;
	VECTOR4 light;	
	FLOAT animspeed;
	FLOAT animtime;
public:
	bool visible;
	void *customdata;
	DWORD color;

	Inst( Mesh *mesh, bool hasparent = false );
	~Inst();
	void Detach();
	void ResetAnim();
	void AnimList( WList<const char*> *names );
	bool Intersect( VECTOR *start, VECTOR *end );
	bool SetAnim( const char *name );

	MATRIX *Position();
	void SetPosition( MATRIX *m );
	void SetDest( MATRIX *m, FLOAT time );
	void Invalidate();
	void Transform( MATRIX *m );
	void Fade( DWORD fcolor, FLOAT time );
	void SetTexture( TEXTURE t );

	float GetAnimTime();
	float GetAnimSpeed();
	void SetAnimTime( float t );
	void SetAnimSpeed( float s );
	
	bool Reset( bool free );	
	MTYPE type() { return MINST; }

	static int Render();
	static void Free( Inst *i );
	static void Pick( FLOAT x, FLOAT y, WList<Inst*> *found );

	friend Mesh;
};

/* ---------------------------------------------------------------------------- */

class Sprite : public Any, public FX::Gfx {	
	struct Data;
	Data *data;

	void Render( bool alpha, TEXTURE *last );

public:
	Sprite( TEXTURE tex );
	~Sprite();

	void SetCoords( FRECT *coords );
	void GetCoords( FRECT *coords );
	void SetDestination( VECTOR2 *v, FLOAT time );
	void SetFade( DWORD color, FLOAT time );
	void Detach();

	static void Free( Sprite *s );
	static int Render( bool alpha );
	static void Close();
	static bool Init();

	VECTOR  pos;
	VECTOR2 size;
	DWORD color;
	bool visible;
	
	void SetDepth( FLOAT z );
	FLOAT GetDepth();
	MTYPE type();
};
	
/* ---------------------------------------------------------------------------- */

class Mesh : public Any {
	struct Weight;
	struct Anim;
	struct Bone;
	struct Data;
	struct BoneFrame;

	WArray<VECTOR> verts;
	WArray<WORD> faces;
	WArray<WORD> tindex;
	WArray<VECTOR2> text;
	WArray<WORD> bones;
	WArray<MATRIX> refpos;
	WArray<Weight> weights;
	WList<Anim*> anims;	
	WList<Path*> paths;
	WList<WString> submeshes;
	Material *material;	
	Data *data;
	
	void UpdateBone( Bone *b, BoneFrame *prev, BoneFrame *next, FLOAT delta );
	void UpdateBox( BoneFrame *f, VECTOR *bmin, VECTOR *bmax );
	void InitBone( Bone *b );

public:
	Mesh();
	Mesh( WString name );
	~Mesh();
	MTYPE type();
	bool export( WOStream *out );
	bool import( WIStream *in );
	Inst *CreateInstance( bool hasparent = false );

	void *customdata;

	int NFaces();
	int NVerts();
	int NBones();
	bool HasMaterial();
	bool HasSkin();

	bool BeginGeometry( int nverts, int nfaces );
	bool BeginSkeleton( int nbones );
	bool BeginMotion( WString name );
	bool BeginMovement( WString name );
	bool BeginMapping( int nfaces );
	void CleanGeometry();

	void AddVertex( FLOAT x, FLOAT y, FLOAT z );
	void AddFace( DWORD *v );
	void AddFaceTV( DWORD *t );
	void AddTV( FLOAT x, FLOAT y );
	void AddBone( WORD parent, MATRIX init );
	void AddWeight( WORD vertex, WORD bone, FLOAT weight );
	void AddBoneFrame( int frame, WORD bone, VECTOR t, QUATERNION q );
	void AddMoveFrame( int frame, VECTOR t, QUATERNION q );
	void AddRef( WString name );
	void AddSub( Mesh *sub );
	void AddPath( Path *sub );
	void EndMapping();
	void EndGeometry();
	void EndSkeleton();
	void EndMotion();
	void EndMovement();
	
	void SetMaterial( Material *mat );

	void Update( VBUF buffer, bool vdefault );	
	int Render( Inst *inst, bool alpha );
	int AnimIndex( const char *name );
	const char *AnimName( int index );
	float GetAnimTime( Inst *inst );
	void SetAnimTime( Inst *inst, float t );
	bool Reset( bool free );
	void GetBBox( VECTOR *vbmin, VECTOR *vbmax );

	static Mesh *Load( WIStream *in );	
	static void Free( Mesh *ptr );
};

/* ---------------------------------------------------------------------------- */

class Flags : public Any {
	DWORD data;
public:

	enum FVal {
		FGROUP = 1,
	};

	Flags();
	~Flags();

	void Add( FVal flags );
	void Del( FVal flags );
	bool Has( FVal flags );
	
	MTYPE type();
	bool export( WOStream *out );
	bool import( WIStream *in );
};

/* ---------------------------------------------------------------------------- */

class Path : public Any {
	struct PathData;
	WList<PathData*> data;
	PathData *current;
public:
	Path();
	Path( WString name );
	~Path();
	MTYPE type();
	bool export( WOStream *out );
	bool import( WIStream *in );

	void Begin( WString name );
	void AddFrame( int frame, VECTOR t, QUATERNION q );
	void End();
};

/* ---------------------------------------------------------------------------- */

class File : protected Any {
	WString filename;
	WList<Any*> items;
public:

	File( WString filename );
	File( WIStream *in );
	~File();
	void AddItem( Any *item );
	void RemoveItem( Any *item );
	bool Export();
	bool Import();
	Any *Get( Any::MTYPE t );
	Any *Find( WString name );

protected:

	bool export( WOStream *out );
	bool import( WIStream *in );

	MTYPE type();

};

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */