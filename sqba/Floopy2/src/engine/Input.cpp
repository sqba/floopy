// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


typedef IFloopySoundInput* (*CreateProc)(char *name);
#define PROC_NAME "CreateInput"
#define PLUG_EXT ".dll"


CInput::CInput(UpdateCallback func)
{
	m_hinst		= NULL;
	m_plugin	= NULL;
	m_offset	= 0;
	m_iCheck	= 23526; // some random number
	m_callback	= func;

	memset(m_name, 0, 50);
	memset(m_szLastError, 0, sizeof(m_szLastError));
	memset(m_szObjPath,   0, sizeof(m_szObjPath));

	memset(m_libraryName, 0, MAX_PATH);
	memset(m_pluginName, 0, 50);

	Enable(TRUE);
	IFloopy::Enable(TRUE);
	
	// Optimization variables
	m_nStartOffset = m_nEndOffset = m_nSamplesToBytes = 0;

	m_red = m_green = m_blue = 255;

#ifdef _DEBUG_TIMER_
	// Timing valiables
	m_bDebugTimer = TRUE;
	m_nFrameSize = m_time = m_nFrameCount = 0;
#endif // _DEBUG_TIMER_

//	m_bRecording = FALSE;

	m_fDebug = 0.f;
}

CInput::~CInput()
{
	if(m_plugin)
		delete m_plugin;

	if(m_hinst)
		FreeLibrary(m_hinst);
}

/**
 * Create the plugin.
 * @param plugin library name.
 * @return TRUE if succesfull.
 */
BOOL CInput::Create(char *name)
{
	BOOL result = FALSE;

	/////////////////////////////////////////
	// Split name: [library].[plugin]
	/////////////////////////////////////////
	char *plugin  = name;
	char *library = name;

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
	/////////////////////////////////////////

	char *tmpstr = new char[strlen(library) + 5];
	char *filename = tmpstr;
	strcpy(filename, library);
	strcat(filename, PLUG_EXT);

	m_hinst = LoadLibraryA(filename);

	if(NULL == m_hinst)
	{
		filename = plugin;
		m_hinst = LoadLibraryA(filename);
	}

	if (NULL != m_hinst)
	{
		strcpy(m_szObjPath, filename);

		CreateProc func = (CreateProc)GetProcAddress(m_hinst, PROC_NAME); 

		if(func != NULL) {
			m_plugin = func( plugin );
			if(m_plugin)
			{
				memset(m_libraryName, 0, MAX_PATH);
				memset(m_pluginName, 0, 50);
				char *tmp = strrchr(library, '\\');
				if(tmp)
					strcpy(m_libraryName, tmp+1);
				else
					strcpy(m_libraryName, library);
				strcpy(m_pluginName, plugin);
				
				memset(m_fullName, 0, MAX_PATH);
				strcpy(m_fullName, m_libraryName);
				strcat(m_fullName, ".");
				strcat(m_fullName, m_pluginName);

				IFloopySoundInput::SetSource(m_plugin);
				result = TRUE;
			}
			else
				sprintf(m_szLastError, "Error: Plugin '%s' not created by %s.\n\0", plugin, filename);
		}
		else
			//fprintf(stderr, "Error: %s() not found in %s.\n", PROC_NAME, filename);
			sprintf(m_szLastError, "Error: Function %s not found in %s.\n\0", PROC_NAME, filename);
	}
	else
		sprintf(m_szLastError, "Error: %s not found.\n\0", filename);

	delete[] tmpstr;
	return result;
}

/**
 * Set another engine to be the source.
 * @param src pointer to the engine.
 * @return TRUE (if succesfull).
 */
BOOL CInput::Create(IFloopySoundEngine *src)
{
	m_plugin = src;
	
	IFloopySoundInput::SetSource(m_plugin);
	
	char *name = src->GetDisplayName();
	if(name)
		SetDisplayName(name, strlen(name));

	return TRUE;
}

BOOL CInput::SetSource(IFloopySoundInput *src)
{
	// Check if src has been created by this engine.
	// Ne radi ako je src engine!
//	if(m_iCheck != ((CInput*)src)->m_iCheck)
//		return FALSE;

	BOOL result = (m_plugin ? m_plugin->SetSource(src) : FALSE);
	
	_recalcVariables();

	//if(m_callback && result) m_callback(this, m_offset/_getSamplesToBytes(), -333);

	return result;
}

