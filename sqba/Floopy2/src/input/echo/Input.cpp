// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{

}

CInput::~CInput()
{

}


UINT CInput::Read(BYTE *data, UINT size, UINT offset)
{
	WAVFORMAT *fmt = m_source->GetFormat();
	UINT len = IFloopySoundInput::Read(data, size, offset);
	if(len <= 0)
		return len;
	UINT numsamples = len / ((fmt->size/8) * fmt->channels);

	// echo doesn't support 8 bit right now cause I'm lazy.
	if (fmt->size==16)
	{
		short int *samples = (short int*)data;
		int max = (int)pow(2, fmt->size) / 2; // 32768
		int x,s;
		s = numsamples*fmt->channels;
		
		memcpy(echo_buf2,       echo_buf,       s*2);
		memcpy(echo_buf,        echo_buf+s,     s*2);
		memcpy(echo_buf+s,      echo_buf+s*2,	s*2);
		memcpy(echo_buf+s*2,	echo_buf+s*3,	s*2);
		memcpy(echo_buf+s*3,	samples,		s*2);
		
		for (x = 0; x < s; x ++)
		{
			int s = samples[x]/2 + echo_buf2[x]/2;
			samples[x] = (s > max ? max : (s < -max ? -max : s));
		}
	}

	return len;
}
