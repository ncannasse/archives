/* ************************************************************************ */
/*																			*/
/*	M3D-SE Standard Library													*/
/*	Nicolas Cannasse														*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include "m3d.h"
#include "m3d_context.h"
#include "m3d_sdk.h"

namespace M3D {

VECTOR defpos(-40,-60,70);
VECTOR deftarget(0,0,20);
VECTOR defworldup(0,0,1);
const VECTOR4 zero (0,0,0,1);
const VECTOR4 one (1,1,1,1);

struct LibContext {
	Variable<MATRIX> camera;
	Variable<MATRIX> proj;
	Variable<MATRIX> world;
	Variable<VECTOR> light;
	Variable<DWORD> light_color;
	Variable<DWORD> ambient_color;

	MATRIX tmp;
	VECTOR4 light_stat_dir;
	COLOR ambient_stat_color, light_stat_color;
	MATRIX camera_transposed;
};

/* ------------------------------------------------------------------------ */

class OneSC : public ShaderConst {
public:
	SHADERCONST(one,1);
	OneSC( Mesh *o ) { }
	void *Data( Context *c ) { return (void*)&one; }
};

/* ------------------------------------------------------------------------ */

class ZeroSC : public ShaderConst {
public:
	SHADERCONST(zero,1);
	ZeroSC( Mesh *o ) { }
	void *Data( Context *c ) { return (void*)&zero; }
};

/* ------------------------------------------------------------------------ */

class AmbientColorSC : public ShaderConst {
public:
	SHADERCONST(ambient_color,1);

	AmbientColorSC( Mesh *o ) {
	}

	void *Data( Context *c ) {
		if( !c->lib->ambient_color.IsValid() ) {
			c->lib->ambient_color.Validate();
			c->lib->ambient_stat_color = COLOR(c->lib->ambient_color.Get());
		}
		return &c->lib->ambient_stat_color;
	}
};

/* ------------------------------------------------------------------------ */

class CameraSC : public ShaderConst {
public:
	SHADERCONST(camera,4);

	CameraSC( Mesh *o ) {
	}
	
	void *Data( Context *c ) {
		LibContext *cl = c->lib;
		if( !cl->camera.IsValid() || !cl->proj.IsValid() || !cl->world.IsValid() ) {
			cl->camera.Validate();
			cl->proj.Validate();
			cl->world.Validate();
			mmult(&cl->tmp,cl->camera.Ref(),cl->proj.Ref());
			mmult(&cl->tmp,cl->world.Ref(),&cl->tmp);
			mtranspose(&cl->camera_transposed,&cl->tmp);
		}
		return &cl->camera_transposed;
	}
};

/* ------------------------------------------------------------------------ */

class LightSC : public ShaderConst {
public:
	SHADERCONST(light,1);

	LightSC( Mesh *o ) {
	}

	void *Data( Context *c ) {
		LibContext *cl = c->lib;
		if( !cl->light.IsValid() ) {
			vnormalize((VECTOR*)&cl->light_stat_dir,cl->light.Ref());
			cl->light_stat_dir.w = 0;
			cl->light.Validate();
		}
		return &cl->light_stat_dir;
	}
};

/* ------------------------------------------------------------------------ */

class LightColorSC : public ShaderConst {
public:
	SHADERCONST(light_color,1);

	LightColorSC( Mesh *o ) {
	}

	void *Data( Context *c ) {
		LibContext *cl = c->lib;
		if( !cl->light_color.IsValid() ) {
			cl->light_color.Validate();
			cl->light_stat_color = COLOR(cl->light_color.Get());
		}
		return &cl->light_stat_color;
	}
};

/* ------------------------------------------------------------------------ */

class ObjectPosSC : public ShaderConst {
	Mesh *o;
	MATRIX transpos;
public:
	SHADERCONST(pos,4);

	ObjectPosSC( Mesh *o ) : o(o) {
		if( o )
			o->PosModified();
	}

	void *Data( Context *c ) {
		if( !o->abs_pos.IsValid() ) {
			o->UpdatePos();
			mtranspose(&transpos,o->abs_pos.Ref());
		}
		return &transpos;
	}
};

/* ------------------------------------------------------------------------ */

class ColorSC : public ShaderConst {
	Mesh *o;
public:
	SHADERCONST(color,1);

	ColorSC( Mesh *o ) : o(o) {
	}

	void *Data( Context *c ) {
		if( !o->abs_color.IsValid() ) 
			o->UpdateColor();
		return o->abs_color.Ref();
	}
};

/* ------------------------------------------------------------------------ */

class CalcNormals : public Renderer {

public:
	CalcNormals( Mesh *o ) {
	}

	~CalcNormals() { 
	}
	
	const char *Name() {
		return "calc_normals";
	}

	bool Validate( Pipeline *p ) {
		return p->Read(MAKETAG(VECT)) && p->Read(MAKETAG(INDX)) && p->Write(MAKETAG(NORM));
	}

