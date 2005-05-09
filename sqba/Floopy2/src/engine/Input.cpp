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
	m_fplog = initLog(plugin);

	m_hinst = NULL;
	m_plugin = NULL;
	m_offset = 0;
	Enable(TRUE);
	IFloopy::Enable(TRUE);

	m_fDebug = 0.f;

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

int CInput::Read(BYTE *data, int size)
{
	int s = m_offset;
	int readBytes = 0;
	int endpos = m_offset + size;
	int len = 0;
	IFloopySoundInput *src = m_plugin;
	int origSize = size;

	//assert(!m_bDebug);
	/*if(m_fDebug == 3.f)
	{
		BOOL a = IFloopy::IsEnabled();
		printf("Read (loop): m_offset=%d\n", m_offset);
		//m_timeline.dump(stdout);
	}*/

	/*if(m_fDebug == 3.f || m_fDebug == 2.f || m_fDebug == 1.f)
	{
		//BOOL a = IFloopy::IsEnabled();
		//m_timeline.dump(stdout);
		int x = (GetSource() ? GetSource()->IsEnabled() : -1);
		int n = (int)m_fDebug;
		switch(n)
		{
		case 1:
			printf("wavfile Read() m_offset=%d %d %d %d\n",
				m_offset, IFloopy::IsEnabled(), IsEnabled, x);
			break;
		case 2:
			printf("cache   Read() m_offset=%d %d %d %d\n",
				m_offset, IFloopy::IsEnabled(), IsEnabled, x);
			break;
		case 3:
			printf("loop    Read() m_offset=%d %d %d %d\n",
				m_offset, IFloopy::IsEnabled(), IsEnabled, x);
			break;
		}
	}*/

	// Apply all due parameters
	applyParamsAt( m_offset );

	while(((s=m_timeline.GetNextOffset(s)) < endpos) && (s>0))
	{
		/*if(m_fDebug == 1.f)
		{
			//BOOL a = IFloopy::IsEnabled();
			m_timeline.dump(stdout);
		}*/

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
		//BOOL b = src->IsEnabled();
		// if src->start + src->len < m_offset ?
		size = size - readBytes;
		readBytes += src->Read(data, size);
	}

	m_offset = endpos;

	//assert( readBytes <= origSize );

	/*if(readBytes < origSize)
	{
		WAVFORMAT *fmt = m_plugin->GetFormat();
		int x = m_offset * ( (fmt->size / 8) * fmt->channels );
		if(x < GetSize())
		{
			return origSize;
		}
	}*/

	return readBytes;
}

void CInput::MoveTo(int samples)
{
	WAVFORMAT *fmt = m_plugin->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	m_offset = samples * ( (fmt->bitsPerSample / 8) * fmt->channels );

//	fprintf(m_fplog, "%s MoveTo %d\n", m_plugin->GetName(), samples);

	//assert(!m_bDebug);
	/*if(m_bDebug)
	{
		int debug = 1;
		int prev = m_timeline.GetPrevOffset(m_offset);
		debug = 1;
	}*/

	/*if(m_fDebug == 3.f || m_fDebug == 2.f || m_fDebug == 1.f)
	{
		//BOOL a = IFloopy::IsEnabled();
		//m_timeline.dump(stdout);
		int n = (int)m_fDebug;
		switch(n)
		{
		case 1:
			printf("wavfile MoveTo(%d) m_offset=%d\n", samples, m_offset);
			break;
		case 2:
			printf("cache   MoveTo(%d) m_offset=%d\n", samples, m_offset);
			break;
		case 3:
			printf("loop    MoveTo(%d) m_offset=%d\n", samples, m_offset);
			break;
		}
	}*/


	/////////////////////////////////////////////////////////////
	// Apply all previous parameters?
	/*int s = 0;
	while((s=m_timeline.GetNextOffset(s)) > 0 && (s < m_offset))
	{
		applyParamsAt( s );
	}*/

	// or:

	// Apply all due parameters
	applyParamsAt( m_timeline.GetPrevOffset(m_offset) );
	/////////////////////////////////////////////////////////////

	/*if(m_fDebug == 3.f)
	{
		BOOL a = IFloopy::IsEnabled();
		//m_timeline.dump(stdout);
		//printf("MoveTo: %f %d %d\n", m_fDebug, m_offset, samples);
	}*/

	//if(m_fDebug == 2.f && samples == 132300) // OK
	/*if(m_fDebug == 2.f && samples == 32716)
	{
		bool b = true;
	}*/

	if(NULL != m_source)
		m_source->MoveTo(samples);
//	if(NULL != GetSource())
//		GetSource()->MoveTo(samples);
}

void CInput::Reset()
{
	m_offset = 0;
	if(NULL != m_source)
		m_source->Reset();
}

void CInput::SetParam(int index, float value)
{
	if(index == -333)
	{
		m_fDebug = value;
		return;
	}

	m_timeline.Set(m_offset, index, value);

	logParamSet(m_offset, index, value);

	m_plugin->SetParam(index, value);

	/*if(m_fplog)
	{
		char *param = m_plugin->GetParamName(index);
		char *comp = m_plugin->GetName();
		fprintf(m_fplog, "%d\t%s.%s=%f\n", m_offset, comp, param, value);
	}*/
}

