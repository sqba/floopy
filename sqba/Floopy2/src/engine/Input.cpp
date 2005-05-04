// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundInput*	(*CreateInputProc)(void);

CInput::CInput(char *plugin)
{
	m_hinst = NULL;
	m_plugin = NULL;
	m_offset = 0;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, TEXT(".dll\0"));
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateInputProc func = (CreateInputProc)GetProcAddress(m_hinst, "CreateInput"); 

		if(func != NULL) {
			printf("CreateInput() found in %s.\n", filename);
			m_plugin = func();
			IFloopySoundInput::SetSource(m_plugin);
		}
		else
 			printf("Error: CreateInput() not found in %s.\n", filename);
	}
}

CInput::~CInput()
{
	if(NULL != m_plugin)
		delete m_plugin;

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);
}

UINT CInput::Read(BYTE *data, UINT size)
{
	UINT len = (NULL != m_plugin ? m_plugin->Read(data, size) : 0);
	m_offset += len;
	return len;
}

void CInput::MoveTo(UINT samples)
{
	m_offset = samples;

	if(NULL != m_source)
		m_source->MoveTo(samples);
}

void CInput::Reset()
{
	m_offset = 0;
	if(NULL != m_source)
		m_source->Reset();
}

/*
BOOL CInput::Open(char *filename)
{
	return (NULL != m_plugin ? m_plugin->Open(filename) : FALSE);
}

DWORD CInput::GetSize()
{
	return (NULL != m_plugin ? m_plugin->GetSize() : 0);
}

UINT CInput::Read(BYTE *data, UINT size, UINT offset)
{
	if(NULL != m_plugin)
	{
		UINT len = m_plugin->Read(data, size, offset);
		m_offset += len;
		return len;
	}
	return 0;
}

void CInput::Close()
{
	if(NULL != m_plugin)
		m_plugin->Close();
}
*/