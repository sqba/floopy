// Convert8to16bit.cpp: implementation of the CConvert8to16bit class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "convert8to16bit.h"


#define SAMPLE	short int


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvert8to16bit::CConvert8to16bit()
{
	m_pBuffer	= NULL;
	m_nBuffSize	= 0;
}

CConvert8to16bit::~CConvert8to16bit()
{
	if(m_pBuffer)
		delete[] m_pBuffer;
}

int CConvert8to16bit::Read(BYTE *data, int size)
{
	size /= 2;

	if(size > m_nBuffSize)
	{
		if(m_pBuffer)
		{
			delete[] m_pBuffer;
			m_nBuffSize = size;
		}
		m_pBuffer = new BYTE[size];
	}
	memset(m_pBuffer, 128, m_nBuffSize); // Fill with 8bit silence

	int len = IFloopySoundFilter::Read(m_pBuffer, size);

	if(len != EOF)
	{
		int numsamples = len;

		BYTE *in	= (BYTE*)m_pBuffer;
		SAMPLE *out	= (SAMPLE*)data;

		while(numsamples--)
			*(out++) = (SAMPLE)(*(in++)) * 256 - 32768;
	}

	return len*2;
}

SOUNDFORMAT *CConvert8to16bit::GetFormat()
{
	memcpy(&m_format, IFloopySoundFilter::GetFormat(), sizeof(SOUNDFORMAT));
	m_format.bitsPerSample = 16;
	return &m_format;
}




// Zashto nece ovako!?
/*
bool CConvert8to16bit::SetSource(IFloopySoundInput *src)
{
	memcpy(&m_format, src->GetFormat(), sizeof(SOUNDFORMAT));
	if(m_format.bitsPerSample == 8)
	{
		m_format.bitsPerSample = 16;

		IFloopySoundFilter::SetSource( src );

		return true;
	}
	return false;
}
*/
