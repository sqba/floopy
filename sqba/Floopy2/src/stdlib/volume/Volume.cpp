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
	m_masterLeft = m_masterRight = m_left = m_right = 100;
}

CVolume::~CVolume()
{

}

int CVolume::Read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);
	//if(EOF == len)
	if(len <= 0)
		return len;

	if(m_masterLeft==100 && m_masterRight==100 && m_left==100 && m_right==100)
		return len;

//	if(m_left == 100 && m_right == 100)
//		return len;

	SOUNDFORMAT *fmt = GetFormat();
	if(!fmt || !fmt->bitsPerSample || !fmt->channels)
		return 0;

	int maxSample = (int)pow(2, fmt->bitsPerSample) / 2 - 1;

	// Only 16 bit samples supported right now
	if(fmt->bitsPerSample == 16)
	{
		int numsamples = len / (fmt->bitsPerSample/8);

		float lpercentmast = (float)m_masterLeft / 100.f;
		float rpercentmast = (float)m_masterRight / 100.f;

		if(m_left != 100 || m_right != 100)
		{
			float lpercent = (float)m_left / 100.f;
			float rpercent = (float)m_right / 100.f;

			lpercentmast *= lpercent;
			rpercentmast *= rpercent;
		}

		short int *sample = (short int*)data;

		do {
			for(int ch=0; ch<fmt->channels; ch++)
			{
				float percent = (ch == 0 ? lpercentmast : rpercentmast);
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
		} while(numsamples -= fmt->channels);
		return len;
	}
	else
		return 0;
}

bool CVolume::GetParamVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)(m_left > m_right ? m_left : m_right);
		return true;
	case 1:
		*value = (float)m_left;
		return true;
	case 2:
		*value = (float)(getChannels() == 2 ? m_right : m_left);
		return true;
	}
	return false;
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

int CVolume::GetParamCount()
{
	//return getChannels() + 1;
	return getChannels()==2 ? 3 : 1;
}

int CVolume::GetPropertyCount()
{
	return 1;
}

bool CVolume::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)(m_masterLeft > m_masterRight ? m_masterLeft : m_masterRight);
		return true;
	case 1:
		*value = (float)m_masterLeft;
		return true;
	case 2:
		*value = (float)(getChannels() == 2 ? m_masterRight : m_masterLeft);
		return true;
	}
	return false;
}

void CVolume::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_masterLeft = m_masterRight = (int)value;
		break;
	case 1:
		m_masterLeft = (int)value;
		break;
	case 2:
		(getChannels() == 2 ? m_masterRight : m_masterLeft) = (int)value;
		break;
	default:
		m_masterLeft = m_masterRight = (int)value;
		break;
	}
}

char *CVolume::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "Master";
	case 1: return "MasterLeft";
	case 2: return "MasterRight";
	}
	return NULL;
}

char *CVolume::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Master volume";
	case 1: return "Master  volume left";
	case 2: return "Master  volume right";
	}
	return NULL;
}
