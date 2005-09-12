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
	m_red = m_green = m_blue = 256;

	memset(m_szDisplayName, 0, 50);
	memset(m_szLastError, 0, sizeof(m_szLastError));
	memset(m_szObjPath,   0, sizeof(m_szObjPath));
	
	// Optimization variables
	m_nStartOffset = m_nEndOffset = m_nSamplesToBytes = 0;

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
				char *tmp = strrchr(plugin, '\\');
				if(tmp)
					SetDisplayName(tmp+1, strlen(plugin));
				else
					SetDisplayName(plugin, strlen(plugin));

				IFloopySoundFilter::SetSource(m_plugin);

				Enable( m_plugin->IsEnabled() ); // Default

				result = TRUE;
			}
			else
				sprintf(m_szLastError,
						"Error: Plugin '%s' not created by %s.\n\0",
						plugin, filename);
		}
		else
			sprintf(m_szLastError,
					"Error: Function %s not found in %s.\n\0",
					PROC_NAME, filename);
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

	Enable(FALSE);

	return TRUE;
}

BOOL CInput::Open(char *filename)
{
	if(m_plugin && m_plugin->Open(filename))
	{
		char *tmp = strrchr(filename, '\\');
		filename = tmp ? tmp+1 : filename;
		SetDisplayName(filename, strlen(filename));
		recalcVariables();
		return TRUE;
	}
	return FALSE;
}

void CInput::Close()
{
	m_plugin->Close();
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
			recalcVariables();
	}

	//if(m_callback && result) m_callback(this, m_offset/getSamplesToBytes(), -333);

	return result;
}

IFloopySoundInput *CInput::GetSource()
{
	if(m_plugin && m_plugin->IsFilter())
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	return NULL;
}

int CInput::Read(BYTE *data, int size)
{
	assert(size >= 0);

	if( GetBypass() )
	{
		m_offset += size;
		IFloopySoundInput *src = GetSource();
		if( src )
			src->Read( data, size );
		return size;
	}

	// Passed the end
	if((m_nEndOffset > 0) && (m_offset >= m_nEndOffset))
		return EOF;

	int endpos = m_offset + size;

	if(isEngine())
	{
		// Avoid wasting processor time
		if(endpos <= m_nStartOffset)
		{
			m_offset += size;
			return size;
		}
	}

	int offset = m_offset;
	int readBytes = 0;
	int len = 0;
	IFloopySoundInput *src = m_plugin;
	BOOL bEOF = FALSE;

	// Apply all due parameters
	applyParamsAt( m_offset );

	while(((offset=m_timeline.GetNextOffset(offset)) < endpos) && (offset>0) && !bEOF)
	{
		// Fill small chunks between parameter changes
		src = getSource();
		//len = src ? src->Read(data, offset - m_offset) : offset-m_offset;
		if(src)
			len = src->Read(data, offset - m_offset);
		else
		{
			// Skip this chunk
			len = offset - m_offset;
			MoveTo( (m_offset + len) / m_nSamplesToBytes );
		}

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
		src = getSource();
		if(src && (size>readBytes))
		{
			len = src->Read(data, size - readBytes);
			if(EOF != len)
				readBytes += len;
		}
		else
		{
			// Skip this chunk
			len = size - readBytes;
			MoveTo( (m_offset + len) / m_nSamplesToBytes );
		}
	}

	m_offset = endpos;

	if(readBytes == 0)
	{
		if(bEOF)
		{
			// Check if this is really the end (no more parameters)
			//int nextOffset = GetNextOffset(m_offset);
			//readBytes = nextOffset ? size : EOF;
			readBytes = m_offset >= m_nEndOffset ? EOF : size;
		}
		else
			readBytes = size;
	}

	return readBytes;
}

/**
 * Moves to the given position.
 * @param samples number of samples.
 */
void CInput::MoveTo(int samples)
{
	if(m_nSamplesToBytes <= 0)
		return;

	m_offset = samples * m_nSamplesToBytes;

	BOOL bMoved = applyPreviousParams( m_offset );

	if( isEngine() && m_nStartOffset > 0)
	{
		if( m_offset >= m_nStartOffset )
			samples -= (m_nStartOffset / m_nSamplesToBytes);
		else
			samples = 0;
	}

	if(!bMoved && NULL != m_source)
	{
		if( GetBypass() )
		{
			IFloopySoundInput *src = GetSource();
			if( src )
				src->MoveTo( samples );
		}
		else
		{
			m_plugin->MoveTo( samples );
		}
	}
}

/**
 * Returns the size of the track source.
 * @return number of samples
 */
