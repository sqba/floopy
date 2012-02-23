// Storage.cpp: implementation of the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "storage.h"
#include "../common/util.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStorage::CStorage(LIB_HANDLE hModule, IFloopySoundEngine *engine, const char *name) : CLoader(hModule)
{
	m_engine = engine;

	m_plugin = NULL;

	char plugin[MAX_PATH]	= {0};
	char library[MAX_FNAME]	= {0};

	get_library_name(name, library);
	get_plugin_name(name, plugin);

	if( !LoadPlugin(library) )
	{
//		sprintf(m_szLastError, "File not found: %s.\n", tmp);
		return;
	}

	m_plugin = CreateStorage( plugin );
}

CStorage::~CStorage()
{
}

bool CStorage::Load(const char *filename)
{
	assert( filename );
	assert( m_plugin );

	return m_plugin->Load(m_engine, filename);
}

bool CStorage::Save(const char *filename)
{
	assert( filename );
	assert( m_plugin );

	return m_plugin->Save(m_engine, filename);
}
