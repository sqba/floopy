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

typedef IFloopySoundInput*	(*CreateInputProc)(void);

CInput::CInput(char *plugin)
{
	m_hinst = NULL;
	m_plugin = NULL;
	m_offset = 0;
	Enable(TRUE);
	IFloopy::Enable(TRUE);

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
	UINT s = m_offset;
	tParam *param = NULL;
	UINT readBytes = 0;
	UINT origSize = size;
	UINT origOffset = m_offset;
	UINT endpos = origOffset + origSize;

	/*if(2116600 < m_offset && m_offset < 2116900)
	{
		char *name = m_plugin->GetName();
		m_offset = m_offset;
	}*/

	//BOOL bActive = IsActive(); //IFloopy::IsActive();

	/*char *name = m_plugin->GetName();
	//BOOL bProblem = (0 == strcmp(name, "tonegen"));
	BOOL bProblem = (0 == strcmp(name, "loop"));
	if(bProblem)
	{
		int z=0;
	}*/

	param = m_timeline.GetParam(m_offset, TIMELINE_PARAM);
	if(param)
	{
		//BOOL bActive = (PARAM_ENABLE == param->value);
		BOOL bActive = (PARAM_DISABLE != param->value);
		IFloopy::Enable(bActive);
	}

	// Apply all due parameters
	for(int i=0; i<GetParamCount(); i++)
	{
		param = m_timeline.GetParam(m_offset, i);
		if(param && (m_offset == param->offset))
		{
			m_plugin->SetParam(param->index, param->value);
		}
	}
	/*if(s==1587600)//1587600)
	{
		char *name = m_plugin->GetName();
	}*/

	while(((s=m_timeline.GetNextOffset(s)) < endpos) && (s>0))
	{
		/*if(s==1587600)//1587600)
		{
			char *name = m_plugin->GetName();
		}*/
		/*if(bProblem)
		{
			param = m_timeline.GetParam(s, TIMELINE_PARAM);
			int z=0;
		}*/

		if(IFloopy::IsEnabled())
		{
			//char *name = m_plugin->GetName();
			size = s - m_offset;
			UINT len = m_plugin->Read(data, size);
			readBytes += len;
			m_offset += len;
		}
		else
		{
			IFloopySoundInput *src = m_plugin->GetSource();
			if(src)
			{
				size = s - m_offset;
				UINT len = src->Read(data, size);
				readBytes += len;
				m_offset += len;
			}
			else
				m_offset += s - m_offset;
		}
		/*else
		{
			m_offset += s - m_offset;
			//char *name = m_plugin->GetName();
		}*/
	
		param = m_timeline.GetParam(m_offset, TIMELINE_PARAM);
		if(param)
		{
			/*if(bProblem)
			{
				int z=0;
			}*/
			//BOOL bActive = (PARAM_ENABLE == param->value);
			BOOL bActive = (PARAM_DISABLE != param->value);
			IFloopy::Enable(bActive);
		}
		
		for(int i=0; i<GetParamCount(); i++)
		{
			param = m_timeline.GetParam(s, i);
			if(param)
			{
				m_plugin->SetParam(param->index, param->value);
				/*if(bProblem)
				{
					int z=0;
				}*/
			}
		}
	}

	if(IFloopy::IsEnabled())
	{
		/*if(bProblem)
		{
			int z=0;
		}*/
		size = size - readBytes;

		UINT len = m_plugin->Read(data, size);
		readBytes += len;

		//assert(origSize >= readBytes);
	}
	else
	{
		IFloopySoundInput *src = m_plugin->GetSource();
		if(src)
		{
			size = size - readBytes;

			UINT len = src->Read(data, size);
			readBytes += len;
		}
	}

	m_offset = endpos;

	return readBytes;
}

void CInput::MoveTo(UINT samples)
{
	WAVFORMAT *fmt = m_plugin->GetFormat();
	int x = (fmt->size / 8) * fmt->channels;

	m_offset = samples * x;

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
	char *param = m_plugin->GetParamName(index);
	char *comp = m_plugin->GetName();
	printf("%d\t%s\t%s: %f\n", m_offset, comp, param, value);

	// Take m_offset in account!!!
	m_timeline.Set(m_offset, index, value);
	//m_plugin->SetParam(index, value);
}

float CInput::GetParam(int index)
{
	// Take m_offset in account!!!
	return m_timeline.Get(m_offset, index);
	//return m_plugin->GetParam(index);
}

void CInput::Enable(BOOL bEnable)
{
	if(m_plugin && !bEnable)
	{
		char *comp = m_plugin->GetName();
		printf("Enable %d\t%s\t%d\n", m_offset, comp, bEnable);
	}
	else
		printf("Enable %d\t%d\n", m_offset, bEnable);
	m_timeline.Set(m_offset, TIMELINE_PARAM, (bEnable ? PARAM_ENABLE : PARAM_DISABLE));
//	IFloopy::SetActive(bActive);
}

BOOL CInput::IsEnabled()
{
	/*tParam *param = NULL;
	param = m_timeline.GetParam(m_offset, TIMELINE_PARAM);
	if(param)
	{
		BOOL bActive = (PARAM_ENABLE == param->value);
		IFloopy::Enable(bActive);
	}*/
	return (IFloopy::IsEnabled() && (PARAM_DISABLE != m_timeline.Get(m_offset, TIMELINE_PARAM)));
//	return IFloopy::IsActive();
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


UINT CInput::Read(BYTE *data, UINT size)
{
	UINT s = 0;
	tParam *param = NULL;
	UINT readBytes = 0;
	UINT origSize = size;

//	WAVFORMAT *fmt = m_plugin->GetFormat();
//	int x = (fmt->size / 8) * fmt->channels;
//	UINT samples = m_offset / x;

	for(int i=0; i<GetParamCount(); i++)
	{
		param = m_timeline.GetParam(m_offset, i);
		if(param && (m_offset == param->offset))
		{
			//char *paramname = m_plugin->GetParamName(param->index);
			//char *pluginname = m_plugin->GetSource()->GetSource()->GetName();
			//printf("%s %s: %f\n", pluginname, paramname, param->value);
			m_plugin->SetParam(param->index, param->value);
		}
	}

//	UINT stemp = (m_offset+size) / x;;
	while(((s=m_timeline.GetNextOffset(m_offset)) < (m_offset+size)) && (s>0))
	{
		//size = (s * x) - m_offset;
		size = s - m_offset;
		UINT len = m_plugin->Read(data, size);
		readBytes += len;
//		m_offset += len;
		for(int i=0; i<GetParamCount(); i++)
		{
			param = m_timeline.GetParam(s, i);
			if(param)
			{
				char *paramname = m_plugin->GetParamName(param->index);
				//char *pluginname = m_plugin->GetSource()->GetSource()->GetName();
				//printf("%s %s: %f\n", pluginname, paramname, param->value);
				m_plugin->SetParam(param->index, param->value);
			}
		}
	}

	//UINT len = (NULL != m_plugin ? m_plugin->Read(data, size) : 0);
	UINT len = m_plugin->Read(data, size);
	readBytes += len;
//	m_offset += len;
	m_offset += size;
//	return len;
	assert(origSize >= readBytes);
	return readBytes;
}
*/