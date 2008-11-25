// PluginLoader.h: interface for the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PLUGIN_LOADER_H)
#define PLUGIN_LOADER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "../platform.h"

/*********************************************************************
 *! \class CPluginLoader
 *  \brief Platform dependant library function loader.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 08. April 2006.
 *********************************************************************/
class CPluginLoader
{
public:
	CPluginLoader();
	virtual ~CPluginLoader();

protected:
	bool LoadPlugin(const char *fileName);
	void *GetFunction(const char *funcName);

private:
	LIB_HANDLE m_hinst;
};

#endif // !defined(PLUGIN_LOADER_H)
