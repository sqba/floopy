// PluginLoader.cpp: implementation of the CPluginLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "loader.h"
#include "../platform.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPluginLoader::CPluginLoader(LIB_HANDLE hModule)
{

}

CPluginLoader::~CPluginLoader()
{
	if(0 != m_hinst)
		PLUGIN_CLOSE(m_hinst);
}

bool CPluginLoader::LoadPlugin(const char *fileName)
{
//	char path[MAX_PATH] = {0};
//	printf("Loading \'%s\': ");
	m_hinst = PLUGIN_OPEN(fileName);
//	printf("%s\n", m_hinst?"OK":"FAILED");
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

void CPluginLoader::get_library_path(LIB_HANDLE hModule, char *buff, int len)
{
#ifdef WIN32
	GetModuleFileName(m_hModule, buff, len);
#else
	// Linux specific

	// 1. executable
	readlink("/proc/self/exe", buff, len);

	// 2. dynamic library
	//DL_info info;
    //if (dladdr( &GetLibraryPath, &info ) == 0)
    //	strcpy(m_szPath, info.dli_fname);

    // 3.
    // g++ -o executable -Wl,-R -Wl,'$ORIGIN' executable.o libhe
#endif

	char *tmp = strrchr(buff, PATH_SEP);
	if(tmp)
		*(tmp+1) = '\0';
}
