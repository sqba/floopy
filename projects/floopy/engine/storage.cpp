// Storage.cpp: implementation of the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "storage.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef bool (*StorageProc)(IFloopySoundEngine*, const char*);
#define PROC_NAME_LOAD "Load"
#define PROC_NAME_SAVE "Save"

CStorage::CStorage(IFloopySoundEngine *engine, const char *plugin)
{
	m_engine = engine;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);

	bool bStorageLoaded = LoadPlugin(filename);
	assert( bStorageLoaded );

	delete[] filename;
}

CStorage::~CStorage()
{
}

bool CStorage::Load(const char *filename)
{
	StorageProc func = (StorageProc)GetFunction(PROC_NAME_LOAD);

	if(NULL == func)
		return false;

	return func(m_engine, filename);
}

bool CStorage::Save(const char *filename)
{
	StorageProc func = (StorageProc)GetFunction(PROC_NAME_SAVE);

	if(NULL == func)
		return false;

	return func(m_engine, filename);
}
