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
	m_plugin = NULL;
	init( plugin );
}

CEngineWrapper::~CEngineWrapper()
{
	if(NULL != m_plugin)
	{
		//m_plugin->Close();
		delete m_plugin;
	}
}

bool CEngineWrapper::init(const char *plugin)
{
	if( !m_Loader.LoadPlugin(plugin) )
		return false;

 	CreateProc func = (CreateProc)m_Loader.GetFunction( PROC_NAME );

	if(0 == func)
		return false;

	//printf("CreateSoundEngine() found in %s.\n", filename);
	m_plugin = func( m_Loader.GetHandle() );
	SetSource( m_plugin );
	return true;
}
