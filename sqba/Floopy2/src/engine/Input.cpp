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
	m_red = m_green = m_blue = 255;

	memset(m_szDisplayName, 0, 50);
	memset(m_szLastError, 0, sizeof(m_szLastError));
	memset(m_szObjPath,   0, sizeof(m_szObjPath));

//	Enable(TRUE);
//	IFloopy::Enable(TRUE);
	
	// Optimization variables
	m_nStartOffset = m_nEndOffset = m_nSamplesToBytes = 0;

#ifdef _DEBUG_TIMER_
	// Timing valiables
	m_bDebugTimer = TRUE;
	m_nFrameSize = m_time = m_nFrameCount = 0;
#endif // _DEBUG_TIMER_

//	m_bRecording = FALSE;
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
				SetDisplayName(plugin, strlen(plugin));

				IFloopySoundFilter::SetSource(m_plugin);

				SOUNDFORMAT *fmt = m_plugin->GetFormat();
				if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
				{
					if( _isFilter() )
					{
						Enable(TRUE);
						IFloopy::Enable(TRUE);
					}
					else
					{
						Enable(FALSE);
						IFloopy::Enable(FALSE);
					}
				}

				result = TRUE;
			}
			else
				sprintf(m_szLastError, "Error: Plugin '%s' not created by %s.\n\0", plugin, filename);
		}
		else
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
	
	IFloopySoundFilter::SetSource(m_plugin);
	
	char *name = src->GetDisplayName();
	if(name)
		SetDisplayName(name, strlen(name));

	SOUNDFORMAT *fmt = m_plugin->GetFormat();
	if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
	{
		/*if( _isFilter() )
		{
			Enable(TRUE);
			IFloopy::Enable(TRUE);
		}
		else
		{*/
			Enable(FALSE);
			IFloopy::Enable(FALSE);
		//}
	}

	return TRUE;
}

BOOL CInput::Open(char *filename)
{
	if(m_plugin && m_plugin->Open(filename))
	{
		char *tmp = strrchr(filename, '\\');
		if(tmp)
			tmp += 1;
		else
			tmp = filename;
		SetDisplayName(tmp, strlen(tmp));

		return TRUE;
	}
	return FALSE;
}

void CInput::Close()
{
	m_plugin->Close();

#ifdef _DEBUG_TIMER_
	_debugPrint();
#endif // _DEBUG_TIMER_
}

BOOL CInput::SetSource(IFloopySoundInput *src)
{
	// Check if src has been created by this engine.
	// Ne radi ako je src engine!
//	if(m_iCheck != ((CInput*)src)->m_iCheck)
//		return FALSE;

	BOOL result = FALSE;
	if(m_plugin && m_plugin->IsFilter())
	{
		result = ((IFloopySoundFilter*)m_plugin)->SetSource(src);
	
		if(result)
			_recalcVariables();
	}

	//if(m_callback && result) m_callback(this, m_offset/_getSamplesToBytes(), -333);

	return result;
}
/*
SOUNDFORMAT *CInput::GetFormat()
{
	if(m_plugin && m_plugin->IsFilter())
		return ((IFloopySoundFilter*)m_plugin)->GetFormat();
	else if(m_plugin)
		return ((IFloopySoundInput*)m_plugin)->GetFormat();
	else
		return IFloopySoundInput::GetFormat();
}
*/
IFloopySoundInput *CInput::GetSource()
{
	if(m_plugin && m_plugin->IsFilter())
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	return NULL;

}

