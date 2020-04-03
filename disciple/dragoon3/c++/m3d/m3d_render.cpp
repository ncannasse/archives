/* **************************************************************************** */
/*																				*/
/*   M3D Render & Buffers functions												*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>
#include <m3d_struct.h>
#include <vfs.h>

struct TCOORDS {
	FLOAT tu,tv;
};

struct VERTEX {
	VECTOR pos;
	VECTOR n;
};

struct SVERTEX {
	VECTOR pos;
	DWORD color;
	FLOAT tu,tv;
	SVERTEX() { }
	SVERTEX( VECTOR pos, VECTOR2 tvs ) : pos(pos), color(0), tu(tvs.x), tv(tvs.y) {}

	bool operator == ( const SVERTEX &v ) {
		return v.pos == pos && v.color == color && v.tu == tu && v.tv == tv;
	}
};

#define TCOORDS_FORMAT	D3DFVF_TEX1
#define VERTEX_FORMAT	(D3DFVF_XYZ | D3DFVF_NORMAL)
#define SVERTEX_FORMAT	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#define ANIM_FREQ 4800

namespace M3D {
/* ---------------------------------------------------------------------------- */

extern WORD fps;

DWORD std_shader_decl[] =
{
    D3DVSD_STREAM( 0 ),				
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),	// Position
    D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),	// Normal
	D3DVSD_STREAM( 1 ),					
    D3DVSD_REG( 6, D3DVSDT_FLOAT2 ),	// Tex coords
    D3DVSD_END()
};

DWORD no_coords_decl[] =
{
    D3DVSD_STREAM( 0 ),	
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),	// Position
    D3DVSD_REG( 3, D3DVSDT_FLOAT3 ),	// Normal
    D3DVSD_END()
};

DWORD static_shader_decl[] =
{
	D3DVSD_STREAM( 0 ),
    D3DVSD_REG( 0, D3DVSDT_FLOAT3 ),	// Position
	D3DVSD_REG( 3, D3DVSDT_UBYTE4 ),	// Color
    D3DVSD_REG( 6, D3DVSDT_FLOAT2 ),	// Tex coords	
};

// v0 = pos
// v3 = normal
// v6 = t_coords
// 
// c0-c3 = mat world
// c4 = light dir
// c5 = light diffuse
// c6 = ambient
// c7 = texture vector

const char *std_shader_asm = "\n\
	vs.1.0\n\
	m4x4 oPos, v0, c0\n\
	dp3 r0, v3, c4\n\
	max r0, r0, c6.w\n\
	mul r1, r0, c5\n\
	add oD0, r1, c6\n\
	mov oT0, v6";

const char *no_coords_asm = "\n\
	vs.1.0\n\
	m4x4 oPos, v0, c0\n\
	dp3 r0, v3, c4\n\
	mul r1, r0, c5\n\
	add oD0, r1, c6";

const char *no_shading_asm = "\n\
	vs.1.0\n\
	m4x4 oPos, v0, c0\n\
	mov oD0, c5\n\
	mov oT0, v6";

const char *colored_shader_asm = "\n\
	vs.1.0\n\
	m4x4 oPos, v0, c0\n\
	mov oD0, c4\n\
	mov oT0, v6";

const char *colored_no_coords_asm = "\n\
	vs.1.0\n\
	m4x4 oPos, v0, c0\n\
	mov oD0, c4";

const char *static_shader_asm = "\n\
	vs.1.0\n\
	m4x4 oPos, v0, c0\n\
	mov oD0, v3\n\
	mov oT0, v6";

/* ---------------------------------------------------------------------------- */

bool Shaders_Init() {
	return
		Shader::Add("STD",std_shader_asm,std_shader_decl) &&
		Shader::Add("NOCOORDS",no_coords_asm,no_coords_decl) &&
		Shader::Add("NOSHADE",no_shading_asm,std_shader_decl) &&
		Shader::Add("COLORED",colored_shader_asm,std_shader_decl) &&
		Shader::Add("COLORED-NOCOORDS",colored_no_coords_asm,no_coords_decl)
	;
}

