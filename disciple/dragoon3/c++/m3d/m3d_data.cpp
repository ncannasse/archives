/* **************************************************************************** */
/*																				*/
/*   M3D Data structures functions												*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>
#include <m3d_struct.h>

namespace M3D {
/* ---------------------------------------------------------------------------- */

Any::Any()  : _name("Unamed object") {
}

Any::Any( WString name ) : _name(name) {
}

Any::~Any() {
}

bool
Any::export( WOStream *out ) {
	return false;
}

bool
Any::import( WIStream *in ) {
	return false;
}

WString
Any::name() {
	return _name;
}

/* ---------------------------------------------------------------------------- */
#ifndef EXPORTER

WList<Inst*> instances;

Inst::Inst( Mesh *mesh, bool hasparent ) : FX::Gfx(), data(new Data(mesh)), 
	visible(true), anim(-1), animtime(0), animspeed(1.0f),
	position(IDMATRIX), worldpos(IDMATRIX), transpos(IDMATRIX),
	customdata(NULL), color(0xFFFFFFFF)
{
	_name = "Inst-"+mesh->name();
	Reset(false);
	if( !hasparent )
		instances.Add(this);
	position.Invalidate();
	mesh->GetBBox((VECTOR*)&data->bmin,(VECTOR*)&data->bmax);
}

void
Inst::Detach() {
	instances.Remove(this);	
	Reset(true);
	if( data) {
		FOREACH(Res<Inst*>,data->subs);
			item.Data()->Detach();
		ENDFOR;
		delete data;
		data = NULL;
	}
	delete customdata;
}

void
Inst::Invalidate() {
	position.Invalidate();
}

Inst::~Inst() {
	if( data ) {
		instances.Remove(this);
		delete data;
	}
}

void
Inst::Pick( FLOAT x, FLOAT y, WList<Inst*> *found ) {
    VECTOR v;
    v.x =  x / matproj.Get()->_11;
    v.y = -y / matproj.Get()->_22;
    v.z =  1.0f;

    MATRIX imatView, mtmp;
    minv( &imatView, Camera::view.Get() );	
	mmult(&imatView,&imatView,&ttransform);

	VECTOR start,dir;
	vtransform3(&dir,&v,&imatView);

    start.x = imatView._41;
    start.y = imatView._42;
    start.z = imatView._43;

	FOREACH(Inst*,instances);
		if( item->Intersect(&start,&dir) )
			found->Add(item);
	ENDFOR;
}

extern int width,height;

VECTOR2
Project( FLOAT x, FLOAT y, FLOAT target_z ) {
    VECTOR v;
    v.x =  x / matproj.Get()->_11;
    v.y = -y / matproj.Get()->_22;
    v.z =  1.0f;

    MATRIX imatView, mtmp;
    minv( &imatView, Camera::view.Get() );	
	mmult(&imatView,&imatView,&ttransform);

	VECTOR start,dir;
	vtransform3(&dir,&v,&imatView);

    start.x = imatView._41;
    start.y = imatView._42;
    start.z = imatView._43;

	VECTOR end = start+dir*1000;
	D3DXPLANE p;
	D3DXPlaneFromPointNormal(&p,&VECTOR(0,0,0),&VECTOR(0,0,1));	
	D3DXPlaneIntersectLine(&v,&p,&start,&end);

	return VECTOR2(v.x,v.y);
}

void
Inst::AnimList( WList<const char*> *names ) {
	if( !data )
		return;
	const char *aname;
	int index = 0;
	do {
		aname = data->parent.Data()->AnimName(index);
		if( !aname )
			break;
		names->Add(aname);
		index++;
	} while(true);
}

float
Inst::GetAnimTime() {
	if( !data )
		return 0;
	return data->parent.Data()->GetAnimTime(this);
}

void
Inst::SetAnimTime( float t ) {
	if( !data )
		return;
	data->parent.Data()->SetAnimTime(this,t);
	FOREACH(Res<Inst*>,data->subs);
		item.Data()->SetAnimTime(t);
	ENDFOR;
}

float
Inst::GetAnimSpeed() {
	return animspeed;
}

void
Inst::SetAnimSpeed( float s ) {
	animspeed = s;
	FOREACH(Res<Inst*>,data->subs);
		item.Data()->SetAnimSpeed(s);
	ENDFOR;
}

MATRIX*
Inst::Position() {
	return position.Get();
}

