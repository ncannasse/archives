//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : maxtwin.cpp
//*     Author  : M.B.
//*     Desc    : exporter main class implementation
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <max.h>
#include <modstack.h>   // IDerivedObject
#include <stdmat.h>     // ID_DI

#include "include/maxtwin.h"
#include "include/mtw_error.h"


///////////////////////////////////////////////////////////////////////////////
TimeValue Maxtwin::startTime = 0;
TimeValue Maxtwin::endTime   = 0;


//-----------------------------------------------------------------------------
// Constructor & Destructor
//
Maxtwin::Maxtwin()
{
    rootTag     = NULL;
    obj         = NULL;
}


Maxtwin::~Maxtwin()
{
    rootTag     = NULL;
    obj         = NULL;
}


//-----------------------------------------------------------------------------
// starting function
//
int
Maxtwin::DoExport(const TCHAR* filePath, ExpInterface* eitf, Interface* maxInterface, BOOL supPrompts, DWORD options)
{
    startTime = maxInterface->GetAnimRange().Start();
    endTime = maxInterface->GetAnimRange().End();

    int pathLength = strlen(filePath);      // replace extension
	((char*)filePath)[pathLength-3] = 'm';
	((char*)filePath)[pathLength-2] = 't';
	((char*)filePath)[pathLength-1] = 'w';

    log.Print(WString("Start Export : ") + filePath);

    rootTag = MTW::ReadFile(filePath);      // rootTag NULL or valid
    if(!rootTag)
        rootTag = new MTW(MAKETAG(Root));

    /* TODO : check rootTag
    else
        FOREACH(MTW*, rootTag)
            res = Validate(item);
            if(res<0) return res;
        ENDFOR; */

    INode *rootNode = maxInterface->GetRootNode();
    int res = EnumNode(rootTag, rootNode);
    if(res==0)
    {
        if(!rootTag->Childs(MAKETAG(Objt)).Empty()) {
            if(!rootTag->WriteFile(filePath))
                res = E_WRITE_FILE;
        }
        else
            res = E_EXPORT_EMPTY;
    }

    SetLog(res);

    SAFE_DELETE(rootTag);

    return (int)(res==0);
}


