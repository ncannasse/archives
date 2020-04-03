//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_anim.cpp
//*     Author  : M.B.
//*     Desc    : export animations
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_obj.h"
#include "include/mtw_error.h"


///////////////////////////////////////////////////////////////////////////////
// Defines
//
#define FLAG(value, flag)   ((value)&(flag))
#define FLAG_POS 0x00000001
#define FLAG_ROT 0x00000010
#define FLAG_SCL 0x00000100
#define ADDFLAG(value, flag) ( (value)|=(flag) )

#define PROP_ANM_NAME "anim_name"


//-----------------------------------------------------------------------------
// Export prs controllers, morph & skin modifiers
// precond : exportMove || exportMrph || exportSkin && node obj is a geom obj
//
int
Obj::AddAnimations()
{
    int res;

    MTW *animTag=NULL;
    char *animName = GetAnimName();
    if(!animName)
        return E_NO_ANIM_NAME;

    int nFrames = Maxtwin::GetNFrames();
    if(animTag) {
        animTag = GetAnimByName(animName);
        if(animTag)
            if(animTag->Child(MAKETAG(nfrm))->number!=nFrames)
                return E_MERGE_ANIM;
    }
    if(!animTag) {
        animTag = CreateAnimationTag(animName, nFrames);
        objtTag->AddChild(animTag);
    }

    if(exportMove) {
        if(node->IsAnimated()) {
            res = GetMove();
            if(res<0) return res;
        }
        else
            return E_NO_ANIMATION;
    }

    if(exportMrph || exportSkin) {

        Object* objt = node->GetObjectRef();
	    while(objt && objt->SuperClassID()==GEN_DERIVOB_CLASS_ID)
        {
		    IDerivedObject* derivObj = (IDerivedObject*)objt;
		    for(int i=0;i<derivObj->NumModifiers();i++)
            {
			    Modifier* modifier = derivObj->GetModifier(i);

                if(exportMrph)
                    if(modifier->ClassID()==MR3_CLASS_ID) {
                        res = GetMorph(modifier);
                        if(res<0) return res;
                    }

                if(exportSkin)
                    if(modifier->ClassID()==SKIN_CLASSID) {
                        res = GetSkin(modifier);
                        if(res<0) return res;
                    }
            }
		    objt = derivObj->GetObjRef();
	    }
    }

    if(!mposKeys && !mrotKeys && !msclKeys && !mrphKeys && !skinKeys)
        return W_EMPTY_MODIFIER;

    AddFrameTags(animTag);

    return 0;
}


//-----------------------------------------------------------------------------
// Retrieve the anim name property of the node. (see properties, user defined)
//
char*
Obj::GetAnimName()
{
    TSTR name;
    if(!node->GetUserPropString(PROP_ANM_NAME, name))
        return NULL;

    int length = strlen((char*)name);
    char* copy = new char[length+1];
    memcpy(copy, name, length*sizeof(char));
    copy[length]='\0';
    return copy;
}


//-----------------------------------------------------------------------------
// Retrieve an object anim by its name
// return the anim tag of the specified anim, NULL if it does not exist
//
MTW*
Obj::GetAnimByName(char* animName)
{
    WList<MTW*> animTags = objtTag->Childs(MAKETAG(Anim));
    FOREACH(MTW*, animTags)
        MTW *nameTag = item->Child(MAKETAG(NAME));
        if(nameTag && nameTag->data && strcmp((char*)nameTag->data, animName)==0)
            return item;
    ENDFOR;
    return NULL;
}


//-----------------------------------------------------------------------------
// 
//
MTW*
Obj::CreateAnimationTag(char* animName, int nFrames)
{
    MTW *tag = new MTW(MAKETAG(Anim));
    tag->AddChild(new MTW(MAKETAG(NAME), strlen(animName)+1, animName));
    tag->AddChild(new MTW(MAKETAG(nfrm), 0, (void*)nFrames));
    tag->AddChild(new MTW(MAKETAG(mflg), 0, 0));
    return tag;
}


//-----------------------------------------------------------------------------
// Retrieve or create the tag corresponding to the specified frame
//
MTW*
Obj::GetFramTag(WList<MTW*>* framTags, DWORD time)
{
    DWORD frame = time/GetTicksPerFrame();
    FOREACH(MTW*, *framTags)
        if(item->Child(MAKETAG(frnb))->number==frame)
            return item;
    ENDFOR;
    MTW* FramTag = new MTW(MAKETAG(Fram));
    FramTag->AddChild(new MTW(MAKETAG(frnb), 0, (void*)frame));
    framTags->Add(FramTag);
    return FramTag;
}


//-----------------------------------------------------------------------------
// Merge all anim keys by frame number in frame tags
//
void
Obj::AddFrameTags(MTW* animTag)
{
    // copy & remove anim frames
    WList<MTW*> framTags = animTag->Childs(MAKETAG(Fram));
    if(!framTags.Empty())
        FOREACH(MTW*, *animTag->childs)
            if(item->tag==MAKETAG(Fram))
                animTag->RemoveChild(item); // no need to free : just break the link
        ENDFOR;

    AddSkin(&framTags);
    AddMorph(&framTags);
    DWORD moveFlag = AddMove(&framTags);

    MTW* mflgTag = animTag->Child(MAKETAG(mflg));
    mflgTag->number = moveFlag;

    // normalize frames : remove frame if no or just 1 child (frnb)
    FOREACH(MTW*, framTags)
        if(item->Childs().Length()<2)
            framTags.Remove(item); // no need to free item (frnb is DWORD)
    ENDFOR;

    // add frames to current anim
    FOREACH(MTW*, framTags)
        animTag->AddChild(item);
    ENDFOR;
}


