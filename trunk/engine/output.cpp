// Output.cpp: implementation of the COutput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include "output.h"
#include <assert.h>
#include <string.h>
#include <sstream>
#include "../platform.h"
#include "../common/util.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutput::COutput(LIB_HANDLE hModule) : CLoader(hModule)
{
	m_plugin			= NULL;
	m_offset			= 0;
	m_samplesToBytes	= 0;
}

COutput::~COutput()
{
	if(NULL != m_plugin)
		delete m_plugin;
}


bool COutput::Create(const char *name, SOUNDFORMAT fmt)
{
	if(NULL == name)
	{
		sprintf(m_szLastError, "plugin is NULL.\n");
		return false;
	}

	if(fmt.bitsPerSample == 0)
	{
		sprintf(m_szLastError, "fmt.bitsPerSample == 0.\n");
		return false;
	}

	if(fmt.channels == 0)
	{
		sprintf(m_szLastError, "fmt.channels == 0.");
		return false;
	}

	if(fmt.format == 0)
	{
		sprintf(m_szLastError, "fmt.format == 0.\n");
		return false;
	}

	if(fmt.frequency == 0)
	{
		sprintf(m_szLastError, "fmt.frequency == 0.\n");
		return false;
	}

	char plugin[MAX_PATH]	= {0};
	char library[MAX_FNAME]	= {0};

	get_library_name(name, library);
	get_plugin_name(name, plugin);

	if( !LoadPlugin(library) )
	{
//		sprintf(m_szLastError, "File not found: %s.\n", tmp);
		return false;
	}

	m_plugin = CreateOutput( plugin, fmt );
	IFloopySoundOutput::SetDest( m_plugin );
	m_samplesToBytes = (fmt.bitsPerSample / 8) * fmt.channels;
	return true;
}

int COutput::Write(BYTE *data, int size)
{
	if(NULL!=m_plugin && m_samplesToBytes>0)
	{
		int len = m_plugin->Write(data, size);
		if(len != size)
			copyErrorDesc();
		m_offset += len / m_samplesToBytes;
		return len;
	}
	return 0;
}

void COutput::Reset()
{
	m_offset = 0;
	m_plugin->Reset();
}

bool COutput::Open(const char *filename)
{
	bool bResult = false;

	if(NULL == m_plugin)
		sprintf(m_szLastError, "NULL == m_plugin.\n");
	else
	{
		bResult = m_plugin->Open( filename );
		if( !bResult )
			copyErrorDesc();
	}

	return bResult;
}

void COutput::copyErrorDesc()
{
	memset(m_szLastError, 0, sizeof(m_szLastError[0]*sizeof(m_szLastError)));

	if(NULL == m_plugin)
		sprintf(m_szLastError, "Plugin not created.\n");
	else
	{
		const char *err = m_plugin->GetLastErrorDesc();
		if(NULL != err)
			strncpy(m_szLastError, err, 99);
	}
}

const char *COutput::GetName()
{
	const char *result = "Plugin not created.\n";
	if(NULL != m_plugin)
		result = m_plugin->GetName();
	else
		copyErrorDesc();
	return result;
}

const char *COutput::GetDescription()
{
	const char *result = "Plugin not created.\n\0";
	if(NULL != m_plugin)
		result = m_plugin->GetDescription();
	else
		copyErrorDesc();
	return result;
}

const char *COutput::GetVersion()
{
	const char *result = "Plugin not created.\n\0";
	if(NULL != m_plugin)
		result = m_plugin->GetVersion();
	else
		copyErrorDesc();
	return result;
}

const char *COutput::GetAuthor()
{
	const char *result = "Plugin not created.\n\0";
	if(NULL != m_plugin)
		result = m_plugin->GetAuthor();
	else
		copyErrorDesc();
	return result;
}

int COutput::GetParamCount()
{
	return NULL!=m_plugin ? m_plugin->GetParamCount() : 0;
}

bool COutput::GetParamVal(int index, float *value)
{
	return NULL!=m_plugin ? m_plugin->GetParamVal(index, value) : false;
}

void COutput::SetParamVal(int index, float value)
{
	if(NULL != m_plugin)
		m_plugin->SetParamVal(index, value);
}

const char *COutput::GetParamName(int index)
{
	return NULL!=m_plugin ? m_plugin->GetParamName(index) : NULL;
}

const char *COutput::GetParamDesc(int index)
{
	return NULL!=m_plugin ? m_plugin->GetParamDesc(index) : NULL;
}

int COutput::GetPropertyCount()
{
	return NULL!=m_plugin ? m_plugin->GetPropertyCount() : 0;
}

bool COutput::GetPropertyVal(int index, float *value)
{
	return NULL!=m_plugin ? m_plugin->GetPropertyVal(index, value) : false;
}

void COutput::SetPropertyVal(int index, float value)
{
	if(NULL != m_plugin)
		m_plugin->SetPropertyVal(index, value);
}

const char *COutput::GetPropertyName(int index)
{
	return NULL!=m_plugin ? m_plugin->GetPropertyName(index) : NULL;
}

const char *COutput::GetPropertyDesc(int index)
{
	return NULL!=m_plugin ? m_plugin->GetPropertyDesc(index) : NULL;
}

void COutput::SetDest(IFloopySoundOutput *dst)
{
	if(NULL != m_plugin)
		m_plugin->SetDest(dst);
}

const char *COutput::GetLastErrorDesc()
{
	return m_szLastError;
}

int COutput::GetPosition()
{
	return NULL!=m_plugin ? m_plugin->GetPosition() : 0;
}
