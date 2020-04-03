//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_error.h
//*     Author  : M.B.
//*     Desc    : error definitions
//*     Date    : 19.09.03
//*
//*****************************************************************************

#ifndef __MTW_ERROR_H__
#define __MTW_ERROR_H__

///////////////////////////////////////////////////////////////////////////////
#define E_ENUM_NODE       -1
#define E_EXPORT_EMPTY    -2
#define E_WRITE_FILE      -3

#define E_MERGE_MESH      -11
#define E_NO_MATERIAL     -12
#define E_GET_MATERIAL    -13

#define E_NO_ANIM_NAME    -21
#define E_MERGE_ANIM      -22
#define E_NO_ANIMATION    -24

#define E_POS_CTRL        -201
#define E_ROT_CTRL        -202
#define E_SCL_CTRL        -203
#define E_POS_BEZ         -204
#define E_ROT_TCB         -205
#define E_SCL_BEZ         -206

#define E_MORPH_CTRL      -211
#define E_MERGE_MORPH     -212

#define E_NO_BONES        -221
#define E_SKIN_INTERFACE  -222
#define E_MERGE_SKIN      -223
#define E_SKIN_WEIGHTS    -224

// TODO :warnings

#define W_EMPTY_MODIFIER  1
#define W_MORPH_EMPTY     2



//*****************************************************************************
//
class Error
{
public:
    int     value;
    WString msg;

    Error()  {}
    Error(int v, char* s) : value(v), msg(s) {}
    ~Error() {}
};


//*****************************************************************************
//
class Errors
{
public:

    WList<Error> data;

    Errors()
    {
        data.Add( Error(E_ENUM_NODE,      "Enum Node failed\n") );
        data.Add( Error(E_EXPORT_EMPTY,   "Nothing specified to be exported. See Properties > User Defined") );
        data.Add( Error(E_WRITE_FILE,     "Write MTW File failed") );

        data.Add( Error(E_MERGE_MESH,     "Can't export geometry, incompatibility new/existing mesh") );
        data.Add( Error(E_NO_MATERIAL,    "Can't export mapping, no material") );
        data.Add( Error(E_GET_MATERIAL,   "Can't export mapping, ") );

        data.Add( Error(E_NO_ANIM_NAME,   "Can't export animation, no anim name") );
        data.Add( Error(E_MERGE_ANIM,     "Can't export animation, incompatibility new/existing animation : not the same number of frames") );
        data.Add( Error(E_NO_ANIMATION,   "Can't export animation, node is not animated") );

        data.Add( Error(E_POS_CTRL,       "Can't export move, position controller error") );
        data.Add( Error(E_ROT_CTRL,       "Can't export move, rotation controller error") );
        data.Add( Error(E_SCL_CTRL,       "Can't export move, scale controller error") );
        data.Add( Error(E_POS_BEZ,        "Can't export move, please asign Bezier controller for position") );
        data.Add( Error(E_ROT_TCB,        "Can't export move, please asign TCB controller for rotation") );
        data.Add( Error(E_SCL_BEZ,        "Can't export move, please asign Bezier controller for scale") );

        data.Add( Error(E_MORPH_CTRL,     "Can't export morph, morph controller error") );
        data.Add( Error(E_MERGE_MORPH,    "Can't export morph, incompatibility morph/existing object") );

        data.Add( Error(E_NO_BONES,       "Can't export skin, no skin modifier") );
        data.Add( Error(E_SKIN_INTERFACE, "Can't export skin, skin interface error") );
        data.Add( Error(E_MERGE_SKIN,     "Can't export skin, incompatibility new/existing skeleton") );
        data.Add( Error(E_SKIN_WEIGHTS,   "Can't export skin, some vertices don't have bone") );
    }

    ~Errors() {}

    WString GetMessage(int id) 
    {
        FOREACH(Error, data)
            if(item.value==id)
                return item.msg;
        ENDFOR;
        return "";
    }

};


#endif  // __MTW_ERROR_H__

