// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include <stdio.h>
#include <direct.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopyEngine* (*CreateProc)(void);
#define PROC_NAME "CreateEngine"
#define PLUG_EXT ".dll"


CEngine::CEngine()
{
	m_hinst = NULL;
	m_plugin = NULL;
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

BOOL CEngine::Create(char *plugin)
{
	/*char path[MAX_PATH] = {0};
	getcwd(path, MAX_PATH);
	MessageBox(NULL,path,"Path",MB_OK);
	int c = chdir("plugins");
	FILE *fp = fopen("in_floopy.cfg", "r");
	if(fp)
	{
		fscanf(fp, "%s", path);
		fclose(fp);
	}
	chdir(path);
	memset(path, 0 , MAX_PATH);
	strcat(path, "D:\\sqba\\Projekti\\Floopy!\\Floopy2\\Debug");*/

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
			delete[] filename;
			return TRUE;
		}
		else
 			fprintf(stderr, "Error: %s not found in %s.\n", PROC_NAME, filename);
	}
	delete[] filename;
	return FALSE;
}