void
Inst::SetPosition( MATRIX *m ) {
	if( !data )
		return;
	position.Set(*m);
	FOREACH(Res<Inst*>,data->subs);
		item.Data()->SetPosition(m);
	ENDFOR;
}

void
Inst::Transform( MATRIX *m ) {
	if( !data )
		return;
	mmult(position.Get(),position.Get(),m);
	position.Invalidate();
	FOREACH(Res<Inst*>,data->subs);
		item.Data()->Transform(m);
	ENDFOR;
}

bool
Inst::Intersect( VECTOR *start, VECTOR *end ) {	
	VECTOR4 btmin, btmax;
#ifdef EXPORTER
	return false;
#else
	vtransform(&btmin,&data->bmin,position.Get());
	vtransform(&btmax,&data->bmax,position.Get());
	return D3DXBoxBoundProbe((VECTOR*)&btmin,(VECTOR*)&btmax,start,end) == TRUE;
#endif
}

void
Inst::Fade( DWORD color, FLOAT time ) {
	if( !data )
		return;
	if( color == this->color ) {
		FX::Lerp::Cancel(&this->color);
		return;
	}
	FX::Lerp::CLerp(&this->color,&this->color,&color,time);
}

void
Inst::SetDest( MATRIX *m, FLOAT time ) {
	if( !data )
		return;
	FX::Lerp::MLerp(&position,position.Get(),m,time);
}

void
Inst::SetTexture( TEXTURE t ) {
	if( !data )
		return;
	data->texture = Res<TEXTURE>::Share((FID)t,t,Material::FreeTexture);
}

void
Inst::Free( Inst *i ) {
	delete i;
}

#endif
/* ---------------------------------------------------------------------------- */

Mesh::Mesh() : data(new Data()), material(NULL)  {
}

Mesh::Mesh( WString name ) : Any(name), data(new Data()), material(NULL)   {
}

Mesh::~Mesh() {	
#ifndef EXPORTER
	Reset(true);
#endif
	delete data;
	delete material;
	paths.Delete();
	anims.Delete();	
}

Any::MTYPE
Mesh::type() {
	return MMESH;
}

#ifndef EXPORTER
Inst*
Mesh::CreateInstance( bool hasparent ) {
	Inst *i = new Inst(this,hasparent);
	FOREACH(Res<Mesh*>,data->subs);
		Inst *sub = item.Data()->CreateInstance(true);
		i->data->subs.Add( Res<Inst*>::Share((FID)sub,sub,Inst::Free) );
	ENDFOR;
	return i;
}
#endif

int
Mesh::AnimIndex( const char *name ) {
	int index = 0;
	FOREACH(Anim*,anims);
		if( item->name == name )
			return index;
		index++;
	ENDFOR;
	return -1;
}

const char *
Mesh::AnimName( int index ) {
	Anim *a = anims.AtIndex(index);
	if( a == NULL )
		return NULL;
	return a->name.c_str();
}

bool
Mesh::BeginGeometry( int nverts, int nfaces ) {
	bool b = (!verts.Length() || nverts == NVerts()) &&
			(!faces.Length() || nfaces == NFaces());
	if( b ) {
		verts.Clean();
		faces.Clean();		
	}
	return b;
}

void
Mesh::CleanGeometry() {
	verts.Clean();
	faces.Clean();
	text.Clean();
}

bool
Mesh::BeginSkeleton( int nbones ) {
	bool b = anims.Length() == 0 || bones.Length() == nbones;
	if( b ) {
		bones.Clean();
		refpos.Clean();
		weights.Clean();
		delete data->skeleton;
		data->skeleton = NULL;
	}
	return b;
}

bool
Mesh::BeginMotion( WString name ) {
	FOREACH(Anim*,anims);
		if( item->name == name && item->type == AMOTION ) {	
			anims.Remove(item);
			anims.AddFirst(item);
			item->Clean();
			return true;
		}
	ENDFOR;
	Anim *a = new Anim(name,AMOTION);
	anims.AddFirst(a);
	return true;
}

bool
Mesh::BeginMovement( WString name ) {
	FOREACH(Anim*,anims);
		if( item->name == name && item->type == AMOVE ) {	
			anims.Remove(item);
			anims.AddFirst(item);
			item->Clean();
			return true;
		}
	ENDFOR;
	Anim *a = new Anim(name,AMOVE);
	anims.AddFirst(a);
	return true;
}

bool
Mesh::BeginMapping( int nfaces ) {
	if( NFaces() != nfaces ) 
		return false;
	text.Clean();
	tindex.Clean();
	return true;
}