//-----------------------------------------------------------------------------
// Export the object node and his children
//
int
Maxtwin::EnumNode(MTW* parentTag, INode* node)
{
    int res=0;

    if(!node)
        return E_ENUM_NODE;

    if(node->IsFrozen() || node->IsHidden())
        return 0;        // ignore frozen and hidden nodes

    MTW *objtTag=NULL;
    if( IsGeomObject(node) )
    {
        obj = new Obj(node);

        objtTag = obj->Init(parentTag);
        if(!objtTag)
            log.Warning(WString("No export property for node : ") + node->GetName());
        else
            res = obj->Export();

        SAFE_DELETE(obj);

        if(res<0) return res;
    }

    int nChildren = node->NumberOfChildren();
    for(int i=0; i<nChildren; i++){
        res = EnumNode(objtTag?objtTag:parentTag, node->GetChildNode(i));
        if(res<0) return res;
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Return geom object from node, or null 
// precond : node not null
//
bool
Maxtwin::IsGeomObject(INode* node) const
{
    char* name = node->GetName();

    Matrix3 m1 = node->GetObjectTM(0);
    MATRIX m2  = MaxToD3DMatrix(&m1);

    Matrix3 m3 = node->GetNodeTM(0);
    MATRIX m4  = MaxToD3DMatrix(&m3);

    Object* obj = node->EvalWorldState(0).obj;
    if(!obj)
        return false;

    SClass_ID scId = obj->SuperClassID();
    if(obj->SuperClassID()!=GEOMOBJECT_CLASS_ID)
        return false;

    // ignore bones
    Class_ID cId = obj->ClassID();
    if( cId == BONE_OBJ_CLASSID || cId == Class_ID(0x9125,0) )
        return false;

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// UTILS


//-----------------------------------------------------------------------------
//
//
void
Maxtwin::SetLog(int res)
{
    WString status;
    if(res<0) {
        status = "Failure";
        log.Error(errors.GetMessage(res));
    }
    else
        status = "Success";

    log.Print(WString("End Export : ") + status);
    MessageBox(NULL, log.ToString(), "Export Log", MB_OK | (res==0?MB_ICONINFORMATION:MB_ICONERROR));
    log.Write();
}


//-----------------------------------------------------------------------------
// Convert a 3DSMax Matrix to a D3D Matrix
//
MATRIX
Maxtwin::MaxToD3DMatrix(Matrix3* m)
{
    return MATRIX( m->GetRow(0).x,m->GetRow(0).y,m->GetRow(0).z,0,
                   m->GetRow(1).x,m->GetRow(1).y,m->GetRow(1).z,0,
                   m->GetRow(2).x,m->GetRow(2).y,m->GetRow(2).z,0,
                   m->GetRow(3).x,m->GetRow(3).y,m->GetRow(3).z,1);
}


//-----------------------------------------------------------------------------
// return the number of frames of the current model
//
int
Maxtwin::GetNFrames()
{
    return (endTime-startTime)/GetTicksPerFrame() + 1;
}



/*
//-----------------------------------------------------------------------------
//
bool
CheckMtwFormat(MTW* tag) {
    WList<MTW*> subtags = tag->Childs();
    FOREACH(MTW*, subtags)
        if(!item)
            return false;
        if(item->IsGroup()) {
            if(!CheckMtwFormat(item))
                return false;
        }
        else
            if(!item->IsNumber() && !item->IsIdent())
                if(item->size==0 || !item->data)     // standard tag
                    return false;
    ENDFOR;
    return true;
}

//-----------------------------------------------------------------------------
// 
// 
int
Validate(MTW* objtTag)
{
    if(!CheckMtwFormat(objtTag))
        return E_MTW_FORMAT;

    // geometry & properties
    MTW* tag = objtTag->Child(MAKETAG(NAME));
    if(!tag) return E_MTW_NO_OBJNAME;
    tag = objtTag->Child(MAKETAG(flag));
    if(!tag) return E_MTW_NO_OBJFLAG;
    tag = objtTag->Child(MAKETAG(VECT));
    if(!tag) return E_MTW_NO_VECT;
    tag = objtTag->Child(MAKETAG(INDX));
    if(!tag) return E_MTW_NO_INDX;
    tag = objtTag->Child(MAKETAG(MREF));
    if(!tag) return E_MTW_NO_MREF;

    // mapping
    tag = objtTag->Child(MAKETAG(MAP0));
    if(tag) {
        tag = objtTag->Child(MAKETAG(TEX0));
        if(!tag) return E_MTW_NO_TEX0;
        tag = objtTag->Child(MAKETAG(VIDX));
        if(!tag) return E_MTW_NO_VIDX;
    }

    // skeleton
    tag = objtTag->Child(MAKETAG(Skel));
    if(tag) {
        MTW* pidxTag = tag->Child(MAKETAG(PIDX));
        if(!pidxTag) return E_MTW_NO_PIDX;
        MTW* refpTag = tag->Child(MAKETAG(REFP));
        if(!refpTag) return E_MTW_NO_REFP;
        MTW* wghtTag = tag->Child(MAKETAG(WGHT));
        if(!wghtTag) return E_MTW_NO_WGHT;

        if(refpTag->size/(sizeof(MATRIX))!=pidxTag->size/sizeof(WORD))
            return E_MTW_COHERENCE; // same nb of indexes and matrices = same nb of bones
    }

    // animations
    WList<MTW*> animTags = objtTag->Childs(MAKETAG(Anim));
    FOREACH(MTW*, animTags)
        tag = item->Child(MAKETAG(NAME));
        if(!tag) return E_MTW_NO_ANIMNAME;
        MTW* nfrmTag = item->Child(MAKETAG(nfrm));
        if(!nfrmTag) return E_MTW_NO_NFRM;
        tag = item->Child(MAKETAG(mflg));
        if(!tag) return E_MTW_NO_MFLG;
        
        WList<MTW*> frameTags = item->Childs(MAKETAG(Fram));

        FOREACH(MTW*, frameTags)
            tag = item->Child(MAKETAG(frnb));
            if(!tag) return E_MTW_NO_FRNB;
            MTW* moveTag = item->Child(MAKETAG(MOVE));
            MTW* mrphTag = item->Child(MAKETAG(MRPH));
            MTW* skelTag = item->Child(MAKETAG(SKEL));
            if(!moveTag && !mrphTag && !skelTag) 
                return E_MTW_ANIM_EMPTY;
        ENDFOR;
    ENDFOR;

    // subobjects
    WList<MTW*> objtTags = objtTag->Childs(MAKETAG(Objt));
    FOREACH(MTW*, objtTags)
        int res = Validate(item);
        if(res<0) return res;
    ENDFOR;

    return 0;
}
*/
