// PluginLoader.cpp: implementation of the CLoader class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "loader.h"
#include "../platform.h"


typedef IFloopySoundInput* (*CreateInputProc)(const char *name);
typedef IFloopySoundEngine* (*CreateEngineProc)(LIB_HANDLE hModule);
typedef IFloopyEngineStorage* (*CreateStorageProc)(const char*);
typedef IFloopySoundOutput* (*CreateOutputProc)(const char*, SOUNDFORMAT);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoader::CLoader()
{
	m_hModule = NULL;
}

CLoader::CLoader(LIB_HANDLE hModule)
{
	m_hModule = hModule;
}

CLoader::~CLoader()
{
	if(0 != m_hPlugin)
		PLUGIN_CLOSE(m_hPlugin);
}

bool CLoader::LoadPlugin(const char *fileName)
{
	char path[MAX_PATH] = {0};
	char sep[2] = {PATH_SEP, 0};

	get_library_path(m_hModule, path, MAX_PATH);
	strcat(path, sep);
	strcat(path, PLUG_PREFIX);
	strcat(path, fileName);
	strcat(path, PLUG_EXT);

//	printf("Loading \'%s\': ");

	m_hPlugin = PLUGIN_OPEN(path);

	if(0 == m_hPlugin)
	{
		fprintf(stderr, ERR_STR_FILENOTFOUND, path);
		return false;
	}

//	printf("%s\n", m_hPlugin?"OK":"FAILED");

	return true;
}

void *CLoader::GetFunction(const char *funcName)
{
	if (0 == m_hPlugin)
	{
		fprintf(stderr, ERR_STR_LIBNOTLOADED);
		return 0;
	}

	void *func = PLUGIN_LOAD(m_hPlugin, funcName);
	if(func == 0)
	{
		fprintf(stderr, ERR_STR_FUNCNOTFOUND, funcName);
		return 0;
	}

	return func;
}

void CLoader::get_library_path(LIB_HANDLE hModule, char *buff, int len)
{
#ifdef WIN32
	GetModuleFileName(hModule, buff, len);
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

IFloopySoundInput *CLoader::CreateInput(const char *name)
{
	CreateInputProc func = (CreateInputProc)GetFunction("CreateInput");
	assert( func );
	IFloopySoundInput *result = func( name );
	assert( result );
	return result;
}

IFloopySoundEngine *CLoader::CreateEngine(LIB_HANDLE hModule)
{
	CreateEngineProc func = (CreateEngineProc)GetFunction("CreateSoundEngine");
	assert( func );
	IFloopySoundEngine *result = func( hModule );
	assert( result );
	return result;
}

IFloopyEngineStorage *CLoader::CreateStorage(const char *name)
{
	CreateStorageProc func = (CreateStorageProc)GetFunction("CreateStorage");
	assert( func );
	IFloopyEngineStorage *result = func( name );
	assert( result );
	return result;
}

IFloopySoundOutput *CLoader::CreateOutput(const char *name, SOUNDFORMAT fmt)
{
	CreateOutputProc func = (CreateOutputProc)GetFunction("CreateOutput");
	assert( func );
	IFloopySoundOutput *result = func(name, fmt);
	assert( result );
	return result;
}
