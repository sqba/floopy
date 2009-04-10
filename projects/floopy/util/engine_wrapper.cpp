
#include "engine_wrapper.h"
#include <stdio.h>
#include <sstream>
#include "../platform.h"


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
	m_plugin = m_Loader.CreateEngine();
	SetSource( m_plugin );
	return true;
}
