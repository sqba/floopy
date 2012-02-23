// PluginLoader.cpp: implementation of the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "PluginLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPluginLoader::CPluginLoader()
{

}

CPluginLoader::~CPluginLoader()
{
	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}

bool CPluginLoader::LoadPlugin(char *fileName)
{
	m_hinst = LoadLibraryA(fileName);
	return (NULL != m_hinst);
}

void *CPluginLoader::GetFunction(char *funcName)
{
	if (NULL == m_hinst)
		return false;

	void *func = GetProcAddress(m_hinst, funcName); 
	if(func != NULL)
		return func;

	return NULL;
}