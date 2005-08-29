// Reverse.cpp: implementation of the CReverse class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "Reverse.h"





// Zasto se periodicno pojavljuju nule koje izazivaju pucketanje?


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

	//memset(m_pBuffer, 0, m_nBuffSize);
	memcpy(m_pBuffer, data, size);


	int stb = samplesToBytes();

	int srcSize = IFloopySoundFilter::GetSize();

	int buffsize = size;

	int tmp = srcSize*stb;
	if(buffsize > tmp)
		buffsize = tmp;

//	if(buffsize/stb > srcSize-m_nPosition)
//		buffsize = (srcSize-m_nPosition)*stb;

	if(m_nPosition < buffsize/stb)
		buffsize = m_nPosition*stb;

	int start = m_nPosition - buffsize/stb;
	//assert(start >= 0);
	IFloopySoundFilter::MoveTo(start);

	int len = IFloopySoundFilter::Read(m_pBuffer, buffsize);

	if(EOF != len)
	{
		//assert(m_nPosition >= len/stb);
		reverse(data, len);
		m_nPosition -= len/stb;
		//IFloopySoundFilter::MoveTo(m_nPosition);
	}
//	else
//		m_nPosition -= srcSize;

//	assert(len == size);

	return len;
}

void CReverse::reverse(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = GetFormat();
	if(size>0 && fmt && fmt->bitsPerSample && fmt->channels)
	{
		short int *tmp = (short int*)(m_pBuffer+size);
		short int *out = (short int*)data;

		int numsamples = size / (fmt->bitsPerSample/8);

		int pos = numsamples;

		int pos1 = numsamples;

		
		do {
			tmp -= fmt->channels;
			for(int ch=0; ch<fmt->channels; ch++)
			{
				*(out++) = *(tmp++);
				pos1--;
			}
			tmp -= fmt->channels;
		} while(pos -= fmt->channels);

		assert(pos == pos1);

		//while(numsamples--)
		//	*(out++) = *(tmp--);
	}
}

void CReverse::MoveTo(int samples)
{
	int size = IFloopySoundFilter::GetSize();

	if(SIZE_INFINITE != size)
	{
		if(size >= samples)
			samples = size - samples;
		//else
		//	samples = size;
	}

	m_nPosition = samples;

	assert(m_nPosition >= 0);

	IFloopySoundFilter::MoveTo(samples);
}

void CReverse::Reset()
{
	int size = IFloopySoundFilter::GetSize();
	m_nPosition = size;
	IFloopySoundFilter::MoveTo(size);
}

int CReverse::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
