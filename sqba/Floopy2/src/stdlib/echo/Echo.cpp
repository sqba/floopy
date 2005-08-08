// Echo.cpp: implementation of the CEcho class.
//
//////////////////////////////////////////////////////////////////////

#include "echo.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEcho::CEcho()
{

}

CEcho::~CEcho()
{

}

int CEcho::Read(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = m_source->GetFormat();

	int bufflen = sizeof(echo_buf) / sizeof(short);

	int result = 0;

	if(size < bufflen)
		return read(data, size);
	else
	{
		int n = size / bufflen;
		BYTE *pdata = data;
		int stb = (fmt->bitsPerSample / 8) * fmt->channels;
		for(int i=0; i<n; i++)
		{
			int len = read(pdata, bufflen);
			if(len == EOF)
				return result;
			pdata += len;
			result += len;
		}
		i = size - bufflen * n;
		if(i>0)
		{
			int len = read(pdata, i);
			if(len == EOF)
				result += len;
		}
		return result;
	}
}

void CEcho::Reset()
{
	memset(echo_buf, 0, sizeof(echo_buf));
	memset(echo_buf2, 0, sizeof(echo_buf));
	IFloopySoundFilter::Reset();
}

int CEcho::read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);
	if(len <= 0)
		return len;

	SOUNDFORMAT *fmt = m_source->GetFormat();
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

/*
BOOL CEcho::SetSource(IFloopySoundInput *src)
{
//	SOUNDFORMAT *fmt = src->GetFormat();
//	assert( fmt->bitsPerSample == 16 );
	return IFloopySoundFilter::SetSource(src);
}
*/