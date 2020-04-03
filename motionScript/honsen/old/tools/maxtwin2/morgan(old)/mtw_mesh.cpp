//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_mesh.cpp
//*     Author  : M.B.
//*     Desc    : Get mesh info : geometry, mapping
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_obj.h"
#include "include/mtw_error.h"


///////////////////////////////////////////////////////////////////////////////
// to construct a Direct3D "Vertex" structure
//
typedef struct mv
{
    WORD vIndex;
    WORD mIndex;
    inline bool operator==(const struct mv &v) {
        return vIndex == v.vIndex && mIndex == v.mIndex;
    }
} MappedVertex;



//-----------------------------------------------------------------------------
// Add mesh tags to the object tag according to export properties
// precond : exportGeom || exportMapp
// hasExtendedMesh=true => merge=true
//
int
Obj::AddMesh()
{
    int res;

    obj = node->GetObjectRef();
    while(obj && (obj->SuperClassID() == GEN_DERIVOB_CLASS_ID))
		obj = ((IDerivedObject*)obj)->GetObjRef();

    triObj   = (TriObject*)obj->ConvertToType(0, triObjectClassID);

    // retrieve node mesh & material
    Mesh      *mesh     = &triObj->GetMesh();
    Mtl       *material = node->GetMtl();

    // validate
    if(exportGeom && hasMapp && !material) 
        return E_MERGE_MESH;

    if(exportMapp && !material) // || (!material || mesh->tVerts==0)
        return E_NO_MATERIAL;

    // clean
    if(exportGeom && merge)
        CleanGeometry();

    if(exportMapp && hasMapp)
        CleanMapping();

    // compute mesh
    if( exportMapp || hasMapp )
    {
        res = ComputeExtendedMesh(mesh);
        if(res<0) return res;
    }
    else
        ComputeSimpleMesh(mesh);

    // add mesh
    if(exportGeom)
        AddGeometry(mesh);

    if(exportMapp) {
        res = AddMapping(mesh, material);
        hasMapp = true;
        if(res<0) return res;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Process a special algorithm to extend geometry and mapping
// set vect & mapp indexes, indx, vidx
//
int
Obj::ComputeExtendedMesh(Mesh* mesh)
{
    int i, j;
    short               currIndx;               // TODO : short to int
    MappedVertex        currMappedVertex;
    WList<MappedVertex> mappedVertices;

    for(i=0; i<mesh->getNumFaces(); i++) {
        for(j=0;j<3;j++)
        {
            currMappedVertex.vIndex = (WORD)(mesh->faces[i].v[j]);
            currMappedVertex.mIndex = (WORD)(mesh->tvFace[i].t[j]);

            // add current mapped vertex if not exists
            currIndx = mappedVertices.IndexOf(currMappedVertex);
            if(currIndx==-1) {
                mappedVertices.Add(currMappedVertex);
                // resplit list into vect/mapp
                geomIndexes.Add(currMappedVertex.vIndex);
                mappIndexes.Add(currMappedVertex.mIndex);
                currIndx = mappedVertices.Length()-1;
            }
            indx.Add((WORD)currIndx);           // extended vertex index
            vidx.Add(currMappedVertex.vIndex);  // original vertex index
        }
    }

    if(hasMapp)
        if(nExtendedVerts!=geomIndexes.Length())
            return E_MERGE_MESH;

    nVerts = mesh->getNumVerts();
    nExtendedVerts = geomIndexes.Length();

    return 0;
}


//-----------------------------------------------------------------------------
// Vertices Coordinates & Faces, composed of 3 Vertices Indexes -> 3 WORDs
//
void
Obj::ComputeSimpleMesh(Mesh* mesh)
{
    nVerts = mesh->getNumVerts();

    int i, j;
    for(i=0; i<nVerts; i++)
        geomIndexes.Add((WORD)i);

    DWORD nFaces = mesh->getNumFaces();
    for(i=0; i<nFaces; i++)
        for(j=0;j<3;j++)
            indx.Add((WORD)(mesh->faces[i].v[j]));
}


//-----------------------------------------------------------------------------
// Add geometry tags to the object tag
//
void
Obj::AddGeometry(Mesh* mesh)
{
    MATRIX nodeMatrix = Maxtwin::MaxToD3DMatrix(&node->GetObjectTM(0));
    MTW *mrefTag = new MTW(MAKETAG(MREF), sizeof(MATRIX), new MATRIX(nodeMatrix));
    objtTag->AddChild(mrefTag);

    matrix = (MATRIX*)mrefTag->data;

    objtTag->AddChild(CreateGeometryTag(mesh));
    objtTag->AddChild(CreateIndexingTag());
}


//-----------------------------------------------------------------------------
// Add mapping tags to the object tag
//
int
Obj::AddMapping(Mesh* mesh, Mtl* material) 
{
    objtTag->AddChild(CreateMappingTag(mesh));
    objtTag->AddChild(CreateReindexingTag());
    MTW *tex0Tag = CreateTextureTag(material);
    if(!tex0Tag)
        return E_GET_MATERIAL;
    objtTag->AddChild(tex0Tag);
    return 0;
}


//-----------------------------------------------------------------------------
// Delete the tags corresponding to the geometry and indexing of an object
//
void
Obj::CleanGeometry()
{
    RemoveExistingTag(objtTag, MAKETAG(VECT));
    RemoveExistingTag(objtTag, MAKETAG(INDX));
    RemoveExistingTag(objtTag, MAKETAG(MREF));
    //matrix = &Maxtwin::MaxToD3DMatrix(&node->GetNodeTM(0));
}


//-----------------------------------------------------------------------------
// Delete the mapping information of an object (texcoords, texture, reindexing)
//
void
Obj::CleanMapping()
{
    RemoveExistingTag(objtTag, MAKETAG(MAP0));
    RemoveExistingTag(objtTag, MAKETAG(TEX0));
    RemoveExistingTag(objtTag, MAKETAG(VIDX));
}


//-----------------------------------------------------------------------------
// 3 coords (x,y,z) for each vertex : global coords
//
MTW*
Obj::CreateGeometryTag(Mesh* mesh)
{
    int i=0, tagSize = geomIndexes.Length();
    Point3* tagData = new Point3[tagSize];
    Point3 vertex;
    FOREACH(WORD, geomIndexes)
        VECTOR4 vec4(0,0,0,1);
        vertex = mesh->getVert(item);
        memcpy(&vec4, &vertex, sizeof(Point3));
        vtransform(&vec4, &vec4, matrix);
        memcpy(&tagData[i], &vec4, sizeof(Point3));
        i++;
    ENDFOR;
    return new MTW(MAKETAG(VECT), tagSize*sizeof(Point3), tagData);
}


//-----------------------------------------------------------------------------
// 2 mapping coords (u,v) for each vertex
//
MTW*
Obj::CreateMappingTag(Mesh* mesh)
{
    int i=0, tagSize = mappIndexes.Length();
    Point2* tagData = new Point2[tagSize];
    UVVert mapp;
    FOREACH(WORD, mappIndexes)
        UVVert mapp = mesh->getTVert(item);
        tagData[i].x = mapp.x;
        tagData[i].y = 1-mapp.y; // convert to d3d texture coordinate system
        i++;
    ENDFOR;
    return new MTW(MAKETAG(MAP0), tagSize*sizeof(Point2), tagData);
}


//-----------------------------------------------------------------------------
// Faces indexes (nFaces*3 indexes)
//
MTW*
Obj::CreateIndexingTag()
{
    int i=0, tagSize = indx.Length();
    WORD* tagData = new WORD[tagSize];
    FOREACH(WORD, indx)
        tagData[i] = item;
        i++;
    ENDFOR;
    return new MTW(MAKETAG(INDX), tagSize*sizeof(WORD), tagData);
}


//-----------------------------------------------------------------------------
// Compute links between extended and original vertices
//
MTW*
Obj::CreateReindexingTag()
{
    int i, j, nIndx = indx.Length(), nVerts = mappIndexes.Length();
    WORD* reIndex = new WORD[nVerts];
    for(i=0; i<nVerts; i++) {
        j=0;
        bool stop = false;
        while(j<nIndx && !stop) {
            if(indx.AtIndex(j)==i) {        // current indx val corresponds to current val 
                reIndex[i]=vidx.AtIndex(j);
                stop = true;
            }
            j++;
        }
    }
    return new MTW(MAKETAG(VIDX), nVerts*sizeof(WORD), reIndex);
}


//-----------------------------------------------------------------------------
// TODO : memory leak ?
//
MTW*
Obj::CreateTextureTag(Mtl* material)
{
    if(material->ClassID()!=Class_ID(DMTL_CLASS_ID,0)) 
        return NULL; // non-default not covered

    Texmap* tex = material->GetSubTexmap(ID_DI); // DIffuse
    if(!tex)
        return NULL; // false here ?

    if(tex->ClassID() != Class_ID(BMTEX_CLASS_ID,0))
        return NULL; // non-bitmap not covered

    char* tex0Name = ((BitmapTex*)tex)->GetMapName();
    TSTR fileName;
    SplitPathFile(TSTR(tex0Name), NULL, &fileName);
    char *text = strdup(fileName);
    return new MTW(MAKETAG(TEX0), strlen(text)+1, text);
}


//-----------------------------------------------------------------------------
// Remove the specified existing child tag from the specified parent
// return true if delete ok, false otherwise
//
bool
Obj::RemoveExistingTag(MTW* parent, TAG name) 
{
    MTW *tag = parent->Child(name);
    if(tag) {
        parent->RemoveChild(tag);
        SAFE_DELETE(tag);
        return true;
    }
    return false;
}