void
Mesh::AddRef( WString name ) {
	submeshes.Add(name);
}

void
Mesh::AddBone( WORD parent, MATRIX init ) {
	bones.Add(parent);
	refpos.Add(init);
}

void
Mesh::AddWeight( WORD vid, WORD bid, FLOAT w ) {
	weights.Add(Weight(vid,bid,w));
}

void
Mesh::AddBoneFrame( int frame, WORD bone, VECTOR t, QUATERNION q ) {
	Anim *a = anims.First();
	BoneFrame *f = NULL;
	FOREACH(BoneFrame*,*a->bframes);
		if( item->frame == frame ) {
			f = item;
			break;
		}
	ENDFOR;
	if( f == NULL ) {
		f = new BoneFrame(frame);
		a->bframes->Add(f);
	}	
	f->bones[bone] = BonePos(t,q);
}

void
Mesh::AddMoveFrame( int frame, VECTOR t, QUATERNION q ) {
	Anim *a = anims.First();
	MoveFrame *f = new MoveFrame(frame);
	f->t = t;
	f->q = q;
	a->mframes->Add(f);
}


int
Mesh::NFaces() {
	return faces.Length()/3;
}

int
Mesh::NVerts() {
	return verts.Length();
}

int
Mesh::NBones() {
	return bones.Length();
}

void
Mesh::AddVertex( FLOAT x, FLOAT y, FLOAT z ) {
	verts.Add(VECTOR(x,y,z));
}

void
Mesh::AddFace( DWORD *v ) {
	faces.Add(v[0]);
	faces.Add(v[1]);
	faces.Add(v[2]);
}

void
Mesh::AddFaceTV( DWORD *t ) {
	tindex.Add(t[0]);
	tindex.Add(t[1]);
	tindex.Add(t[2]);
}

void
Mesh::AddTV( FLOAT x, FLOAT y ) {
	text.Add(VECTOR2(x,y));
}

void
Mesh::AddSub( Mesh *sub ) {
#ifndef EXPORTER
	data->subs.Add(Res<Mesh*>::Share((FID)sub,sub,Mesh::Free));
#endif
}

void
Mesh::AddPath( Path *path ) {
	paths.Add(path);
}

bool
Mesh::HasMaterial() {
	return material != NULL;
}

bool
Mesh::HasSkin() {
	return bones.Length() != 0;
}

void
Mesh::EndGeometry() {
#ifndef EXPORTER
	data->normals.Clean();
	int i;
	for(i=0;i<verts.Length();i++)
		data->normals[i] = VECTOR(0,0,0);
	for(i=0;i<NFaces();i++) {
		WORD a = faces[i*3];
		WORD b = faces[i*3+1];
		WORD c = faces[i*3+2];
		VECTOR v1 = verts[a];
		VECTOR v2 = verts[b];
		VECTOR v3 = verts[c];
		VECTOR n,area;
		vnormalize(&n, vcross(&n, &(v2-v1), &(v3-v2) ) );
		vcross(&area,&(v2-v1), &(v3-v1) );
		n *= vlength(&area);
		data->normals[a] += n;
		data->normals[b] += n;
		data->normals[c] += n;
	}
	for(i=0;i<verts.Length();i++)
		vnormalize(&data->normals[i],&data->normals[i]);
	delete data->tmp;
	data->tmp = new VECTOR[verts.Length()];

	VECTOR vmin = VECTOR(9999999,9999999,9999999);
	VECTOR vmax = VECTOR(-9999999,-9999999,-9999999);
	for( i=0; i< verts.Length(); i++ ) {
		D3DXVec3Minimize(&vmin,&vmin,&verts[i]);
		D3DXVec3Maximize(&vmax,&vmax,&verts[i]);
	}
	data->bmin = VECTOR4(vmin.x,vmin.y,vmin.z,1);
	data->bmax = VECTOR4(vmax.x,vmax.y,vmax.z,1);
#endif
}

void
Mesh::GetBBox( VECTOR *bmin, VECTOR *bmax ) {
	*bmin = *(VECTOR*)&data->bmin;
	*bmax = *(VECTOR*)&data->bmax;
}

void
Mesh::EndMovement() {
}

void
Mesh::EndMapping() {
}

