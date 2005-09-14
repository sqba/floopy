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

typedef IFloopySoundOutput* (*CreateProc)(char*, SOUNDFORMAT);
#define PROC_NAME "CreateOutput"
#define PLUG_EXT ".dll"

COutput::COutput()
{
	m_hinst = NULL;
	m_plugin = NULL;
	m_offset = 0;
	m_samplesToBytes = 0;
}

COutput::~COutput()
{
	if(NULL != m_plugin)
		delete m_plugin;

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}


bool COutput::Create(char *plugin, SOUNDFORMAT fmt)
{
	char *library = plugin;

	char *sep = strrchr(plugin, '.');
	if(sep)
	{
		char tmp[MAX_PATH] = {0};
		strcpy(tmp, plugin);
		char *sep = strrchr(tmp, '.');
		plugin = sep+1;
		*sep = 0;
		library = tmp;
	}

	m_hinst = NULL;
	m_plugin = NULL;

	char *filename = new char[strlen(library) + 5];
	strcpy(filename, library);
	strcat(filename, PLUG_EXT);
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateProc func = (CreateProc)GetProcAddress(m_hinst, PROC_NAME); 

		if(func != NULL) {
			//printf("CreateOutput() found in %s.\n", filename);
			//assert((fmt.bitsPerSample > 0) && (fmt.channels > 0));
			if((fmt.bitsPerSample > 0) && (fmt.channels > 0))
			{
				m_plugin = func( plugin, fmt );
				IFloopySoundOutput::SetDest( m_plugin );
				m_samplesToBytes = (fmt.bitsPerSample / 8) * fmt.channels;
				return true;
			}
		}
		else
		{
 			//fprintf(stderr, "Error: %s not found in %s.\n", PROC_NAME, filename);
			sprintf(m_szLastError, "Error: %s not found in %s\0.\n", PROC_NAME, filename);
		}
	}
	else
	{
		sprintf(m_szLastError, "Error: %s not found.\n\0", filename);
	}
	return false;
}

int COutput::Write(BYTE *data, int size)
{
	int len = (NULL != m_plugin ? m_plugin->Write(data, size) : 0);
	m_offset += len / m_samplesToBytes;
	return len;
}

bool COutput::GetParamVal(int index, float *value)
{
	return m_plugin->GetParamVal(index, value);
}

void COutput::SetParamVal(int index, float value)
{
	m_plugin->SetParamVal(index, value);
}

int COutput::GetPosition()
{
	if(NULL != m_plugin)
		return m_plugin->GetPosition();
	else
		return 0; // m_offset?
}

void COutput::Reset()
{
	m_offset = 0;
	m_plugin->Reset();
}
