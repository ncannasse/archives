//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_obj.h
//*     Author  : M.B.
//*     Desc    : Obj corresponds to current 3ds object being exported
//*     Date    : 19.09.03
//*
//*****************************************************************************

#ifndef __MTW_OBJ_H__
#define __MTW_OBJ_H__

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <max.h>
#include <iparamb2.h>               // ClassDesc2 needed by iskin.h
#include <iskin.h>                  // ISkin

#include "../../m3dse/mdx.h"        // MATRIX
#include "../../common/mtw.h"		// MTW + TAG

#include "include/wm3.h"            // MorphR3
#include "include/mtw_log.h"        // Log


class Obj;

#include "include/maxtwin.h"

///////////////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p) { if((p)) { delete (p); (p)=NULL; } }


//*****************************************************************************

class Obj
{

public:

    //-------------------------------------------------------------------------
    // Constructor & Destructor
    //-------------------------------------------------------------------------
    Obj(INode* node);
    ~Obj();

    //-------------------------------------------------------------------------
    // Main functions
    //-------------------------------------------------------------------------
    MTW*    Init(MTW* parentTag);
    int     Export();


private:

    INode*  node;       // src struct
    MTW*    objtTag;    // dest struct

    Object* obj;
    TriObject* triObj;

    bool    exportGeom, exportMapp, exportMove, exportMrph, exportSkin;

    int     nVerts, nExtendedVerts, nBones;
    bool    hasMapp, hasBones, merge;
    MATRIX  *matrix;

    WList<WORD>             geomIndexes, mappIndexes, indx, vidx;
    WList<IBezPoint3Key*>*  mposKeys;
    WList<ITCBRotKey*>*     mrotKeys;
    WList<Quat*>*           mrotQuats;
    WList<IBezScaleKey*>*   msclKeys;

    WList<MTW*>*            mrphKeys;
    WList<MTW*>*            skinKeys;


    //-------------------------------------------------------------------------
    // Geometry, mapping, indexing
    //-------------------------------------------------------------------------
    int     AddMesh();
    void    AddGeometry(Mesh* mesh);
    int     AddMapping(Mesh* mesh, Mtl* material);
    int     ComputeExtendedMesh(Mesh* mesh);
    void    ComputeSimpleMesh(Mesh* mesh);
    void    CleanGeometry();
    void    CleanMapping();

    MTW*    GetObjectByName(MTW* parent, char* name);
    MTW*    CreateGeometryTag(Mesh* mesh);
    MTW*    CreateMappingTag(Mesh* mesh);
    MTW*    CreateIndexingTag();
    MTW*    CreateReindexingTag();
    MTW*    CreateTextureTag(Mtl* material);

    //-------------------------------------------------------------------------
    // Bones
    //-------------------------------------------------------------------------    
    int     AddBones();
    void    CleanBones();

    Modifier* GetSkinModifier();
    MTW*    CreateBonePositionsTag(ISkin* skin, INode** bones);
    MTW*    CreateBoneParentsTag(ISkin* skin, INode** bones);
    MTW*    CreateBoneWeightsTag(ISkinContextData* skinContext);

    //-------------------------------------------------------------------------
    // Anims
    //-------------------------------------------------------------------------    
    int     AddAnimations();
    void    AddFrameTags(MTW* animTag);
    void    AddSkin(WList<MTW*>* frames);
    void    AddMorph(WList<MTW*>* frames);
    DWORD   AddMove(WList<MTW*>* frames);
    void    CleanAnim(WList<MTW*>* frames, TAG name);
    MTW*    GetAnimByName(char* animName);
    MTW*    CreateAnimationTag(char* name, int nFrames);
    MTW*    GetFramTag(WList<MTW*>* list, DWORD time);

    // move
    int     GetMove();
    int     GetPosKeys();
    int     GetRotKeys();
    int     GetSclKeys();
    bool    SamePosKeys();
    bool    SameRotKeys();
    bool    SameSclKeys();
    void    MergeMoveKeys();
    void    InterpolateMoveKeys(WList<DWORD>* posKeys, WList<DWORD>* rotKeys, WList<DWORD>* sclKeys);
    void    InterpolatePosKey(IBezPoint3Key* key);
    void    InterpolateRotKey(ITCBRotKey* key, Quat* quat);
    void    InterpolateSclKey(IBezScaleKey* key);
    void    MergePosKeys(WList<IBezPoint3Key*>* iKeys);
    void    MergeRotKeys(WList<ITCBRotKey*>* iKeys, WList<Quat*>* iQuats);
    void    MergeSclKeys(WList<IBezScaleKey*>* iKeys);

    // morph
    int      GetMorph(Modifier* morph);
    Control* GetMorphController(MorphR3* morph);
    int      GetMorphKey(TimeValue time);

    // skin
    int     GetSkin(Modifier* skin);

    //-------------------------------------------------------------------------
    // Utils
    //-------------------------------------------------------------------------
    bool    GetExportProperty(TSTR property);
    char*   GetAnimName();
    bool    RemoveExistingTag(MTW* parent, TAG child);

};


#endif //__MTW_OBJ_H__

