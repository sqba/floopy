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

	if(size != m_nBuffSize)
	{
		if(m_pBuffer)
			delete m_pBuffer;
		m_nBuffSize = size;
		m_pBuffer = new BYTE[m_nBuffSize];
	}
	memset(m_pBuffer, 128, size); // Fill with 8bit silence

	int len = IFloopySoundFilter::Read(m_pBuffer, size);

	if(len != EOF)
	{
		SOUNDFORMAT *fmt = this->GetFormat();
		if(!fmt || !fmt->bitsPerSample || !fmt->channels)
			return 0;

		int numsamples = len;

		BYTE *in		= (BYTE*)m_pBuffer;
		short int *out	= (short int*)data;

		while(numsamples--)
			*(out++) = (short int)(*(in++)) * 256 - 32768;
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
BOOL CConvert8to16bit::SetSource(IFloopySoundInput *src)
{
	memcpy(&m_format, src->GetFormat(), sizeof(SOUNDFORMAT));
	if(m_format.bitsPerSample == 8)
	{
		m_format.bitsPerSample = 16;

		IFloopySoundFilter::SetSource( src );

		return TRUE;
	}
	return FALSE;
}
*/
