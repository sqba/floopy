// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "engine_wrapper.h"
#include <stdio.h>
#include <sstream>
#include "../platform.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundEngine* (*CreateProc)(LIB_HANDLE hModule);

#define PROC_NAME "CreateSoundEngine"

CEngineWrapper::CEngineWrapper(const char *plugin)
{
	m_hModule = NULL;
	m_plugin = NULL;

	GetLibraryPath();

	init( plugin );
}

CEngineWrapper::~CEngineWrapper()
{
	if(NULL != m_plugin)
	{
		//m_plugin->Close();
		delete m_plugin;
	}

	if(NULL != m_hModule)
		PLUGIN_CLOSE( m_hModule );
}

void CEngineWrapper::init(const char *plugin)
{
    std::ostringstream strPath;
    strPath << m_szPath << PLUG_PREFIX << plugin << PLUG_EXT;
    const char *pszPath = strPath.str().c_str();
    char tmp[MAX_PATH] = {0};
    strcpy(tmp, pszPath);

	m_hModule = PLUGIN_OPEN( tmp );

	if (NULL == m_hModule)
	{
		fprintf(stderr, "Error: %s not found.\n", pszPath);
		return;
	}
	CreateProc func = (CreateProc)PLUGIN_LOAD( m_hModule, PROC_NAME );

	if(func != NULL)
	{
		//printf("CreateSoundEngine() found in %s.\n", filename);
		m_plugin = func( m_hModule );
		SetSource( m_plugin );
	}
	else
		fprintf(stderr, "Error: %s not found in %s.\n", PROC_NAME, pszPath);
}

void CEngineWrapper::GetLibraryPath()
{
#ifdef WIN32
	GetModuleFileName(m_hModule, m_szPath, MAX_PATH);
#else
	// Linux specific

	// 1. executable
	int r = readlink("/proc/self/exe", m_szPath, MAX_PATH);

	// 2. dynamic library
	//DL_info info;
    //if (dladdr( &GetLibraryPath, &info ) == 0)
    //	strcpy(m_szPath, info.dli_fname);

    // 3.
    // g++ -o executable -Wl,-R -Wl,'$ORIGIN' executable.o libhe
#endif

	char *tmp = strrchr(m_szPath, PATH_SEP);
	if(tmp)
		*(tmp+1) = '\0';
}
