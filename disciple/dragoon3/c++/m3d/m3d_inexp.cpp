/* **************************************************************************** */
/*																				*/
/*   M3D Import & Export functions												*/
/*	 Nicolas Cannasse															*/
/*	 (c)2002 Motion-Twin														*/
/*																				*/
/* **************************************************************************** */
#include <m3d.h>
#include <m3d_struct.h>

namespace M3D {
/* ---------------------------------------------------------------------------- */

bool
Material::export( WOStream *out ) {
	str_out(textname.c_str(),out);
	ptr_out(diffuse,out);
	ptr_out(specular,out);
	ptr_out(strength,out);
	ptr_out(alpha,out);
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
Material::import( WIStream *in ) {
	textname = str_in(in);
	ptr_in(diffuse,in);
	ptr_in(specular,in);
	ptr_in(strength,in);
	ptr_in(alpha,in);
	return true;

}

/* ---------------------------------------------------------------------------- */

bool
Mesh::export( WOStream *out )
{
	// name
	str_out(_name,out);

	// flags
	int flags = (text.Length()?1:0) |
				(material?2:0) |
				(bones.Length()?4:0) |
				(anims.Length()?8:0) |
				(submeshes.Length()?16:0);
	int_out(flags,out);

	// vertexes & faces
	int_out(verts.Length(),out);
	out->write((char*)verts.Data(),sizeof(VECTOR)*verts.Length());
	int_out(faces.Length(),out);
	out->write((char*)faces.Data(),sizeof(WORD)*faces.Length());

	// textures coordinates
	if( text.Length() ) {
		out->write((char*)tindex.Data(),sizeof(WORD)*tindex.Length());
		int_out(text.Length(),out);
		out->write((char*)text.Data(),sizeof(VECTOR2)*text.Length());
		if( material && !material->export(out) )
			return false;
	}

	// skeleton
	if( bones.Length() ) {
		int_out(bones.Length(),out);
		out->write((char*)bones.Data(),sizeof(WORD)*bones.Length());
		out->write((char*)refpos.Data(),sizeof(MATRIX)*bones.Length());
		int_out(weights.Length(),out);
		out->write((char*)weights.Data(),sizeof(Weight)*weights.Length());
	}

	// motions
	if( anims.Length() ) {
		int_out(anims.Length(),out);
		FOREACH(Anim*,anims);
			str_out(item->name,out);
			int_out(item->type,out);
			switch( item->type ) {
			case AMOTION:
				int_out(item->bframes->Length(),out);
				FOREACH(BoneFrame*,*item->bframes);
					int_out(item->frame,out);
					int_out(item->bones.Length(),out);
					out->write((char*)item->bones.Data(),sizeof(BonePos)*item->bones.Length());
				ENDFOR;
				break;
			case AMOVE:
				int_out(item->mframes->Length(),out);
				FOREACH(MoveFrame*,*item->mframes);
					int_out(item->frame,out);
					out->write((char*)&item->q,sizeof(QUATERNION));
					out->write((char*)&item->t,sizeof(VECTOR));
				ENDFOR;
				break;
			}
		ENDFOR;
	}

	// submeshes
	if( submeshes.Length() ) {
		int_out(submeshes.Length(),out);
		FOREACH(WString,submeshes);
			str_out(item,out);
		ENDFOR;
	}
	return true;
}

/* ---------------------------------------------------------------------------- */

	typedef struct VERTEX {
		VECTOR v;
		FLOAT tu,tv;

		bool operator == (const VERTEX &a ) {
			return fabs(v.x-a.v.x) < 0.0000001 &&
				fabs(v.y-a.v.y) < 0.0000001 &&
				fabs(v.z-a.v.z) < 0.0000001 &&
				fabs(tu-a.tu) < 0.0001 &&
				fabs(tv-a.tv) < 0.0001;
		}
		
		VERTEX() { }
		VERTEX( VECTOR v, VECTOR2 t ) : v(v), tu(t.x), tv(t.y) { }
	} VERTEX;

/* ---------------------------------------------------------------------------- */

bool
Mesh::import( WIStream *in )
{
	anims.Delete();
	BeginGeometry(NVerts(),NFaces());
	BeginSkeleton(bones.Length());

	// name
	_name = str_in(in);

	// flags
	int flags = int_in(in);
	bool has_tcoords = (flags & 1) != 0;
	bool has_mat = (flags & 2) != 0;
	bool has_skel = (flags & 4) != 0;
	bool has_motion = (flags & 8) != 0;
	bool has_submesh = (flags & 16) != 0;

	// vertexes & faces
	int n = int_in(in);
	VECTOR *vdata = new VECTOR[n];
	in->read((char*)vdata,sizeof(VECTOR)*n);
	verts.Set(vdata,n);
	
	int nfaces = int_in(in);
	WORD *fdata = new WORD[nfaces];
	in->read((char*)fdata,sizeof(WORD)*nfaces);
	faces.Set(fdata,nfaces);

	// textures coordinates
	if( has_tcoords ) {
		WORD *tidata = new WORD[nfaces];
		in->read((char*)tidata,sizeof(WORD)*nfaces);		

		n = int_in(in);
		VECTOR2 *tdata = new VECTOR2[n];
		in->read((char*)tdata,sizeof(VECTOR2)*n);		
		if( has_mat ) {
			Material *material = new Material();
			if( !material->import(in) )
				return false;
			SetMaterial(material);
		}
		tindex.Set(tidata,nfaces);
		text.Set(tdata,n);
	}

//************* indexize *************


	WArray<VERTEX> idata;
	WArray<WORD> iindexes;
	int i,j;
	for(i=0;i<nfaces;i++) {
		VERTEX v( verts[fdata[i]], text[tindex[i]] );
		for(j=0;j<idata.Length();j++)
			if( idata[j] == v ) {
				iindexes.Add(j);
				break;
			}
		if( j == idata.Length() ) {
			idata.Add(v);
			iindexes.Add(j);
		}
	}


//************************************/

	// skeleton
	if( has_skel ) {
		n = int_in(in);
		WORD *bdata = new WORD[n];
		in->read((char*)bdata,sizeof(WORD)*n);
		bones.Set(bdata,n);

		MATRIX *idata = new MATRIX[n];
		in->read((char*)idata,sizeof(MATRIX)*n);
		refpos.Set(idata,n);

		n = int_in(in);
		Weight *wdata = new Weight[n];
		in->read((char*)wdata,sizeof(Weight)*n);
		weights.Set(wdata,n);
	}

	// motions
	if( has_motion ) {
		int i;
		n = int_in(in);
		for(i=0;i<n;i++) {
			WString name = str_in(in);			
			int type = int_in(in);
			switch( type )  {
			case AMOTION: {
				BeginMotion(name);
				Anim *a = anims.First();
				int f;
				int nframes = int_in(in);
				for(f=0;f<nframes;f++) {
					int frame = int_in(in);
					int nbones = int_in(in);
					BoneFrame *f = new BoneFrame(frame);
					BonePos *bpos = new BonePos[nbones];
					in->read((char*)bpos,sizeof(BonePos)*nbones);
					f->bones.Set(bpos,nbones);
					a->bframes->Add(f);
				}
				EndMotion();
				}; break;
			case AMOVE: {
				BeginMovement(name);
				Anim *a = anims.First();
				int f;
				int nframes = int_in(in);
				for(f=0;f<nframes;f++) {
					int frame = int_in(in);
					MoveFrame *mfr = new MoveFrame(frame);
					in->read((char*)&mfr->q,sizeof(QUATERNION));
					in->read((char*)&mfr->t,sizeof(VECTOR));
					a->mframes->Add(mfr);
				}
				EndMovement();
				}; break;
			}

		}
	}

	// submeshe
	if( has_submesh ) {
		int i;
		n = int_in(in);
		for(i=0;i<n;i++)
			submeshes.Add(str_in(in));
	}

	EndGeometry();
	EndSkeleton();
#ifndef EXPORTER
	Reset(true);
	Reset(false);
#endif
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
Flags::import( WIStream *in ) {
	int i = int_in(in);
	if( i == -1 )
		return false;
	*((int*)&data) = i;
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
Flags::export( WOStream *out ) {
	int_out(*(int*)&data,out);
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
Path::export( WOStream *out ) {
	int_out(data.Length(),out);
	FOREACH(PathData*,data);
		int_out(item->frames.Length(),out);
		str_out(item->anim,out);
		FOREACH(MoveFrame*,item->frames);
			int_out(item->frame,out);
			out->write((char*)&item->q,sizeof(QUATERNION));
			out->write((char*)&item->t,sizeof(VECTOR));			
		ENDFOR;
	ENDFOR;
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
Path::import( WIStream *in ) {
	int n = int_in(in);
	if( n == -1 )
		return false;
	while( n > 0 ) {
		int f = int_in(in);		
		if( f == -1 )
			return false;
		WString anim = str_in(in);
		Begin(anim);
		while( f > 0 ) {
			int frame = int_in(in);
			if( frame == -1 )
				return false;
			QUATERNION q;
			VECTOR t;
			in->read((char*)&q,sizeof(QUATERNION));
			in->read((char*)&t,sizeof(VECTOR));					
			AddFrame(frame,t,q);
		}
		End();
	}
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
File::Export()
{
	filebuf fbuf;
	fbuf.open(filename.c_str(),ios::binary | ios::out | ios::trunc );	
	WOStream out(&fbuf);
	if( !export(&out) )
		return false;
	fbuf.close();		
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
File::Import()
{
	filebuf fbuf;
	fbuf.open(filename.c_str(),ios::binary | ios::in | ios::nocreate);
	WIStream in(&fbuf);
	if( !import(&in) )
		return false;
	fbuf.close();
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
File::export( WOStream *out )
{		
	*out << "M3D";
	FOREACH(Any*,items);
		int_out(item->type(),out);
		int pos = out->tellp();
		int_out(0,out);
		if( !item->export(out) )
			return false;
		int epos = out->tellp();
		out->seekp(pos);
		int_out(epos-pos,out);
		out->seekp(epos);		
	ENDFOR;
	int_out(0,out);
	return true;
}

/* ---------------------------------------------------------------------------- */

bool
File::import( WIStream *in )
{
	char tag[4];
	tag[3] = 0;
	in->read(tag,3);
	if( strcmp(tag,"M3D") != 0 )
		return false;
	while(1) {
		MTYPE tag = (MTYPE)int_in(in);
		if( !tag )
			break;
		int size = int_in(in);
		switch(tag) {
		case MFILE:
			return false;
		case MMESH:
			{
				Mesh *m = new Mesh();
				if( !m->import(in) ) {
					delete m;
					return false;
				}
				items.Add(m);				
			}
			break;
		case MFLAGS:
			{
				Flags *f = new Flags();
				if( !f->import(in) ) {
					delete f;
					return false;
				}
				items.Add(f);
			}
			break;
		default:
			return false;
		}
	}
	return true;
}

/* ---------------------------------------------------------------------------- */
}; // namespace
/* **************************************************************************** */
