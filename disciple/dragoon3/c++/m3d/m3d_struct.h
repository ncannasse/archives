/* **************************************************************************** */
/*																				*/
/*    3D data structures														*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#pragma once
#include <m3d.h>
#include <m3d_res.h>

/* **************************************************************************** */
namespace M3D {
	
	extern MMat matproj;
	extern MMat world;

	struct Mesh::Bone {
		WORD id;
		MATRIX m;
		WList<Bone*> childs;
		Bone( WORD id ) : id(id) { }
		~Bone() { childs.Delete(); }
	};

	struct Mesh::Data {
		WArray<VECTOR> normals;
		VECTOR *tmp;
		Bone* skeleton;
		WArray<Bone*> bones;
		VBUF tcoords;
		VECTOR4 bmin,bmax;
		Res<TEXTURE> texture;
		WList< Res<Mesh*> > subs;
		Data() : tcoords(NULL), skeleton(NULL), tmp(NULL) { }
		~Data() { M3DRELEASE(tcoords); delete skeleton; delete tmp; }
	};

	struct Mesh::Weight {
		WORD vid;
		WORD bid;
		FLOAT weight;
		Weight() { }
		Weight( WORD vid, WORD bid, FLOAT w ) : vid(vid), bid(bid), weight(w) { }
	};

	struct BonePos {
		VECTOR t;
		QUATERNION q;
		BonePos() { }
		BonePos( VECTOR t, QUATERNION q ) : t(t), q(q) { }
	};

	struct Mesh::BoneFrame {
		int frame;
		WArray<BonePos> bones;
		BoneFrame( int f ) : frame(f) { }
	};

	struct MoveFrame {
		int frame;
		QUATERNION q;
		VECTOR t;
		MoveFrame( int f ) : frame(f) { }
		MoveFrame( int f, VECTOR t, QUATERNION q ) : frame(f), t(t), q(q) { }
	};

	enum ATYPE {
		AMOTION,
		AMOVE
	};

	struct Mesh::Anim {
		ATYPE type;
		WString name;

		union {
			WList<BoneFrame*> *bframes;
			WList<MoveFrame*> *mframes;
		};

		Anim( WString name, ATYPE type ) : name(name), type(type) { 
			switch(type) {
			case AMOTION: bframes = new WList<BoneFrame*>(); break;
			case AMOVE: mframes = new WList<MoveFrame*>(); break;
			}
		}

		void Clean() {
			switch(type) {
			case AMOTION: bframes->Delete(); break;
			case AMOVE: mframes->Delete(); break;
			}
		}

		~Anim() {
			Clean();
			switch(type) {
			case AMOTION: delete bframes; break;
			case AMOVE: delete mframes; break;
			}
		}
	};

	struct Inst::Data {		
		Res<Mesh*> parent;
		VBUF buffer;
		VECTOR4 bmin,bmax;
		bool invalid_box;
		WList< Res<Inst*> > subs;
		Res<TEXTURE> texture;
		Data( Mesh *m ) : parent(Res<Mesh*>::Share(0,m,Mesh::Free)), buffer(NULL), invalid_box(true) { }
		~Data() { M3DRELEASE(buffer); }
	};	


	struct Sprite::Data {
		Res<TEXTURE> texture;
		FRECT coords;
		Data() { }
	};


	struct Path::PathData {
		WString anim;
		WList<MoveFrame*> frames;

		~PathData() {
			frames.Delete();
		}
	};


/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */