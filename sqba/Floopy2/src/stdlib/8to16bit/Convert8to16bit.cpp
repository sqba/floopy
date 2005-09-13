// Convert8to16bit.cpp: implementation of the CConvert8to16bit class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "Convert8to16bit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvert8to16bit::CConvert8to16bit()
{
	m_pBuffer = NULL;
	m_nBuffSize = NULL;
}

CConvert8to16bit::~CConvert8to16bit()
{
	if(m_pBuffer)
		delete m_pBuffer;
}

int CConvert8to16bit::Read(BYTE *data, int size)
{
	size /= 2;

	BYTE *pBuffer = m_pBuffer;
	bool bDelete = false;

	if(size != m_nBuffSize)
	{
		if(pBuffer)
		{
			try {
				delete pBuffer;
				m_nBuffSize = size;
			} catch(...) {
				bDelete = true;
			}
		}
		pBuffer = new BYTE[size];
	}
	memset(pBuffer, 128, size); // Fill with 8bit silence

	int len = IFloopySoundFilter::Read(pBuffer, size);

	if(len != EOF)
	{
		int numsamples = len;

		BYTE *in		= (BYTE*)pBuffer;
		short int *out	= (short int*)data;

		while(numsamples--)
			*(out++) = (short int)(*(in++)) * 256 - 32768;
	}

	if(bDelete)
		delete pBuffer;

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
