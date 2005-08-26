// Volume.cpp: implementation of the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "volume.h"
#include <stdio.h>
#include <math.h>
//#include <assert.h>

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
	if(!fmt || !fmt->bitsPerSample || !fmt->channels)
		return 0;

	int maxSample = (int)pow(2, fmt->bitsPerSample) / 2 - 1;

	// Only 16 bit samples supported right now
	if(fmt->bitsPerSample == 16)
	{
		int numsamples = len / (fmt->bitsPerSample/8);

		float lpercent = (float)m_left / 100.f;
		float rpercent = (float)m_right / 100.f;

		short int *sample = (short int*)data;

		while(numsamples -= fmt->channels)
		{
			for(int ch=0; ch<fmt->channels; ch++)
			{
				float percent = (ch == 0 ? lpercent : rpercent);
				if(percent != 1.f && *sample != 0)
				{
					int tmp = (int)((float)*sample * percent);
					//int tmp = (int)floor((float)*sample * percent);
					//int tmp = (int)ceil((float)*sample * percent);
					
					if(tmp > maxSample || tmp < -maxSample)
						tmp = (short int)(*sample > 0 ? +1 : -1) * maxSample;
					
					/*if(tmp > maxSample)
						tmp = maxSample;
					else if(tmp < -maxSample)
						tmp = -maxSample;*/
					
					*sample = (short int)tmp;
				}
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
	switch(index)
	{
	case 0:
		*value = (float)(m_left > m_right ? m_left : m_right);
		return TRUE;
	case 1:
		*value = (float)m_left;
		return TRUE;
	case 2:
		*value = (float)(getChannels() == 2 ? m_right : m_left);
		return TRUE;
	}
	return FALSE;
}

void CVolume::SetParamVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_left = m_right = (int)value;
		break;
	case 1:
		m_left = (int)value;
		break;
	case 2:
		(getChannels() == 2 ? m_right : m_left) = (int)value;
		break;
	default:
		m_left = m_right = (int)value;
		break;
	}
}

char *CVolume::GetParamName(int index)
{
	switch(index)
	{
	case 0:
		return (getChannels() == 2 ? "both" : "volume");
	case 1:
		return (getChannels() == 2 ? "left" : "volume");
	case 2:
		return (getChannels() == 2 ? "right" : "volume");
	}
	return NULL;
}

char *CVolume::GetParamDesc(int index)
{
	switch(index)
	{
	case 0:
		return "Volume";
	case 1:
		return (getChannels() == 2 ? "Left volume" : "Volume");
	case 2:
		return (getChannels() == 2 ? "Right volume" : "Volume");
	}
	return NULL;
}

int CVolume::getChannels()
{
	SOUNDFORMAT *fmt = GetFormat();
	if(fmt)
		return fmt->channels;
	else
		return 0;
}
