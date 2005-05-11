// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	freq = 2600.0f;
	angle = 0.0;
	memset(name, 0, sizeof(name));
	sprintf(name, "%.2f.hz", freq);
	m_wavformat.channels = 2;
	m_wavformat.frequency = 44100;
	m_wavformat.bitsPerSample = 16;
}

CInput::~CInput()
{

}

int CInput::Read(BYTE *data, int size)
{
	short int *sample_buffer = (short*)data;

	WAVFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int numsamples = size / ((fmt->bitsPerSample/8) * fmt->channels);

	int l = numsamples, x=l*fmt->channels;
	double dangle = M_PI * (double)fmt->channels * freq / (double)fmt->frequency / 2.0;

	double max = (pow(2, fmt->bitsPerSample) - 1.0) / 2.0;

	short int *s = sample_buffer;
	while (x--)
	{
		int i;
		double d = sin(angle)*max;
		__asm {
			fld d
			fistp i
		}
		*(s++) = i;
		angle += dangle;
	}

	return size;
}

int CInput::atoi(char *s)
{
	int n=0;
	while (s != NULL && *s >= '0' && *s <= '9')
	{
		n*=10;
		n += *s++ -'0';
	}
	return n;
}
/*
char *CInput::GetName()
{
	memset(name, 0, sizeof(name));
	sprintf(name, "%.2f.hz", freq);
	return name;
}
*/