/* ---------------------------------------------------------------------------- */

extern FLOAT realtime;

int DoRender()
{
	int t = 0;	
	VECTOR4 lightdif(3.0f,3.0f,3.0f,1);
	VECTOR4 ambient(0.5f,0.5f,0.5f,0);
	BOOL tismod = transform.IsModified();

#ifdef USE_R_THREAD
	Camera::Update();
#endif
	if( Camera::view.IsModified() || tismod || matproj.IsModified() ) {
		Camera::view.ClearModified();
		transform.ClearModified();
		matproj.ClearModified();
		if( tismod )
			mtranspose(&ttransform,transform.Get());
		if( tismod || dirlight.IsModified() ) {
			vtransform(calclight.Get(),dirlight.Get(),&ttransform);	
			calclight.Invalidate();
		}
		MATRIX mat;
		mmult( &mat, Camera::view.Get(), matproj.Get() );
		mtranspose( &mat, &mat );	
		mmult( world.Get(), &mat, &ttransform );
		world.Invalidate();
	}
	
	device->SetVertexShaderConstant( 0, world.Get(), 4 );
	device->SetVertexShaderConstant( 5, &lightdif, 1 );
	device->SetVertexShaderConstant( 6, &ambient, 1 );

	FX::Lerp::Process();


	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
    device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
	Shader::Set(do_shading?"STD":"NOSHADE");
	t += Inst::Render();
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	t += Sprite::Render(false);
	t += Text::Render();
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	FX::Render();
	t += Sprite::Render(true);
	Infos::Render();

	dirlight.ClearModified();
	calclight.ClearModified();
	world.ClearModified();
	return t;
}

/* ---------------------------------------------------------------------------- */

extern WList<Inst*> instances;

int
Inst::Render() {
	int tcount = 0;

	FOREACH(Inst*,instances);
		if( item->visible )
			tcount += item->data->parent.Data()->Render( item, false );
	ENDFOR;

	device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);	

	FOREACH(Inst*,instances);
		if( item->visible )
			tcount += item->data->parent.Data()->Render( item, true );
	ENDFOR;

	return tcount;
}

/* ---------------------------------------------------------------------------- */

bool
Mesh::Reset( bool free ) {
	FOREACH(Res<Mesh*>,data->subs)
		item.Data()->Reset(free);
	ENDFOR;
	if( free ) {
		M3DRELEASE(data->tcoords);
		return true;
	}
	else {
		int i;
		TCOORDS *tptr;

		if( text.Length() && !M3DCHECK(device->CreateVertexBuffer(sizeof(TCOORDS)*NFaces()*3,D3DUSAGE_WRITEONLY,TCOORDS_FORMAT,D3DPOOL_MANAGED,&data->tcoords)) )
			return false;

		if( text.Length() ) {
			data->tcoords->Lock(0,0,(BYTE**)&tptr,0);
			for(i=0;i<tindex.Length();i++) {
				tptr->tu = text[tindex[i]].x;
				tptr->tv = 1.0f-text[tindex[i]].y;
				tptr++;
			}
			data->tcoords->Unlock();
		}

		if( material ) {
			FID id = Vfs::find_hlink(material->textname);
			WString realfile;
			if( id == 0 || !Vfs::get_hlink(id,&realfile) )
				M3DTRACE("Texture not found : "+material->textname);				
			else
				data->texture = Res<TEXTURE>::Load(id,Vfs::ftype("PIC"),Material::LoadTexture,Material::FreeTexture);
		}

		return true;
	}
}

/* ---------------------------------------------------------------------------- */

bool
Inst::Reset( bool free ) {
	if( !data )
		return true;
	FOREACH(Res<Inst*>,data->subs);
		item.Data()->Reset(free);
	ENDFOR;	

	if( free ) {
		M3DRELEASE(data->buffer);
		return true;
	} else {		
		int nfaces = data->parent.Data()->NFaces();
		if( !nfaces )
			return true;
		bool dynamic = data->parent.Data()->HasSkin();
		if( !M3DCHECK(device->CreateVertexBuffer(sizeof(VERTEX)*nfaces*3,(dynamic?D3DUSAGE_DYNAMIC:0) | D3DUSAGE_WRITEONLY, VERTEX_FORMAT, D3DPOOL_DEFAULT, &data->buffer)) )
			return false;
		data->parent.Data()->Update(data->buffer,anim==-1);
		return true;
	}
}

