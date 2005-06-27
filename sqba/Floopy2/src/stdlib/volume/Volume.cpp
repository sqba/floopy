// Volume.cpp: implementation of the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "volume.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVolume::CVolume()
{
	m_volume = 100;
}

CVolume::~CVolume()
{

}

int CVolume::Read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);
	if(EOF == len)
		return len;

	SOUNDFORMAT *fmt = GetFormat();
	if((NULL == fmt) || (fmt->bitsPerSample == 0) || (fmt->channels == 0))
		return 0;


	int numsamples = len / (fmt->bitsPerSample/8);

	float percent = (float)m_volume / 100.f;

	// Only 16bit samples supported right now
	if(fmt->bitsPerSample == 16)
	{
		short int *sample = (short int*)data;

		while(numsamples--)
		{
			*sample = ((float)*sample * percent);
			sample++;
		}
		return len;
	}
	else
		return 0;
}

BOOL CVolume::GetParamVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)m_volume;
		return TRUE;
	}
	return FALSE;
}
