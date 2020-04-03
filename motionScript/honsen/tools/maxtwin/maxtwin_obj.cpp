/* ************************************************************************ */
/*																			*/
/*	MaxTwin																	*/
/*	Morgan Bachelier, Nicolas Cannasse										*/
/*	(c)2003 Motion-Twin														*/
/*																			*/
/* ************************************************************************ */
#include <max.h>
#include <modstack.h>
#include <stdmat.h>
#include "../../common/mtw.h"
#include "maxtwin_obj.h"
/* ------------------------------------------------------------------------ */

struct VECTOR {
	FLOAT x,y,z;
};

/* ------------------------------------------------------------------------ */

struct TCOORDS {
	FLOAT tu,tv;
};

/* ------------------------------------------------------------------------ */

MaxObj::MaxObj( MTW *o, INode *node ) : o(o), node(node), geometry(NULL), mesh(NULL), free_geometry(false), geom_modified(false) {
	obj_name = node->GetName();
}

/* ------------------------------------------------------------------------ */

MaxObj::~MaxObj() {
	if( geometry && free_geometry )
		geometry->DeleteMe();
}

/* ------------------------------------------------------------------------ */

void 
MaxObj::Log( WString line ) {
	if( log_text.length() != 0 )
		log_text += "\n";
	log_text += line;
}

/* ------------------------------------------------------------------------ */

bool
MaxObj::Init() {
    Object* obj = node->GetObjectRef();
    while( obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID) )
		obj = ((IDerivedObject*)obj)->GetObjRef();

	if( obj == NULL ) {
		Log("No object");
		return false;
	}
    geometry = (TriObject*)obj->ConvertToType(0, triObjectClassID);
	if( geometry == NULL ) {
		Log("No geometry");
		return false;
	}
    mesh = &geometry->GetMesh();
	free_geometry = (geometry != obj);

	MTW *indx = o->Child(MAKETAG(INDX));
	int nfaces = mesh->getNumFaces();
	geom_modified = (indx && indx->size/sizeof(WORD) != nfaces*3);
	
	return true;
}

/* ------------------------------------------------------------------------ */

bool
MaxObj::Question( WString txt ) {
	return MessageBox(NULL,txt.c_str(),"MaxTwin:",MB_YESNO | MB_ICONQUESTION) == IDYES;
}

/* ------------------------------------------------------------------------ */

bool
MaxObj::IsExportMesh() {
	return true;
}

/* ------------------------------------------------------------------------ */

bool
MaxObj::IsExportTexture() {
	return true;
}

/* ------------------------------------------------------------------------ */

bool
MaxObj::ReplaceMesh() {
	if( !IsExportMesh() )
		return true;

	int nfaces = mesh->getNumFaces();
	int nverts = mesh->getNumVerts();
	WORD *indexes = new WORD[nfaces*3];
	VECTOR *data = new VECTOR[nverts];

	int i,j;
    for(i=0;i<nfaces;i++)
        for(j=0;j<3;j++)
            indexes[j+i*3] = (WORD)mesh->faces[i].v[j];

	Matrix3 offset(1);
	offset.PreTranslate(node->GetObjOffsetPos());
	PreRotateMatrix(offset,node->GetObjOffsetRot());
	ApplyScaling(offset,node->GetObjOffsetScale());

	for(i=0;i<nverts;i++) {
		Point3 v = mesh->getVert(i);
		v = offset.PointTransform(v);
		data[i].x = v.x;
		data[i].y = v.y;
		data[i].z = v.z;
	}

	MTW *vect = o->Child(MAKETAG(VECT));
	MTW *indx = o->Child(MAKETAG(INDX));
	o->RemoveChild(vect);
	o->RemoveChild(indx);
	delete vect;
	delete indx;

	o->AddChild( new MTW(MAKETAG(VECT),nverts*sizeof(VECTOR),data) );
	o->AddChild( new MTW(MAKETAG(INDX),nfaces*3*sizeof(WORD),indexes) );
	return true;
}

/* ------------------------------------------------------------------------ */

void 
MaxObj::RemoveTexture() {
	MTW *mid0 = o->Child(MAKETAG(MID0));
	MTW *map0 = o->Child(MAKETAG(MAP0));
	MTW *tex0 = o->Child(MAKETAG(TEX0));
	o->RemoveChild(mid0);
	o->RemoveChild(map0);
	o->RemoveChild(tex0);
	delete mid0;
	delete map0;
	delete tex0;
}

/* ------------------------------------------------------------------------ */

bool
MaxObj::ReplaceTexture() {
	MTW *mid0 = o->Child(MAKETAG(MID0));
	bool has_texture = (mid0 != NULL);
    Mtl *material = node->GetMtl();
	int ntverts = mesh->getNumTVerts();
	int nfaces = mesh->getNumFaces();

	if( !IsExportTexture() || material == NULL || ntverts == 0 ) {
		if( IsExportTexture() && material != NULL && ntverts == 0 )
			Log("No texture mapping, collapse the stack");
		if( geom_modified && has_texture ) {
			if( !Question("Geometry modified, remove old mapping ?") )
				return false;
			RemoveTexture();
			return true;
		}
		return true;
	}

	if( geom_modified && !IsExportMesh() ) {
		Log("Texture cannot be updated because geometry have been modified : export the mesh data");
		return false;
	}

	WORD *indexes = new WORD[nfaces*3];
	TCOORDS *data = new TCOORDS[ntverts];

	int i,j;
	for(i=0;i<nfaces;i++)
        for(j=0;j<3;j++)
            indexes[j+i*3] = (WORD)mesh->tvFace[i].t[j];

	for(i=0;i<ntverts;i++) {
		UVVert t = mesh->getTVert(i);
		data[i].tu = t.x;
		data[i].tv = 1 - t.y; // tcoords from top to bottom
	}

	RemoveTexture();
	o->AddChild( new MTW(MAKETAG(MID0),nfaces*3*sizeof(WORD),indexes) );
	o->AddChild( new MTW(MAKETAG(MAP0),ntverts*sizeof(TCOORDS),data) );

    if( material->ClassID() != Class_ID(DMTL_CLASS_ID,0) ) {
		Log("Not a basic material cannot be exported");
        return false; 
	}

    Texmap* tex = material->GetSubTexmap(ID_DI);
    if( !tex ) {
		Log("Not a diffuse texture in material");
		return false;
	}

    if( tex->ClassID() != Class_ID(BMTEX_CLASS_ID,0) ) {
		Log("Not a bitmap in material diffuse texture");
		return false;
	}

    char *tex0_name = ((BitmapTex*)tex)->GetMapName();
	char *file = strrchr(tex0_name,'\\');
	if( file == NULL )
		file = tex0_name;
	else
		file++;
 	o->AddChild( new MTW(MAKETAG(TEX0), strlen(file)+1, strdup(file)) );
	return true;
}

/* ************************************************************************ */