IFloopySoundInput *CInput::GetSource()
{
	return m_plugin ? m_plugin->GetSource() : NULL;
}

int CInput::Read(BYTE *data, int size)
{
	assert(size >= 0);

	// Passed the end
	if(m_offset >= m_nStartOffset + m_nEndOffset)
		return EOF;

	if(_isEngine())
	{
		// Avoid wasting processor time
		if((m_offset + size) <= m_nStartOffset)
		{
			m_offset += size;
			return size;
		}
	}

	int offset = m_offset;
	int readBytes = 0;
	int endpos = m_offset + size;
	int len = 0;
	IFloopySoundInput *src = m_plugin;
	BOOL bEOF = FALSE;

#ifdef _DEBUG_TIMER_
	clock_t start = _debugStartMeasuring();
#endif // _DEBUG_TIMER_

	// Apply all due parameters
	applyParamsAt( m_offset );

	while(((offset=m_timeline.GetNextOffset(offset)) < endpos) && (offset>0) && !bEOF)
	{
		// Fill small chunks between parameter changes
		src = (IFloopy::IsEnabled() ? m_plugin : m_plugin->GetSource());
		if(src && (IFloopy::IsEnabled() || m_plugin->ReadSourceIfDisabled()))
			len = src->Read(data, offset - m_offset);
		else
			len = offset - m_offset;

		if(EOF != len)
		{
			data += len;
			readBytes += len;
			m_offset += len;
		}
		else
			bEOF = TRUE;
	
		applyParamsAt( offset );
	}

	if( !bEOF )
	{
		// Fill the rest of the data
		src = (IFloopy::IsEnabled() ? m_plugin : m_plugin->GetSource());
		if(src && (IFloopy::IsEnabled() || m_plugin->ReadSourceIfDisabled()))
		{
			if(size > readBytes)
			{
				len = src->Read(data, size - readBytes);
				if(EOF != len)
					readBytes += len;
			}
		}
	}

#ifdef _DEBUG_TIMER_
	_debugStopMeasuring(start, size);
#endif // _DEBUG_TIMER_


	m_offset = endpos;

	if(readBytes == 0)
		readBytes = bEOF ? EOF : size;

	return readBytes;
}

/**
 * Moves to the given position.
 * @param samples number of samples.
 */
void CInput::MoveTo(int samples)
{
//	_recalcVariables();

	if(m_nSamplesToBytes > 0)
	{
		m_offset = samples * m_nSamplesToBytes;

		applyParamsAt( m_timeline.GetPrevOffset(m_offset) );

		if( _isEngine() )
		{
			int start = m_nStartOffset / m_nSamplesToBytes;
			if(start > 0 && samples > start)
				samples -= start;
		}

		if(NULL != m_source)
			m_source->MoveTo(samples);
	}
}

/**
 * Returns the size of the track source.
 * @return number of samples
 */
int CInput::GetSize()
{
	_recalcVariables();

	int size = 0;

	if(m_plugin)
	{
		if(m_nEndOffset > 0 && m_nSamplesToBytes > 0)
			size = m_nEndOffset / m_nSamplesToBytes;
		else if (m_nSamplesToBytes > 0)
			size = m_nStartOffset / m_nSamplesToBytes + m_plugin->GetSize();
		else
			size = m_plugin->GetSize();
	}

	return size;
}

int CInput::GetSourceSize()
{
	return m_plugin ? m_plugin->GetSourceSize() : 0;
}

void CInput::Reset()
{
	m_offset = 0;

	if(NULL != m_source)
		m_source->Reset();
}

int CInput::GetNextOffset(int offset)
{
	int next = m_timeline.GetNextOffset(offset*m_nSamplesToBytes);
	return (next > 0 ? next / m_nSamplesToBytes : 0);
}

/**
 * Enable at the current offset.
 * @param bEnable enable/disable.
 */
