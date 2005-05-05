// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundInput* (*CreateProc)(void);
#define PROC_NAME "CreateInput"
#define PLUG_EXT ".dll"

CInput::CInput(char *plugin)
{
	initLog(plugin);

	m_hinst = NULL;
	m_plugin = NULL;
	m_offset = 0;
	Enable(TRUE);
	IFloopy::Enable(TRUE);

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateProc func = (CreateProc)GetProcAddress(m_hinst, PROC_NAME); 

		if(func != NULL) {
			if(m_fplog)
				fprintf(m_fplog, "%s() found in %s.\n", PROC_NAME, filename);
			m_plugin = func();
			IFloopySoundInput::SetSource(m_plugin);
		}
		else
		{
			if(m_fplog)
 				fprintf(m_fplog, "Error: %s() not found in %s.\n", PROC_NAME, filename);
		}
	}
	delete[] filename;
}

CInput::~CInput()
{
	if(NULL != m_plugin)
		delete m_plugin;

	if(NULL != m_hinst)
		FreeLibrary(m_hinst);

	if( m_fplog && (m_fplog != stdout) )
	{
		fclose( m_fplog );
	}
}

UINT CInput::Read(BYTE *data, UINT size)
{
	UINT s = m_offset;
	UINT readBytes = 0;
	UINT endpos = m_offset + size;
	UINT len = 0;
	IFloopySoundInput *src = m_plugin;
	UINT origSize = size;

	if(m_plugin){
		char *name = m_plugin->GetName();
		if(strcmp(name, "loop"))
		{
			int dbg = 0;
		}
	}

	// Apply all due parameters
	applyParamsAt( m_offset );

	while(((s=m_timeline.GetNextOffset(s)) < endpos) && (s>0))
	{
		// Fill small chunks between parameter changes
		src = (IFloopy::IsEnabled() ? m_plugin : m_plugin->GetSource());
		if(src)
		{
			size = s - m_offset;
			len = src->Read(data, size);
			readBytes += len;
		}
		else
			len = s - m_offset;
		
		m_offset += len;
	
		applyParamsAt( s );
	}

	// Fill the rest of the data
	src = (IFloopy::IsEnabled() ? m_plugin : m_plugin->GetSource());
	if(src)
	{
		// if src->start + src->len < m_offset ?
		size = size - readBytes;
		readBytes += src->Read(data, size);
	}

	m_offset = endpos;

	//assert( readBytes <= origSize );

	return readBytes;
}

void CInput::MoveTo(UINT samples)
{
	WAVFORMAT *fmt = m_plugin->GetFormat();
	m_offset = samples * ( (fmt->size / 8) * fmt->channels );

	// Apply all due parameters
	applyParamsAt( m_timeline.GetLastOffset(m_offset) );

	if(NULL != m_source)
		m_source->MoveTo(samples);
}

void CInput::Reset()
{
	m_offset = 0;
	if(NULL != m_source)
		m_source->Reset();
}

void CInput::SetParam(int index, float value)
{
	m_timeline.Set(m_offset, index, value);

	logParamSet(m_offset, index, value);

	/*if(m_fplog)
	{
		char *param = m_plugin->GetParamName(index);
		char *comp = m_plugin->GetName();
		fprintf(m_fplog, "%d\t%s.%s=%f\n", m_offset, comp, param, value);
	}*/
}

float CInput::GetParam(int index)
{
	return m_timeline.Get(m_offset, index);
}

void CInput::Enable(BOOL bEnable)
{
	m_timeline.Set(m_offset, TIMELINE_PARAM, (bEnable ? PARAM_ENABLE : PARAM_DISABLE));
	logParamSet(m_offset, TIMELINE_PARAM, (bEnable ? PARAM_ENABLE : PARAM_DISABLE));
}

BOOL CInput::IsEnabled()
{
	return (IFloopy::IsEnabled() && (PARAM_DISABLE != m_timeline.Get(m_offset, TIMELINE_PARAM)));
}


void CInput::applyParamsAt(UINT offset)
{
	tParam *param = m_timeline.GetParam(offset, TIMELINE_PARAM);
	if(param)
	{
		IFloopy::Enable( PARAM_DISABLE != param->value );
		logParamChange(offset, param);
	}

	for(int i=0; i<GetParamCount(); i++)
	{
		param = m_timeline.GetParam(offset, i);
		if(param)
		{
			m_plugin->SetParam(param->index, param->value);
			logParamChange(offset, param);
		}
	}
}

void CInput::initLog(char *plugin)
{
	//m_fplog = NULL;
	//m_fplog = stdout;
	char *logname = new char[strlen(plugin) + 5];
	strcpy(logname, plugin);
	strcat(logname, ".txt");
	m_fplog = fopen(logname, "w");
	delete[] logname;
}

void CInput::logParamChange(UINT offset, tParam *param)
{
	if(m_fplog)
	{
		char *plugname = m_plugin->GetName();
		if(TIMELINE_PARAM == param->index)
		{
			BOOL bEnable = (PARAM_DISABLE != param->value);
			fprintf(m_fplog, "%d\t%s %s\n", offset,
				(bEnable?"Enabled":"Disabled"), plugname);
		}
		else
		{
			char *paramname = m_plugin->GetParamName(param->index);
			fprintf(m_fplog, "%d\t%s.%s = %f\n", offset, plugname,
				paramname, param->value);
		}
	}
}

void CInput::logParamSet(UINT offset, int index, float value)
{
	if(m_fplog)
	{
		if(m_plugin)
		{
			char *plugname = m_plugin->GetName();
			if(TIMELINE_PARAM == index)
			{
				BOOL bEnable = (PARAM_DISABLE != value);
				fprintf(m_fplog, "SetParam: %d\t%s %s\n", offset,
					(bEnable?"Enabled":"Disabled"), plugname);
			}
			else
			{
				char *paramname = m_plugin->GetParamName(index);
				fprintf(m_fplog, "SetParam: %d\t%s.%s set to %f\n", offset, plugname,
					paramname, value);
			}
		}
	}
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

void CInput::Close()
{
	if(NULL != m_plugin)
		m_plugin->Close();
}
*/