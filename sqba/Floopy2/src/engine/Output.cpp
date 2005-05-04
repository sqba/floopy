// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include "Output.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundOutput*	(*CreateOutputProc)(int, int, int);

COutput::COutput(char *plugin, WAVFORMAT *fmt)
{
	m_hinst = NULL;
	m_plugin = NULL;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, TEXT(".dll\0"));
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateOutputProc func = (CreateOutputProc)GetProcAddress(m_hinst, "CreateOutput"); 

		if(func != NULL) {
			printf("CreateOutput() found in %s.\n", filename);
			m_plugin = func(
			fmt->freq,
			fmt->size,
			fmt->channels);
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

UINT COutput::Write(BYTE *data, UINT size)
{
	UINT len = (NULL != m_plugin ? m_plugin->Write(data, size) : 0);
	m_offset += len;
	return len;
}

void COutput::SetParam(int index, float value)
{
	// Take m_offset in account!!!
	m_plugin->SetParam(index, value);
}

float COutput::GetParam(int index)
{
	// Take m_offset in account!!!
	return m_plugin->GetParam(index);
}

/*
BOOL COutput::Open(char *filename)
{
	if(NULL != m_plugin)
		return m_plugin->Open(filename);
	return FALSE;
}

UINT COutput::Write(BYTE *data, UINT size)
{
	if(NULL != m_plugin)
		return m_plugin->Write(data, size);
	return 0;
}

void COutput::Close()
{
	if(NULL != m_plugin)
		m_plugin->Close();
}
*/