void CInput::Enable(BOOL bEnable)
{
	float value = (bEnable ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED);
	m_timeline.SetParamVal(m_offset, TIMELINE_PARAM_ENABLE, value);

	if(m_plugin)
		m_plugin->Enable(bEnable);

	_recalcVariables();
	//m_nStartOffset = _getStartOffset();
	//m_nEndOffset = _getEndOffset();

//	SOUNDFORMAT *fmt = GetFormat();
//	if(m_callback && (fmt->bitsPerSample > 0) && (fmt->channels > 0))
//		m_callback(this, m_offset/_getSamplesToBytes(), TIMELINE_PARAM_ENABLE);
}

/**
 * @return TRUE if the object is enabled at the current offset.
 */
BOOL CInput::IsEnabled()
{
	float value = m_timeline.GetParamVal(m_offset, TIMELINE_PARAM_ENABLE);
	BOOL bEnabled = (PARAM_VALUE_DISABLED != value);
	//BOOL bEnabled = (PARAM_VALUE_ENABLED == value);
	return bEnabled;
}


/**
 * Used to convert number of samples to number of bytes and vice versa.
 */
int CInput::_getSamplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	//assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
		return (fmt->bitsPerSample / 8) * fmt->channels;
	else
		return 0;
}

/**
 * Applies all parameters at the given offset.
 * @param offset number of bytes.
 */
void CInput::applyParamsAt(int offset)
{
	int sample = -1;
	if(m_nSamplesToBytes)
		sample = offset / m_nSamplesToBytes;

	//if(_isEngine() && (offset == _getStartOffset()))
	if(_isEngine() && (offset == m_nStartOffset))
		m_source->Reset();

	tParam *param = m_timeline.GetParam(offset, TIMELINE_PARAM_ENABLE);
	if(param)
	{
		BOOL bEnable = ( PARAM_VALUE_DISABLED != param->value );
		
		IFloopy::Enable( bEnable );

		if(m_plugin)
			m_plugin->Enable(bEnable);

		if(m_callback && sample >= 0)
			m_callback(this, sample, TIMELINE_PARAM_ENABLE);
	}

	param = m_timeline.GetParam(offset, TIMELINE_PARAM_MOVETO);
	if(param)
	{
		if(0.f == param->value)
			m_source->Reset();
		else
			m_source->MoveTo((int)param->value);

		if(m_callback && sample >= 0)
			m_callback(this, sample, TIMELINE_PARAM_MOVETO);
	}

	for(int i=0; i<GetParamCount(); i++)
	{
		param = m_timeline.GetParam(offset, i);
		if(param)
		{
			m_plugin->SetParamVal(param->index, param->value);

			if(m_callback && sample >= 0)
				m_callback(this, sample, i);
		}
	}
}

/**
 * Calculates the distance in bytes at which the track starts playing.
 * @return number of bytes.
 */
int CInput::_getStartOffset()
{
	int offset = 0;
	int tmp = 0;

	while((tmp=m_timeline.GetNextOffset(tmp)) > 0)
	{
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM_ENABLE);
		if(param && (param->value == PARAM_VALUE_ENABLED))
		{
			offset = tmp;
			break;
		}
	}

	return offset;
}

/**
 * Calculates the distance in bytes at which the track ends playing.
 * @return number of bytes.
 */
int CInput::_getEndOffset()
{
	int offset = 0;
	int tmp = 0;

	float last = IsEnabled() ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED;

	while((tmp=m_timeline.GetNextOffset(tmp)) > 0)
	{
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM_ENABLE);
		if(param)
		{
			last = param->value;
			if(param->value == PARAM_VALUE_DISABLED)
			{
				if(tmp > offset)
					offset = tmp;
			}
		}
	}

	// Proveriti da li se na kraju iskljucuje.
	// U suprotnom vratiti duzinu!
	if((last==PARAM_VALUE_ENABLED) && m_plugin)
	{
		offset = m_nStartOffset + m_plugin->GetSize() * m_nSamplesToBytes;
	}

	return offset;
}

int CInput::GetParamCount()
{
	return m_plugin->GetParamCount();
}

BOOL CInput::GetParamVal(int index, float *value)
{
	tParam *param = m_timeline.GetParam(m_offset, index);
	if(param)
	{
		*value = param->value;
		return TRUE;
	}
	return FALSE;
}

