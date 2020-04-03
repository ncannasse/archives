//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_log.h
//*     Author  : M.B.
//*     Desc    : logger
//*     Date    : 19.09.03
//*
//*****************************************************************************

#ifndef __MTW_LOG_H__
#define __MTW_LOG_H__

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "../../common/wstring.h"


//*****************************************************************************
//
class Log 
{
    WString text;

public:
    Log() { text = ""; }
    ~Log() {}
    inline bool Print(WString t) {
        text += t + "\n";
        return true;
    }
    inline bool Error(WString t) {
        text += "Error : ";
        Print(t);
        return false;
    }
    inline bool Warning(WString t) {
        text += "Warning : ";
        Print(t);
        return true;
    }
    const char* ToString() const {
        return text.c_str();
    }
    bool Write();
};

#endif // __MTW_LOG_H__

