// Volume.cpp: implementation of the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#include "volume.h"
#include <stdio.h>
#include <math.h>
//#include <assert.h>


#define LEFT	0
#define RIGHT	1


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVolume::CVolume()
{
	m_master[LEFT] = m_master[RIGHT] = 100;
	m_volume[LEFT] = m_volume[RIGHT] = 100;
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

	if( m_master[LEFT]==100 && m_master[RIGHT]==100 &&
		m_volume[LEFT]==100 && m_volume[RIGHT]==100 )
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

		float lpercentmast = (float)m_master[LEFT]	/ 100.f;
		float rpercentmast = (float)m_master[RIGHT]	/ 100.f;

		if(m_volume[LEFT] != 100 || m_volume[RIGHT] != 100)
		{
			float lpercent = (float)m_volume[LEFT]	/ 100.f;
			float rpercent = (float)m_volume[RIGHT] / 100.f;

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

void CVolume::Reset()
{
	m_master[LEFT] = m_master[RIGHT] = 100;
	m_volume[LEFT] = m_volume[RIGHT] = 100;
}

int CVolume::GetParamCount()
{
	if(NULL == GetSource())
		return 3;
	else
		return getChannels() + 1;
}

bool CVolume::GetParamVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		if(m_volume[LEFT] == m_volume[RIGHT])
		{
			*value = (float)m_volume[LEFT];
			return true;
		}
	case 1:
		*value = (float)m_volume[LEFT];
		return true;
	case 2:
		*value = (float)m_volume[RIGHT];
		return true;
	}
	return false;
}

// Problem: kada se jednom promeni vrednost lednog kanala posebno,
// ta vrednost se upisuje u tajmlajn i vishe nije moguce postaviti
// vrednost za oba kanala zajedno (case 0).
void CVolume::SetParamVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_volume[LEFT] = m_volume[RIGHT] = (int)value;
		break;
	case 1:
		m_volume[LEFT] = (int)value;
		break;
	case 2:
		m_volume[RIGHT] = (int)value;
		break;
	}
}

const char *CVolume::GetParamName(int index)
{
	switch(index)
	{
	case 0: return "volume";
	case 1: return getChannels() > 1 ? "left" : "Volume";
	case 2: return "right";
	}
	return NULL;
}

const char *CVolume::GetParamDesc(int index)
{
	switch(index)
	{
	case 0: return "Volume";
	case 1: return getChannels() > 1 ? "Left volume" : "Volume";
	case 2: return "Right volume";
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


int CVolume::GetPropertyCount()
{
	return 1;
}

bool CVolume::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)m_master[LEFT];
		return true;
	case 1:
		*value = (float)m_master[RIGHT];
		return true;
	}
	return false;
}

void CVolume::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_master[LEFT] = (int)value;
		break;
	case 1:
		m_master[RIGHT] = (int)value;
		break;
	}
}

const char *CVolume::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "Master";
	case 1: return "MasterLeft";
	case 2: return "MasterRight";
	}
	return NULL;
}

const char *CVolume::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Master volume";
	case 1: return "Master  volume left";
	case 2: return "Master  volume right";
	}
	return NULL;
}