void CInput::SetParamVal(int index, float value)
{
	if(index == -333)
	{
		m_fDebug = value;
		return;
	}

	//SOUNDFORMAT *fmt = GetFormat();
	//if(m_callback && (fmt->bitsPerSample > 0) && (fmt->channels > 0))
	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	m_plugin->SetParamVal(index, value);
	m_timeline.SetParamVal(m_offset, index, value);

	_recalcVariables();
}

BOOL CInput::GetParamIndex(char *name, int *index)
{
	if(NULL==name || NULL==index)
		return FALSE;

	if(0==strcmpi(name, "Enable"))
	{
		*index = TIMELINE_PARAM_ENABLE;
		return TRUE;
	}
	if(0==strcmpi(name, "MoveTo"))
	{
		*index = TIMELINE_PARAM_MOVETO;
		return TRUE;
	}

	for(int i=0; i<m_plugin->GetParamCount(); i++)
	{
		if(0==strcmpi(m_plugin->GetParamName(i), name))
		{
			*index = i;
			return TRUE;
		}
	}

	return FALSE;
}

char *CInput::GetParamName(int index)
{
	return m_plugin->GetParamName(index);
}
char *CInput::GetParamDesc(int index)
{
	return m_plugin->GetParamDesc(index);
}

void CInput::SetParamAt(int offset, int index, float value)
{
	m_timeline.SetParamVal(offset * m_nSamplesToBytes, index, value);

	_recalcVariables();
}

BOOL CInput::ResetParamAt(int offset, int index)
{
	if(m_timeline.Remove(offset * m_nSamplesToBytes, index))
	{
		_recalcVariables();
		return TRUE;
	}
	return FALSE;
}

/**
 * Enable at the given offset.
 * @param offset number of samples.
 * @param bEnable enable/disable.
 */
void CInput::EnableAt(int offset, BOOL bEnable)
{
	float value = (bEnable ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED);
	m_timeline.SetParamVal(offset * m_nSamplesToBytes, TIMELINE_PARAM_ENABLE, value);

	_recalcVariables();
	//m_nStartOffset = _getStartOffset();
	//m_nEndOffset = _getEndOffset();

//	if(m_callback)
//		m_callback(this, offset/_getSamplesToBytes(), index);
}

void CInput::Close()
{
	m_plugin->Close();

#ifdef _DEBUG_TIMER_
	_debugPrint();
#endif // _DEBUG_TIMER_
}


BOOL CInput::GetColor(UINT *r, UINT *g, UINT *b)
{
	*r = m_red;
	*g = m_green;
	*b = m_blue;
	return TRUE;
}

void  CInput::SetColor(UINT r, UINT g, UINT b)
{
	m_red	= r;
	m_green	= g;
	m_blue	= b;
}


/**
 * Calculates optimization variables.
 */
void CInput::_recalcVariables()
{
	m_nSamplesToBytes	= _getSamplesToBytes();
	m_nStartOffset		= _getStartOffset();
	m_nEndOffset		= _getEndOffset();

//	assert(m_nSamplesToBytes > 0);
}










int CInput::AddSource(IFloopySoundInput *src)
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
	{
		return ((IFloopySoundMixer*)m_plugin)->AddSource(src);
	}
	else
	{
		m_plugin = src;
		return 0;
	}
}
/*
IFloopySoundInput *CInput::GetSource(int index)
{
	if(m_plugin)
	{
		if(m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER)
			return ((IFloopySoundMixer*)m_plugin)->GetSource(index);
		else if(index == 0)
			return m_plugin;
	}
	else
		return NULL;
}
*/
IFloopySoundInput *CInput::GetSource(int index)
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
		return ((IFloopySoundMixer*)m_plugin)->GetSource(index);
	else
		return m_plugin;
}

void CInput::RemoveSource(IFloopySoundInput *src)
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
		((IFloopySoundMixer*)m_plugin)->RemoveSource(src);
}

int CInput::GetInputCount()
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
		return ((IFloopySoundMixer*)m_plugin)->GetInputCount();
	else
		return(m_plugin ? 1 : 0);
}











#ifdef _DEBUG_TIMER_

