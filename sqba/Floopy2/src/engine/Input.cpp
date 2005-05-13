// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef IFloopySoundInput* (*CreateProc)(void);
#define PROC_NAME "CreateInput"
#define PLUG_EXT ".dll"

CInput::CInput(char *plugin)
{
	m_hinst = NULL;
	m_plugin = NULL;
	m_offset = 0;
	Enable(TRUE);
	IFloopy::Enable(TRUE);

//	m_bRecording = FALSE;

	m_fDebug = 0.f;

	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);
	m_hinst = LoadLibraryA(filename);

	if (NULL != m_hinst)
	{
		CreateProc func = (CreateProc)GetProcAddress(m_hinst, PROC_NAME); 

		if(func != NULL) {
			m_plugin = func();
			IFloopySoundInput::SetSource(m_plugin);
		}
		else {
			fprintf(stderr, "Error: %s() not found in %s.\n", PROC_NAME, filename);
		}
	}

	delete[] filename;
}

CInput::~CInput()
{
	if(m_plugin)
		delete m_plugin;

	if(m_hinst)
		FreeLibrary(m_hinst);
}

int CInput::Read(BYTE *data, int size)
{
	int s = m_offset;
	int readBytes = 0;
	int endpos = m_offset + size;
	int len = 0;
	IFloopySoundInput *src = m_plugin;
	int origSize = size;

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
		// Ovde je bio problem sa loop-m.
		// Nisam siguran da je ovo pravi nachin
		// ali za sada radi!
		//if(IFloopy::IsEnabled() || m_plugin->GetSize() > 0)	// Fix: loop problem
		// Ovo resenje izgleda logicnije!
		if(IFloopy::IsEnabled() || m_plugin->ReadSourceIfDisabled())	// Fix: loop problem
		{													// Fix: loop problem
			size = size - readBytes;
			readBytes += src->Read(data, size);
		} else												// Fix: loop problem
			m_plugin->Reset();								// Fix: loop problem
	}

	m_offset = endpos;

	return readBytes;
}

void CInput::MoveTo(int samples)
{
	m_offset = samples * samplesToBytes();

	applyParamsAt( m_timeline.GetPrevOffset(m_offset) );

//	if(m_bRecording)
//		m_timeline.Set(m_offset, TIMELINE_PARAM_MOVETO, (float)m_offset);

	if(NULL != m_source)
		m_source->MoveTo(samples);
}

int CInput::GetSize()
{
	int tmp = 0;
	int size = 0;
	int stb = samplesToBytes();
	
	int end = getEndOffset();
	if(end > m_offset)
		size = (end - m_offset) / stb;
	else if(end > 0)
		size = end / stb;

	IFloopySoundInput *src = IFloopySoundInput::GetSource();
	tmp = src->GetSize();
	if(tmp > size)
		size = tmp;

	return size;
}

void CInput::Reset()
{
	m_offset = 0;

//	if(m_bRecording)
//		m_timeline.Set(m_offset, TIMELINE_PARAM_MOVETO, PARAM_VALUE_RESET);

	if(NULL != m_source)
		m_source->Reset();
}

int CInput::GetNextOffset(int offset)
{
	int stb = samplesToBytes();
	int next = m_timeline.GetNextOffset(offset*stb);
	return (next > 0 ? next / stb : 0);
}

void CInput::Enable(BOOL bEnable)
{
	float value = (bEnable ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED);
	m_timeline.Set(m_offset, TIMELINE_PARAM_ENABLE, value);
}

BOOL CInput::IsEnabled()
{
	float value = m_timeline.Get(m_offset, TIMELINE_PARAM_ENABLE);
	BOOL bEnabled = (PARAM_VALUE_DISABLED != value);
	return bEnabled;
}


int CInput::samplesToBytes()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}

void CInput::applyParamsAt(int offset)
{
	tParam *param = m_timeline.GetParam(offset, TIMELINE_PARAM_ENABLE);
	if(param)
	{
		IFloopy::Enable( PARAM_VALUE_DISABLED != param->value );
	}

	param = m_timeline.GetParam(offset, TIMELINE_PARAM_MOVETO);
	if(param)
	{
		if(0.f == param->value)
			m_source->Reset();
		else
			m_source->MoveTo((int)param->value);
	}

	/*param = m_timeline.GetParam(offset, TIMELINE_PARAM_RESET);
	if(param)
	{
		m_source->Reset();
	}*/

	for(int i=0; i<GetParamCount(); i++)
	{
		param = m_timeline.GetParam(offset, i);
		if(param)
		{
			m_plugin->SetParam(param->index, param->value);
		}
	}
}

int CInput::getEndOffset()
{
	int offset = 0;
	int tmp = 0;

	while((tmp=m_timeline.GetNextOffset(tmp)) > 0)
	{
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM_ENABLE);
		if(param && (param->value == PARAM_VALUE_DISABLED))
		{
			if(tmp > offset)
				offset = tmp;
		}
	}

	return offset;
}

int CInput::GetParamCount()
{
	return m_plugin->GetParamCount();
	//return m_plugin->GetParamCount() + LOCAL_PARAM_COUNT;
}


float CInput::GetParam(int index)
{
	return m_timeline.Get(m_offset, index);
	/*switch(index)
	{
	case 0:
		return m_timeline.Get(m_offset, TIMELINE_PARAM_MOVETO);
	default:
		return m_timeline.Get(m_offset, index-LOCAL_PARAM_COUNT);
	}*/
}

BOOL CInput::GetParam(int index, float *value)
{
	/*switch(index)
	{
	case 0:
		index = TIMELINE_PARAM_MOVETO;
		break;
	default:
		index -= LOCAL_PARAM_COUNT;
		break;
	}*/

	tParam *param = m_timeline.GetParam(m_offset, index);
	if(param)
	{
		*value = param->value;
		return TRUE;
	}
	return FALSE;
}

void CInput::SetParam(int index, float value)
{
	if(index == -333)
	{
		m_fDebug = value;
		return;
	}

	m_plugin->SetParam(index, value);
	m_timeline.Set(m_offset, index, value);

	/*switch(index)
	{
	case 0:
		if(0.f == value)
			m_source->Reset();
		else
			m_source->MoveTo((int)value);
		m_timeline.Set(m_offset, TIMELINE_PARAM_MOVETO, value);
		break;
	default:
		m_plugin->SetParam(index-LOCAL_PARAM_COUNT, value);
		m_timeline.Set(m_offset, index, value);
		break;
	}*/
}

int CInput::GetParamIndex(char *name)
{
	return m_plugin->GetParamIndex(name);
	//int result = m_plugin->GetParamIndex(name);
	//return (result != -1 ? result+LOCAL_PARAM_COUNT : -1);
}

char *CInput::GetParamName(int index)
{
	return m_plugin->GetParamName(index);

	/*switch(index)
	{
	case 0:
		return "MoveTo";
	default:
		return m_plugin->GetParamName(index-LOCAL_PARAM_COUNT);
	}*/
}
char *CInput::GetParamDesc(int index)
{
	return m_plugin->GetParamDesc(index);

	/*switch(index)
	{
	case 0:
		return "MoveTo";
	default:
		return m_plugin->GetParamDesc(index-LOCAL_PARAM_COUNT);
	}*/
}

void CInput::SetParamAt(int offset, int index, float value)
{
	m_timeline.Set(offset * samplesToBytes(), index, value);
}

void CInput::EnableAt(int offset, BOOL bEnable)
{
	float value = (bEnable ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED);
	m_timeline.Set(offset * samplesToBytes(), TIMELINE_PARAM_ENABLE, value);
}
