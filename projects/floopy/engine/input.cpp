// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "input.h"

typedef IFloopySoundInput* (*CreateProc)(const char *name);
#define PROC_NAME	"CreateInput"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CInput::CInput(UpdateCallback func, COutputCache *outputCache)
{
	m_callback		= func;
	m_plugin		= NULL;
	m_pOutputCache	= outputCache;
	m_pDefaultParams = NULL;
	m_bFadeEdges	= true;
	m_pSignature	= NULL;

	m_red = m_green = m_blue = 256; /// Invalid value

	memset(m_szDisplayName,	0, 50);
	memset(m_szLastError,	0, sizeof(m_szLastError));
	memset(m_szObjPath,		0, sizeof(m_szObjPath));
	
	m_offset = m_nStartOffset = m_nEndOffset = 0;
	m_nSkipBytes = m_nSamplesToBytes = 0;

}

CInput::~CInput()
{
	if(NULL != m_plugin)
		delete m_plugin;

	if(NULL != m_pDefaultParams)
		delete[] m_pDefaultParams;

	if(NULL != m_pSignature)
		delete m_pSignature;
}

/**
 * Create the plugin.
 * @param plugin library name.
 * @return true if succesfull.
 */
bool CInput::Create(const char *name)
{
	char plugin[MAX_PATH]	= {0};
	char library[MAX_FNAME]	= {0};

	getLibraryName(name, library);
	getPluginName(name, plugin);

	if( !LoadPlugin(library) )
	{
		sprintf(m_szLastError, "File '%s' not found.\n", library);
		return false;
	}

	CreateProc func = (CreateProc)GetFunction(PROC_NAME); 
	if(NULL == func)
	{
		sprintf(m_szLastError, "Function '%s' not found in library '%s'.\n",
			PROC_NAME, library);
		return false;
	}

	m_plugin = func( plugin );
	if(NULL == m_plugin)
	{
		sprintf(m_szLastError, "Plugin '%s' not created by function %s.%s.\n",
			plugin, library, PROC_NAME);
		return false;
	}

	IFloopySoundFilter::SetSource(m_plugin);

	Enable( m_plugin->IsEnabled() ); // Default

	strcpy(m_szObjPath, library);

	char *tmp = strrchr(plugin, PATH_SEP);
	tmp = tmp ? ++tmp : plugin;
	SetDisplayName(plugin, strlen(tmp));

	loadDefaultParams();

	createSignature();

	return true;
}

void CInput::loadDefaultParams()
{
	if(NULL != m_pDefaultParams)
	{
		delete[] m_pDefaultParams;
		m_iParamCount = 0;
	}

	int count = m_plugin->GetParamCount();
	if(count > 0)
	{
		m_iParamCount = count;
		m_pDefaultParams = new float[count];
		memset(m_pDefaultParams, 0, count*sizeof(float));
		for(int i=0; i<count; i++)
		{
			m_plugin->GetParamVal(i, &m_pDefaultParams[i]);
		}
	}
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

	Enable( false ); // Default
	
	const char *name = src->GetDisplayName();
	if(NULL != name)
		SetDisplayName(name, strlen(name));

	loadDefaultParams();

	return true;
}

