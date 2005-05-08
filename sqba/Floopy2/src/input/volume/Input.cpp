// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "Input.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	m_volume = 100;
}

CInput::~CInput()
{

}

int CInput::Read(BYTE *data, int size)
{
	int len = IFloopySoundInput::Read(data, size);

	short int *sample = (short int*)data;

	WAVFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	//int numsamples = size / ((fmt->size/8) * fmt->channels);
	int numsamples = len / (fmt->bitsPerSample/8);
	//int numsamples = len / ((fmt->size / 8) * GetFormat()->channels);

//	int max = (int)pow(2, fmt->size) / 2;

	float percent = (float)m_volume / 100.f;
//	float coef = (float)max * (float)m_volume / 100.f;

	int i = numsamples;
	while(i--)
	{
		//*(sample++) += ((float)*sample * coef);
		//*(sample++) += coef;
		//*(sample++) += ((float)*sample * (float)m_volume / 100.f);
		//if(*sample < 100 || *sample > -100)
		//	*sample = *sample;
		*(sample++) = ((float)*sample * percent);

		/*short int s = (short int)((float)*sample * m_volume);
		if((s > -max) && (s < max))
			*sample = s;
		sample++;*/
	}
	return len;
}