/* ---------------------------------------------------------------------------- */

void
Inst::ResetAnim() {
	if( !data )
		return;
	anim = -1;
	data->parent.Data()->Update(data->buffer,true);
	data->parent.Data()->GetBBox((VECTOR*)&data->bmin,(VECTOR*)&data->bmax);
	FOREACH(Res<Inst*>,data->subs);
		item.Data()->ResetAnim();
	ENDFOR;
}

/* ---------------------------------------------------------------------------- */

bool
Inst::SetAnim( const char *name ) {
	if( !data )
		return true;
	if( name == NULL ) {
		animtime = curtime;
		anim = -1;
		FOREACH(Res<Inst*>,data->subs);
			if( !item.Data()->SetAnim(name) )
				return false;
		ENDFOR;
		return true;
	}
	int animfound = data->parent.Data()->AnimIndex(name);
	if( animfound == -1 )
		return false;
	FOREACH(Res<Inst*>,data->subs);
		if( !item.Data()->SetAnim(name) )
			return false;
	ENDFOR;
	animtime = curtime;
	anim = animfound;
	data->invalid_box = true;
	return true;
}

/* ---------------------------------------------------------------------------- */

float
Mesh::GetAnimTime( Inst *inst ) {
	if( inst->anim == -1 )
		return 0;
	Anim *a = anims.AtIndex(inst->anim);
	int framecount;
	switch( a->type ) {
	case AMOTION:
		framecount = a->bframes->Last()->frame+1;
		break;
	case AMOVE:
		framecount = a->mframes->Last()->frame+1;
		break;
	}
	FLOAT time = (curtime-inst->animtime)*inst->animspeed*ANIM_FREQ;
	time -= (((int)time)/framecount)*framecount;
	time /= framecount;
	return time;
}

/* ---------------------------------------------------------------------------- */

void
Mesh::SetAnimTime( Inst *inst, float t ) {
	if( inst->anim == -1 )
		return;
	Anim *a = anims.AtIndex(inst->anim);
	int framecount;
	switch( a->type ) {
	case AMOTION:
		framecount = a->bframes->Last()->frame+1;
		break;		
	case AMOVE:
		framecount = a->mframes->Last()->frame+1;
		break;
	}	
	inst->animtime = curtime-((t*framecount)/(inst->animspeed*ANIM_FREQ));
}

/* ---------------------------------------------------------------------------- */