void
Mesh::InitBone( Bone *b ) {
	int i;
	for(i=0;i<bones.Length();i++) {
		if( bones[i] == b->id ) {
			Bone *child = new Bone(i);
			data->bones[i] = child;
			b->childs.Add(child);
			InitBone(child);
		}
	}
}

void
Mesh::EndSkeleton() {
	if( bones.Length() ) {
		data->skeleton = new Bone(-1);	
		InitBone(data->skeleton);
	}
}

void
Mesh::EndMotion() {
}

void
Mesh::SetMaterial( Material *mat ) {
	delete material;
	material = mat;
}

Mesh*
Mesh::Load( WIStream *in ) {
	File f(in);
	Flags *flags = (Flags*)f.Get(MFLAGS);
	Mesh *m = NULL;

	if( flags && flags->Has(Flags::FGROUP) ) {
		m = new Mesh("group");
		Mesh *sub;
		while( (sub=(Mesh*)f.Get(MMESH)) )
			m->AddSub(sub);
	}
	else {
		m  = (Mesh*)f.Get(MMESH);
		if( m ) {
			FOREACH(WString,m->submeshes);
				Any *sub = f.Find(item);
				if( sub ) {
					f.RemoveItem(sub);
					switch( sub->type() ) {
					case MMESH: m->AddSub((Mesh*)sub); break;
					case MPATH: m->AddPath((Path*)sub); break;
					}
				}
			ENDFOR;
		}
	}
	
	delete flags;
	return m;
}

void
Mesh::Free( Mesh *m ) {
	delete m;
}

/* ---------------------------------------------------------------------------- */

File::File( WString filename ) : filename(filename) {
}

File::File( WIStream *in ) {
	if( !import(in) )
		items.Delete();
}

File::~File() {
	items.Delete();
}

void
File::AddItem( Any *item ) {
	if( !Find(item->name()) )
		items.Add(item);
}

void
File::RemoveItem( Any *item ) {
	items.Remove(item);
}

Any*
File::Get( Any::MTYPE t ) {
	FOREACH(Any*,items);
		if( item->type() == t ) {
			items.Remove(item);
			return item;
		}
	ENDFOR;
	return NULL;
}

Any*
File::Find( WString name ) {
	FOREACH(Any*,items);		
		if( item->name() == name )
			return item;
	ENDFOR;
	return NULL;
}

Any::MTYPE
File::type() {
	return MFILE;
}

/* ---------------------------------------------------------------------------- */

Flags::Flags() : data(0) {
}

Flags::~Flags() {
}

void
Flags::Add( FVal flag ) {
	data |= flag;	
}

void
Flags::Del( FVal flag ) {
	if( Has(flag) )
		data ^= flag;
}

bool
Flags::Has( FVal flag ) {
	return ((data & flag) == flag);
}

Any::MTYPE
Flags::type() {
	return MFLAGS;
}

/* ---------------------------------------------------------------------------- */

Material::Material() {
}

Any::MTYPE
Material::type() {
	return MMATERIAL;
}

#ifndef EXPORTER

TEXTURE
Material::LoadTexture( WIStream *s ) {
	TEXTURE t;
	s->seekg(0,ios::end);
	long length = s->tellg();
	if( length <= 0 ) 		
		return NULL;

	s->seekg(0,ios::beg);
	char *data = new char[length];
	s->read(data,length);

	if( M3DERROR(D3DXCreateTextureFromFileInMemoryEx(
			device,
			data,
			length,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			1,
			0,
			D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT,
			D3DX_FILTER_NONE,
			D3DX_FILTER_LINEAR,
			0xFFFF00FF,
			NULL,
			NULL,
			&t),"LoadTexture") ) {
		delete data;
		return NULL;
	}

	delete data;

	return t;
}


void
Material::FreeTexture( TEXTURE t ) {
	t->Release();
}

#endif

/* ---------------------------------------------------------------------------- */

Path::Path( WString name ) : Any(name), current(NULL) {
}

Path::~Path() {
	data.Delete();
}

Any::MTYPE
Path::type() {
	return MPATH;
}

void
Path::Begin( WString name ) {
	FOREACH(PathData*,data);
		if( item->anim == name ) {
			item->frames.Delete();
			current = item;
			break;
		}
	ENDFOR;
	current = new PathData();
	current->anim = name;
	data.Add(current);
}

void
Path::End() {
	current = NULL;
}

void
Path::AddFrame( int frame, VECTOR t, QUATERNION q ) {
	current->frames.Add( new MoveFrame(frame,t,q) );
}

};//namespace
/* **************************************************************************** */