clock_t CInput::_debugStartMeasuring()
{
	clock_t start = 0;
	if(m_bDebugTimer)
		start = clock();
	return start;
}

void CInput::_debugStopMeasuring(clock_t start, int size)
{
	if(m_bDebugTimer)
	{
		m_time += clock() - start;
		m_nFrameSize += size;
		m_nFrameCount++;
	}
}

void CInput::_debugFormatBytes(int bytes, char *str)
{
	if(bytes < 1024)
		sprintf(str, "%d b", bytes);
	else if(bytes > 1024 * 1024)
		sprintf(str, "%.2f Mb", (float)bytes / (1024.f*1024.f));
	else
		sprintf(str, "%.2f Kb", (float)bytes / 1024.f);
}

void CInput::_debugPrint()
{
	if(m_bDebugTimer)
	{
		if(m_nSamplesToBytes > 0)
		{
			printf("%s\n", GetName());

			float avgFrameTimeMs  = (float)m_time / (float)m_nFrameCount;
			float avgFrameBytes   = (float)m_nFrameSize / (float)m_nFrameCount;
			float avgFrameSamples = avgFrameBytes / (float)m_nSamplesToBytes;
			float avgFrameTimeSec = (float)avgFrameTimeMs / 1000.f;

			printf(" Average frame time: %f ms (%f sec)\n",
				avgFrameTimeMs, avgFrameTimeSec);

			char tmp[100] = {0};
			_debugFormatBytes((int)avgFrameBytes, tmp);

			printf(" Average frame size: %d samples (%s)\n",
				(int)avgFrameSamples, tmp);

			float readTimeBytes   = avgFrameBytes / avgFrameTimeSec;
			float readTimeSamples = avgFrameSamples / avgFrameTimeSec;
			memset(tmp, 0, sizeof(tmp));
			_debugFormatBytes((int)readTimeBytes, tmp);
			printf(" Average read time:  %.2f samples/sec (%s/sec)\n",
				readTimeSamples, tmp);
		}
	}
	m_nFrameSize = m_time = m_nFrameCount = 0;
}

#endif // _DEBUG_TIMER_




/*
int CInput::getEnd()
{
	int end = _getEndOffset();
	int count = GetInputCount();
//	if(count > 1)
//	{
		for(int i=0; i<count; i++)
		{
			CInput *input = (CInput*)GetSource(i);
			int tmp = input->getEnd();
			if(tmp > end)
				end = tmp;
		}
//	}

	return end;
}

int CInput::GetLastError()
{
	return 0;
}

BOOL CInput::GetLastError(char *str, int len)
{
	return FALSE;
}

int CInput::calcRelativeOffset(int offset)
{
	int start = _getStartOffset();
	BOOL bEngine = (m_source->GetType() == TYPE_FLOOPY_ENGINE);

	if(bEngine && start > 0 && offset > start)
		offset -= start;

	return offset;
}

int CInput::getSize()
{
	int stb   = _getSamplesToBytes();
	int start = 0;//_getStartOffset();//m_offset;
	//int start = m_offset * stb;
	int end   = _getEndOffset();
//	int size  = m_plugin->GetSize();
	//if(end > start || size == -1)
//	if(size == -1)
		int size = (end - start) / stb;

		if(size <= 0)
			size = m_plugin->GetSize();

	return size;
}

BOOL CInput::_isEnabledAt(int offset)
{
	int tmp = 1;
	while((tmp=m_timeline.GetNextOffset(tmp)) < offset && tmp > 0)
	{
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM_ENABLE);
		if(param && param->value != PARAM_VALUE_ENABLED)
		{
			return TRUE;
		}
	}
	return FALSE;
}
*/


/*
int CInput::_read(BYTE *data, int size)
{
	int result = 0;
	IFloopySoundInput *src = (IFloopy::IsEnabled() ? m_plugin : m_plugin->GetSource());
	if(src)
	{
		if(IFloopy::IsEnabled() || m_plugin->ReadSourceIfDisabled())
		{
			assert(size > 0);
			int len = src->Read(data, size);
			if(EOF != len)
				result = len;
		}
	}
	else
		result = s - m_offset;
}
*/

