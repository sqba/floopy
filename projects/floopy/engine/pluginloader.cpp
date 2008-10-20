// PluginLoader.cpp: implementation of the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "pluginloader.h"
#include "../platform.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPluginLoader::CPluginLoader()
{

}

CPluginLoader::~CPluginLoader()
{
	if(0 != m_hinst)
		PLUGIN_CLOSE(m_hinst);
}

bool CPluginLoader::LoadPlugin(const char *fileName)
{
	m_hinst = PLUGIN_OPEN(fileName);
	return (0 != m_hinst);
}

void *CPluginLoader::GetFunction(const char *funcName)
{
	if (0 == m_hinst)
		return false;

	void *func = PLUGIN_LOAD(m_hinst, funcName); 
	if(func != 0)
		return func;

	return 0;
}
