// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopyEngine*	(*CreateEngineProc)(void);

CEngine::CEngine(char *plugin)
{
	m_hinst = NULL;
	m_plugin = NULL;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, TEXT(".dll\0"));
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateEngineProc func = (CreateEngineProc)GetProcAddress(m_hinst, "CreateEngine"); 

		if(func != NULL) {
			printf("CreateEngine() found in %s.\n", filename);
			m_plugin = func();
			SetSource( m_plugin );
		}
		else
 			printf("Error: CreateEngine() not found in %s.\n", filename);
	}
}

CEngine::~CEngine()
{
	if(NULL != m_plugin)
	{
		m_plugin->Close();
		delete m_plugin;
	}

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}
