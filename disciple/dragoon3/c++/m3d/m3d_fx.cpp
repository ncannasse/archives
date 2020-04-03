/* **************************************************************************** */
/*																				*/
/*   M3D Special Effects														*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>
#include <m3d_res.h>
#include "../fxlib/anyfx.h"

/* ---------------------------------------------------------------------------- */
// EXPORTS for FXLIB

struct RTexture {
	M3D::Res<TEXTURE> r;
	RTexture( M3D::Res<TEXTURE> r ) : r(r) {}
};

struct RMesh {
	M3D::Res<M3D::Inst*> r;
	RMesh( M3D::Res<M3D::Inst*> r ) : r(r) {}
};

FX_EXTERN RTexture *load_texture( const char *name ) {
	if( name == NULL )
		return NULL;
	FID id = Vfs::find_hlink(name);
	WString realfile;
	if( id == 0 || !Vfs::get_hlink(id,&realfile) )
		M3DTRACE("Texture not found : "+(WString)name);
	else
		return new RTexture( M3D::Res<TEXTURE>::Load(id,Vfs::ftype("PIC"),M3D::Material::LoadTexture,M3D::Material::FreeTexture) );
	return NULL;
}

FX_EXTERN void free_texture( RTexture *t ) {
	delete t;
}

FX_EXTERN void select_texture( RTexture *t ) {
	M3D::device->SetTexture(0,t?t->r.Data():NULL);
}

FX_EXTERN DEVICE get_device() {
	return M3D::device;
}

FX_EXTERN void print( const WString &s ) {
	M3D::Print(s);
}

FX_EXTERN RMesh *load_mesh( const char *name ) {
	if( name == NULL )
		return NULL;
	FID id = Vfs::find_hlink(name);
	WString realfile;
	if( id == 0 || !Vfs::get_hlink(id,&realfile) )
		M3DTRACE("Mesh not found : "+(WString)name);
	else {
		M3D::Res<M3D::Mesh*> r = M3D::Res<M3D::Mesh*>::Load(id,Vfs::ftype("MTW"),M3D::Mesh::Load,M3D::Mesh::Free);
		if( !r.IsLoaded() ) {
			M3DTRACE("Failed to load MTW file #"+itos(id));
			return NULL;
		}
		M3D::Inst *i = r.Data()->CreateInstance();	
		return new RMesh( M3D::Res<M3D::Inst*>::Share((int)i,i,M3D::Inst::Free) );
	}
	return NULL;
}

FX_EXTERN void free_mesh( RMesh *m ) {
	delete m;
}

FX_EXTERN void set_mesh_pos( RMesh *m, MATRIX *p ) {
	if( m )
		m->r.Data()->SetPosition(p);
}

FX_EXTERN void set_mesh_color( RMesh *m, DWORD color ) {
	if( m )
		m->r.Data()->color = color;
}

FX_EXTERN void set_mesh_alpha( RMesh *m, BLEND src, BLEND dst ) {
	if( m ) {
		m->r.Data()->fx_src = src;
		m->r.Data()->fx_dst = dst;
	}
}


namespace M3D {
/* ---------------------------------------------------------------------------- */

	extern MMat world;
	extern MMat matproj;

namespace FX {


/* ---------------------------------------------------------------------------- */
namespace Lerp {

	enum LType {
		LVECTOR,
		LMVECTOR,
		LCOLOR,
		LMATRIX
	};

	struct MATCALC {
		VECTOR t;
		QUATERNION q;
	};

	template<class T> struct LData {
		LType t;
		T *out;
		FLOAT stime;
		FLOAT etime;
		T src;
		T dst;
		LData( LType t, T *out, FLOAT time ) : t(t), out(out), stime(curtime), etime(curtime+SecTo3DTime(time)) {}
	};

	typedef LData<int>* _ldata;
	typedef LData<VECTOR>* lvec;
	typedef LData<DWORD>* lcolor;
	typedef LData<MATCALC>* lmat;

	WList<_ldata> lerps;


	void VLerp( VECTOR *out, VECTOR *src, VECTOR *dst, FLOAT time) {
		lvec d = new LData<VECTOR>(LVECTOR,out,time);
		d->src = *src;
		d->dst = *dst;
		lerps.Add((_ldata)d);
	}

	void V2Lerp( MVec *out, VECTOR *src, VECTOR *dst, FLOAT time) {
		lvec d = new LData<VECTOR>(LMVECTOR,(VECTOR*)out,time);
		d->src = *src;
		d->dst = *dst;
		lerps.Add((_ldata)d);
	}

	void CLerp( DWORD *out, DWORD *src, DWORD *dst, FLOAT time ) {
		lcolor d = new LData<DWORD>(LCOLOR,out,time);
		d->src = *src;
		d->dst = *dst;
		lerps.Add((_ldata)d);
	}

