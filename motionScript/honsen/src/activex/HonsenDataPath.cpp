/* ************************************************************************ */
/*																			*/
/*	Honsen ActiveX Plugin													*/
/*	Julien Chevreux															*/
/*	(c)2004-2005 Motion-Twin												*/
/*																			*/
/* ************************************************************************ */
#define WINVER 0x0400
#include "HonsenDataPath.h"

HonsenDataPath::HonsenDataPath(COleControl *control, const char *filename )
:CDataPathProperty(filename, control), progress(0), progressMax(0), 
started(false)
{
}

HonsenDataPath::~HonsenDataPath() {
}

void HonsenDataPath::OnDataAvailable(DWORD dwSize, DWORD bscfFlag) 
{
	if (bscfFlag & BSCF_LASTDATANOTIFICATION) {
		GetControl()->InternalSetReadyState(READYSTATE_COMPLETE);
		GetControl()->Invalidate(TRUE);
	}
}


void HonsenDataPath::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR szStatusText) 
{	
	switch(ulStatusCode){
	case BINDSTATUS_BEGINDOWNLOADDATA:
		started = true;
		progress = ulProgress;
		progressMax = ulProgressMax;
		ResetData();
		break;
	case BINDSTATUS_ENDDOWNLOADDATA:
	case BINDSTATUS_DOWNLOADINGDATA:
		progress = ((double)ulProgress / (double)ulProgressMax) * 100.0;
		break;
	}
}

/* ************************************************************************ */