bool CInput::Open(const char *filename)
{
	if(NULL!=m_plugin && m_plugin->Open(filename))
	{
		char *tmp = strrchr(filename, '\\');
		filename = tmp ? tmp+1 : filename;
		SetDisplayName(filename, strlen(filename));
		recalcVariables();
		createSignature();
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

	if(NULL!=m_plugin && isFilter())
	{
		result = ((IFloopySoundFilter*)m_plugin)->SetSource(src);
	
		if(result)
		{
			recalcSourceVariables();
			recalcVariables();
		}
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

	int result = readFromCache(data, size);
	if(result > 0)
		return result;

	int len			= 0;
	int endpos		= m_offset + size;
	IFloopySoundInput *src = NULL;

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
	}

	if(result==0 && len==EOF)
		result = EOF;

//	gOutputCache->Add(signature, data);

	return result;
}

/**
 * Moves to the given position.
 * @param samples number of samples.
 */
void CInput::MoveTo(int samples)
{
	m_nSkipBytes = 0;

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
	else if(NULL != m_plugin)
	{
		size = m_plugin->GetSize();

		if(m_nSamplesToBytes > 0)
		{
			if(m_nEndOffset > 0)
				size = m_nEndOffset / m_nSamplesToBytes;
			else
				size += m_nStartOffset / m_nSamplesToBytes;
		}

		if( m_plugin->CanReadSourceIfDisabled() )
		{
			int srcSize = GetSourceSize();
			if(srcSize > size)
				size = srcSize;
		}
	}

	//size = size > 0 ? size : -1;

	return size;
}

int CInput::GetSourceSize()
{
	if(NULL!=m_plugin && isFilter())
		return ((IFloopySoundFilter*)m_plugin)->GetSourceSize();
	return 0;
}

void CInput::Reset()
{
	m_offset = 0;

	m_nSkipBytes = 0;

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

int CInput::GetNextOffset(int offset, int index)
{
	offset *= m_nSamplesToBytes;
	int next = m_timeline.GetNextOffset(offset, index);
	return (next > 0 ? next/m_nSamplesToBytes : 0);
}

int CInput::GetPrevOffset(int offset, int index)
{
	offset *= m_nSamplesToBytes;
	int prev = m_timeline.GetPrevOffset(offset, index);
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
		int prev = m_timeline.GetPrevOffset( m_offset );
		if(m_timeline.GetParamVal(prev, TIMELINE_PARAM_ENABLE, &value))
			result = (PARAM_VALUE_DISABLED != value);
	}
	return result;
}

const char *CInput::GetParamName(int index)
{
	switch(index)
	{
	case TIMELINE_PARAM_ENABLE:	return "Enable";
	case TIMELINE_PARAM_MOVETO:	return "MoveTo";
	default:					return m_plugin->GetParamName(index);
	}
}

const char *CInput::GetParamDesc(int index)
{
	switch(index)
	{
	case TIMELINE_PARAM_ENABLE:	return "Enable";
	case TIMELINE_PARAM_MOVETO:	return "MoveTo";
	default:					return m_plugin->GetParamDesc(index);
	}
}

bool CInput::GetParamVal(int index, float *value)
{
//	return m_timeline.GetParamVal(m_offset, index, value);

	if(m_timeline.GetParamVal(m_offset, index, value))
		return true;
	else if(index != TIMELINE_PARAM_MOVETO)
	{
		int prev = m_timeline.GetPrevOffset( m_offset, index );
		if(m_timeline.GetParamVal(prev, index, value))
			return true;
		else if(index>=0 && index<m_iParamCount)
		{
			*value = m_pDefaultParams[index];
			return true;
		}
		else
			return m_plugin->GetParamVal(index, value);
	}

	return false;
}

void CInput::SetParamVal(int index, float value)
{
	if(NULL == m_plugin)
		return;

	if(m_callback && m_nSamplesToBytes > 0)
		m_callback(this, m_offset/m_nSamplesToBytes, index);

	m_timeline.SetParamVal(m_offset, index, value);

	m_plugin->SetParamVal(index, value);

	recalcVariables();
}

bool CInput::GetParamIndex(const char *name, int *index)
{
	if(NULL==name || NULL==index)
		return false;

	if(0 == strcmpi(name, "Enable"))
	{
		*index = TIMELINE_PARAM_ENABLE;
		return true;
	}

	if(0 == strcmpi(name, "MoveTo"))
	{
		*index = TIMELINE_PARAM_MOVETO;
		return true;
	}

	if(NULL == m_plugin)
		return false;

	if( m_plugin->GetParamIndex(name, index) )
		return true;
	else
	{
		for(int i=0; i<m_plugin->GetParamCount(); i++)
		{
			if(0 == strcmpi(m_plugin->GetParamName(i), name))
			{
				*index = i;
				return true;
			}
		}
	}

	return false;
}

bool CInput::GetPropertyIndex(const char *name, int *index)
{
	if(NULL == m_plugin)
		return false;

	if( m_plugin->GetPropertyIndex(name, index) )
		return true;
	else
	{
		for(int i=0; i<m_plugin->GetPropertyCount(); i++)
		{
			if(0 == strcmpi(m_plugin->GetPropertyName(i), name))
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
	offset *= m_nSamplesToBytes;

	*value = 0.f;

//	return m_timeline.GetParamVal(offset, index, value);

	if(m_timeline.GetParamVal(offset, index, value))
		return true;
	else if(offset==0)// && index!=TIMELINE_PARAM_MOVETO)
	{
		int prev = m_timeline.GetPrevOffset( offset, index );
		if(m_timeline.GetParamVal(prev, index, value))
			return true;
		else if(index>=0 && index<m_iParamCount)
		{
			*value = m_pDefaultParams[index];
			return true;
		}
		else if(index >= 0)
		{
			try {
				m_plugin->GetParamVal(index, value);
				return true;
			} catch(...) {
				*value = 0.f;
				return false;
			}
		}
	}
	return false;
}

bool CInput::SetParamAt(int offset, int index, float value)
{
	offset *= m_nSamplesToBytes;
	m_timeline.SetParamVal(offset, index, value);
	if(offset == 0)
		m_plugin->SetParamVal(index, value);
	recalcVariables();
	return true;
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
bool CInput::EnableAt(int offset, bool bEnable)
{
	offset *= m_nSamplesToBytes;
	m_timeline.EnableAt(offset, bEnable);
	recalcVariables();
	return true;
}

bool CInput::ClearAllParams()
{
	m_timeline.Clear();
	return true;
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

void CInput::SetDisplayName(const char *name, int len)
{
	memset(m_szDisplayName, 0, 50);
	memcpy(m_szDisplayName, name, (len<50?len:50));
}

bool CInput::CanReadSourceIfDisabled()
{
	return (isFilter() ? m_plugin->CanReadSourceIfDisabled() : false);
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
 * samples = bytes / samplesToBytes
 * bytes = samples * samplesToBytes
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
	int sample	= offset / m_nSamplesToBytes;
	float value	= 0.f;

	bool bResult = m_timeline.GetParamVal(offset, TIMELINE_PARAM_ENABLE, &value);
	if(bResult || offset==0)
	{
		bool bEnable = bResult ? PARAM_VALUE_DISABLED!=value : false;
		IFloopy::Enable( bEnable );
		m_plugin->Enable( bEnable );

		// Optimization: skipChunk did not call MoveTo so
		// we have to do it now.
		if(bEnable && m_nSkipBytes>0)
		{
			int pos = m_plugin->GetPosition();
			if(EOF != pos)
				m_plugin->MoveTo( pos + m_nSkipBytes / m_nSamplesToBytes );
			m_nSkipBytes = 0;
		}

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

	if( CanReadSourceIfDisabled() )
	{
		int srcSize = GetSourceSize();
		if(srcSize > offset)
			offset = srcSize;
	}

	return offset;
}

/**
 * Reads an empty buffer.
 * @param size number of bytes to skip.
 * @return number of bytes skipped.
 */
int CInput::skipChunk(int size)
{
	// Optimization: MoveTo will be called in the first call
	// to applyParamsAt in which the plugin is enabled.
	m_nSkipBytes += size;

	/*int pos = m_plugin->GetPosition();
	if(EOF == pos)
		return EOF;
	m_plugin->MoveTo( pos + (size / m_nSamplesToBytes) );*/

	return size;
}

/**
 * @return the component scheduled for reading from or NULL if disabled.
 */
IFloopySoundInput *CInput::getSource()
{
	if( GetBypass() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else if( IFloopy::IsEnabled() )
		return m_plugin;
	else if( CanReadSourceIfDisabled() )
		return ((IFloopySoundFilter*)m_plugin)->GetSource();
	else
		return NULL;
}

/**
 * @return true the plugin has it's own source.
 */
bool CInput::isFilter()
{
	int type = m_plugin->GetType();
	//bool b = (type & TYPE_FLOOPY_SOUND_FILTER);
	return (type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

/**
 * @return true if the plugin is another engine.
 */
inline bool CInput::isEngine()
{
	return(m_source?m_source->GetType()==TYPE_FLOOPY_SOUND_ENGINE:false);
}

/**
 * @return true the plugin has several sources.
 */
bool CInput::isMixer()
{
	if(NULL != m_plugin)
		return (m_plugin->GetType() == TYPE_FLOOPY_SOUND_MIXER);
	return false;
}

/**
 * Checks if the current offset (m_offset) is at the
 * or past the end of the track.
 * @return true if the current offset (m_offset) is at the
 * or past the end of the track.
 */
bool CInput::isEndOfTrack()
{
	return(m_nEndOffset>0 && m_offset>=m_nEndOffset && !CanReadSourceIfDisabled());
}

/**
 * Extracts the library name from full plugin name (library.plugin).
 * @param fullname pointer to buffer containing full plugin name.
 * @param name pointer to the buffer to receive the library name.
 * @return void.
 */
void CInput::getLibraryName(const char *fullname, char *name)
{
	char *sep = strrchr(fullname, '.');
	int len = sep ? sep-fullname : strlen(fullname);

	int extlen = strlen(PLUG_EXT);
	if( len >= MAX_PATH-extlen-1 )
		len = MAX_PATH-extlen-1;

	strncpy(name, fullname, len);
	strcat(name, PLUG_EXT);
}

/**
 * Extracts the plugin name from full plugin name (library.plugin).
 * @param fullname pointer to buffer containing full plugin name.
 * @param name pointer to the buffer to receive the plugin name.
 * @return void.
 */
void CInput::getPluginName(const char *fullname, char *name)
{
	const char *path = strrchr(fullname, PATH_SEP);
	if(path)
		path++;
	else
		path = fullname;
	
	char *sep = strrchr(path, '.');
	const char *tmp = sep ? sep+1 : path;

	if( strlen(tmp) <= MAX_FNAME )
		strcpy(name, tmp);
	else
		strncpy(name, tmp, MAX_FNAME);
}

void CInput::createSignature()
{
	return;

	if(m_pSignature != NULL)
	{
		delete m_pSignature;
		m_pSignature = NULL;
	}

	if(m_nSamplesToBytes == 0)
		return;

	const char *inSig	= m_source->GetSignature();
	const char *name	= m_source->GetName();
	int count	= GetPropertyCount() + GetParamCount();
	float *fSig = new float[ count ];

	int i;
	for(i=0; i<GetPropertyCount(); i++)
	{
		float val = 0.f;
		GetPropertyVal(i, &val);
		fSig[i] = val;
	}
	int n;
	for(n=i; n<GetParamCount()+i; n++)
	{
		float val = 0.f;
		GetParamVal(n, &val);
		fSig[n] = val;
	}

	char *params = new char[count * 11];
	memset(params, 0, count * 11);
	for(i=0; i<count; i++)
	{
		char param[11] = {0};
		sprintf(param, "%f", fSig[i]);
		strncat(params, param, 10);
	}
	
	//int pos = GetPosition();

	int len = sizeof(inSig) * sizeof(char)
		+ sizeof(name) * sizeof(char)
		+ sizeof(params) * sizeof(char);
	m_pSignature = new char[len];
	memset(m_pSignature, 0, len);
	if(inSig != NULL)
		strcat(m_pSignature, inSig);
	strcat(m_pSignature, name);
	strcat(m_pSignature, params);

	delete fSig;
	delete params;
}

/**
 * Returns the signature of the component, along with it's input signature.
 */
const char *CInput::GetSignature()
{
	return m_pSignature;
}

/**
 * Tries to read the buffer from the global cache.
 */
int CInput::readFromCache(BYTE *data, int size)
{
	const char *signature = GetSignature();
	if(signature != NULL)
	{
		int result = m_pOutputCache->Get(signature, GetPosition(), data, size);
		if(result > 0)
			return result;
	}
	return 0;
}
