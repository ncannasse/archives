//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_move.cpp
//*     Author  : M.B.
//*     Desc    : Get Move info (position, rotation, sacle) from 3DSMax
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_obj.h"
#include "include/mtw_error.h"


//-----------------------------------------------------------------------------
// export the data of the controller. For TCB, Bezier and Linear 
// keyframe PRS controllers the position, rotation and scale values
// are displayed in the local space of the controller
//
int
Obj::GetMove()
{
    int res;
    res = GetPosKeys();
    if(res<0) return res;

    res = GetRotKeys();
    if(res<0) return res;

    res = GetSclKeys();
    if(res<0) return res;

    MergeMoveKeys();
    return 0;
}


//-----------------------------------------------------------------------------
// TODO : convert linear move to bezier
//
int
Obj::GetPosKeys()
{
    Control* control = node->GetTMController()->GetPositionController();
    IKeyControl* ikeys = GetKeyControlInterface(control);
    if(!ikeys)
        return E_POS_CTRL;

    int i, nKeys = ikeys->GetNumKeys();
    if(nKeys==0)
        return 0; // warning ?

    mposKeys = new WList<IBezPoint3Key*>();

    if(control->ClassID() == Class_ID(HYBRIDINTERP_POSITION_CLASS_ID, 0)) {
        Point3 refPos;
        IBezPoint3Key key;
        for(i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &key);
            mposKeys->Add(new IBezPoint3Key(key));
        }
        if(SamePosKeys()) {
            mposKeys->Delete();
            mposKeys = NULL;
        }
        return 0;
    }

    /*
    else if(control->ClassID() == Class_ID(LININTERP_POSITION_CLASS_ID, 0)) {
        ILinPoint3Key linPosKey;
        IBezPoint3Key bezPosKey;
        for(i=0; i<nKeys; i++) 
        {
            ikeys->GetKey(i, &linPosKey);
            bezPosKey.flags = linPosKey.flags;
            bezPosKey.time = linPosKey.time;
            bezPosKey.val = linPosKey.val;
            //bezPosKey.intan = 

            //ExportPositionKey(linPosKey.time, &linPosKey.val);
        }
    }
    */

    return E_POS_BEZ;
}


//-----------------------------------------------------------------------------
// TODO : tcb rotation only ?
//
int
Obj::GetRotKeys()
{
    int i;
    Control* control = node->GetTMController()->GetRotationController();
    IKeyControl* ikeys = GetKeyControlInterface(control);
    if(!ikeys)
        return E_ROT_CTRL;

    int nKeys = ikeys->GetNumKeys();
    if(nKeys==0)
        return 0;

    mrotKeys = new WList<ITCBRotKey*>();
    mrotQuats = new WList<Quat*>();

    Quat newQuat, prevQuat;
    if(control->ClassID() == Class_ID(TCBINTERP_ROTATION_CLASS_ID, 0)) {
        ITCBRotKey key;
        for(i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &key);
            newQuat = QFromAngAxis(key.val.angle, key.val.axis);
            if(i>0)
                newQuat = prevQuat * newQuat;
            prevQuat = newQuat;

            mrotKeys->Add(new ITCBRotKey(key));
            mrotQuats->Add(new Quat(prevQuat));
        }
        if(SameRotKeys()) {
            mrotKeys->Delete();
            mrotKeys = NULL;
        }
        return 0;
    }

    /* else if(control->ClassID() == Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID, 0)) {
        IBezQuatKey bezRotKey;
        for(i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &bezRotKey);
            newQuat = bezRotKey.val;
            if(i) newQuat = prevQuat * newQuat;
            prevQuat = newQuat;
            ExportRotationKey(bezRotKey.time, &newQuat);
        }
    }
    else if(control->ClassID() == Class_ID(LININTERP_ROTATION_CLASS_ID, 0)) {
        ILinRotKey linRotKey;
        for(i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &linRotKey);          
            newQuat = linRotKey.val;
            if(i) newQuat = prevQuat * newQuat;
            prevQuat = newQuat;
            ExportRotationKey(linRotKey.time, &newQuat);
        }
    } */

    return E_ROT_TCB;
}