	void MLerp( MMat *out, MATRIX *src, MATRIX *dst, FLOAT time) {
		lmat d = new LData<MATCALC>(LMATRIX,(MATCALC*)out,time);
		D3DXQuaternionRotationMatrix(&d->src.q,src);
		D3DXQuaternionRotationMatrix(&d->dst.q,dst);
		d->src.t = VECTOR(src->_41,src->_42,src->_43);
		d->dst.t = VECTOR(dst->_41,dst->_42,dst->_43);
		lerps.Add((_ldata)d);
	}

	void Destroy( _ldata d ) {
		switch( d->t ) {
		case LVECTOR:
		case LMVECTOR: delete ((lvec)d); break;
		case LCOLOR: delete ((lcolor)d); break;
		case LMATRIX: delete ((lmat)d); break;
		}
	}

	void Cancel( void *out ) {
		FOREACH(_ldata,lerps);
			if( item->out == out ) {
				lerps.Remove(item);
				Destroy(item);
				Cancel(out);
				return;
			}
		ENDFOR;
	}

	void Close() {
		FOREACH(_ldata,lerps);
			Destroy(item);
		ENDFOR;
		lerps.Clean();
	}

	void Process() {
		FOREACH(_ldata,lerps);
			if( curtime < item->etime ) {
				FLOAT delta = (curtime-item->stime)/(item->etime-item->stime);
				switch( item->t ) {
				case LVECTOR:
					vlerp( ((lvec)item)->out, &((lvec)item)->src, &((lvec)item)->dst, delta);
					break;
				case LMVECTOR:
					{
						MVec *out = (MVec*)item->out;
						vlerp( out->Get(), &((lvec)item)->src, &((lvec)item)->dst, delta);
						out->Invalidate();
					}
					break;
				case LCOLOR:
					{	
						lcolor c = (lcolor)item;
						COLOR cdst;
						cdst.b = (c->dst&0xFF)/(0xFF*1.0);
						cdst.g = (c->dst&0xFF00)/(0xFF00*1.0);
						cdst.r = (c->dst&0xFF0000)/(0xFF0000*1.0);
						cdst.a = (c->dst&0xFF000000)/(0xFF000000*1.0);
						COLOR csrc;
						csrc.b = (c->src&0xFF)/(0xFF*1.0);
						csrc.g = (c->src&0xFF00)/(0xFF00*1.0);
						csrc.r = (c->src&0xFF0000)/(0xFF0000*1.0);
						csrc.a = (c->src&0xFF000000)/(0xFF000000*1.0);
						COLOR calc;
						clerp(&calc,&csrc,&cdst,delta);	
						*c->out = (((DWORD)(calc.a*0xFF))<<24) | (((DWORD)(calc.r*0xFF))<<16) | (((DWORD)(calc.g*0xFF))<<8) | (((DWORD)(calc.b*0xFF)));
					}
					break;
				case LMATRIX:
					{
						MATCALC c;
						vlerp(&c.t, &((lmat)item)->src.t, &((lmat)item)->dst.t, delta);
						qslerp(&c.q, &((lmat)item)->src.q, &((lmat)item)->dst.q, delta);
						MMat *out = (MMat*)item->out;
						D3DXMatrixAffineTransformation(	out->Get(), 1.0f, NULL, &c.q, &c.t );
						out->Invalidate();
					}
					break;
				}
			}
			else {
				switch( item->t ) {
				case LVECTOR: *((lvec)item)->out = ((lvec)item)->dst; break;
				case LMVECTOR: ((MVec*)item->out)->Set( ((lvec)item)->dst ); break;
				case LCOLOR: *((lcolor)item)->out = ((lcolor)item)->dst; break;
				case LMATRIX:
					{
						MMat *out = (MMat*)item->out;
						D3DXMatrixAffineTransformation(	out->Get(), 1.0f, NULL, &((lmat)item)->dst.q, &((lmat)item)->dst.t );
						out->Invalidate();
					}
					break;
				}
				lerps.Remove(item);
				Destroy(item);
			}
		ENDFOR;
	}
};

/* ---------------------------------------------------------------------------- */
// PARAMS

class FXParams : public Params {
	WList<FXParam*> params;	

	FXParam *Get( const char *name ) {
		FOREACH(FXParam*,params);
			if( item->name == name )
				return item;
		ENDFOR;
		return NULL;
	}

public:
	FXParams( const WList<FXParam*> &params ) : params(params) {
	}

	bool Default() {
		return Get("_default") != NULL;
	}

	VECTOR *VectorRef( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PVECREF )
			return NULL;
		return (VECTOR*)p->data;
	}

