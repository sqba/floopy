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
	m_nSize = 0;
	m_nPosition = 0;
}

CInput::~CInput()
{
	if(m_pBuffer)
		delete[] m_pBuffer;
}

UINT CInput::Read(BYTE *data, UINT size)
{
	// Buffer is empty
	if(m_nSize == 0)
		createBuffer();

	// createBuffer failed
	if(m_nSize == 0)
		return 0;

	if((m_nPosition + size) > m_nSize)
		size = m_nSize - m_nPosition;

	BYTE *p = m_pBuffer+m_nPosition;

	memcpy(data, m_pBuffer+m_nPosition, size);

	m_nPosition += size;

	return size;
}

int CInput::samplesToBytes()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	return (fmt->size / 8) * fmt->channels;
}

void CInput::createBuffer()
{
	if(m_pBuffer)
	{
		delete[] m_pBuffer;
		m_pBuffer = NULL;
		m_nSize = 0;
	}

	IFloopySoundInput *src = GetSource();
	if(src)
	{
		// Allocate the memory
		UINT size = src->GetSize();
		m_nSize = size * samplesToBytes();
		m_pBuffer = new BYTE[m_nSize];
		memset(m_pBuffer, 0, m_nSize);

		// Fill the buffer
		src->Reset();
		size = 128;
		BYTE *pbuff = m_pBuffer;
		UINT len = 0;
		//while((size = src->Read(pbuff, size)) == size && (len < m_nSize))
		while(len < m_nSize)
		{
			if((len + size) > m_nSize)
				size = m_nSize - len;
			UINT read = src->Read(pbuff, size);
			pbuff += read;
			len += read;
		}
	}

	m_nPosition = 0;
}

void CInput::SetSource(IFloopySoundInput *src)
{
	createBuffer();

	IFloopySoundInput::SetSource(src);
}

void CInput::MoveTo(UINT samples)
{
	m_nPosition = samples * samplesToBytes();
}

UINT CInput::GetPos()
{
	return m_nPosition / samplesToBytes();
}

void CInput::Reset()
{
	m_nPosition = 0;
}