//-----------------------------------------------------------------------------
// Merge skin keys with frame tags
//
void
Obj::AddSkin(WList<MTW*>* framTags) 
{
    if(skinKeys) {
        CleanAnim(framTags, MAKETAG(SKEL));
        FOREACH(MTW*, *skinKeys)
            MTW* framTag = item->Child(MAKETAG(fram));
            MTW* dataTag = item->Child(MAKETAG(DATA));
            MTW* FramTag = GetFramTag(framTags, framTag->number);
            FramTag->AddChild(new MTW(MAKETAG(SKEL), dataTag->size, dataTag->data));
        ENDFOR;
    }
}


//-----------------------------------------------------------------------------
// Merge morph keys with frame tags
//
void
Obj::AddMorph(WList<MTW*>* framTags) 
{
    if(mrphKeys) {
        CleanAnim(framTags, MAKETAG(MRPH));
        FOREACH(MTW*, *mrphKeys)
            MTW* framTag = item->Child(MAKETAG(fram));
            MTW* dataTag = item->Child(MAKETAG(DATA));
            // existing frame to merge with current key frame ?
            MTW* FramTag = GetFramTag(framTags, framTag->number);            
            FramTag->AddChild(new MTW(MAKETAG(MRPH), dataTag->size, dataTag->data));
        ENDFOR;
    }
}


//-----------------------------------------------------------------------------
// Merge move keys with frame tags, return a flag specifiing the move type
//
DWORD
Obj::AddMove(WList<MTW*>* framTags) 
{
    if(mposKeys || mrotKeys || msclKeys)
        CleanAnim(framTags, MAKETAG(MOVE));
        // can't merge 2 moves from 2 different files

    DWORD moveFlag=0, rotOffset=0, sclOffset=0;
    if(mposKeys)
    {
        ADDFLAG(moveFlag, FLAG_POS);
        rotOffset=sclOffset=2*sizeof(Point3);
        FOREACH(IBezPoint3Key*, *mposKeys)
            MTW* FramTag = GetFramTag(framTags, (DWORD)item->time);
            Point3* data = new Point3[2];
            data[0] = item->val;
            data[1] = item->intan;  // TODO : a verif
            FramTag->AddChild(new MTW(MAKETAG(MOVE), 2*sizeof(Point3), data));
        ENDFOR;
    }

    if(mrotKeys)
    {
        ADDFLAG(moveFlag, FLAG_ROT);
        int i=0;
        sclOffset+=sizeof(Quat);
        FOREACH(ITCBRotKey*, *mrotKeys)
            MTW* FramTag = GetFramTag(framTags, (DWORD)item->time);
            // existing mouvement at this key frame ?
            MTW* moveTag = FramTag->Child(MAKETAG(MOVE));
            if(moveTag) {
                moveTag->size += sizeof(Quat);
                char* data = new char[moveTag->size];
                memcpy(data, moveTag->data, moveTag->size); // keep prevoius data (position)
                memcpy(data+rotOffset, mrotQuats->AtIndex(i), sizeof(Quat)); // add new data (rotation) behind
                moveTag->data = data;
            }
            else {
                // mouvement is only rotation
                Quat* data = new Quat(*mrotQuats->AtIndex(i));
                FramTag->AddChild(new MTW(MAKETAG(MOVE), sizeof(Quat), data));
            }
            i++;
        ENDFOR;
    }

    if(msclKeys)
    {
        ADDFLAG(moveFlag, FLAG_SCL);
        FOREACH(IBezScaleKey*, *msclKeys)
            MTW* FramTag = GetFramTag(framTags, (DWORD)item->time);
            MTW* moveTag = FramTag->Child(MAKETAG(MOVE));
            if(moveTag) {
                moveTag->size += 2*sizeof(Point3);
                char* data = new char[moveTag->size];
                memcpy(data, moveTag->data, moveTag->size); // keep prevoius data (position and/or rotation)
                memcpy(data+sclOffset, &item->val.s, sizeof(Point3)); // add new data (scale)
                memcpy(data+sclOffset+sizeof(Point3), &item->intan, sizeof(Point3)); // add new data (scale)
                moveTag->data = data;
            }
            else {
                // mouvement is only scale
                Point3* data = new Point3[2];
                data[0] = item->val.s;
                data[1] = item->intan;
                FramTag->AddChild(new MTW(MAKETAG(MOVE), 2*sizeof(Point3), data));
            }
        ENDFOR;
    }

    return moveFlag;
}


//-----------------------------------------------------------------------------
// Delete all tags of a given type (name) in the specified anim
//
void
Obj::CleanAnim(WList<MTW*> *framTags, TAG name)
{
    if(!framTags)
        return;

    FOREACH(MTW*, *framTags)
        RemoveExistingTag(item, name);
    ENDFOR;
}