	bool Render( StreamPipeline *s ) {
		VECTOR *vect = (VECTOR*)s->Read(MAKETAG(VECT));
		WORD *indx = (WORD*)s->Read(MAKETAG(INDX));
		WriteStream<VECTOR> norm(s,MAKETAG(NORM));

		int nverts = s->NVerts();
		int nfaces = s->NFaces();
		VECTOR *tmp_norm = new VECTOR[nverts];
		int i;
		for(i=0;i<nverts;i++)
			tmp_norm[i] = VECTOR(0,0,0);

		for(i=0;i<nfaces;i++) {
			WORD a = indx[i*3];
			WORD b = indx[i*3+1];
			WORD c = indx[i*3+2];
			VECTOR v1 = vect[a];
			VECTOR v2 = vect[b];
			VECTOR v3 = vect[c];
			VECTOR n,area;
			vnormalize(&n, vcross(&n, &(v2-v1), &(v3-v2) ) );
			vcross(&area,&(v2-v1), &(v3-v1) );
			n *= vlength(&area);
			tmp_norm[a] += n;
			tmp_norm[b] += n;
			tmp_norm[c] += n;
		}
		
		for(i=0;i<nverts;i++)
			vnormalize(&norm[i],&tmp_norm[i]);
		delete tmp_norm;
		return true;
	}
};

/* ------------------------------------------------------------------------ */

class AnimSkin : public Renderer {

	struct BONEPOS {
		VECTOR t;
		QUATERNION q;
	};

#pragma pack(1)
	struct SKEL {
		WORD vidx;
		WORD bidx;
		FLOAT weight;
	};
#pragma pack()

	struct BoneList {
		WORD bidx;
		FLOAT weight;
		BoneList *next;
	};

	SKEL *last_skel;
	BONEPOS *last_rpos;
	int nbones;
	int nverts;
	BoneList** indexes;
	MATRIX *mat;
	MATRIX *matref;

public:

	AnimSkin( Mesh *o ) {
		nverts = 0;
		nbones = 0;
		last_skel = NULL;
		last_rpos = NULL;
		indexes = NULL;
		mat = NULL;
		matref = NULL;
	}

	~AnimSkin() {
		Clean();
	}

	void Clean() {
		int i;
		for(i=0;i<nverts;i++) {
			BoneList *l = indexes[i];
			while( l != NULL ) {
				BoneList *next = l->next;
				delete l;
				l = next;
			}
		}
		delete indexes;
		delete mat;
		delete matref;
	}

	const char *Name() {
		return "anim_skin";
	}

	bool Validate( Pipeline *p ) {
		return p->Read(MAKETAG(RPOS)) && p->Read(MAKETAG(SKEL)) && p->Read(MAKETAG(RIDX)) && p->Read(MAKETAG(VECT)) && p->Write(MAKETAG(VECT));
	}

	bool Render( StreamPipeline *s ) {
		BONEPOS *fdata1;
		BONEPOS *fdata2;
		FLOAT time;
		if( !s->TimeCoef(MAKETAG(sKEL),(void**)&fdata1,(void**)&fdata2,&time) )
			return true;

		// *TODO* check size of fdata1&2
		int nverts = s->NVerts();
		int skel_size;
		int rpos_size;
		SKEL *skel = (SKEL*)s->Read(MAKETAG(SKEL),&skel_size);
		BONEPOS *rpos = (BONEPOS*)s->Read(MAKETAG(RPOS),&rpos_size);
		if( skel != last_skel || rpos != last_rpos || nverts != this->nverts ) {
			int nskel_elts = skel_size / sizeof(SKEL);
			WORD *ridx = (WORD*)s->Read(MAKETAG(RIDX));

			// --- update skeleton ---
			Clean();
			this->nverts = nverts;
			last_skel = skel;
			last_rpos = rpos;
			nbones = -1;
			indexes = new BoneList*[nverts];
			int i,j;
			for(i=0;i<nverts;i++)
				indexes[i] = NULL;

			// construct weight tables with index remapping
			for(i=0;i<nskel_elts;i++) {
				SKEL *s = &skel[i];
				int vidx = s->vidx;
				for(j=0;j<nverts;j++)
					if( ridx[j] == vidx ) {
						BoneList *b = new BoneList;
						b->bidx = s->bidx;
						b->weight = s->weight;
						b->next = indexes[j];
						indexes[j] = b;
					}
				if( s->bidx > nbones ) 
					nbones = s->bidx;
			}
			nbones++;
			mat = new MATRIX[nbones];
			matref = new MATRIX[nbones];
			
			if( rpos_size != nbones * sizeof(BONEPOS) )
				return false;

			for(i=0;i<nbones;i++) {
				MATRIX *bmat = &matref[i];
				BONEPOS *p = &rpos[i];
				D3DXMatrixRotationQuaternion(bmat,&p->q);
				bmat->_41 = p->t.x;
				bmat->_42 = p->t.y;
				bmat->_43 = p->t.z;
				minv(bmat,bmat);				
			}
		}

		// ---- calc skeleton ---
		VECTOR t;
		QUATERNION q;
		int i;
		for(i=0;i<nbones;i++) {
			MATRIX *bmat = &mat[i];
			vlerp(&t,&fdata1[i].t,&fdata2[i].t,time);
			qslerp(&q,&fdata1[i].q,&fdata2[i].q,time);
			D3DXMatrixRotationQuaternion(bmat,&q);
			bmat->_41 = t.x;
			bmat->_42 = t.y;
			bmat->_43 = t.z;
			mmult(bmat,&matref[i],bmat);			
		}

		// ---- apply skin ---
		VECTOR *vects = (VECTOR*)s->Read(MAKETAG(VECT));
		WriteStream<VECTOR> out_vects(s,MAKETAG(VECT));

		VECTOR tmp;
		VECTOR4 v_origin;
		VECTOR4 v_transform;
		v_origin.w = 1;		
		for(i=0;i<nverts;i++) {
			tmp.x = 0;
			tmp.y = 0;
			tmp.z = 0;
			v_origin.x = vects[i].x;
			v_origin.y = vects[i].y;
			v_origin.z = vects[i].z;			
			BoneList *l = indexes[i];
			while( l != NULL ) {
				vtransform(&v_transform,&v_origin,&mat[l->bidx]);
				D3DXVec3Scale((VECTOR*)&v_transform,(VECTOR*)&v_transform,l->weight);
				D3DXVec3Add(&tmp,&tmp,(VECTOR*)&v_transform);
				l = l->next;
			}
			out_vects.Set(tmp);
			out_vects.Next();
		}

		return true;
	}

};

