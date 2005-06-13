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
	memset(m_szObjPath, 0, sizeof(m_szObjPath));

	Enable(TRUE);
	IFloopy::Enable(TRUE);
	
	// Optimization variables
	m_nStartOffset	= 0;
	m_nEndOffset	= 0;
	m_nSamplesToBytes = 0;

#ifdef _DEBUG_TIMER_
	// Timing valiables
	m_bDebugTimer = TRUE;
	m_nFrameSize=m_dwSpeed=m_nFrameCount=0;
#endif // _DEBUG_TIMER_

//	m_bRecording = FALSE;

	m_fDebug = 0.f;
}

/**
 * Create the plugin.
 * @param plugin library name.
 * @return TRUE if succesfull.
 */
BOOL CInput::Create(char *name)
{
	BOOL result = FALSE;

	char *plugin = name;

	char *library = plugin;

	char tmp[MAX_PATH] = {0};

	char *sep = strrchr(plugin, '.');
	if(sep)
	{
		strcpy(tmp, plugin);
		char *sep = strrchr(tmp, '.');
		plugin = sep+1;
		*sep = 0;
		library = tmp;
	}

	char *filename = new char[strlen(library) + 5];
	strcpy(filename, library);
	strcat(filename, PLUG_EXT);

	strcpy(m_szObjPath, filename);

	m_hinst = LoadLibraryA(filename);

	if(NULL == m_hinst)
		m_hinst = LoadLibraryA(plugin);

	if (NULL != m_hinst)
	{
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
		else {
			//fprintf(stderr, "Error: %s() not found in %s.\n", PROC_NAME, filename);
			sprintf(m_szLastError, "Error: %s() not found in %s.\n\0", PROC_NAME, filename);
		}
	}
	else
	{
		sprintf(m_szLastError, "Error: %s not found.\n\0", filename);
	}

	delete[] filename;
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

CInput::~CInput()
{
	if(m_plugin)
		delete m_plugin;

	if(m_hinst)
		FreeLibrary(m_hinst);
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
		/*if(offset > m_nEndOffset)
		{
			m_offset += size;
			return size;
		}*/
	}

#ifdef _DEBUG_TIMER_
	clock_t start = 0;
	if(m_bDebugTimer)
		start = clock();
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
		/*else if(src->GetSize() == -1)
		{
			m_plugin->Reset();	// Ako ovo izbacim, posle pauze ne nastavlja
		}*/
	}

	m_offset = endpos;

#ifdef _DEBUG_TIMER_
	if(m_bDebugTimer && readBytes>0)
	{
		m_dwSpeed += clock() - start;
		m_nFrameSize += size / m_nSamplesToBytes;
		m_nFrameCount++;
	}
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
	if(!m_plugin)
		return 0;
//	int size;// = m_plugin->GetSize();
	//if(size <= 0)
//		size = getSize();
/*
	//return getSize();
	//return IFloopySoundInput::GetSize();
//	if(m_plugin->GetSize() > 0)
		return m_plugin->GetSize();
//	else 
//		return IFloopySoundInput::GetSize();
*/
//	_recalcVariables();

	int tmp = 0;
	int size = 0;
	int stb = m_nSamplesToBytes;
	
	//int end = _getEndOffset();
	int end = m_nEndOffset;

	if(end == 0)
		size = m_plugin->GetSize();
	else if(end > m_offset)
		size = (end - m_offset) / stb;
	else if(end > 0)
		size = end / stb;

	IFloopySoundInput *src = IFloopySoundInput::GetSource();
	tmp = src->GetSize();
	if(tmp > size)
		size = tmp;

	/*if(_isEngine())
	{
		char *name = m_plugin->GetDisplayName();
		if(0==strcmpi(name, "t.xml"))
		{
			int d=0;
		}
	}*/

	return size;
}

