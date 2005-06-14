// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
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

	Enable(TRUE);
	IFloopy::Enable(TRUE);
	
	// Optimization variables
	m_nStartOffset = m_nEndOffset = m_nSamplesToBytes = 0;

	m_red = m_green = m_blue = 255;

#ifdef _DEBUG_TIMER_
	// Timing valiables
	m_bDebugTimer = TRUE;
	m_nFrameSize = m_dwSpeed = m_nFrameCount = 0;
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
				IFloopySoundInput::SetSource(m_plugin);
				result = TRUE;
			}
			else
				sprintf(m_szLastError, "Error: %s not created by %s.\n\0", plugin, filename);
		}
		else
			//fprintf(stderr, "Error: %s() not found in %s.\n", PROC_NAME, filename);
			sprintf(m_szLastError, "Error: %s() not found in %s.\n\0", PROC_NAME, filename);
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

int CInput::Read(BYTE *data, int size)
{
	assert(size >= 0);

	if(_isEngine())
	{
		// Passed the end
		if(m_offset > m_nEndOffset)
			return EOF;

		// Avoid wasting processor time
		int offset = m_offset + size;
		if(offset < m_nStartOffset)
		{
			m_offset += size;
			return size;
		}
	}

#ifdef _DEBUG_TIMER_
	_debugStartMeasuring();
#endif // _DEBUG_TIMER_

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
			if(IFloopy::IsEnabled() || m_plugin->ReadSourceIfDisabled())
			{
				size = s - m_offset;
				assert(size > 0);
				len = src->Read(data, size);
				if(EOF != len)
					readBytes += len;
			}
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
		if(IFloopy::IsEnabled() || m_plugin->ReadSourceIfDisabled())
		{
			if(size > readBytes)
			{
				size = size - readBytes;
				assert(size > 0);
				len = src->Read(data, size);
				if(EOF != len)
					readBytes += len;
			}
		}
	}

	m_offset = endpos;

#ifdef _DEBUG_TIMER_
	_debugStopMeasuring();
#endif // _DEBUG_TIMER_

	if(readBytes == 0)
	{
		// Check if we have reached the end.
		int start = m_nStartOffset;//_getStartOffset();
		int end = m_nEndOffset;//_getEndOffset();
		if(m_offset >= (start + end))
			readBytes = (_isEngine() ? origSize : EOF);
		else
			return origSize;
	}

	return readBytes;
}

/**
 * Moves to the given position.
 * @param samples number of samples.
 */
void CInput::MoveTo(int samples)
{
//	_recalcVariables();
	if(m_nSamplesToBytes == 0)
		return;

	m_offset = samples * m_nSamplesToBytes;

	applyParamsAt( m_timeline.GetPrevOffset(m_offset) );

//	IFloopySoundInput::Enable(_isEnabledAt(m_offset));

//	_recalcVariables();

	///////////////////////////////////////////////////////////
	//int start = _getStartOffset() / _getSamplesToBytes();
	int start = m_nStartOffset / m_nSamplesToBytes;
	if(_isEngine() && start > 0 && samples > start)
		samples -= start;
	///////////////////////////////////////////////////////////

//	if(m_bRecording)
//		m_timeline.Set(m_offset, TIMELINE_PARAM_MOVETO, (float)m_offset);

	if(NULL != m_source)
		m_source->MoveTo(samples);
}

/**
 * Returns number of samples from the beginning.
 * @return number of samples
 */
int CInput::GetLength()
{
	_recalcVariables();

	int len = m_nEndOffset;//_getEndOffset();
	/*int len = tmp > 0 ? tmp : GetSize();
	if(GetSource())
	{
		int tmp = GetSource()->GetLength();
		if(tmp > len)
			len = tmp;
	}*/

	if(len == 0 && GetSource())
	{
		len = GetSource()->GetLength();
		/*int d=1;
		if(GetSource())
		{
			int tmp = GetSource()->GetLength();
		}*/
		if(len == 0)
			len = GetSize();
	}
	else
		len = GetSize();

	/*if(_isEngine())
	{
		char *name = m_plugin->GetDisplayName();
		if(0==strcmpi(name, "t.xml"))
		{
			int d=0;
		}
	}*/

	return len;
}

/**
 * Returns the size of the track source.
 * @return number of samples
 */
int CInput::GetSize()
{
	int size = 0;

	if(m_plugin)
	{
		if(m_nEndOffset == 0)
			size = m_plugin->GetSize();
		//else if(m_nEndOffset > m_offset)
		//	size = (m_nEndOffset - m_offset) / m_nSamplesToBytes;
		else if(m_nEndOffset > 0)
			size = m_nEndOffset / m_nSamplesToBytes;

		IFloopySoundInput *src = IFloopySoundInput::GetSource();
		int tmp = src->GetSize();
		if(tmp > size)
			size = tmp;
	}

	return size;
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
	m_timeline.Set(m_offset, TIMELINE_PARAM_ENABLE, value);

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
	float value = m_timeline.Get(m_offset, TIMELINE_PARAM_ENABLE);
	BOOL bEnabled = (PARAM_VALUE_DISABLED != value);
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

float CInput::GetParam(int index)
{
	return m_timeline.Get(m_offset, index);
}

BOOL CInput::GetParam(int index, float *value)
{
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

	//SOUNDFORMAT *fmt = GetFormat();
	//if(m_callback && (fmt->bitsPerSample > 0) && (fmt->channels > 0))
	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	m_plugin->SetParam(index, value);
	m_timeline.Set(m_offset, index, value);

	_recalcVariables();
}

int CInput::GetParamIndex(char *name)
{
	return m_plugin->GetParamIndex(name);
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
	m_timeline.Set(offset * m_nSamplesToBytes, index, value);

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
	m_timeline.Set(offset * m_nSamplesToBytes, TIMELINE_PARAM_ENABLE, value);

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

/**
 * Calculates optimization variables.
 */
void CInput::_recalcVariables()
{
	m_nStartOffset		= _getStartOffset();
	m_nEndOffset		= _getEndOffset();
	m_nSamplesToBytes	= _getSamplesToBytes();

//	assert(m_nSamplesToBytes > 0);
}




#ifdef _DEBUG_TIMER_
void CInput::_debugStartMeasuring()
{
	clock_t start = 0;
	if(m_bDebugTimer)
		start = clock();
}

void CInput::_debugStopMeasuring()
{
	if(m_bDebugTimer && readBytes>0)
	{
		m_dwSpeed += clock() - start;
		m_nFrameSize += size / m_nSamplesToBytes;
		m_nFrameCount++;
	}
}

void CInput::_debugPrint()
{
	if(m_bDebugTimer)
	{
		SOUNDFORMAT *fmt = GetFormat();
		assert(fmt->bitsPerSample > 0);
		int sampleSize = fmt->bitsPerSample / 8;

		printf("%s\n", GetName());

		float afmt = (float)m_dwSpeed / (float)m_nFrameCount;
		float afsz = (float)m_nFrameSize / (float)m_nFrameCount;
		float amr = afsz * sampleSize / (afmt / 1000.f);
		if(amr < 1024.f)
			printf("Read time:\t%.2f b/sec\n", amr);
		else if(amr > 1024.f*1024.f)
			printf("Read time:\t%.2f Mb/sec\n", amr / (1024.f*1024.f));
		else
			printf("Mixing rate:\t%.2f Kb/sec\n", amr / 1024.f);
		printf("Average read time:\t%f ms\n", afmt);
		printf("Average frame size:\t%.2f samples\n", afsz);
	}
	m_nFrameSize = m_dwSpeed = m_nFrameCount = 0;
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

