//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_skin.cpp
//*     Author  : M.B.
//*     Desc    : Get bone & skin information from 3DSMax
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_obj.h"
#include "include/mtw_error.h"


///////////////////////////////////////////////////////////////////////////////
//
struct Weight
{
    WORD    vertId;
    WORD    boneId;
    float   weight;
    Weight(WORD v, WORD b, float w): vertId(v), boneId(b), weight(w) {}
    Weight() {}
};



//-----------------------------------------------------------------------------
// 
// TODO : verif createTags ?
//
int
Obj::AddBones()
{
    Modifier *modifier = GetSkinModifier();
    if(!modifier)
        return E_NO_BONES;

    ISkin *skin = (ISkin*)modifier->GetInterface(I_SKIN);
	if(!skin)
		return E_SKIN_INTERFACE;

    ISkinContextData *skinContext = (ISkinContextData*)skin->GetContextInterface(node);
	if(!skinContext) {
		modifier->ReleaseInterface(I_SKIN, skin);
		return E_SKIN_INTERFACE;
	}

    int nCurBones = skin->GetNumBones();
    if(merge && hasBones)
    {
        if(nCurBones!=nBones)
            return E_MERGE_SKIN;
        //else
            //log->Print("Found compatible skeleton");

        modifier->ReleaseInterface(I_SKIN, skin);
        return 0;
    }

    nBones = nCurBones;
    hasBones = true;

    INode** bones = new INode*[nBones];

    MTW *skelTag = new MTW(MAKETAG(Skel));
    skelTag->AddChild(CreateBonePositionsTag(skin, bones));
    skelTag->AddChild(CreateBoneParentsTag(skin, bones));
    MTW* wghtTag = CreateBoneWeightsTag(skinContext);
    if(!wghtTag)
        return E_SKIN_WEIGHTS;
    skelTag->AddChild(wghtTag);
    objtTag->AddChild(skelTag);

    modifier->ReleaseInterface(I_SKIN, skin);
    return 0;
}


//-----------------------------------------------------------------------------
// Delete bone tags
//
void
Obj::CleanBones()
{
    RemoveExistingTag(objtTag, MAKETAG(Skel));
    /*
    RemoveExistingTag(skelTag, MAKETAG(PIDX));
    RemoveExistingTag(skelTag, MAKETAG(REFP));
    RemoveExistingTag(skelTag, MAKETAG(WGHT));
    */
}


//-----------------------------------------------------------------------------
// return the object skin modifier, NULL if does not exist
//
Modifier*
Obj::GetSkinModifier()
{
    Object* objt = node->GetObjectRef();
	while(objt && objt->SuperClassID()==GEN_DERIVOB_CLASS_ID)
    {
		IDerivedObject* derivObj = (IDerivedObject*)objt;
		for(int i=0;i<derivObj->NumModifiers();i++) 
        {
            Modifier* modifier = derivObj->GetModifier(i);
            if(modifier->ClassID()==SKIN_CLASSID)
                return modifier;
        }
		objt = derivObj->GetObjRef();
	}
    return NULL;
}


//-----------------------------------------------------------------------------
// bone reference position : matrix of each bone
// TODO : verif scale de biped ?
//
MTW*
Obj::CreateBonePositionsTag(ISkin* skin, INode** bones)
{
    int i;
    MATRIX* tagData = new MATRIX[nBones];
    MATRIX mBone;
    Matrix3 m;
    for(i=0; i<nBones; i++)
    {
        INode *bone = skin->GetBone(i);
        char* name = bone->GetName();
        bones[i] = bone;
        skin->GetBoneInitTM(bone, m);
        MATRIX mBone = Maxtwin::MaxToD3DMatrix(&m);
        mmult(&mBone, &mBone, matrix);
        // position du bone dans le repere de l'objet
        minv(&mBone, &mBone);
        tagData[i] = mBone;
    }
    return new MTW(MAKETAG(REFP), nBones*sizeof(MATRIX), tagData);
}


