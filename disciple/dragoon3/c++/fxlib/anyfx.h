#pragma once
#include <wstring.h>

#ifdef M3D_EXPORTS
#define FX_EXTERN __declspec(dllexport)
#else
#define FX_EXTERN __declspec(dllimport)
#endif

struct RTexture;
struct RMesh;

FX_EXTERN DEVICE get_device();
FX_EXTERN void free_fx( void *fx );
FX_EXTERN void print( const WString & );

FX_EXTERN RTexture *load_texture( const char *name );
FX_EXTERN void free_texture( RTexture *t );
FX_EXTERN void select_texture( RTexture *t );

FX_EXTERN RMesh *load_mesh( const char *name );
FX_EXTERN void free_mesh( RMesh *m );
FX_EXTERN void set_mesh_pos( RMesh *m, MATRIX *p );
FX_EXTERN void set_mesh_color( RMesh *m, DWORD color );
FX_EXTERN void set_mesh_alpha( RMesh *m, BLEND src, BLEND dst );

class AnyFX {
protected:
	FLOAT time;
	FLOAT total_time;
	bool killflag;
	virtual void Begin() = 0;
	virtual void Process() = 0;
	virtual void End() = 0;
public :
	
	AnyFX() : total_time(0), killflag(false) {
	}
	
	virtual ~AnyFX() {
		free_fx(this);
	}

	void Render( FLOAT elapsed ) {
		if( killflag ) {
			delete this;
			return;
		}
		time = elapsed;
		total_time += elapsed;
		Begin();
		Process();
		End();
	}

	void Kill() {
		killflag = true;
	}
};


class Mesh {
	RMesh *data;
public:
	
	Mesh( const char *name ) {
		data = load_mesh(name);
	}

	~Mesh() {
		free_mesh(data);
	}

	void SetPos( MATRIX *m ) {
		set_mesh_pos(data,m);
	}

	void SetColor( DWORD c ) {
		set_mesh_color(data,c);
	}

	void SetAlpha( BLEND src, BLEND dst ) {
		set_mesh_alpha(data,src,dst);
	}
};

class Params {
public:
	virtual bool Default() = 0;
	virtual VECTOR *VectorRef( const char * ) = 0;
	virtual MATRIX *MatrixRef( const char * ) = 0;
	virtual VECTOR Vector( const char * ) = 0;
	virtual MATRIX Matrix( const char * ) = 0;
	virtual FLOAT Float( const char * ) = 0;
	virtual bool Bool( const char * ) = 0;
	virtual int Int( const char * ) = 0;
};

typedef AnyFX *(*FX_NEW)( Params * );