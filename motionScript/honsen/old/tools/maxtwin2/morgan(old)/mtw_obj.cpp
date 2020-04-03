//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_obj.cpp
//*     Author  : M.B.
//*     Desc    : Obj corresponds to current 3ds object being exported
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_obj.h"


///////////////////////////////////////////////////////////////////////////////
#define PROP_EXP_GEOM "export_geom"
#define PROP_EXP_MAPP "export_mapp"
#define PROP_EXP_MOVE "export_move"
#define PROP_EXP_MRPH "export_morph"
#define PROP_EXP_SKIN "export_skin"


//-------------------------------------------------------------------------
//ructors
//
Obj::Obj(INode* node) : node(node)
{
    nVerts = nExtendedVerts = nBones = 0;

    merge = hasMapp = hasBones = false;

    objtTag  = NULL;
    obj      = NULL;
    triObj   = NULL;

    matrix   = NULL;

    mposKeys = NULL;
    mrotKeys = NULL;
    mrotQuats = NULL;
    msclKeys = NULL;

    mrphKeys = skinKeys = NULL;

    exportGeom = GetExportProperty(PROP_EXP_GEOM);
    exportMapp = GetExportProperty(PROP_EXP_MAPP);
    exportMove = GetExportProperty(PROP_EXP_MOVE);
    exportMrph = GetExportProperty(PROP_EXP_MRPH);
    exportSkin = GetExportProperty(PROP_EXP_SKIN);
}


//-----------------------------------------------------------------------------
// check if the node has the specified property (ex : "export_skin=True")
//
bool
Obj::GetExportProperty(TSTR property) 
{
    BOOL value=FALSE;
    if( !node->GetUserPropBool(property, value) )
        value = FALSE;
    return (value==FALSE)?false:true;
}


//-------------------------------------------------------------------------
// Destructor
//
Obj::~Obj()
{
    // convertToType can generate a new object
    if(triObj!=obj)
	    triObj->DeleteMe();

    geomIndexes.Clean();
    mappIndexes.Clean();
    indx.Clean();
    vidx.Clean();

    if(mposKeys) {
        mposKeys->Delete();
        mposKeys = NULL;
    }
    if(mrotKeys) {
        mrotKeys->Delete();
        mrotKeys = NULL;
    }
    if(mrotQuats) {
        mrotQuats->Delete();
        mrotQuats = NULL;
    }
    if(msclKeys) {
        msclKeys->Delete();
        msclKeys = NULL;
    }
    SAFE_DELETE(mrphKeys);             // TODO : test it !
    SAFE_DELETE(skinKeys);
}


//-------------------------------------------------------------------------
// 
//
MTW*
Obj::Init(MTW* parentTag)
{
    if(!exportGeom && !exportMapp && !exportMove && !exportMrph && !exportSkin)
        return NULL;

    // search for an existing object named "nodeName"
    char *nodeName  = node->GetName();
    objtTag = GetObjectByName(parentTag, nodeName);
    if(!objtTag)
    {
        // not found
        objtTag = new MTW(MAKETAG(Objt));
        objtTag->AddChild(new MTW(MAKETAG(NAME), strlen(nodeName)+1, strdup(nodeName)));
        objtTag->AddChild(new MTW(MAKETAG(flag), 0, 0));
        parentTag->AddChild(objtTag);

        if(exportMapp || exportMove || exportMrph || exportSkin)
            exportGeom = true;  // force export geometry in that case

        return objtTag;
    }

    // object "nodeName" found

    merge = true;

    // mesh information
    MTW* vectTag = objtTag->Child(MAKETAG(VECT));
    nVerts = vectTag->size/sizeof(Point3);

    MTW* refpTag = objtTag->Child(MAKETAG(MREF));
    matrix = (MATRIX*)refpTag->data;

    hasMapp = objtTag->HasChild(MAKETAG(MAP0));
    if(hasMapp)
    {
        // compute real number of vertices
        nExtendedVerts = nVerts;
        MTW* vidxTag = objtTag->Child(MAKETAG(VIDX));
        WORD* indexes = (WORD*)vidxTag->data;
        WORD indexMax=0;
        for(int i=0; i<nVerts; i++) {
            if(indexes[i]>indexMax)
                indexMax = indexes[i];
        }
        nVerts = indexMax+1;
    }

    // skeleton information
    MTW *skelTag = objtTag->Child(MAKETAG(Skel));
    if(skelTag) {
        hasBones = true;
        MTW *pidxTag = skelTag->Child(MAKETAG(PIDX));
        nBones = pidxTag->size/sizeof(WORD);
    }

    // force export geometry if simple mesh
    if(exportMapp && !hasMapp)
        exportGeom = true;

    return objtTag;
}


//-----------------------------------------------------------------------------
// Search for the first object with the specified name in existing tags
//
MTW*
Obj::GetObjectByName(MTW* parent, char* name)
{
    WList<MTW*> objtTags = parent->Childs(MAKETAG(Objt));
    FOREACH(MTW*, objtTags)
        MTW *nameTag = item->Child(MAKETAG(NAME));
        if(nameTag && nameTag->data && strcmp(name, (char*)nameTag->data)==0)
            return item;    // we will update this existing object
    ENDFOR;
    return NULL;
}


//-----------------------------------------------------------------------------
// 
//
int
Obj::Export() 
{
    int res;

    if(exportGeom || exportMapp){
        res = AddMesh();
        if(res<0) return res;
    }

    if(exportSkin) {
        res = AddBones();
        if(res<0) return res;
    }

    if(exportMove || exportMrph || exportSkin) {
        res = AddAnimations();
        if(res<0) return res;
    }

    return 0;
}

