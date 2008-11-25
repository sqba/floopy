#include <assert.h>
#include "reverse.h"


#define SAMPLE	short int


CReverse::CReverse()
{
	m_nPosition	= 0;
	m_pBuffer	= NULL;
	m_nBuffSize	= 0;
	m_nSamplesToBytes = 0;
}

CReverse::~CReverse()
{
	if(m_pBuffer)
		delete m_pBuffer;
}

int CReverse::Read(BYTE *data, int size)
{
	int srcSize = IFloopySoundFilter::GetSize();

	if(SIZE_INFINITE == srcSize)
		return IFloopySoundFilter::Read(data, size);

	if(m_nPosition <= 0)
		return EOF;

	if(size > m_nBuffSize)
	{
		if(m_pBuffer)
			delete m_pBuffer;
		m_nBuffSize = size;
		m_pBuffer = new BYTE[m_nBuffSize];
	}

	GetEngine()->EmptyBuffer(m_pBuffer, m_nBuffSize);

	int buffsize = size;

	int tmp = srcSize * m_nSamplesToBytes;
	if(buffsize > tmp)
		buffsize = tmp;

	if(m_nPosition < buffsize/m_nSamplesToBytes)
		buffsize = m_nPosition * m_nSamplesToBytes;

	int start = m_nPosition - (buffsize / m_nSamplesToBytes);
	//assert(start >= 0);
	IFloopySoundFilter::MoveTo(start);

	int len = IFloopySoundFilter::Read(m_pBuffer, buffsize);

	if(EOF != len)
	{
		//assert(m_nPosition >= len/m_nSamplesToBytes);
		reverse(data, len);
		m_nPosition -= len / m_nSamplesToBytes;
	}

	//assert(len == size);

	return len;
}

void CReverse::reverse(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = GetFormat();
	if(size>0 && fmt && fmt->channels>0 && fmt->bitsPerSample==16)
	{
		SAMPLE *tmp = (short int*)(m_pBuffer+size);
		SAMPLE *out = (short int*)data;

		int numsamples = size / (fmt->bitsPerSample/8);

		int pos = numsamples;

		do {
			tmp -= fmt->channels;
			for(int ch=0; ch<fmt->channels; ch++)
				*(out++) = *(tmp++);
			tmp -= fmt->channels;
		} while(pos -= fmt->channels);
	}
}

void CReverse::MoveTo(int samples)
{
	int size = IFloopySoundFilter::GetSize();

//	assert(samples <= size);

	if(SIZE_INFINITE != size)
	{
		if(size >= samples)
			samples = size - samples;
		else if(samples > size)
			samples = 0;
	}

	assert(m_nPosition >= 0);
	assert(m_nPosition <= size);

	m_nPosition = samples;

	IFloopySoundFilter::MoveTo(samples);
}

void CReverse::Reset()
{
	int size = IFloopySoundFilter::GetSize();
	m_nPosition = size;
	IFloopySoundFilter::MoveTo(size);
}

int CReverse::GetPosition()
{
	int size = IFloopySoundFilter::GetSize();
	return size - m_nPosition;
}

bool CReverse::SetSource(IFloopySoundInput *src)
{
	SOUNDFORMAT *fmt = src->GetFormat();
	assert(fmt && (fmt->bitsPerSample > 0) && (fmt->channels > 0));
	m_nSamplesToBytes = (fmt->bitsPerSample / 8) * fmt->channels;
	return IFloopySoundFilter::SetSource(src);
}