int CInput::GetSize()
{
	recalcVariables();

	int size = 0;

	if( GetBypass() && m_plugin->IsFilter() )
	{
		IFloopySoundInput *src = this->GetSource();
		size = src->GetSize();
	}
	else if(m_plugin)
	{
		size = m_plugin->GetSize();

		if((m_nEndOffset > 0) && (m_nSamplesToBytes > 0))
			size = m_nEndOffset / m_nSamplesToBytes;
		else if (m_nSamplesToBytes > 0)
			size += m_nStartOffset / m_nSamplesToBytes;
	}

	size = size > 0 ? size : -1;

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

	recalcVariables();

	if(NULL != m_plugin)
		m_plugin->Reset();
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

//	if(m_plugin)
//		m_plugin->Enable(bEnable);

	IFloopy::Enable(bEnable);

	recalcVariables();
}

/**
 * @return TRUE if the object is enabled at the current offset.
 */
BOOL CInput::IsEnabled()
{
	float value = PARAM_VALUE_ENABLED;
	if(m_timeline.GetParamVal(m_offset, TIMELINE_PARAM_ENABLE, &value))
	{
		BOOL bEnabled = (PARAM_VALUE_DISABLED != value);
		return bEnabled;
	}
	return FALSE;
}

BOOL CInput::GetParamVal(int index, float *value)
{
	return m_timeline.GetParamVal(m_offset, index, value);
}

void CInput::SetParamVal(int index, float value)
{
	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	//if( m_bRecording )
	m_timeline.SetParamVal(m_offset, index, value);

	m_plugin->SetParamVal(index, value);

	recalcVariables();
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

	if( m_plugin->GetParamIndex(name, index) )
		return TRUE;
	else
	{
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
	}

	return FALSE;
}