	MATRIX *MatrixRef( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PMATREF )
			return NULL;
		return (MATRIX*)p->data;
	}

	VECTOR Vector( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PVECTOR )
			return VECTOR(0,0,0);
		return *(VECTOR*)p->data;
	}

	MATRIX Matrix( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PMATRIX )
			return IDMATRIX;
		return *(MATRIX*)p->data;
	}

	FLOAT Float( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PFLOAT )
			return 0;
		return *(FLOAT*)&p->data;
	}

	bool Bool( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PBOOL )
			return false;
		return *(bool*)&p->data;
	}

	int Int( const char *name ) {
		FXParam *p = Get(name);
		if( !p || p->type != FXParam::PINT )
			return 0;
		return *(int*)&p->data;
	}
};


/* ---------------------------------------------------------------------------- */
// FX Load & Create

	typedef AnyFX *(*FCREATE)( int, Params * );
	typedef int (*FGETNUMFX)();
	typedef const char* (*FGETFXNAME)( int );

	struct FXId {
		int n;
		WString name;
		FXId( int n, WString name ) : n(n), name(name) {}
	};

	struct Lib {
		WString name;
		HMODULE handle;
		FCREATE getfx;
		WList<FXId*> effects;

		~Lib() {
			FreeLibrary(handle);
			effects.Delete();
		}
	};

	WList<Lib*> loaded_libs;
	WList<AnyFX*> effects;

	Lib *GetLib( const char *name ) {
		FOREACH(Lib*,loaded_libs)
			if( strcmp(item->name.c_str(),name) )
				return item;
		ENDFOR;
		return NULL;
	}

	bool LoadLib( WString name ) {
		Lib *old = GetLib(name.c_str());
		if( old )
			loaded_libs.Delete(old);

		HMODULE h = LoadLibrary(name.c_str());
		if( h == NULL ) {
			SetLastError(0);
			return false;
		}

		FGETNUMFX fnum = (FGETNUMFX)GetProcAddress(h,"get_num_fx");
		FGETFXNAME fname = (FGETFXNAME)GetProcAddress(h,"get_fx_name");
		FCREATE fcreate = (FCREATE)GetProcAddress(h,"get_new_fx");

		if( !fnum || !fname || !fcreate ) {			
			FreeLibrary(h);
			SetLastError(0);
			return false;
		}
		
		Lib *l = new Lib();
		l->handle = h;
		l->getfx = fcreate;
		l->name = name;
		
		int i;
		int numfx = fnum();
		for(i=0;i<numfx;i++)
			l->effects.Add( new FXId(i,fname(i)) );

		loaded_libs.Add(l);
		SetLastError(0);
		return true;
	}
	
	void Names( WList<WString> *names ) {
		FOREACH(Lib*,loaded_libs);
			Lib *l = item;
			FOREACH(FXId*,l->effects);
				names->Add( item->name );
			ENDFOR;
		ENDFOR;
	}


	bool Play( WString name, WList<FXParam*> *params ) {
		FXParams p(*params);
		FOREACH(Lib*,loaded_libs)
			Lib *l = item;
			FOREACH(FXId*,l->effects);
				if( item->name == name ) {
					AnyFX *fx = l->getfx(item->n,&p);
					effects.Add(fx);
					return true;
				}
			ENDFOR;
		ENDFOR;
		return false;
	}

	void Render() {
		if( world.IsModified() ) {
			device->SetTransform(D3DTS_VIEW,Camera::view.Get());
			device->SetTransform(D3DTS_PROJECTION,matproj.Get());
			device->SetTransform(D3DTS_WORLD,transform.Get());
		}		

		FOREACH(AnyFX*,effects)
			item->Render(elapsed_time);
		ENDFOR;
	}

	void Close() {
		effects.Delete();
		loaded_libs.Delete();
		Lerp::Close();
	}

/* ---------------------------------------------------------------------------- */

	Gfx::Gfx() : fx_src(D3DBLEND_SRCALPHA), fx_dst(D3DBLEND_ONE), fx_color(D3DTOP_MODULATE), fx_alpha(D3DTOP_MODULATE) { }

	void Gfx::SetupDevice( bool has_alpha ) {
		if( has_alpha ) {
			device->SetTextureStageState(0,D3DTSS_ALPHAOP,fx_alpha);
			device->SetRenderState(D3DRS_SRCBLEND,fx_src);
			device->SetRenderState(D3DRS_DESTBLEND,fx_dst);
		}		
		device->SetTextureStageState(0,D3DTSS_COLOROP,fx_color);
	}
/* **************************************************************************** */

}; // namespace FX

}; // namespace M3D


FX_EXTERN void free_fx( void *fx ) {
	M3D::FX::effects.Remove((AnyFX*)fx);
}


/* **************************************************************************** */
