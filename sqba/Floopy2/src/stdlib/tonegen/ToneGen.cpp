// ToneGen.cpp: implementation of the CToneGen class.
//
//////////////////////////////////////////////////////////////////////

#include "tonegen.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToneGen::CToneGen()
{
	freq = 2600.0f;
	angle = 0.0;
	memset(name, 0, sizeof(name));
	sprintf(name, "%.2f.hz", freq);
	m_format.channels = 2;
	m_format.frequency = 44100;
	m_format.bitsPerSample = 16;
}

CToneGen::~CToneGen()
{

}

int CToneGen::Read(BYTE *data, int size)
{
	short int *sample_buffer = (short*)data;

	SOUNDFORMAT *fmt = GetFormat();
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

int CToneGen::atoi(char *s)
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
char *CToneGen::GetName()
{
	memset(name, 0, sizeof(name));
	sprintf(name, "%.2f.hz", freq);
	return name;
}
*/

BOOL CToneGen::GetParamVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)freq;
		return TRUE;
	}
	return FALSE;
}
