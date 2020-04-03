/* **************************************************************************** */
/*																				*/
/*  W3D_SPRITE : 3D sprites														*/
/*																				*/
/* **************************************************************************** */
#include <m3d_res.h>
#include <m3d_struct.h>

namespace M3D {

/* **************************************************************************** */
// SPRITES

extern int width;
extern int height;

static WList<Sprite*> sprites;

void add_sorted( Sprite *s, FLOAT z ) {
	int p = 0;		
	FOREACH(Sprite*,sprites);
		if( item->GetDepth() < z ) {
			sprites.Insert(s,p);
			return;
		}	
		p++;
	ENDFOR;
	sprites.Add(s);	
}

/* **************************************************************************** */
// SPRITE

Sprite::Sprite( TEXTURE tex ) : FX::Gfx() {
	FRECT coords(0,0,1,1);
	visible = true;
	pos = VECTOR(0,0,0.001f);
	size = VECTOR2(0.1f,0.1f);	
	color = 0xFFFFFFFF;
	data = new Data();
	data->texture = Res<TEXTURE>::Share(0,tex,Material::FreeTexture);
	SetCoords(&coords);
	add_sorted(this,pos.z);
}

Sprite::~Sprite() {	
	Detach();
}

void
Sprite::SetDepth( FLOAT newz ) {
	if( data && pos.z != newz ) {
		pos.z = newz;
		sprites.Remove(this);
		add_sorted(this,pos.z);	
	}
}

FLOAT
Sprite::GetDepth() {
	return pos.z;
}

void
Sprite::SetCoords( FRECT *coords ) {
	if( data )
		data->coords = *coords;
}

void
Sprite::GetCoords( FRECT *coords ) {
	if( data )
		*coords = data->coords;
	else
		*coords = FRECT(0,0,0,0);
}

void
Sprite::Detach() {
	if( data ) {
		FX::Lerp::Cancel(&pos);
		FX::Lerp::Cancel(&color);
		sprites.Remove(this);
		delete data;
		data = NULL;
	}
}

void
Sprite::SetDestination( VECTOR2 *v, FLOAT time ) {
	if( !data )
		return;	
	FX::Lerp::VLerp(&pos,&pos,&VECTOR(v->x,v->y,pos.z),time);
}

void
Sprite::SetFade( DWORD color, FLOAT time ) {
	if( !data )
		return;
	if( color == this->color ) {
		FX::Lerp::Cancel(&this->color);
		return;
	}
	FX::Lerp::CLerp(&this->color,&this->color,&color,time);
}

Any::MTYPE
Sprite::type() {
	return MSPRITE;
}

/* **************************************************************************** */
// RENDER

typedef struct SPRITEVERTEX {
	VECTOR pos;
	FLOAT w;
	DWORD color;
	FLOAT tu,tv;
	SPRITEVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT tu, FLOAT tv, DWORD color) : pos(x,y,z), tu(tu), tv(tv), w(1.0f), color(color) { }
} SPRITEVERTEX;

#define SPRITESHADER (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

int
Sprite::Render( bool alpha ) {
	int t = 0;
	device->SetVertexShader(SPRITESHADER);
	TEXTURE lastt = NULL;
	FOREACH(Sprite*,sprites);
		if( item->visible && item->data && item->size.x > 0.001f && item->size.y > 0.001f ) {
			item->SetupDevice(alpha);
			item->Render(alpha,&lastt);
			t+=2;
		}
	ENDFOR;
	return t;
}

void
Sprite::Render( bool alpha, TEXTURE *last ) {
	if( color <= 0x00FFFFFF || (!alpha && color < 0xFF000000) || (alpha && color >= 0xFF000000) )
		return;

	FLOAT x = width * (pos.x+1.0f)/2.0f;
	FLOAT y = height * (pos.y+1.0f)/2.0f;
	FLOAT z = pos.z;
	FLOAT swidth = (size.x / 2.0f) * width;
	FLOAT sheight = (size.y / 2.0f) * height;	
	TEXTURE t = data->texture.Data();

	if( *last != t ) {
		*last = t;
		device->SetTexture(0,t);
	}
	
	FRECT *map = &data->coords;	
	SPRITEVERTEX data[6] = 
		{ SPRITEVERTEX(x-swidth/2,y-sheight/2,z,map->left,map->top,color),
		  SPRITEVERTEX(x+swidth/2,y-sheight/2,z,map->right,map->top,color),
		  SPRITEVERTEX(x-swidth/2,y+sheight/2,z,map->left,map->bottom,color),

		  SPRITEVERTEX(x+swidth/2,y-sheight/2,z,map->right,map->top,color),
		  SPRITEVERTEX(x+swidth/2,y+sheight/2,z,map->right,map->bottom,color),
		  SPRITEVERTEX(x-swidth/2,y+sheight/2,z,map->left,map->bottom,color) };

	device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,&data,sizeof(SPRITEVERTEX));
}


void
Sprite::Free( Sprite *s ) {
	delete s;
}


bool
Sprite::Init() {
	return true;
}

void
Sprite::Close() {
	FOREACH(Sprite*,sprites);
		item->Detach();
	ENDFOR;
}

/* **************************************************************************** */
}; // namespace
/* **************************************************************************** */