void CInput::Reset()
{
	/*if(GetSource())
	{
		char *name = GetSource()->GetName();
		if(name)
		{
			if(0 == strcmpi(name, "D:\\sqba\\Projekti\\Floopy!\\Samples\\acid\\WAH2.WAV"))
			{
				int d=0;
			}
		}
	}*/

	m_offset = 0;

//	_recalcVariables();

//	if(m_bRecording)
//		m_timeline.Set(m_offset, TIMELINE_PARAM_MOVETO, PARAM_VALUE_RESET);

	if(NULL != m_source)
		m_source->Reset();
}

int CInput::GetNextOffset(int offset)
{
	int stb = m_nSamplesToBytes;
	int next = m_timeline.GetNextOffset(offset*stb);
	return (next > 0 ? next / stb : 0);
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
 * Used to convert number of samples
 * to number of bytes and vice versa.
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
	/*int x = offset/_getSamplesToBytes();
	if(x >= 5342626 && x <= 5342628)
	{
		int d=0;
	}*/
	/*if(GetSource())
	{
		char *name = GetSource()->GetName();
		if(name)
		{
			if(0 == strcmpi(name, "D:\\sqba\\Projekti\\Floopy!\\Samples\\acid\\WAH2.WAV"))
			{
				int d=0;
			}
		}
	}*/

	//SOUNDFORMAT *fmt = GetFormat();
	int sample = -1;
	//if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
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
	//float last = PARAM_VALUE_DISABLED;
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
	//if(last!=PARAM_VALUE_DISABLED)
	if((last==PARAM_VALUE_ENABLED) && m_plugin)
	{
		//int stb = _getSamplesToBytes();
		//offset = (m_plugin->GetSize() * stb);
		//offset = _getStartOffset() + (m_plugin->GetSize() * stb);
		//offset = _getStartOffset() + IFloopySoundInput::GetSize();
		//offset = m_plugin->GetLength();
		//offset = _getStartOffset() + m_plugin->GetSize() * _getSamplesToBytes();
		offset = m_nStartOffset + m_plugin->GetSize() * m_nSamplesToBytes;
	}
/*
	if(offset == 0 && GetSource())
	{
		//offset = GetSource()->GetLength();
		offset = _getStartOffset() + m_plugin->GetSize();
	}
*/
/*
	int count = GetInputCount();
	if(count > 1)
	{
		for(int i=0; i<count; i++)
		{
			CInput *input = (CInput*)GetSource(i);
			int tmp = input->_getEndOffset();
			if(tmp > offset)
				offset = tmp;
		}
	}
*/
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

	//SOUNDFORMAT *fmt = GetFormat();
	//if(m_callback && (fmt->bitsPerSample > 0) && (fmt->channels > 0))
	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	m_plugin->SetParam(index, value);
	m_timeline.Set(m_offset, index, value);

	_recalcVariables();

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
	m_timeline.Set(offset * m_nSamplesToBytes, index, value);

	_recalcVariables();

//	if(m_callback)
//		m_callback(this, offset/_getSamplesToBytes(), index);
}

BOOL CInput::ResetParamAt(int offset, int index)
{
	if(m_timeline.Remove(offset * m_nSamplesToBytes, index))
	{
		_recalcVariables();
		return TRUE;
	}
	return FALSE;

//	if(m_callback)
//		m_callback(this, offset/_getSamplesToBytes(), index);
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
			printf("Mixing rate:\t%.2f b/sec\n", amr);
		else if(amr > 1024.f*1024.f)
			printf("Mixing rate:\t%.2f Mb/sec\n", amr / (1024.f*1024.f));
		else
			printf("Mixing rate:\t%.2f Kb/sec\n", amr / 1024.f);
		printf("Average frame mixing time:\t%f ms\n", afmt);
		printf("Average frame size:\t\t%.2f samples\n", afsz);
	}
	m_nFrameSize=m_dwSpeed=m_nFrameCount=0;
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