BOOL CInput::GetPropertyIndex(char *name, int *index)
{
	if( m_plugin->GetPropertyIndex(name, index) )
		return TRUE;
	else
	{
		for(int i=0; i<m_plugin->GetPropertyCount(); i++)
		{
			float propVal = 0.f;
			char *tmp = m_plugin->GetPropertyName(i);
			if( 0 == strcmpi(name, tmp) )
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
	return m_timeline.GetParamVal(offset * m_nSamplesToBytes, index, value);
}

void CInput::SetParamAt(int offset, int index, float value)
{
	m_timeline.SetParamVal(offset * m_nSamplesToBytes, index, value);
	if(offset == 0)
		m_plugin->SetParamVal(index, value);
	recalcVariables();
}

BOOL CInput::ResetParamAt(int offset, int index, float value)
{
	if(m_timeline.RemoveParam(offset * m_nSamplesToBytes, index, value))
	{
		recalcVariables();
		return TRUE;
	}
	return FALSE;
}

BOOL CInput::MoveParam(int offset, int index, float value, int newoffset)
{
	if(m_timeline.MoveParam(offset * m_nSamplesToBytes, index, value, newoffset * m_nSamplesToBytes))
	{
		recalcVariables();
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
	recalcVariables();
}


BOOL CInput::GetColor(UINT *r, UINT *g, UINT *b)
{
	if(m_red<256 && m_green<256 && m_blue<256)
	{
		*r = m_red;
		*g = m_green;
		*b = m_blue;
		return TRUE;
	}
	return FALSE;
}

void CInput::SetColor(UINT r, UINT g, UINT b)
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

bool CInput::RemoveSource(IFloopySoundInput *src)
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
		return ((IFloopySoundMixer*)m_plugin)->RemoveSource(src);
	else
		return false;
}

int CInput::GetInputCount()
{
	if(m_plugin && (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER))
		return ((IFloopySoundMixer*)m_plugin)->GetInputCount();
	else
		return(m_plugin ? 1 : 0);
}

void CInput::MoveAllParamsBetween(int start, int end, int offset)
{
	m_timeline.MoveAllParamsBetween(start*m_nSamplesToBytes,
									end*m_nSamplesToBytes,
									offset*m_nSamplesToBytes);
}










/**
 * Calculates optimization variables.
 */
void CInput::recalcVariables()
{
	recalcSourceVariables();

	m_nSamplesToBytes	= getSamplesToBytes();	// 1
	m_nStartOffset		= getStartOffset();		// 2
	m_nEndOffset		= getEndOffset();		// 3
}

/**
 * A little trick to update all source variables.
 */
void CInput::recalcSourceVariables()
{
	IFloopySoundInput *src = this->GetSource();
	if(src)
	{
		if(src->GetType() == TYPE_FLOOPY_SOUND_MIXER)
		{
			IFloopySoundMixer *mixer = (IFloopySoundMixer*)src;
			for(int i=0; i<mixer->GetInputCount(); i++)
			{
				mixer->GetSource(i)->GetSize();
			}
		}
		else
			src->GetSize();
	}
}

/**
 * Used to convert number of samples to number of bytes and vice versa.
 */
int CInput::getSamplesToBytes()
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
void CInput::applyParamsAt(int offset)
{
	int sample = -1;
	if(m_nSamplesToBytes)
		sample = offset / m_nSamplesToBytes;

	float value = 0.f;

	BOOL bResult = m_timeline.GetParamVal(offset, TIMELINE_PARAM_ENABLE, &value);
	if(bResult || offset == 0)
	{
		BOOL bEnable = bResult ? ( PARAM_VALUE_DISABLED != value ) : FALSE;
		IFloopy::Enable( bEnable );
		m_plugin->Enable(bEnable);

		if(m_callback && sample >= 0)
			m_callback(this, sample, TIMELINE_PARAM_ENABLE);
	}

	if( m_timeline.GetParamVal(offset, TIMELINE_PARAM_MOVETO, &value) )
	{
		m_offset = (int)value * m_nSamplesToBytes;

		m_plugin->MoveTo((int)value);

		if(m_callback && sample >= 0)
			m_callback(this, sample, TIMELINE_PARAM_MOVETO);
	}

	int count = GetParamCount();
	if(count > 0)
	{
		for(int index=0; index<count; index++)
		{
			if( m_timeline.GetParamVal(offset, index, &value) )
			{
				m_plugin->SetParamVal(index, value);

				if(m_callback && sample >= 0)
					m_callback(this, sample, index);
			}
		}
	}
}

/**
 * 
 * @return TRUE if MoveTo has been called.
 * Not the nicest way to tell MoveTo() not to move the source.
 */
BOOL CInput::applyPreviousParams(int offset)
{
	float value = 0.f;
	BOOL bMoved = FALSE;

	int prevOffset = m_timeline.GetPrevOffset(offset, TIMELINE_PARAM_ENABLE);

	BOOL bResult = m_timeline.GetParamVal(prevOffset, TIMELINE_PARAM_ENABLE, &value);
	if(bResult || prevOffset == 0)
	{
		BOOL bEnable = bResult ? ( PARAM_VALUE_DISABLED != value ) : FALSE;
		IFloopy::Enable( bEnable );
		m_plugin->Enable(bEnable);
	}

	prevOffset = m_timeline.GetPrevOffset(offset, TIMELINE_PARAM_MOVETO);
	if( m_timeline.GetParamVal(prevOffset, TIMELINE_PARAM_MOVETO, &value) )
	{
		int newOffset =  (int)value;
		int diff = (offset - prevOffset) / m_nSamplesToBytes;
		int pos = newOffset + diff;
		m_plugin->MoveTo( pos );
//		m_offset = pos * m_nSamplesToBytes;
		bMoved = TRUE;
	}

	int count = GetParamCount();
	if(count > 0)
	{
		for(int index=0; index<count; index++)
		{
			prevOffset = m_timeline.GetPrevOffset(offset, index);
			if( m_timeline.GetParamVal(prevOffset, index, &value) )
				m_plugin->SetParamVal(index, value);
		}
	}

	return bMoved;
}

/**
 * Calculates the distance in bytes at which the track starts playing.
 * @return number of bytes.
 */
int CInput::getStartOffset()
{
	return m_timeline.GetStartOffset();
}

/**
 * Calculates the distance in bytes at which the track ends playing.
 * @return number of bytes.
 */
int CInput::getEndOffset()
{
	int offset = 0;
	int tmp = 0;
	float value = 0.f;

	float last = IsEnabled() ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED;

	do {
		if( m_timeline.GetParamVal(tmp, TIMELINE_PARAM_ENABLE, &value) )
		{
			last = value;
			if((value == PARAM_VALUE_DISABLED) && (tmp > offset))
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

IFloopySoundInput *CInput::getSource()
{
	if( m_plugin->IsFilter() && ((IFloopySoundFilter*)m_plugin)->GetBypass() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else if( IFloopy::IsEnabled() )// || _isSource() )
		return m_plugin;
	else if( m_plugin->IsFilter() && m_plugin->ReadSourceIfDisabled() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else
		return NULL;
}

void CInput::ClearAllParams()
{
	m_timeline.Clear();
}

BOOL CInput::GetBypass()
{
	if( m_plugin->IsFilter() )
		return ((IFloopySoundFilter*)m_plugin)->GetBypass();
	return FALSE;
}

void CInput::SetBypass(BOOL bBypass)
{
	if( m_plugin->IsFilter() )
		((IFloopySoundFilter*)m_plugin)->SetBypass(bBypass);
}
