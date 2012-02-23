// PluginLoader.h: interface for the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(PLUGIN_LOADER_H)
#define PLUGIN_LOADER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
	bool LoadPlugin(char *fileName);
	void *GetFunction(char *funcName);

private:
	HINSTANCE m_hinst;
};

#endif // !defined(PLUGIN_LOADER_H)
