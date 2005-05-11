// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopyEngine* (*CreateProc)(void);
#define PROC_NAME "CreateEngine"
#define PLUG_EXT ".dll"

CEngine::CEngine(char *plugin)
{
	m_hinst = NULL;
	m_plugin = NULL;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateProc func = (CreateProc)GetProcAddress(m_hinst, PROC_NAME); 

		if(func != NULL) {
			//printf("CreateEngine() found in %s.\n", filename);
			m_plugin = func();
			SetSource( m_plugin );
		}
		else
 			fprintf(stderr, "Error: %s not found in %s.\n", PROC_NAME, filename);
	}
}

CEngine::~CEngine()
{
	if(NULL != m_plugin)
	{
		//m_plugin->Close();
		delete m_plugin;
	}

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}
