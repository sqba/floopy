// Storage.cpp: implementation of the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "Storage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef BOOL (*StorageProc)(IFloopyEngine*, char*);
#define PROC_NAME_LOAD "Load"
#define PROC_NAME_SAVE "Save"
#define PLUG_EXT ".dll"

CStorage::CStorage(IFloopyEngine *engine, char *plugin)
{
	m_hinst = NULL;
	m_plugin = NULL;
	m_engine = engine;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);
	m_hinst = LoadLibraryA(filename);

	delete[] filename;
}

CStorage::~CStorage()
{
	if(NULL != m_plugin)
		delete m_plugin;

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}

BOOL CStorage::Load(char *filename)
{
	if(NULL == m_hinst)
		return FALSE;

	StorageProc func = (StorageProc)GetProcAddress(m_hinst, PROC_NAME_LOAD); 
	if(NULL == func)
		return FALSE;

	return func(m_engine, filename);
}

BOOL CStorage::Save(char *filename)
{
	if(NULL == m_hinst)
		return FALSE;

	StorageProc func = (StorageProc)GetProcAddress(m_hinst, PROC_NAME_SAVE); 
	if(NULL == func)
		return FALSE;

	return func(m_engine, filename);
}
