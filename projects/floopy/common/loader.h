// PluginLoader.h: interface for the CLoader class.
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

#include "../ifloopy.h"
#include "../platform.h"

#define ERR_STR_FILENOTFOUND	"File '%s' not found.\n"
#define ERR_STR_FUNCNOTFOUND	"Function '%s' not found.\n"
#define ERR_STR_LIBNOTLOADED	"Library not loaded.\n"

/*********************************************************************
 *! \class CLoader
 *  \brief Platform independent library function loader.
 *  \author Filip Pavlovic
 *  \version 0.0
 *  \date 08. April 2006.
 *********************************************************************/
class CLoader
{
public:
	CLoader();
	CLoader(LIB_HANDLE hModule);
	virtual ~CLoader();

	bool LoadPlugin(const char *fileName);

	IFloopySoundInput *CreateInput(const char *name);
	IFloopySoundEngine *CreateEngine();
	IFloopySoundOutput *CreateOutput(const char *name, SOUNDFORMAT fmt);
	IFloopyEngineStorage *CreateStorage(const char *name);

private:
	void *get_function(const char *funcName);
	void get_library_path(LIB_HANDLE hModule, char *buff, int len);

private:
	LIB_HANDLE m_hModule;	/// Main application handle
	LIB_HANDLE m_hPlugin;	/// Plugin library handle
};

#endif // !defined(PLUGIN_LOADER_H)
