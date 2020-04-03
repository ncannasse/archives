//*****************************************************************************
//*                                                     3DSMAX MTW Exporter
//*                                                     (c)2003 Motion-Twin
//*     File    : mtw_utils.cpp
//*     Author  : M.B.
//*     Desc    : 
//*     Date    : 26.08.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes - Standards
//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Includes - Project
//
#include "include/maxtwin.h"

/*const TAG Objt = MAKETAG(Objt);
const TAG NAME = MAKETAG(NAME);
const TAG flag = MAKETAG(flag);
const TAG VECT = MAKETAG(VECT);
const TAG INDX = MAKETAG(INDX);
const TAG MAP0 = MAKETAG(MAP0);
const TAG TEX0 = MAKETAG(TEX0);
const TAG VIDX = MAKETAG(VIDX);
const TAG Anim = MAKETAG(Anim);*/


/*

#define PROP_ANM_NAME "anim_name"

//-----------------------------------------------------------------------------
// 
//
char*
Maxtwin::GetAnimName(INode* node) const
{
    TSTR name;
    if(!node->GetUserPropString(PROP_ANM_NAME, name)) // || name == (TSTR)"default" )
        return NULL;

    int length = strlen((char*)name);
    char* copy = new char[length+1];
    memcpy(copy, name, length*sizeof(char));
    copy[length]='\0';
    return copy;
}


//-----------------------------------------------------------------------------
// converts a 3DSMax Matrix to a D3D Matrix
//
MATRIX
Maxtwin::MaxToD3DMatrix(Matrix3* m) const
{
    return MATRIX( m->GetRow(0).x,m->GetRow(0).y,m->GetRow(0).z,0,
                   m->GetRow(1).x,m->GetRow(1).y,m->GetRow(1).z,0,
                   m->GetRow(2).x,m->GetRow(2).y,m->GetRow(2).z,0,
                   m->GetRow(3).x,m->GetRow(3).y,m->GetRow(3).z,1);
}


//-----------------------------------------------------------------------------
// remove the specified existing child tag from the specified parent
// return true if delete ok, false otherwise
//
bool
Maxtwin::RemoveExistingTag(Tag* parent, TAG name) const {
    TAG test = MAKETAG(MRPH);
    Tag *tag = parent->Child(name);
    if(tag) {
        parent->RemoveChild(tag);   // TODO : test it !
        SAFE_DELETE(tag);
        return true;
    }
    return false;
}

*/
