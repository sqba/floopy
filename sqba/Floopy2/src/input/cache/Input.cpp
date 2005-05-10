// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <math.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	m_pBuffer = NULL;
	m_nPosition = m_nSize = 0;
}

CInput::~CInput()
{
	if( !bufferIsEmpty() )
		clearBuffer();
}

int CInput::Read(BYTE *data, int size)
{
	if( (bufferIsEmpty() && !createBuffer()) || passedTheEnd() )
		return 0;

	// Last chunk
	if( (m_nPosition + size) > m_nSize )
		size = m_nSize - m_nPosition;

	memcpy(data, m_pBuffer+m_nPosition, size);

	m_nPosition += size;

	return size;
}

BOOL CInput::SetSource(IFloopySoundInput *src)
{
	if(	createBuffer() )
		return FALSE;

	return IFloopySoundInput::SetSource(src);
}

void CInput::Close()
{
	if( !bufferIsEmpty() )
		clearBuffer();

	m_nPosition = 0;

	IFloopySoundInput::Close();
}

int CInput::samplesToBytes()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}

void CInput::clearBuffer()
{
	delete[] m_pBuffer;
	m_pBuffer = NULL;
	m_nSize = 0;
}

BOOL CInput::createBuffer()
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
				if(read == 0)
					break; // End of file
				pbuff += read;
				len += read;
			}
			m_nSize = len;
		}
	}

	return (m_nSize > 0);
}
