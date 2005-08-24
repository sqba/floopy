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
	m_left = m_right = 100;
}

CVolume::~CVolume()
{

}

int CVolume::Read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);
	if(EOF == len)
		return len;

	if(m_left == 100 && m_right == 100)
		return len;

	SOUNDFORMAT *fmt = GetFormat();
	if((NULL == fmt) || (fmt->bitsPerSample == 0) || (fmt->channels == 0))
		return 0;


	// Only 16bit samples supported right now
	if(fmt->bitsPerSample == 16)
	{
		int numsamples = len / (fmt->bitsPerSample/8);

		float lpercent = (float)m_left / 100.f;
		float rpercent = (float)m_right / 100.f;

		short int *sample = (short int*)data;

		while(numsamples -= fmt->channels)
		{
			//*sample = (short int)((float)*sample * percent);
			//sample++;
			for(int ch=0; ch<fmt->channels; ch++)
			{
				float percent = (ch == 0 ? lpercent : rpercent);
				*sample = (short int)((float)*sample * percent);
				sample++;
			}
		}
		return len;
	}
	else
		return 0;
}

BOOL CVolume::GetParamVal(int index, float *value)
{
	SOUNDFORMAT *fmt = GetFormat();

	switch(index)
	{
	case 0:
		*value = (float)(m_left > m_right ? m_left : m_right);
		return TRUE;
	case 1:
		*value = (float)m_left;
		return TRUE;
	case 2:
		*value = (float)(fmt->channels == 2 ? m_right : m_left);
		return TRUE;
	}
	return FALSE;
}

void CVolume::SetParamVal(int index, float value)
{
	SOUNDFORMAT *fmt = GetFormat();

	switch(index)
	{
	case 0:
		m_left = m_right = (int)value;
		break;
	case 1:
		m_left = (int)value;
		break;
	case 2:
		(fmt->channels == 2 ? m_right : m_left) = (int)value;
		break;
	}
}

char *CVolume::GetParamName(int index)
{
	SOUNDFORMAT *fmt = GetFormat();

	switch(index)
	{
	case 0:
		return (fmt->channels == 2 ? "both" : "volume");
	case 1:
		return (fmt->channels == 2 ? "left" : "volume");
	case 2:
		return (fmt->channels == 2 ? "right" : "volume");
	}
	return NULL;
}

char *CVolume::GetParamDesc(int index)
{
	SOUNDFORMAT *fmt = GetFormat();

	switch(index)
	{
	case 0:
		return "Volume";
	case 1:
		return (fmt->channels == 2 ? "Left volume" : "Volume");
	case 2:
		return (fmt->channels == 2 ? "Right volume" : "Volume");
	}
	return NULL;
}

char *CVolume::GetParamUnit(int index)
{
	return "%";
}