int
Mesh::Render( Inst *inst, bool alpha ) {

	if( inst->color <= 0x00FFFFFF || (alpha && inst->color >= 0xFF000000) || (!alpha && inst->color < 0xFF000000) )
		return 0;

	int tcount = NFaces();
	MATRIX mtmp;
	MATRIX *rel_pos = NULL;

	if( inst->anim != -1 ) {		
		Anim *a = anims.AtIndex(inst->anim);
		if( inst->data->invalid_box ) {
			inst->data->bmin = VECTOR4(9999999,9999999,9999999,1);
			inst->data->bmax = VECTOR4(-9999999,-9999999,-9999999,1);
		}

		switch( a->type ) {
		case AMOTION: {

			int framecount = a->bframes->Last()->frame+1;
			FLOAT time = (curtime-inst->animtime)*inst->animspeed*ANIM_FREQ;
			time -= (((int)time)/framecount)*framecount;	
			int frame = (int)time;
			BoneFrame *fprev = NULL;
			BoneFrame *fnext = NULL;
			FOREACH(BoneFrame*,*a->bframes);
				if( item->frame >= frame ) {
					fnext = item;
					break;
				}
				fprev = item;
			ENDFOR;
			if( fnext == NULL )
				fnext = fprev;
			if( fprev == NULL )
				fprev = fnext;
			FLOAT delta = (fprev == fnext)?0:(time-fprev->frame)/(fnext->frame-fprev->frame);			

			UpdateBone(data->skeleton, fprev, fnext, delta );
			Update(inst->data->buffer,false);
			if( inst->data->invalid_box ) {
				UpdateBox(fprev,(VECTOR*)&inst->data->bmin,(VECTOR*)&inst->data->bmax);
				inst->data->invalid_box = false;
			}
			};
			break;
		case AMOVE: {
			int framecount = a->mframes->Last()->frame+1;
			FLOAT time = (curtime-inst->animtime)*inst->animspeed*ANIM_FREQ;
			time -= (((int)time)/framecount)*framecount;	
			int frame = (int)time;
			MoveFrame *fprev = NULL;
			MoveFrame *fnext = NULL;
			FOREACH(MoveFrame*,*a->mframes);
				if( item->frame >= frame ) {
					fnext = item;
					break;
				}
				fprev = item;
			ENDFOR;
			if( fnext == NULL )
				fnext = fprev;
			if( fprev == NULL )
				fprev = fnext;			
			FLOAT delta = (fprev == fnext)?0:(time-fprev->frame)/(fnext->frame-fprev->frame);			
			QUATERNION q;
			VECTOR t;
			vlerp(&t,&fprev->t,&fnext->t,delta);
			qslerp(&q,&fprev->q,&fnext->q,delta);
			D3DXMatrixAffineTransformation(&mtmp,1.0f,NULL,&q,&t);
			mtranspose(&mtmp,&mtmp);
			rel_pos = &mtmp;
			};
			break;
		}
	}

	inst->SetupDevice(alpha);

	if( rel_pos || world.IsModified() || inst->position.IsModified() ) {
		if( inst->position.IsModified() ) {
			inst->position.ClearModified();
			mtranspose(inst->transpos.Get(),inst->position.Get());
		}
		mmult(inst->worldpos.Get(),world.Get(),inst->transpos.Get());
		if( rel_pos )
			mmult(inst->worldpos.Get(),inst->worldpos.Get(),rel_pos);
		vtransform(&inst->light,calclight.Get(),inst->transpos.Get());
		vnormalize((VECTOR*)&inst->light,(VECTOR*)&inst->light);
	}	
	device->SetVertexShaderConstant(0,inst->worldpos.Get(),4);
	
	if( NFaces() > 0 ) {
		device->SetStreamSource(0,inst->data->buffer,sizeof(VERTEX));
		if( inst->color == (DWORD)-1 ) {
			device->SetVertexShaderConstant(4,&inst->light,1);
			if( data->tcoords ) {
				if( inst->data->texture.IsLoaded() )
					device->SetTexture(0,inst->data->texture.Data());
				else
					device->SetTexture(0,data->texture.Data());
				device->SetStreamSource(1,data->tcoords,sizeof(TCOORDS));
				device->DrawPrimitive(D3DPT_TRIANGLELIST,0,NFaces());
			} else {
				device->SetTexture(0,NULL);
				Shader::Set("NOCOORDS");
				device->DrawPrimitive(D3DPT_TRIANGLELIST,0,NFaces());
				Shader::Set(do_shading?"STD":"NOSHADE");
			}
		}
		else {
			DWORD color = inst->color;
			COLOR vcolor;
			vcolor.a = (color&0xFF000000)/(0xFF000000*1.0);
			vcolor.r = (color&0xFF0000)/(0xFF0000*1.0);
			vcolor.g = (color&0xFF00)/(0xFF00*1.0);
			vcolor.b = (color&0xFF)/(0xFF*1.0);
			device->SetVertexShaderConstant(4,&vcolor,1);
			if( data->tcoords ) {
				Shader::Set("COLORED");
				if( inst->data->texture.IsLoaded() )
					device->SetTexture(0,inst->data->texture.Data());
				else
					device->SetTexture(0,data->texture.Data());
				device->SetStreamSource(1,data->tcoords,sizeof(TCOORDS));
				device->DrawPrimitive(D3DPT_TRIANGLELIST,0,NFaces());
			} else {
				Shader::Set("COLORED-NOCOORDS");
				device->SetTexture(0,NULL);
				device->DrawPrimitive(D3DPT_TRIANGLELIST,0,NFaces());
			}
			Shader::Set(do_shading?"STD":"NOSHADE");
		}
	}
	if( show_infos )
		Infos::DrawBox(0xFFFF00FF,(VECTOR*)&inst->data->bmin,(VECTOR*)&inst->data->bmax,inst->position.Get());
	FOREACH(Res<Inst*>,inst->data->subs);
		Inst* sub = item.Data();
		if( sub->visible )
			tcount += sub->data->parent.Data()->Render( sub, alpha  );
	ENDFOR;
	return tcount;
}