int CInput::Read(BYTE *data, int size)
{
	assert(size >= 0);

	// Passed the end
	if(m_offset >= m_nEndOffset)
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
	_applyParamsAt( m_offset );

	while(((offset=m_timeline.GetNextOffset(offset)) < endpos) && (offset>0) && !bEOF)
	{
		// Fill small chunks between parameter changes
		src = _getSource();
		len = src ? src->Read(data, offset - m_offset) : offset-m_offset;

		if(EOF != len)
		{
			data += len;
			readBytes += len;
			m_offset += len;
		}
		else
			bEOF = TRUE;
	
		_applyParamsAt( offset );
	}

	if( !bEOF )
	{
		// Fill the rest of the data
		src = _getSource();
		if(src && (size>readBytes))
		{
			len = src->Read(data, size - readBytes);
			if(EOF != len)
				readBytes += len;
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
	if(m_nSamplesToBytes > 0)
	{
		m_offset = samples * m_nSamplesToBytes;

		//_applyParamsAt( m_timeline.GetPrevOffset(m_offset) );
		//_applyParamsUntil( m_offset );
		_applyPreviousParams( m_offset );

		if( m_nStartOffset > 0)
		{
			if( m_offset >= m_nStartOffset )
				samples -= (m_nStartOffset / m_nSamplesToBytes);
			else
				samples = 0;
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
	if(m_plugin && m_plugin->IsFilter())
		return ((IFloopySoundFilter*)m_plugin)->GetSourceSize();
	return 0;
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

int CInput::GetPrevOffset(int offset)
{
	int prev = m_timeline.GetPrevOffset(offset*m_nSamplesToBytes);
	return (prev > 0 ? prev / m_nSamplesToBytes : 0);
}

/**
 * Enable at the current offset.
 * @param bEnable enable/disable.
 */
void CInput::Enable(BOOL bEnable)
{
	//if( m_bRecording )
	float value = (bEnable ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED);
	m_timeline.SetParamVal(m_offset, TIMELINE_PARAM_ENABLE, value);

	if(m_plugin)
		m_plugin->Enable(bEnable);

	_recalcVariables();
}

/**
 * @return TRUE if the object is enabled at the current offset.
 */
BOOL CInput::IsEnabled()
{
	float value = m_timeline.GetParamVal(m_offset, TIMELINE_PARAM_ENABLE);
	BOOL bEnabled = (PARAM_VALUE_DISABLED != value);
	return bEnabled;
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
	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	//if( m_bRecording )
	m_timeline.SetParamVal(m_offset, index, value);

	m_plugin->SetParamVal(index, value);

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

	int count = m_plugin->GetParamCount();
	if(count > 0)
	{
		for(int i=0; i<count; i++)
		{
			if(0==strcmpi(m_plugin->GetParamName(i), name))
			{
				*index = i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CInput::GetParamAt(int offset, int index, float *value)
{
	tParam *param = m_timeline.GetParam(offset * m_nSamplesToBytes, index);
	if( param )
	{
		*value = param->value;
		return TRUE;
	}
	return FALSE;
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

BOOL CInput::MoveParam(int offset, int index, int newoffset)
{
	if(m_timeline.MoveParam(offset * m_nSamplesToBytes, index, newoffset * m_nSamplesToBytes))
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











int CInput::AddSource(IFloopySoundInput *src)
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
		return ((IFloopySoundMixer*)m_plugin)->AddSource(src);
	else
	{
		m_plugin = src;
		return 0;
	}
}

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












/**
 * Calculates optimization variables.
 */
void CInput::_recalcVariables()
{
	m_nSamplesToBytes	= _getSamplesToBytes();	// 1
	m_nStartOffset		= _getStartOffset();	// 2
	m_nEndOffset		= _getEndOffset();		// 3
}

/**
 * Used to convert number of samples to number of bytes and vice versa.
 */
int CInput::_getSamplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
		return (fmt->bitsPerSample / 8) * fmt->channels;
	else
		return 0;
}

/**
 * Applies all parameters at the given offset.
 * @param offset number of bytes.
 */
void CInput::_applyParamsAt(int offset)
{
	int sample = -1;
	if(m_nSamplesToBytes)
		sample = offset / m_nSamplesToBytes;

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
			m_plugin->Reset();
		else
			m_plugin->MoveTo((int)param->value);

		if(m_callback && sample >= 0)
			m_callback(this, sample, TIMELINE_PARAM_MOVETO);
	}

	int count = GetParamCount();
	if(count > 0)
	{
		for(int index=0; index<count; index++)
		{
			param = m_timeline.GetParam(offset, index);
			if(param)
			{
				m_plugin->SetParamVal(param->index, param->value);

				if(m_callback && sample >= 0)
					m_callback(this, sample, index);
			}
		}
	}
}

void CInput::_applyPreviousParams(int offset)
{
	int prevOffset = m_timeline.GetPrevOffset(offset, TIMELINE_PARAM_ENABLE);
	tParam *param = m_timeline.GetParam(prevOffset, TIMELINE_PARAM_ENABLE);
	if(param)
	{
		BOOL bEnable = ( PARAM_VALUE_DISABLED != param->value );
		IFloopy::Enable( bEnable );
		if(m_plugin)
			m_plugin->Enable(bEnable);
	}

	prevOffset = m_timeline.GetPrevOffset(offset, TIMELINE_PARAM_MOVETO);
	param = m_timeline.GetParam(prevOffset, TIMELINE_PARAM_MOVETO);
	if(param)
	{
		if(0.f == param->value)
			m_source->Reset();
		else
			m_source->MoveTo((int)param->value);
	}

	int count = GetParamCount();
	if(count > 0)
	{
		for(int index=0; index<count; index++)
		{
			prevOffset = m_timeline.GetPrevOffset(offset, index);
			param = m_timeline.GetParam(prevOffset, index);
			if(param)
				m_plugin->SetParamVal(param->index, param->value);
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

	do {
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM_ENABLE);
		if(param && (param->value == PARAM_VALUE_ENABLED))
		{
			offset = tmp;
			break;
		}
	} while((tmp=m_timeline.GetNextOffset(tmp)) > 0);

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

	do {
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM_ENABLE);
		if(param)
		{
			last = param->value;
			if((param->value == PARAM_VALUE_DISABLED) && (tmp > offset))
				offset = tmp;
		}
	} while((tmp=m_timeline.GetNextOffset(tmp)) > 0);

	// Proveriti da li se na kraju iskljucuje.
	if((last==PARAM_VALUE_ENABLED) && m_plugin)
	{
		offset = m_nStartOffset + m_plugin->GetSize() * m_nSamplesToBytes;
	}

	return offset;
}

IFloopySoundInput *CInput::_getSource()
{
	if( IFloopy::IsEnabled() )
		return m_plugin;
	else if( m_plugin->IsFilter() && m_plugin->ReadSourceIfDisabled() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else
		return NULL;
}

/*
void CInput::_applyParamsUntil(int endoffset)
{
	int offset = 0;
	do {
		_applyParamsAt( offset );
		offset = m_timeline.GetNextOffset(offset);
	} while((offset > 0) && (offset < endoffset));
}
*/












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



