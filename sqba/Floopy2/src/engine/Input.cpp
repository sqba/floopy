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
	m_callback	= func;

	m_red = m_green = m_blue = 256;

	memset(m_szDisplayName,	0, 50);
	memset(m_szLastError,	0, sizeof(m_szLastError));
	memset(m_szObjPath,		0, sizeof(m_szObjPath));
	
	m_offset = m_nStartOffset = m_nEndOffset = m_nSamplesToBytes = 0;

	m_bFadeEdges = true;
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
 * @return true if succesfull.
 */
bool CInput::Create(char *name)
{
	bool result = false;

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

		if(func != NULL)
		{
			m_plugin = func( plugin );
			if(m_plugin)
			{
				char *tmp = strrchr(plugin, '\\');
				plugin = tmp ? ++tmp : plugin;
				
				SetDisplayName(plugin, strlen(plugin));

				IFloopySoundFilter::SetSource(m_plugin);

				Enable( m_plugin->IsEnabled() ); // Default

				result = true;
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
 * @return true (if succesfull).
 */
bool CInput::Create(IFloopySoundEngine *src)
{
	m_plugin = src;
	
	IFloopySoundFilter::SetSource(m_plugin);
	
	char *name = src->GetDisplayName();
	if(name)
		SetDisplayName(name, strlen(name));

	Enable(false);

	return true;
}

bool CInput::Open(char *filename)
{
	if(m_plugin && m_plugin->Open(filename))
	{
		char *tmp = strrchr(filename, '\\');
		filename = tmp ? tmp+1 : filename;
		SetDisplayName(filename, strlen(filename));
		recalcVariables();
		return true;
	}
	return false;
}

void CInput::Close()
{
	if(NULL != m_plugin)
		m_plugin->Close();
}

bool CInput::SetSource(IFloopySoundInput *src)
{
	bool result = false;

	if(m_plugin && isFilter())
	{
		result = ((IFloopySoundFilter*)m_plugin)->SetSource(src);
	
		if(result)
			recalcVariables();
	}

	return result;
}

IFloopySoundInput *CInput::GetSource()
{
	if( isFilter() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	return NULL;
}

/**
 * Writes samples into the buffer.
 * @param data pointer to the buffer.
 * @param size size of the receiving buffer in bytes.
 * @return number of bytes written into buffer.
 */
int CInput::Read(BYTE *data, int size)
{
	assert(m_nSamplesToBytes > 0);
	assert(size > 0);

	int len			= 0;
	int endpos		= m_offset + size;
	IFloopySoundInput *src = NULL;
	int result		= 0;

	if( isEndOfTrack() )
	{
		result = EOF;
		m_offset += size;
	}
	else if( GetBypass() )
	{
		if( isEngine() || NULL == (src=GetSource()) )
		{
			skipChunk(size);
			result = 0;
			m_offset += size;
		}
		else
		{
			len = src->Read( data, size );
			m_offset += size;
			result = len;
		}
	}
	else if(isEngine() && endpos<=m_nStartOffset)
	{
		m_offset += size;
		result = 0;
	}
	else
	{
		// Varijanta 1:
		// Ova varijanta vraca ukupan broj bajtova racunajuci
		// i tisinu na kraju (skipChunk).
		while(m_offset<endpos && result<size && len!=EOF)
		{
			applyParamsAt( m_offset );

			int chunkSize	= size - result;
			int nextOffset	= m_timeline.GetNextOffset(m_offset);

			if(nextOffset>0 && m_offset+chunkSize>nextOffset)
				chunkSize = nextOffset - m_offset;

			if( NULL != (src=getSource()) )
				len = src->Read(data, chunkSize);
			else
				len = skipChunk( chunkSize );

			if(EOF != len)
			{
				data	+= len;
				result	+= len;
			}

			m_offset += chunkSize;
		}

/*
		// Varijanta 2:
		// Ova varijanta vraca broj bajtova ne racunajuci
		// tisinu posle poslednjeg iscitanog bajta (skipChunk).
		int bytesRead = 0;
		while(m_offset<endpos && bytesRead<size && len!=EOF)
		{
			applyParamsAt( m_offset );

			int chunkSize	= size - bytesRead;
			int nextOffset	= m_timeline.GetNextOffset(m_offset);

			if(nextOffset>0 && m_offset+chunkSize>nextOffset)
				chunkSize = nextOffset - m_offset;

			if( NULL != (src=getSource()) )
			{
				len = src->Read(data, chunkSize);
				if(EOF != len)
					result = bytesRead + len;
			}
			else
				len = skipChunk( chunkSize );

			if(EOF != len)
			{
				data		+= len;
				bytesRead	+= len;
			}

			m_offset += chunkSize;
		}
*/
/*
		// Varijanta 3:
		int bytesRead = 0;
		while(m_offset<endpos && bytesRead<size && len!=EOF)
		{
			applyParamsAt( m_offset );

			int chunkSize	= size - bytesRead;
			int nextOffset	= m_timeline.GetNextOffset(m_offset);

			if(nextOffset>0 && m_offset+chunkSize>nextOffset)
				chunkSize = nextOffset - m_offset;

			if( NULL != (src=getSource()) )
			{
				len = src->Read(data, chunkSize);
				
				if(EOF != len)
				{
					//assert( len == chunkSize );

					if(len!=chunkSize && !src->IsEOF())
						len = chunkSize;

					result = bytesRead + len;
				}
			}
			else
				len = skipChunk( chunkSize );

			if(EOF != len)
			{
				data		+= len;
				bytesRead	+= len;
			}

			m_offset += chunkSize;
		}
*/
	}

	if(result==0 && len==EOF)
		result = EOF;

	return result;
}

/**
 * Moves to the given position.
 * @param samples number of samples.
 */
void CInput::MoveTo(int samples)
{

	// Varijanta 1:
	// Ova varijanta se vraca na prethodnu promenu svakog
	// parametra, primeni je i, ako je to bio poziv MoveTo
	// onda skache na razliku izmedju trazenog ofseta i ovog
	// poziva, a ako ne onda samo skache na trazeni ofset.

	assert(m_nSamplesToBytes > 0);

	m_offset = samples * m_nSamplesToBytes;

	int offset = applyPreviousParams( m_offset );

	if( isEngine() && m_nStartOffset>0)
	{
		if( m_offset >= m_nStartOffset )
			offset -= (m_nStartOffset / m_nSamplesToBytes);
		else
			offset = 0;
	}

	IFloopySoundInput *src = m_plugin;
	if( GetBypass() )
		src = ((IFloopySoundFilter*)m_plugin)->GetSource();
	src->MoveTo( offset );

/*
	// Varijanta 2:
	// Ova varijanta se vraca na pocetak i primenjuje
	// redom sve parametre sve do novog ofseta.

	char *name = GetName();

	int endOffset = samples * m_nSamplesToBytes;
	int chunkSize = endOffset;

	m_offset = 0;

	m_plugin->MoveTo( 0 );
	applyParamsAt( 0 );

	if(0 == samples)
		return;

	int srcPos = 0;
	int prevOffset = 0;
	int nextOffset = 1;
	
	while(m_offset<endOffset) {
		nextOffset = m_timeline.GetNextOffset(m_offset);

		if(0 == nextOffset)
			break;

		chunkSize = nextOffset - m_offset;
		
		if(m_offset+chunkSize > endOffset)
			chunkSize = endOffset - m_offset;

		prevOffset = m_offset;
		m_offset += chunkSize;

		IFloopySoundInput *src = getSource();
		if(NULL != src)
		{
			srcPos = src->GetPosition();
			//int diff = (m_offset - chunkSize) / m_nSamplesToBytes;
			int diff = (m_offset - prevOffset) / m_nSamplesToBytes;
			src->MoveTo( srcPos + diff);
		}
		else
			skipChunk( chunkSize );

		applyParamsAt( m_offset );
	}
*/
}

/**
 * Returns the size of the track source.
 * @return number of samples
 */
int CInput::GetSize()
{
	recalcVariables();

	int size = 0;

	if( GetBypass() && isFilter() )
	{
		IFloopySoundInput *src = this->GetSource();
		size = src->GetSize();
	}
	else if(m_plugin)
	{
		size = m_plugin->GetSize();

		if(m_nSamplesToBytes > 0)
		{
			if(m_nEndOffset > 0)
				size = m_nEndOffset / m_nSamplesToBytes;
			else
				size += m_nStartOffset / m_nSamplesToBytes;
		}

		if(m_plugin->ReadSourceIfDisabled())
		{
			int srcSize = GetSourceSize();
			if(srcSize > size)
				size = srcSize;
		}
	}

	size = size > 0 ? size : -1;

	return size;
}

int CInput::GetSourceSize()
{
	if(m_plugin && isFilter())
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
	offset *= m_nSamplesToBytes;
	int next = m_timeline.GetNextOffset(offset);
	return (next > 0 ? next/m_nSamplesToBytes : 0);
}

int CInput::GetPrevOffset(int offset)
{
	offset *= m_nSamplesToBytes;
	int prev = m_timeline.GetPrevOffset(offset);
	return (prev > 0 ? prev/m_nSamplesToBytes : 0);
}

/**
 * Enable at the current offset.
 * @param bEnable enable/disable.
 */
void CInput::Enable(bool bEnable)
{
	m_timeline.EnableAt(m_offset, bEnable);
	IFloopy::Enable(bEnable);
	recalcVariables();
}

/**
 * @return true if the object is enabled at the current offset.
 */
bool CInput::IsEnabled()
{
	bool result = false;
	float value = PARAM_VALUE_ENABLED;
	if(m_timeline.GetParamVal(m_offset, TIMELINE_PARAM_ENABLE, &value))
	{
		result = (PARAM_VALUE_DISABLED != value);
	}
	else
	{
		int prev = m_timeline.GetParamVal(m_offset, TIMELINE_PARAM_ENABLE, &value);
		result = (PARAM_VALUE_DISABLED != value);
	}
	return result;
}

bool CInput::GetParamVal(int index, float *value)
{
	return m_timeline.GetParamVal(m_offset, index, value);
}

void CInput::SetParamVal(int index, float value)
{
	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	m_timeline.SetParamVal(m_offset, index, value);

	m_plugin->SetParamVal(index, value);

	recalcVariables();
}

bool CInput::GetParamIndex(char *name, int *index)
{
	if(NULL==name || NULL==index)
		return false;

	if(0==strcmpi(name, "Enable"))
	{
		*index = TIMELINE_PARAM_ENABLE;
		return true;
	}

	if(0==strcmpi(name, "MoveTo"))
	{
		*index = TIMELINE_PARAM_MOVETO;
		return true;
	}

	if( m_plugin->GetParamIndex(name, index) )
		return true;
	else
	{
		for(int i=0; i<m_plugin->GetParamCount(); i++)
		{
			if(0==strcmpi(m_plugin->GetParamName(i), name))
			{
				*index = i;
				return true;
			}
		}
	}

	return false;
}

bool CInput::GetPropertyIndex(char *name, int *index)
{
	if( m_plugin->GetPropertyIndex(name, index) )
		return true;
	else
	{
		for(int i=0; i<m_plugin->GetPropertyCount(); i++)
		{
			if(0==strcmpi(m_plugin->GetPropertyName(i), name))
			{
				*index = i;
				return true;
			}
		}
	}
	return false;
}

bool CInput::GetParamAt(int offset, int index, float *value)
{
	return m_timeline.GetParamVal(offset*m_nSamplesToBytes, index, value);
}

void CInput::SetParamAt(int offset, int index, float value)
{
	offset *= m_nSamplesToBytes;
	m_timeline.SetParamVal(offset, index, value);
	if(offset == 0)
		m_plugin->SetParamVal(index, value);
	recalcVariables();
}

bool CInput::ResetParamAt(int offset, int index, float value)
{
	offset *= m_nSamplesToBytes;
	if(m_timeline.RemoveParam(offset, index, value))
	{
		recalcVariables();
		return true;
	}
	return false;
}

bool CInput::MoveParam(int offset, int index, float value, int newOffset)
{
	offset		*= m_nSamplesToBytes;
	newOffset	*= m_nSamplesToBytes;
	if(m_timeline.MoveParam(offset, index, value, newOffset))
	{
		recalcVariables();
		return true;
	}
	return false;
}

/**
 * Enable at the given offset.
 * @param offset number of samples.
 * @param bEnable enable/disable.
 */
void CInput::EnableAt(int offset, bool bEnable)
{
	offset *= m_nSamplesToBytes;
	m_timeline.EnableAt(offset, bEnable);
	recalcVariables();
}

void CInput::ClearAllParams()
{
	m_timeline.Clear();
}

bool CInput::GetBypass()
{
	if( isFilter() )
		return ((IFloopySoundFilter*)m_plugin)->GetBypass();
	return false;
}

void CInput::SetBypass(bool bBypass)
{
	if( isFilter() )
		((IFloopySoundFilter*)m_plugin)->SetBypass(bBypass);
}


bool CInput::GetColor(UINT *r, UINT *g, UINT *b)
{
	if(m_red<256 && m_green<256 && m_blue<256)
	{
		*r = m_red;
		*g = m_green;
		*b = m_blue;
		return true;
	}
	return false;
}

void CInput::SetColor(UINT r, UINT g, UINT b)
{
	m_red	= r;
	m_green	= g;
	m_blue	= b;
}











int CInput::AddSource(IFloopySoundInput *src)
{
	if( isMixer() )
		return ((IFloopySoundMixer*)m_plugin)->AddSource(src);
	else if( SetSource(src) )
		return 0;
	else
		return -1;
}

IFloopySoundInput *CInput::GetSource(int index)
{
	if( isMixer() )
		return ((IFloopySoundMixer*)m_plugin)->GetSource(index);
	else if( isFilter() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else
		return false;
}

bool CInput::RemoveSource(IFloopySoundInput *src)
{
	if( isMixer() )
		return ((IFloopySoundMixer*)m_plugin)->RemoveSource(src);
	else
		return false;
}

int CInput::GetInputCount()
{
	if( isMixer() )
		return ((IFloopySoundMixer*)m_plugin)->GetInputCount();
	else if(NULL!=GetSource())
		return 1;
	else
		return 0;
}

bool CInput::MoveAllParamsBetween(int start, int end, int offset)
{
	return m_timeline.MoveAllParamsBetween( start*m_nSamplesToBytes,
											end*m_nSamplesToBytes,
											offset*m_nSamplesToBytes );
}

void CInput::SetDisplayName(char *name, int len)
{
	memset(m_szDisplayName, 0, 50);
	memcpy(m_szDisplayName, name, (len<50?len:50));
}

bool CInput::ReadSourceIfDisabled()
{
	return (isFilter() ? m_plugin->ReadSourceIfDisabled() : false);
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
				mixer->GetSource(i)->Reset();
		}
		else
			src->Reset();
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
	int sample		= offset / m_nSamplesToBytes;
	float value		= 0.f;

	bool bResult = m_timeline.GetParamVal(offset, TIMELINE_PARAM_ENABLE, &value);
	if(bResult || offset==0)
	{
		bool bEnable = bResult ? PARAM_VALUE_DISABLED!=value : false;
		IFloopy::Enable( bEnable );
		m_plugin->Enable( bEnable );

		if(m_callback && sample>=0)
			m_callback(this, sample, TIMELINE_PARAM_ENABLE);
	}

	if( m_timeline.GetParamVal(offset, TIMELINE_PARAM_MOVETO, &value) )
	{
		m_plugin->MoveTo( (int)value );

		if(m_callback && sample>=0)
			m_callback(this, sample, TIMELINE_PARAM_MOVETO);
	}

	for(int index=0; index<GetParamCount(); index++)
	{
		if( m_timeline.GetParamVal(offset, index, &value) )
		{
			m_plugin->SetParamVal(index, value);

			if(m_callback && sample>=0)
				m_callback(this, sample, index);
		}
	}
}

/**
 * 
 * @return true if MoveTo has been called.
 * Not the nicest way to tell MoveTo() not to move the source.
 */
int CInput::applyPreviousParams(int offset)
{
	float value		= 0.f;
	int prevOffset	= 0;
	bool bResult	= false;
	int result		= offset / m_nSamplesToBytes;

	prevOffset = m_timeline.GetPrevOffset(offset, TIMELINE_PARAM_ENABLE);
	bResult = m_timeline.GetParamVal(prevOffset, TIMELINE_PARAM_ENABLE, &value);
	if(bResult || prevOffset==0)
	{
		bool bEnable = bResult ? ( PARAM_VALUE_DISABLED != value ) : false;
		IFloopy::Enable( bEnable );
		m_plugin->Enable(bEnable);
	}

	prevOffset = m_timeline.GetPrevOffset(offset, TIMELINE_PARAM_MOVETO);
	if( m_timeline.GetParamVal(prevOffset, TIMELINE_PARAM_MOVETO, &value) )
	{
		int diff = (offset - prevOffset) / m_nSamplesToBytes;
		result = (int)value + diff;
	}

	for(int index=0; index<GetParamCount(); index++)
	{
		prevOffset = m_timeline.GetPrevOffset(offset, index);
		if( m_timeline.GetParamVal(prevOffset, index, &value) )
			m_plugin->SetParamVal(index, value);
	}

	return result;
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

	float status = IsEnabled()?PARAM_VALUE_ENABLED:PARAM_VALUE_DISABLED;

	do {
		if( m_timeline.GetParamVal(tmp, TIMELINE_PARAM_ENABLE, &value) )
		{
			status = value;
			if((value == PARAM_VALUE_DISABLED) && (tmp > offset))
				offset = tmp;
		}
	} while((tmp=m_timeline.GetNextOffset(tmp)) > 0);

	// Proveriti da li se na kraju iskljucuje.
	if((status==PARAM_VALUE_ENABLED) && m_plugin)
		offset = m_nStartOffset + m_plugin->GetSize() * m_nSamplesToBytes;

	if( ReadSourceIfDisabled() )
	{
		int srcSize = GetSourceSize();
		if(srcSize > offset)
			offset = srcSize;
	}

	return offset;
}

int CInput::skipChunk(int size)
{
	int pos = m_plugin->GetPosition();
	if(EOF == pos)
		return EOF;
	m_plugin->MoveTo( pos + (size / m_nSamplesToBytes) );
	return size;
}

IFloopySoundInput *CInput::getSource()
{
	if( GetBypass() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else if( IFloopy::IsEnabled() )
		return m_plugin;
	else if( ReadSourceIfDisabled() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else
		return NULL;
}

bool CInput::isFilter()
{
	int type = m_plugin->GetType();
	return (type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

bool CInput::isEngine()
{
	return(m_source?m_source->GetType()==TYPE_FLOOPY_SOUND_ENGINE:false);
}

bool CInput::isMixer()
{
	if(NULL != m_plugin)
		return (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER);
	return false;
}

bool CInput::isEndOfTrack()
{
	return(m_nEndOffset>0 && m_offset>=m_nEndOffset && !ReadSourceIfDisabled());
}