/* ---------------------------------------------------------------------------- */

void
Mesh::Update( VBUF buffer, bool vdefault ) {
	int i;
	VERTEX *ptr;
	VECTOR *tmp = data->tmp;
	VECTOR *normals = data->normals.Data();
	if( !vdefault ) {
		VECTOR *points = verts.Data();
		memset(tmp,0,sizeof(VECTOR)*verts.Length());
		
		VECTOR4 calc;
		VECTOR4 calc2;
		int vid,bid;
		int len = weights.Length();		
		Weight *weights = this->weights.Data();
		float weight;

		calc.w = 1;
		for(i=0;i<len;i++) {
			vid = weights[i].vid;
			bid = weights[i].bid;
			weight = weights[i].weight;
			calc.x = points[vid].x;
			calc.y = points[vid].y;
			calc.z = points[vid].z;
			vtransform(&calc2,&calc,&data->bones[bid]->m);
			D3DXVec3Scale((VECTOR*)&calc,(VECTOR*)&calc2,weight);
			D3DXVec3Add(&tmp[vid],&tmp[vid],(VECTOR*)&calc);
		}
	}
	else
		tmp = verts.Data();

	bool dynamic = HasSkin();
	if( !M3DCHECK(buffer->Lock(0,0,(BYTE**)&ptr,dynamic?D3DLOCK_DISCARD:0)) )
		return;

	for(i=0;i<NFaces()*3;i++,ptr++) {
		ptr->pos = tmp[faces[i]];
		ptr->n = normals[faces[i]];
	}
	buffer->Unlock();
}

/* ---------------------------------------------------------------------------- */

void
Mesh::UpdateBone( Bone *b, BoneFrame *prev, BoneFrame *next, FLOAT delta ) {
	if( b->id != (WORD)-1 ) {
		QUATERNION q;
		VECTOR t;
		BonePos *pprev = &prev->bones[b->id];
		BonePos *pnext = &next->bones[b->id];
		vlerp(&t,&pprev->t,&pnext->t,delta);
		qslerp(&q,&pprev->q,&pnext->q,delta);
		// M = inv( Ref->T & Ref->Q ) * (Frame->T & Frame->Q)		
		D3DXMatrixRotationQuaternion(&b->m,&q);
		b->m._41 = t.x;
		b->m._42 = t.y;
		b->m._43 = t.z;		
		if( show_infos ) { // SHOW BONES
			VECTOR bmin(-1,-1,-1);
			VECTOR bmax(1,1,1);
			DWORD color;
			switch(b->childs.Length()) {
			case 0: color = 0x90FF0000; break;
			case 1: color = 0x900000FF; break;
			default: color = 0x9000FF00; break;
			}
			Infos::DrawBox(color,&bmin,&bmax,&b->m);
		}
		mmult(&b->m,&refpos[b->id],&b->m);

	}
	FOREACH(Bone*,b->childs);
		UpdateBone(item,prev,next,delta);
	ENDFOR;
}

/* ---------------------------------------------------------------------------- */

void
Mesh::UpdateBox( BoneFrame *f, VECTOR *bmin, VECTOR *bmax ) {
	int i;
	for(i=0;i<f->bones.Length();i++) {
		BonePos *p = &f->bones[i];
		D3DXVec3Minimize(bmin,bmin,&p->t);
		D3DXVec3Maximize(bmax,bmax,&p->t);
	}
}

/* ---------------------------------------------------------------------------- */

};//namespace
/* **************************************************************************** */