//-----------------------------------------------------------------------------
// TODO : convert linear keys to bezier
//
int
Obj::GetSclKeys()
{
    int i;
    Control *control = node->GetTMController()->GetScaleController();
    IKeyControl* ikeys = GetKeyControlInterface(control);
    if(!ikeys)
        return E_SCL_CTRL;

    int nKeys = ikeys->GetNumKeys();
    if(nKeys==0)
        return 0;

    msclKeys = new WList<IBezScaleKey*>();

    if(control->ClassID() == Class_ID(HYBRIDINTERP_SCALE_CLASS_ID, 0)) 
    {
        IBezScaleKey key;
        for(i=0; i<nKeys; i++) {
            ikeys->GetKey(i, &key);              
            msclKeys->Add(new IBezScaleKey(key));
        }
        if(SameSclKeys()) {
            msclKeys->Delete();
            msclKeys = NULL;
        }
        return 0;
    }

    /*else if(control->ClassID() == Class_ID(LININTERP_SCALE_CLASS_ID, 0))
    {
        ILinScaleKey* key = new ILinScaleKey;
        for(i=0; i<nKeys; i++) {
            ikeys->GetKey(i, key);
            //ExportScaleKey(linScaleKey.time, &linScaleKey.val);
            msclKeys->Add(key);
        }
    }*/

    return E_SCL_BEZ;
}


//-----------------------------------------------------------------------------
// 
//
bool
Obj::SamePosKeys()
{
    DWORD i=0;
    Point3 refData;
    FOREACH(IBezPoint3Key*, *mposKeys)
        if(i>0)
            if(!refData.Equals(item->val, 1E-3f))
                return false;
        refData = item->val;
        i++;
    ENDFOR;
    return true;
}


//-----------------------------------------------------------------------------
// 
//
bool
Obj::SameRotKeys()
{
    DWORD i=0;
    Quat refData;
    FOREACH(Quat*, *mrotQuats)
        if(i>0)
            if(!refData.Equals(*item, 1E-3f))
                return false;
        refData = *item; 
        i++;
    ENDFOR;
    return true;
}


//-----------------------------------------------------------------------------
// 
//
bool
Obj::SameSclKeys()
{
    DWORD i=0;
    ScaleValue refData;
    FOREACH(IBezScaleKey*, *msclKeys)
        if(i>0)
            if(!refData.s.Equals(item->val.s, 1E-3f) || !refData.q.Equals(item->val.q, 1E-3f))
                return false;
        refData = item->val;
        i++;
    ENDFOR;
    return true;
}


//-----------------------------------------------------------------------------
//
void
Obj::MergeMoveKeys()
{
    WList<DWORD> *posFrames=NULL, *rotFrames=NULL, *sclFrames=NULL;
    WList<DWORD> posFramesToAdd, rotFramesToAdd, sclFramesToAdd;

    // compute frame lists
    if(mposKeys) {
        posFrames = new WList<DWORD>();
        FOREACH(IBezPoint3Key*, *mposKeys)
            posFrames->Add(item->time);
        ENDFOR;
    }
    if(mrotKeys) {
        rotFrames = new WList<DWORD>();
        FOREACH(ITCBRotKey*, *mrotKeys)           
            rotFrames->Add(item->time);
        ENDFOR;
    }
    if(msclKeys) {
        sclFrames = new WList<DWORD>();
        FOREACH(IBezScaleKey*, *msclKeys)
            sclFrames->Add(item->time);
        ENDFOR;
    }

    // search for posFrames in the other lists
    if(posFrames)
    FOREACH(DWORD, *posFrames)
        if(rotFrames && !rotFrames->Exists(item))
            rotFramesToAdd.Add(item);
        if(sclFrames && !sclFrames->Exists(item))
            sclFramesToAdd.Add(item);
    ENDFOR;

    // search for rotFrames in the other lists
    if(rotFrames)
    FOREACH(DWORD, *rotFrames)
        if(posFrames && !posFrames->Exists(item))
            posFramesToAdd.Add(item);
        if(sclFrames && !sclFrames->Exists(item))
            sclFramesToAdd.Add(item);
    ENDFOR;

    // search for sclFrames in the other lists
    if(sclFrames)
    FOREACH(DWORD, *sclFrames)
        if(posFrames && !posFrames->Exists(item))
            posFramesToAdd.Add(item);
        if(rotFrames && !rotFrames->Exists(item))
            rotFramesToAdd.Add(item);
    ENDFOR;

    InterpolateMoveKeys(&posFramesToAdd, &rotFramesToAdd, &sclFramesToAdd);
}


