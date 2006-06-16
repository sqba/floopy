// OutputCache.h: interface for the COutputCache class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTCACHE_H__12D2CBC7_2DCE_4536_BC23_21C7FFD46676__INCLUDED_)
#define AFX_OUTPUTCACHE_H__12D2CBC7_2DCE_4536_BC23_21C7FFD46676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\IFloopy.h"

class COutputCache : public IFloopySoundFilter  
{
public:
	COutputCache();
	virtual ~COutputCache();

};

#endif // !defined(AFX_OUTPUTCACHE_H__12D2CBC7_2DCE_4536_BC23_21C7FFD46676__INCLUDED_)
