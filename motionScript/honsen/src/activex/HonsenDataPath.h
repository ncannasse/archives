/* ************************************************************************ */
/*																			*/
/*	Honsen ActiveX Plugin													*/
/*	Julien Chevreux															*/
/*	(c)2004-2005 Motion-Twin												*/
/*																			*/
/* ************************************************************************ */
#pragma once

#include <Afxctl.h>

class HonsenDataPath: public CDataPathProperty {
public:
	HonsenDataPath(COleControl *control, const char *filename );
	virtual ~HonsenDataPath();
	virtual void OnDataAvailable(DWORD dwSize, DWORD bscfFlag);
	virtual void OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR szStatusText);

	double progress;
	DWORD progressMax;
	bool started;
};

/* ************************************************************************ */
