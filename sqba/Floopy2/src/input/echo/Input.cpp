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

}

CInput::~CInput()
{

}

int CInput::Read(BYTE *data, int size)
{
	int len = IFloopySoundInput::Read(data, size);
	if(len <= 0)
		return len;

	WAVFORMAT *fmt = m_source->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int numsamples = len / ((fmt->bitsPerSample/8) * fmt->channels);

	// echo doesn't support 8 bit right now cause I'm lazy.
	if (fmt->bitsPerSample==16)
	{
		short int *samples = (short int*)data;
		int max = (int)pow(2, fmt->bitsPerSample) / 2; // 32768
		int x,s;
		s = numsamples * fmt->channels;
		
		memcpy(echo_buf2,       echo_buf,       s*2);
		memcpy(echo_buf,        echo_buf+s,     s*2);
		memcpy(echo_buf+s,      echo_buf+s*2,	s*2);
		memcpy(echo_buf+s*2,	echo_buf+s*3,	s*2);
		memcpy(echo_buf+s*3,	samples,		s*2);
		
		for(x=0; x<s; x++)
		{
			int s = samples[x]/2 + echo_buf2[x]/2;
			samples[x] = (s > max ? max : (s < -max ? -max : s));
		}
	}

	return len;
}

void CInput::Reset()
{
	memset(echo_buf, 0, sizeof(echo_buf));
	memset(echo_buf2, 0, sizeof(echo_buf));
	IFloopySoundInput::Reset();
}
/*
BOOL CInput::SetSource(IFloopySoundInput *src)
{
//	WAVFORMAT *fmt = src->GetFormat();
//	assert( fmt->bitsPerSample == 16 );
	return IFloopySoundInput::SetSource(src);
}
*/