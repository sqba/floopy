// sndtch.cpp: implementation of the CSoundTouch class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>

#include "sndtch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundTouch::CSoundTouch()
{
	m_fTempo	= 0.f;
	m_fPitch	= -5.f;
	m_fRate		= 0.f;
	m_bQuickSeek	= false;
	m_bUseAAFilter	= true;
}

CSoundTouch::~CSoundTouch()
{

}

bool CSoundTouch::SetSource(IFloopySoundInput *src)
{
	if( IFloopySoundFilter::SetSource(src) )
	{
		SOUNDFORMAT *fmt = GetFormat();


		m_SoundTouch.setSampleRate(fmt->frequency);
		m_SoundTouch.setChannels(fmt->channels);

		m_SoundTouch.setTempoChange(m_fTempo);
		m_SoundTouch.setPitchSemiTones(m_fPitch);
		m_SoundTouch.setRateChange(m_fRate);

		m_SoundTouch.setSetting(SETTING_USE_QUICKSEEK, m_bQuickSeek);
		m_SoundTouch.setSetting(SETTING_USE_AA_FILTER, m_bUseAAFilter);
	}
	return false;
}

int CSoundTouch::Read(BYTE *data, int size)
{
	int nBytes = 0;

	BYTE *buffer = new BYTE[size];

	int len = 0;
	int nSamples = 0;

	SOUNDFORMAT *fmt = GetFormat();
	int bytesPerSample = fmt->bitsPerSample / 8;

	do {
		len = IFloopySoundFilter::Read(buffer, size);

		if(len >0)
		{
			nSamples = len / bytesPerSample;
			m_SoundTouch.putSamples((const short*)buffer, nSamples);
		}

		do {
			nSamples = (size-nBytes) / bytesPerSample;
			nSamples = m_SoundTouch.receiveSamples((short*)(data+nBytes), nSamples);
			nBytes += nSamples * bytesPerSample;
		} while (nSamples!=0 && nBytes<size);
		
	} while (nSamples>0 && nBytes<size);

	if(len<size && nBytes<size)
	{
		//m_SoundTouch.flush();
		do {
			nSamples = (size-nBytes) / bytesPerSample;
			nSamples = m_SoundTouch.receiveSamples((short*)(data+nBytes), nSamples);
			nBytes += nSamples * bytesPerSample;
		} while (nSamples!=0 && nBytes<size);
	}

	if(len!=size && nBytes==0)
		nBytes = EOF;

	delete buffer;

	return nBytes;
}


void CSoundTouch::MoveTo(int samples)
{
	IFloopySoundFilter::MoveTo(samples);
}

void CSoundTouch::Reset()
{
	IFloopySoundFilter::Reset();
}

void CSoundTouch::Close()
{
	IFloopySoundFilter::Close();
}

int CSoundTouch::GetSize()
{
	return SIZE_VARIABLE;
/*
	float size = (float)IFloopySoundFilter::GetSize();

//	float f = fabs(m_fTempo)>fabs(m_fRate) ? m_fTempo : m_fRate;
//	int result = (int)floor((float)(size - ((size * f*2.f) / 100.f)));
//	return result;

	float f = m_fTempo*2.f + m_fPitch*2.f + m_fRate*2.f;
	int result = (int)floor((float)(size - ((size * f) / 100.f)));
	return result;
*/
}


int CSoundTouch::GetPropertyCount()
{
	return 5;
}

//bool  GetPropertyIndex(char *name, int *index);

bool CSoundTouch::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = m_fTempo;
		return true;
	case 1:
		*value = m_fPitch;
		return true;
	case 2:
		*value = m_fRate;
		return true;
	case 3:
		*value = (float)m_bQuickSeek;
		return true;
	case 4:
		*value = (float)m_bUseAAFilter;
		return true;
	}
	return false;
}

void CSoundTouch::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_fTempo = value;
		break;
	case 1:
		m_fPitch = value;
		break;
	case 2:
		m_fRate = value;
		break;
	case 3:
		m_bQuickSeek = (value == 0.f);
		break;
	case 4:
		m_bUseAAFilter = (value == 0.f);
		break;
	}
}

float CSoundTouch::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0:
		return -95.f;
	case 1:
		return -60.f;
	case 2:
		return -95.f;
	case 3:
		return 0.f;
	case 4:
		return 0.f;
	}
	return 0.f;
}

float CSoundTouch::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0:
		return 5000.f;
	case 1:
		return 60;
	case 2:
		return 5000.f;
	case 3:
		return 1.f;
	case 4:
		return 1.f;
	}
	return 0.f;
}

float CSoundTouch::GetPropertyStep(int index)
{
	switch(index)
	{
	case 0:
	case 1:
	case 2:
		return 0.1f;
	case 3:
	case 4:
		return 1.f;
	}
	return 0.f;
}

char *CSoundTouch::GetPropertyName(int index)
{
	switch(index)
	{
	case 0:
		return "Tempo";
	case 1:
		return "Pitch";
	case 2:
		return "Rate";
	case 3:
		return "Quick";
	case 4:
		return "AntiAliasing";
	}
	return NULL;
}

char *CSoundTouch::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0:
		return "Tempo";
	case 1:
		return "Pitch";
	case 2:
		return "Rate";
	case 3:
		return "Quick tempo change algorithm";
	case 4:
		return "Use anti alias filtering";
	}
	return NULL;
}

//char *GetPropertyUnit(int index);
