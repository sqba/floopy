// Mono2Stereo.cpp: implementation of the CMono2Stereo class.
//
//////////////////////////////////////////////////////////////////////

#include "mono2stereo.h"
#include <stdio.h>
#include <assert.h>


#define SAMPLE	short int


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMono2Stereo::CMono2Stereo()
{
	m_pBuffer	= NULL;
	m_nBuffSize	= 0;
}

CMono2Stereo::~CMono2Stereo()
{
	if(m_pBuffer)
		delete[] m_pBuffer;
}

int CMono2Stereo::Read(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	assert(fmt->bitsPerSample==16 && fmt->channels==1);
	if(fmt->bitsPerSample!=16 && fmt->channels!=1)
		return 0;

	if(size > m_nBuffSize)
	{
		if(m_pBuffer)
			delete[] m_pBuffer;
		m_nBuffSize = size;
		m_pBuffer = new BYTE[m_nBuffSize];
	}

	GetEngine()->EmptyBuffer(m_pBuffer, m_nBuffSize);

	int len = IFloopySoundFilter::Read(m_pBuffer, size/2);

	if(len <= 0)
		return len; // EOF

	SAMPLE *mono	= (SAMPLE*)m_pBuffer;
	SAMPLE *stereo	= (SAMPLE*)data;
	int samples = len / sizeof(SAMPLE);

	for(int i=0; i<samples; i++)
	{
		*(stereo++) = *mono;
		*(stereo++) = *(mono++);
	}

	return len * 2;
}

int CMono2Stereo::Read2(BYTE **data, int channels, int samples)
{
	return 0;
}

SOUNDFORMAT *CMono2Stereo::GetFormat()
{
	memcpy(&m_format, IFloopySoundFilter::GetFormat(), sizeof(SOUNDFORMAT));
	m_format.channels = 2;
	return &m_format;
}