int CInput::GetNextOffset(int offset)
{
	int next = m_timeline.GetNextOffset(offset*samplesToBytes());
	return (next > 0 ? next / samplesToBytes() : 0);
}

int CInput::samplesToBytes()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}

float CInput::GetParam(int index)
{
	return m_timeline.Get(m_offset, index);
}

void CInput::Enable(BOOL bEnable)
{
	m_timeline.Set(m_offset, TIMELINE_PARAM, (bEnable ? PARAM_ENABLE : PARAM_DISABLE));
	logParamSet(m_offset, TIMELINE_PARAM, (bEnable ? PARAM_ENABLE : PARAM_DISABLE));

//	if(m_plugin)
//		m_plugin->Enable(bEnable);
}

BOOL CInput::IsEnabled()
{
	BOOL bEnabled = (PARAM_DISABLE != m_timeline.Get(m_offset, TIMELINE_PARAM));
	return bEnabled;
//	return (IFloopy::IsEnabled() && bEnabled);
}

int CInput::GetSize()
{
	int size = 0;
	int tmp = 0;

	// Find last offset
	while((tmp=m_timeline.GetNextOffset(tmp)) > 0)
	{
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM);
		if(param && (param->value == PARAM_DISABLE))
		{
			if(tmp > size)
				size = tmp;
		}
	}

	WAVFORMAT *fmt = m_plugin->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int x = ( (fmt->bitsPerSample / 8) * fmt->channels );
	size /= x;

	/*if(m_fDebug == 3.f)
	{
		int debug = 1;
		IFloopySoundInput *tmp = m_plugin->GetSource();
		BOOL b = IFloopy::IsEnabled();
		int x=m_timeline.GetPrevOffset(m_offset);
	}*/

//	if(0 == size)
//	{
		IFloopySoundInput *src = IFloopySoundInput::GetSource();
/*		while(src)
		{
			try
			{*/
				tmp = src->GetSize();
				if(tmp > size)
					size = tmp;
				src = src->GetSource();
			/*}
			catch(...)
			{
				printf("This happens only in debug mode!!!\n");
			}
		}*/
//	}

	return size;
}


void CInput::applyParamsAt(int offset)
{
	tParam *param = m_timeline.GetParam(offset, TIMELINE_PARAM);
	if(param)
	{
		IFloopy::Enable( PARAM_DISABLE != param->value );
		logParamChange(offset, param);
		/*if(m_fDebug == 1.f)
		{
			int a=0;
		}*/
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
/*
void CInput::Close()
//void CInput::dump(FILE *fp)
{
	if(0 == stricmp(m_plugin->GetName(), "tonegen"))
	{
		FILE *fp = stdout;
		fprintf(fp, "%s timeline dump:\n", m_plugin->GetName());
		m_timeline.dump(fp);
	}
	IFloopySoundInput::Close();
}
*/
FILE *CInput::initLog(char *plugin)
{
	return NULL;
	//return stdout;

	//if(0 != strcmpi(plugin, "tonegen"))
	//	return NULL;

	char *logname = new char[strlen(plugin) + 5];
	strcpy(logname, plugin);
	strcat(logname, ".txt");
	FILE *fp = fopen(logname, "w");
	delete[] logname;

	return fp;
}

void CInput::logParamChange(int offset, tParam *param)
{
//	if(!(m_fDebug == 3.f || m_fDebug == 2.f || m_fDebug == 1.f))
//		return;

	if(m_fplog)
	{
		char *plugname = m_plugin->GetName();
		if(TIMELINE_PARAM == param->index)
		{
			BOOL bEnable = (PARAM_DISABLE != param->value);
			fprintf(m_fplog, "%d\t%s %s\n", offset,
				(bEnable?"Enabled":"Disabled"), plugname);
				//fflush(m_fplog);
		}
		else
		{
			char *paramname = m_plugin->GetParamName(param->index);
			fprintf(m_fplog, "%d\t%s.%s = %f\n", offset, plugname,
				paramname, param->value);
				//fflush(m_fplog);
		}
	}
}

void CInput::logParamSet(int offset, int index, float value)
{
//	if(!(m_fDebug == 3.f || m_fDebug == 2.f || m_fDebug == 1.f))
//		return;

	if(m_fplog && m_plugin)
	{
		WAVFORMAT *fmt = m_plugin->GetFormat();
		assert(fmt->frequency > 0);
		if(m_plugin)
		{
			char *plugname = m_plugin->GetName();
			if(TIMELINE_PARAM == index)
			{
				BOOL bEnable = (PARAM_DISABLE != value);
				fprintf(m_fplog, "SetParam: %d %d\t%s %s\n", offset, offset/fmt->frequency,
					(bEnable?"Enabled":"Disabled"), plugname);
				//fflush(m_fplog);
			}
			else
			{
				char *paramname = m_plugin->GetParamName(index);
				fprintf(m_fplog, "SetParam: %d %d\t%s.%s set to %f\n", offset,
					offset/fmt->frequency, plugname,
					paramname, value);
				//fflush(m_fplog);
			}
		}
	}
}