//-----------------------------------------------------------------------------
//
// precond : at least 2 keys in each list, or list.length==0
//
void
Obj::InterpolateMoveKeys(WList<DWORD>* posFrames, WList<DWORD>* rotFrames, WList<DWORD>* sclFrames)
{
    if(posFrames->Length()>0)
    {
        WList<IBezPoint3Key*> iPos;
        FOREACH(DWORD, *posFrames)
            IBezPoint3Key* key = new IBezPoint3Key;
            key->time = item;
            InterpolatePosKey(key);
            iPos.Add(key);
        ENDFOR;
        MergePosKeys(&iPos);
    }
    if(rotFrames->Length()>0)
    {
        WList<ITCBRotKey*> iRot;
        WList<Quat*> iQuat;
        FOREACH(DWORD, *rotFrames)
            Quat* quat = new Quat;
            ITCBRotKey* key = new ITCBRotKey;
            key->time = item;
            InterpolateRotKey(key, quat);
            iRot.Add(key);
            iQuat.Add(quat);
        ENDFOR;
        MergeRotKeys(&iRot, &iQuat);
    }
    if(sclFrames->Length()>0)
    {
        WList<IBezScaleKey*> iScl;
        FOREACH(DWORD, *sclFrames)
            IBezScaleKey* key = new IBezScaleKey;
            key->time = item;
            InterpolateSclKey(key);
            iScl.Add(key);
        ENDFOR;
        MergeSclKeys(&iScl);
    }
}


//-----------------------------------------------------------------------------
//
//
void
Obj::InterpolatePosKey(IBezPoint3Key* newKey)
{
    IBezPoint3Key *key1=NULL, *key2=NULL;
    FOREACH(IBezPoint3Key*, *mposKeys);
        if(item->time < newKey->time)
            key1 = item;
        else {
            key2 = item;
            break;
        }
    ENDFOR;

    // cyclic interp
    if(!key1)
        key1 = mposKeys->Last();
    if(!key2)
        key2 = mposKeys->First();

    float coef = (float)fabs((newKey->time-key1->time)/(float)(key2->time - key1->time));
    vlerp((VECTOR*)&newKey->val, (VECTOR*)&key1->val, (VECTOR*)&key2->val, coef);
    vlerp((VECTOR*)&newKey->intan, (VECTOR*)&key1->intan, (VECTOR*)&key2->intan, coef);
}


//-----------------------------------------------------------------------------
// 
//
void
Obj::InterpolateRotKey(ITCBRotKey* newKey, Quat* quat)
{
    ITCBRotKey *key1=NULL, *key2=NULL;
    int i=0, index1, index2;
    FOREACH(ITCBRotKey*, *mrotKeys);
        if(item->time < newKey->time) {
            key1 = item;
            index1 = i;
        }
        else {
            key2 = item;
            index2 = i;
            break;
        }
        i++;
    ENDFOR;

    // cyclic interp
    if(!key1){
        index1 = 0;
        key1 = mrotKeys->Last();
    }
    if(!key2){
        index2 = mrotQuats->Length()-1;
        key2 = mrotKeys->First();
    }

    float coef = (float)fabs((newKey->time - key1->time)/(float)(key2->time - key1->time));
    Quat* quat1 = mrotQuats->AtIndex(index1);
    Quat* quat2 = mrotQuats->AtIndex(index2);
    qslerp((QUATERNION*)quat, (QUATERNION*)quat1, (QUATERNION*)quat2, coef);
}


//-----------------------------------------------------------------------------
// 
//
void
Obj::InterpolateSclKey(IBezScaleKey* newKey)
{
    IBezScaleKey *key1=NULL, *key2=NULL;
    FOREACH(IBezScaleKey*, *msclKeys);
        if(item->time < newKey->time)
            key1 = item;
        else {
            key2 = item;
            break;
        }
    ENDFOR;

    // cyclic interp
    if(!key1)
        key1 = msclKeys->Last();
    if(!key2)
        key2 = msclKeys->First();

    float coef = (float)fabs((newKey->time - key1->time)/(float)(key2->time - key1->time));
    vlerp((VECTOR*)&newKey->val.s, (VECTOR*)&key1->val.s, (VECTOR*)&key2->val.s, coef);
    qslerp((QUATERNION*)&newKey->val.q, (QUATERNION*)&key1->val.q, (QUATERNION*)&key2->val.q, coef);
}


