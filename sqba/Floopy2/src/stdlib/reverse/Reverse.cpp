// Reverse.cpp: implementation of the CReverse class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "Reverse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReverse::CReverse()
{
	m_nPosition = 0;
	m_pBuffer = NULL;
	m_nBuffSize = NULL;
}

CReverse::~CReverse()
{
	if(m_pBuffer)
		delete m_pBuffer;
}

int CReverse::Read(BYTE *data, int size)
{
	if(SIZE_INFINITE == size)
		return IFloopySoundFilter::Read(data, size);


	if(size != m_nBuffSize)
	{
		if(m_pBuffer)
			delete m_pBuffer;
		m_nBuffSize = size;
		m_pBuffer = new BYTE[m_nBuffSize];
	}
	memset(m_pBuffer, 0, m_nBuffSize);


	int stb = samplesToBytes();

	int srcSize = IFloopySoundFilter::GetSize();

	int s = srcSize*stb;
	if(size > s)
		size = s;

	int start = m_nPosition - size/stb;
	IFloopySoundFilter::MoveTo(start);
	int len = IFloopySoundFilter::Read(m_pBuffer, size);

	if(EOF != len)
	{
		m_nPosition -= len/stb;
		reverse(data, len);
	}
//	else
//		m_nPosition -= srcSize;


//	assert(len == size);

	return len;
}

void CReverse::MoveTo(int samples)
{
	int size = IFloopySoundFilter::GetSize();

	if(SIZE_INFINITE != size)
	{
		if(size > samples)
		{
			samples = size - samples;
			m_nPosition = samples;
		}
		else
		{
			samples = size;
			m_nPosition = size;
		}
	}
	else
		m_nPosition = samples;

	IFloopySoundFilter::MoveTo(samples);
}

int CReverse::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}

void CReverse::reverse(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = GetFormat();
	if(fmt && fmt->bitsPerSample && fmt->channels)
	{
		int numsamples = size / (fmt->bitsPerSample/8);

		short int *tmp = (short int*)(m_pBuffer+size);
		short int *out = (short int*)data;

		while(numsamples--)
			*(out++) = *(tmp--);

		/*while(numsamples -= fmt->channels)
		{
			tmp -= fmt->channels;
			for(int ch=0; ch<fmt->channels; ch++)
				*(out++) = *(tmp++);
			tmp -= fmt->channels;
		}*/
	}
}

void CReverse::Reset()
{
	int size = IFloopySoundFilter::GetSize();
	m_nPosition = size;
	IFloopySoundFilter::MoveTo(size);
}

// Zasto se periodicno pojavljuju nule koje izazivaju pucketanje?