//-----------------------------------------------------------------------------
// Get the parent index of each bone
//
MTW*
Obj::CreateBoneParentsTag(ISkin* skin, INode** bones)
{
    WORD* tagData = new WORD[nBones];
    for(int i=0; i<nBones; i++)
    {
        DWORD j=0;
        bool stop = false;
        tagData[i] = 0xffff;      // init
        while(j<nBones && !stop) 
        {
            INode* bone = skin->GetBone(j);
            if(bones[i]->GetParentNode()==bone) {
                // j parent index of 'bones[i]'
                tagData[i] = (WORD)j;
                stop = true;
            }
            j++;
        }
    }
    return new MTW(MAKETAG(PIDX), nBones*sizeof(WORD), tagData);
}


//-----------------------------------------------------------------------------
// Get the weight assigned to each couple (vertex,bone)
//
MTW*
Obj::CreateBoneWeightsTag(ISkinContextData* skinContext)
{
    WList<Weight*> weightList;
    int i, j;
    int nPoints = skinContext->GetNumPoints();

	for(i=0; i<nPoints; i++)
	{
		int nAssignedBones = skinContext->GetNumAssignedBones(i);
        if(nAssignedBones==0)
            return NULL;
		for(j=0; j<nAssignedBones; j++)
		{
			int assignedBone = skinContext->GetAssignedBone(i, j);
			if(assignedBone<0)
				continue;

            float weight = skinContext->GetBoneWeight(i, j);
            if(weight!=0.f)
                weightList.Add(new Weight(i, assignedBone, weight));
		}
	}
    // convert list to tab, to cast to void*
    i=0;
    int nWeights = weightList.Length();
    Weight* weights = new Weight[nWeights];
    FOREACH(Weight*, weightList)
       weights[i++] = *item;                // TODO : éviter cette recopie ?
    ENDFOR;
    weightList.Delete();

    return new MTW(MAKETAG(WGHT), nWeights*sizeof(Weight), weights);
}


//-----------------------------------------------------------------------------
// Get the object skinning : bone movements for each frame
// TODO : keyControlInterface instead of mquaternion ?
//
int
Obj::GetSkin(Modifier* modifier)
{
    ISkin* skin = (ISkin*)modifier->GetInterface(I_SKIN);
	if(!skin)
		return E_SKIN_INTERFACE;

    skinKeys = new WList<MTW*>();

    int i, time;
	int delta = GetTicksPerFrame();
    int tstart = Maxtwin::startTime;
    int tend = Maxtwin::endTime;

    int tagSize = nBones*7;

    for(time=tstart; time<=tend; time+=delta)
    {
        int offset=0;
        float* tagData = new float[tagSize];
        for(i=0;i<nBones;i++)
        {
			INode*  bone = skin->GetBone(i);
			//Matrix3 m = bone->GetNodeTM(time);
            //Control* control = bone->GetTMController()->GetPositionController();
            //Control* control = bone->GetTMController()->GetRotationController();

            Matrix3 m = bone->GetNodeTM(time);
            MATRIX  boneMat = Maxtwin::MaxToD3DMatrix(&m);

            VECTOR  vector(boneMat._41, boneMat._42, boneMat._43);
			QUATERNION quaternion;
			mquaternion(&quaternion, &boneMat);
			qnormalize(&quaternion, &quaternion);

            memcpy(tagData+offset, vector, sizeof(VECTOR));
            memcpy(tagData+offset+3, quaternion, sizeof(QUATERNION));
            offset+=7;
		}
        MTW* skinTag = new MTW(MAKETAG(Skkf));
        skinTag->AddChild(new MTW(MAKETAG(fram), 0, (void*)time));
        skinTag->AddChild(new MTW(MAKETAG(DATA), tagSize*sizeof(float), tagData));
        skinKeys->Add(skinTag);
    }
    modifier->ReleaseInterface(I_SKIN, skin);
    return 0;
}

