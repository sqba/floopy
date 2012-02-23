// Cache.cpp: implementation of the CCache class.
//
//////////////////////////////////////////////////////////////////////

#include "cache.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCache::CCache()
{
	m_pBuffer = NULL;
	m_nPosition = m_nSize = 0;
	m_pSourceParams = NULL;
}

CCache::~CCache()
{
	if( !bufferIsEmpty() )
		clearBuffer();
}

int CCache::Read(BYTE *data, int size)
{
	if( (bufferIsEmpty() && !createBuffer()) || passedTheEnd() || sourceParameterChanged() )
		return EOF;

	// Last chunk
	if( (m_nPosition + size) > m_nSize )
		size = m_nSize - m_nPosition;

	memcpy(data, m_pBuffer+m_nPosition, size);

	m_nPosition += size;

	return size;
}

bool CCache::SetSource(IFloopySoundInput *src)
{
	if(	createBuffer() )
		return false;

	loadSourceParams( src );

	return IFloopySoundFilter::SetSource(src);
}

void CCache::Close()
{
	if( !bufferIsEmpty() )
		clearBuffer();

	m_nPosition = 0;

	IFloopySoundFilter::Close();
}

int CCache::GetSize()
{
	int size = IFloopySoundFilter::GetSize();
	int stb = samplesToBytes();
	if(stb>0 && m_nSize>0)
		return m_nSize / samplesToBytes();
	else if(stb>0 && size==SIZE_VARIABLE)
	{
		if( createBuffer() )
			return m_nSize / samplesToBytes();
		else
			return 0;
	}

	return size;
}

int CCache::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
		return (fmt->bitsPerSample / 8) * fmt->channels;
	else
		return 0;
}

void CCache::clearBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = NULL;
	m_nSize = 0;
}

bool CCache::createBuffer()
{
	if( !bufferIsEmpty() )
		clearBuffer();

	IFloopySoundInput *src = GetSource();
	if(src)
	{
		int size = src->GetSize();
		if(size > 0)	// Infinite sources (-1) not allowed!
		{
			// Allocate the memory
			m_nSize = size * samplesToBytes();
			m_pBuffer = new BYTE[m_nSize];
			if(m_pBuffer)
			{
				memset(m_pBuffer, 0, m_nSize);

				// Fill the buffer
				src->Reset();
				size = (1024 < m_nSize ? 1024 : m_nSize);
				BYTE *pbuff = m_pBuffer;
				int len = 0;
				while(len < m_nSize)
				{
					if((len + size) > m_nSize)
						size = m_nSize - len;
					int read = src->Read(pbuff, size);
					if(read == EOF)
						break; // End of file
					pbuff += read;
					len += read;
				}
				m_nSize = len;
			}
			else
				return false;
		}
		else if(size == SIZE_VARIABLE)
		{
			size = 1024; //samples
			//int type = src->GetType();
			//if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
			//	size = ((IFloopySoundFilter*)src)->GetSource()->GetSize();
			// Allocate the memory
			m_nSize = 1024;//size * samplesToBytes();

			int buffsize = m_nSize;

			m_pBuffer = new BYTE[m_nSize];

			if(m_pBuffer)
			{
				memset(m_pBuffer, 0, buffsize);

				// Fill the buffer
				src->Reset();
				//size = (1024 < m_nSize ? 1024 : m_nSize);
				BYTE *pbuff = m_pBuffer;
				int len = 0;

				int read = 0;
				int n=0;
				do
				{
					if(buffsize==0 || len+buffsize > m_nSize)
					{
						m_nSize *= 2;
						BYTE *tmp = new BYTE[m_nSize];
						assert(NULL != tmp);
						memset(tmp, 0, m_nSize);
						memcpy(tmp, m_pBuffer, len);
						delete m_pBuffer;
						m_pBuffer = tmp;
						pbuff = m_pBuffer+len;
						buffsize = m_nSize - len;
					}

					read = src->Read(pbuff, buffsize);
					if(read != EOF)
					{
						buffsize -= read;
						pbuff += read;
						len += read;
					}
					n++;
				} while(read > 0);

				m_nSize = len;
			}
			else
				return false;
		}
	}

	return (m_nSize > 0);
}

bool CCache::sourceParameterChanged()
{
	if(bufferIsEmpty())
		return false;
	//if(NULL == m_pSourceParams)
	//	return true;

	IFloopySoundInput *src = GetSource();

	sourceParam *tmp = m_pSourceParams;
	float value = 0.f;
	for(int i=0; i<src->GetParamCount(); i++)
	{
		if(tmp && src->GetParamVal(i, &value) && tmp->value!=value )
			return true;
		tmp = tmp->next;
	}
	return false;
}

void CCache::loadSourceParams(IFloopySoundInput *src)
{
	sourceParam *prev = NULL;
	sourceParam *tmp = m_pSourceParams;
	float value = 0.f;
	for(int i=0; i<src->GetParamCount(); i++)
	{
		if( src->GetParamVal(i, &value) )
		{
			if(NULL == tmp)
				tmp = new sourceParam;
			tmp->value = value;
			prev->next = tmp;
		}
		tmp = tmp->next;
	}
}

void CCache::MoveTo(int samples)
{
	m_nPosition = samples * samplesToBytes();
}

int CCache::GetPosition()
{
	return m_nPosition / samplesToBytes();
}

void CCache::Reset()
{
	m_nPosition = 0;
}
