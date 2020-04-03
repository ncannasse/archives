//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_morph.cpp
//*     Author  : M.B.
//*     Desc    : Get Morph animations from 3DSMax
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_obj.h"        // Obj 
#include "include/wm3.h"            // MorphR3
#include "include/mtw_error.h"      // ERRs


//-----------------------------------------------------------------------------
// Get object morphing : retrieve keys of animation
// precond : modifier not null
// TODO : convertir morph lineaire en bezier
//
int
Obj::GetMorph(Modifier* modifier)
{
    int res;

    Control* controller = GetMorphController((MorphR3*)modifier);
    IKeyControl* ikeys = GetKeyControlInterface(controller);
    if(!ikeys)
        return E_MORPH_CTRL;
    int nKeys = ikeys->GetNumKeys();
    if(nKeys==0)
        return W_MORPH_EMPTY;    // no key but a morph modifier : it's ok

    mrphKeys = new WList<MTW*>();

    if(controller->ClassID() == Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID, 0)) {
        IBezFloatKey bezKey;
        for(int i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &bezKey);
            res = GetMorphKey(bezKey.time);
            if(res<0) return res;
        }
        return 0;
    }
    else if(controller->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0)) {
        ILinFloatKey linKey;
        for(int i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &linKey);
            res =GetMorphKey(linKey.time);
            if(res<0) return res;
        }
        return 0;
    }
    return E_MORPH_CTRL;
}


//-----------------------------------------------------------------------------
// 
//
Control*
Obj::GetMorphController(MorphR3* morph)
{
    for(int i=0;i<MR3_NUM_CHANNELS;i++)
    {
        if(morph->chanBank[i].mActive && morph->chanBank[i].mConnection!=NULL)
        {
            INode* morphedNode = morph->chanBank[i].mConnection;
            //Matrix3 tm1 = morphedNode->GetNodeTM(0);
            //Matrix3 tm2 = morphedNode->GetObjectTM(0);
            return morph->chanBank[i].cblock->GetController(0);
        }
    }
    return NULL;
}


//-----------------------------------------------------------------------------
// 
// TODO : object en param?
//
int
Obj::GetMorphKey(TimeValue time)
{
    Object* obj = node->EvalWorldState(time).obj;
    TriObject* triobj = ((TriObject*)obj->ConvertToType(time, triObjectClassID));
    Mesh* curMesh = &triobj->GetMesh();

    int nCurVerts = curMesh->getNumVerts();
    Point3* data = new Point3[nVerts];

    // check object/morph compatibility
    if(merge && nVerts!=nCurVerts)
        return E_MERGE_MORPH;

    Point3 point;
    for(int i=0; i<nVerts; i++)
    {
        VECTOR4 vec4(0,0,0,1);
        point = curMesh->getVert(i);
        memcpy(vec4, point, sizeof(Point3));
        vtransform(&vec4, &vec4, matrix);
        memcpy(&data[i], vec4, sizeof(Point3));
    }

    MTW* morphTag = new MTW(MAKETAG(Mrkf));
    morphTag->AddChild(new MTW(MAKETAG(fram), 0, (void*)time));
    morphTag->AddChild(new MTW(MAKETAG(DATA), nVerts*sizeof(Point3), data));
    mrphKeys->Add(morphTag);

    if(triobj!=obj)
        triobj->DeleteMe();

    return 0;
}

