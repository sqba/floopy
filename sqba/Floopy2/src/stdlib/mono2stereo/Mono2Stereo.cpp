// Mono2Stereo.cpp: implementation of the CMono2Stereo class.
//
//////////////////////////////////////////////////////////////////////

#include "mono2stereo.h"
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMono2Stereo::CMono2Stereo()
{

}

CMono2Stereo::~CMono2Stereo()
{

}

int CMono2Stereo::Read(BYTE *data, int size)
{
	//return IFloopySoundFilter::Read(data, size);

	BYTE *tmp = new BYTE[size/2];
	memset(tmp, 0, size/2);

	int len = IFloopySoundFilter::Read(tmp, size/2);

	if(EOF == len)
		return len;

	short int *mono = (short int*)tmp;
	short int *stereo = (short int*)data;
	int samples = size/2 / sizeof(short int);

	for(int i=0; i<samples; i++)
	{
		*(stereo++) = *mono;
		*(stereo++) = *(mono++);
	}

	delete[] tmp;

//	assert(len == size/2);

	return len * 2;
}

SOUNDFORMAT *CMono2Stereo::GetFormat()
{
	memcpy(&m_format, IFloopySoundFilter::GetFormat(), sizeof(SOUNDFORMAT));
	m_format.channels = 2;
	return &m_format;
}

/*
void CMono2Stereo::MoveTo(int samples)
{
	IFloopySoundFilter::MoveTo(samples/2);
}

int CMono2Stereo::GetSize()
{
	return IFloopySoundFilter::GetSize() * 2;
}
*/
/*
int CMono2Stereo::Read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);
	for(int i=0; i<len-1; i++)
	{
		*(++data) = *(++data);
	}
	return len;
}

int CMono2Stereo::Read(BYTE *data, int size)
{
	BYTE *tmp = new BYTE[size*2];
	memset(tmp, 0, size*2);

	int len = IFloopySoundFilter::Read(tmp, size*2);

	BYTE *pTmp = tmp;
	BYTE *pData = data;
	for(int i=0; i<len/2; i++)
	{
		*pData = *pTmp;
		*(++pData) = *pTmp;
		pData++;
		pTmp+=2;
	}

	delete[] tmp;

	return len / 2;
}
*/