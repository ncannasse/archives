//*****************************************************************************
//*                                                 Maxtwin
//*                                                 Motion-Twin 3dsmax Exporter
//*                                                 (c)2003 Motion-Twin
//*     File    : mtw_log.cpp
//*     Author  : M.B.
//*     Desc    : logger
//*     Date    : 19.09.03
//*
//*****************************************************************************

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "include/mtw_log.h"


//-----------------------------------------------------------------------------
// Write logs to a text file
//
bool
Log::Write()
{
    FILE* logFile = fopen("maxtwin.log", "w");
    if(!logFile)
        return false;
    fprintf(logFile, text.c_str());
    fclose(logFile);
    return true;
}
