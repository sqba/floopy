// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include "Output.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundOutput* (*CreateProc)(WAVFORMAT);
#define PROC_NAME "CreateOutput"
#define PLUG_EXT ".dll"

COutput::COutput(char *plugin, WAVFORMAT fmt)
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
			printf("CreateOutput() found in %s.\n", filename);
			assert((fmt.bitsPerSample > 0) && (fmt.channels > 0));
			m_plugin = func( fmt );
			IFloopySoundOutput::SetDest( m_plugin );
		}
		else
 			printf("Error: CreateOutput() not found in %s.\n", filename);
   }
}

COutput::~COutput()
{
	if(NULL != m_plugin)
		delete m_plugin;

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}

int COutput::Write(BYTE *data, int size)
{
	int len = (NULL != m_plugin ? m_plugin->Write(data, size) : 0);
	m_offset += len;
	return len;
}

void COutput::SetParam(int index, float value)
{
	m_plugin->SetParam(index, value);
}

float COutput::GetParam(int index)
{
	return m_plugin->GetParam(index);
}
