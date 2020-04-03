/* ****************************************************************	*/
/*																	*/
/*	namespace WMem													*/
/*		(c)2000 by Nicolas Cannasse									*/
/*		standard memory loss check									*/
/*																	*/
/* ****************************************************************	*/
#ifndef WMEM_H
#define WMEM_H

#ifdef _DEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace WMem {

	inline void Init( bool FullDebug )
	{
#ifdef _DEBUG
		_CrtSetDbgFlag ( 
			_CRTDBG_ALLOC_MEM_DF 
			| (FullDebug?_CRTDBG_CHECK_ALWAYS_DF:0)
			| _CRTDBG_DELAY_FREE_MEM_DF 
			| _CRTDBG_LEAK_CHECK_DF 
			);
#endif
	}

};

#endif
/* ****************************************************************	*/