//-----------------------------------------------------------------------------
// return a new list, sorting the 2 ones (from controller & interpolated) by frame number
//
void
Obj::MergePosKeys(WList<IBezPoint3Key*>* iKeys)
{
    WList<IBezPoint3Key*>* list = new WList<IBezPoint3Key*>();
    WList<IBezPoint3Key*>::LCel *ctrlKey = mposKeys->Begin();
    WList<IBezPoint3Key*>::LCel *itplKey = iKeys->Begin();
    IBezPoint3Key *cKey, *iKey;
    while(ctrlKey && itplKey)
    {
        cKey = (IBezPoint3Key*)ctrlKey->item;
        iKey = (IBezPoint3Key*)itplKey->item;
        if(cKey->time < iKey->time) {
            list->Add(new IBezPoint3Key(*cKey));
            ctrlKey = ctrlKey->next;
        }
        else {
            list->Add(new IBezPoint3Key(*iKey));
            itplKey = itplKey->next;
        }
    }
    // if one list is not empty (cKeys->Length() != iKeys->Length())
    while(ctrlKey) {
        cKey = ctrlKey->item;
        list->Add(new IBezPoint3Key(*cKey));
        ctrlKey = ctrlKey->next;
    }
    while(itplKey) {
        iKey = itplKey->item;
        list->Add(new IBezPoint3Key(*iKey));
        itplKey = itplKey->next;
    }
    mposKeys->Delete();
    *mposKeys = *list;
}


//-----------------------------------------------------------------------------
// return a new list, sorting the 2 ones by frame number
//
void
Obj::MergeRotKeys(WList<ITCBRotKey*>* iKeys, WList<Quat*>* iQuats)
{
    ITCBRotKey *cKey, *iKey;
    WList<ITCBRotKey*>* keyList = new WList<ITCBRotKey*>();
    WList<ITCBRotKey*>::LCel *ctrlKey = mrotKeys->Begin();
    WList<ITCBRotKey*>::LCel *itplKey = iKeys->Begin();

    Quat *cQuat, *iQuat;
    WList<Quat*>* quatList = new WList<Quat*>();
    WList<Quat*>::LCel *ctrlQuat = mrotQuats->Begin();
    WList<Quat*>::LCel *itplQuat = iQuats->Begin();    

    while(ctrlKey && itplKey && ctrlQuat && itplQuat)
    {
        cKey = ctrlKey->item;
        iKey = itplKey->item;
        cQuat = ctrlQuat->item;
        iQuat = itplQuat->item;
        if(cKey->time < iKey->time) 
        {
            keyList->Add(new ITCBRotKey(*cKey));
            quatList->Add(new Quat(*cQuat));
            ctrlKey = ctrlKey->next;
            ctrlQuat = ctrlQuat->next;
        }
        else 
        {
            keyList->Add(new ITCBRotKey(*iKey));
            quatList->Add(new Quat(*iQuat));
            itplKey = itplKey->next;
            itplQuat = itplQuat->next;
        }
    }
    // if one list is not empty (cKeys->Length() != iKeys->Length())
    while(ctrlKey)
    {
        cKey = ctrlKey->item;
        cQuat = ctrlQuat->item;

        keyList->Add(new ITCBRotKey(*cKey));
        quatList->Add(new Quat(*cQuat));

        ctrlKey = ctrlKey->next;
        ctrlQuat = ctrlQuat->next;
    }
    while(itplKey) 
    {
        iKey = itplKey->item;
        iQuat = itplQuat->item;

        keyList->Add(new ITCBRotKey(*iKey));
        quatList->Add(new Quat(*iQuat));

        itplKey = itplKey->next;
        itplQuat = itplQuat->next;
    }
    mrotKeys->Delete();
    mrotQuats->Delete();
    *mrotKeys = *keyList;
    *mrotQuats = *quatList;
}


//-----------------------------------------------------------------------------
// return a new list, sorting the 2 ones by frame number
//
void
Obj::MergeSclKeys(WList<IBezScaleKey*>* iKeys)
{
    WList<IBezScaleKey*>* list = new WList<IBezScaleKey*>();
    WList<IBezScaleKey*>::LCel *ctrlKey = msclKeys->Begin();
    WList<IBezScaleKey*>::LCel *itplKey = iKeys->Begin();
    IBezScaleKey *cKey, *iKey;
    while(ctrlKey && itplKey)
    {
        cKey = (IBezScaleKey*)ctrlKey->item;
        iKey = (IBezScaleKey*)itplKey->item;
        if(cKey->time < iKey->time) {
            list->Add(new IBezScaleKey(*cKey));
            ctrlKey = ctrlKey->next;
        }
        else {
            list->Add(new IBezScaleKey(*iKey));
            itplKey = itplKey->next;
        }
    }
    // if one list is not empty (cKeys->Length() != iKeys->Length())
    while(ctrlKey) {
        cKey = ctrlKey->item;
        list->Add(new IBezScaleKey(*cKey));
        ctrlKey = ctrlKey->next;
    }
    while(itplKey) {
        iKey = itplKey->item;
        list->Add(new IBezScaleKey(*iKey));
        itplKey = itplKey->next;
    }
    msclKeys->Delete();
    *msclKeys = *list;
}