/* ------------------------------------------------------------------------ */

MATRIX *GetMatrixProj( Context *c ) {
	return c->lib->proj.Ref();
}

/* ------------------------------------------------------------------------ */

void SetMatrixProj( Context *c, MATRIX *m ) {
	c->lib->proj.Set(*m);
}

/* ------------------------------------------------------------------------ */

void SetCamera( Context *c, MATRIX *m ) {
	c->lib->camera.Set(*m);
}

/* ------------------------------------------------------------------------ */

void SetWorld( Context *c, MATRIX *m ) {
	c->lib->world.Set(*m);
}

MATRIX *GetWorld( Context *c ) {
	return c->lib->world.Ref();
}

/* ------------------------------------------------------------------------ */

void SetLightDir( Context *c, VECTOR *m ) {
	c->lib->light.Set(*m);
}

VECTOR *GetLightDir( Context *c ) {
	return c->lib->light.Ref();
}

/* ------------------------------------------------------------------------ */

void SetLightColor( Context *c, DWORD m ) {
	m |= 0xff000000;
	c->lib->light_color.Set(m);
}

DWORD GetLightColor( Context *c ) {
	DWORD res = *(c->lib->light_color.Ref());
	return res & 0x00ffffff;
}

/* ------------------------------------------------------------------------ */

void SetAmbient( Context *c, DWORD m ) {
	m |= 0xff000000;
	c->lib->ambient_color.Set(m);
}

DWORD GetAmbient( Context *c ) {
	DWORD res = *(c->lib->ambient_color.Ref());
	return res & 0x00ffffff;
}

/* ------------------------------------------------------------------------ */

template<class T> class RegisterSC {
	static ShaderConst *Create( Mesh *o ) {
		return new T(o);
	}
public:
	RegisterSC() { 
		ShaderConst::Register( M3D::__GetContext(), Create );
	}
};

template<class T> class RegisterRnd {
	static Renderer *Create( Mesh *o ) {
		return new T(o);
	}
public:
	RegisterRnd() {
		Renderer::Register( M3D::__GetContext(), Create );
	}
};
 
void InitLib( Context *c ) {
	c->lib = new LibContext;
	c->lib->light.Set(VECTOR(0,-1,1));
	c->lib->light_color.Set(0xFF909090);
	c->lib->ambient_color.Set(0xFF707070);

	TagFormat::RegisterFormat(c,MAKETAG(VECT),TF_FLOAT3);
	TagFormat::RegisterFormat(c,MAKETAG(MAP0),TF_FLOAT2);
	TagFormat::RegisterFormat(c,MAKETAG(NORM),TF_FLOAT3);
	TagFormat::RegisterStride(c,MAKETAG(INDX),2);
	RegisterSC<AmbientColorSC>();
	RegisterSC<CameraSC>();
	RegisterSC<ObjectPosSC>();
	RegisterSC<ColorSC>();
	RegisterSC<LightSC>();
	RegisterSC<ZeroSC>();
	RegisterSC<OneSC>();
	RegisterSC<LightColorSC>();
	RegisterRnd<CalcNormals>();
	RegisterRnd<AnimSkin>();
	
	FLOAT ratio = c->width * 1.0f / c->height;
	D3DXMatrixIdentity(c->lib->world.Ref());
	D3DXMatrixLookAtLH(c->lib->camera.Ref(),&defpos,&deftarget,&defworldup);
	D3DXMatrixPerspectiveFovLH(c->lib->proj.Ref(),D3DX_PI / 4, ratio, 1.0f, 10000.0f);
	c->lib->camera.Invalidate();
	c->lib->proj.Invalidate();
}

void CloseLib( Context *c ) {
	delete c->lib;
}

};

/* ************************************************************************ */
