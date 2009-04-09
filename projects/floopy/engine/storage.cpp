// Storage.cpp: implementation of the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "storage.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopyEngineStorage* (*CreateProc)(const char*);
#define PROC_NAME "CreateStorage"

//typedef bool (*StorageProc)(IFloopySoundEngine*, const char*);
//#define PROC_NAME_LOAD "Load"
//#define PROC_NAME_SAVE "Save"

CStorage::CStorage(LIB_HANDLE hModule, IFloopySoundEngine *engine, const char *name) : CLoader(hModule)
{
	m_engine = engine;

//	bool bStorageLoaded = LoadPlugin(name);
//	assert( bStorageLoaded );

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

	CreateProc func = (CreateProc)GetFunction(PROC_NAME);
	if(func == NULL)
	{
//		sprintf(m_szLastError, "Function %s() not found in file: %s.\n", PROC_NAME, tmp);
		return;
	}

	m_plugin = func( plugin );
}

CStorage::~CStorage()
{
}

bool CStorage::Load(const char *filename)
{
	assert( filename );
	assert( m_plugin );

	return m_plugin->Load(m_engine, filename);
/*
	StorageProc func = (StorageProc)GetFunction(PROC_NAME_LOAD);

	if(NULL == func)
		return false;

	return func(m_engine, filename);
*/
}

bool CStorage::Save(const char *filename)
{
	assert( filename );
	assert( m_plugin );

	return m_plugin->Save(m_engine, filename);
/*
	StorageProc func = (StorageProc)GetFunction(PROC_NAME_SAVE);

	if(NULL == func)
		return false;

	return func(m_engine, filename);
*/
}
