// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundEngine* (*CreateProc)(HMODULE hModule);
#define PROC_NAME "CreateSoundEngine"
#define PLUG_EXT ".dll"

CEngine::CEngine(char *plugin)
{
	m_hModule = NULL;
	m_plugin = NULL;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);
	m_hModule = LoadLibraryA(filename);

	if (NULL != m_hModule)
	{
		CreateProc func = (CreateProc)GetProcAddress(m_hModule, PROC_NAME); 

		if(func != NULL) {
			//printf("CreateSoundEngine() found in %s.\n", filename);
			m_plugin = func( m_hModule );
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

	if(NULL != m_hModule)
		FreeLibrary(m_hModule);
}

