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

	//printf("CreateSoundEngine() found in %s.\n", filename);
	m_plugin = m_Loader.CreateEngine( m_Loader.GetHandle() );
	SetSource( m_plugin );
	return true;
}
