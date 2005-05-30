// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include <stdio.h>
#include <direct.h>

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundEngine* (*CreateProc)(HMODULE);
#define PROC_NAME "CreateSoundEngine"
#define PLUG_EXT ".dll"


CEngine::CEngine(HINSTANCE hDLLInstance)
{
	m_hDLLInstance = hDLLInstance;
	m_hModule = NULL;
	m_plugin = NULL;
	memset(m_path, 0, MAX_PATH);
	memset(m_szLastError, 0, sizeof(m_szLastError));
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

BOOL CEngine::Create(char *plugin)
{
	//MessageBox(NULL, m_path, "m_path", MB_OK);
	/*getcwd(path, MAX_PATH);
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
	if(!m_path && !*m_path)
		return FALSE;

	char *filename = new char[strlen(m_path) + strlen(plugin) + 5];
	strcpy(filename, m_path);
	strcat(filename, plugin);
	strcat(filename, PLUG_EXT);

//	MessageBox(NULL, filename, "LoadLibraryA", MB_OK);

	m_hModule = LoadLibraryA(filename);

	if (NULL != m_hModule)
	{
		CreateProc func = (CreateProc)GetProcAddress(m_hModule, PROC_NAME); 

		if(func != NULL) {
			//printf("CreateSoundEngine() found in %s.\n", filename);
			m_plugin = func( m_hModule );
			SetSource( m_plugin );
//			MessageBox(NULL, "Created engine", "In_Floopy", MB_OK);
			delete[] filename;
			return TRUE;
		}
		else
 			sprintf(m_szLastError, "Error: %s not found in %s.\n", PROC_NAME, filename);
 			//fprintf(stderr, "Error: %s not found in %s.\n", PROC_NAME, filename);
	}
	else
		sprintf(m_szLastError, "Error: %s not found in %s.\n", filename, m_path);
	delete[] filename;
	return FALSE;
}

void CEngine::init()
{
	//char path[MAX_PATH] = {0};
	//getcwd(path, MAX_PATH);
	//MessageBox(NULL, path, "path", MB_OK);
	//GetModuleFileName(GetModuleHandle(NULL), m_path, MAX_PATH);
	//char *tmp = strrchr(m_path, '\\');
	//if(tmp)
	//	*(tmp+1) = '\0';
	//MessageBox(NULL, m_path, "path", MB_OK);
	FILE *fp = fopen("in_floopy.cfg", "r");
	if(fp)
	{
		fscanf(fp, "%s", m_path);
		if(m_path[strlen(m_path)-1] != '\\')
			m_path[strlen(m_path)] = '\\';
		fclose(fp);
	}
}
