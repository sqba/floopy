// sndtch.cpp: implementation of the CSoundTouch class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>

#include "SoundTouch.h"
#include "BPMDetect.h"


//#define VARIABLE_SIZE	/// If this is defined then the cache must be used!

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundTouch::CSoundTouch()
{
	m_fTempo		= 0.f;	// percents
	m_fTempoDelta	= 0.f;	// percents
	m_fPitch		= 0.f;	// percents
	m_fRate			= 0.f;	// percents
	m_bQuickSeek	= false;
	m_bUseAAFilter	= true;
	m_fNewBPM		= 0.f;
	m_fOrigBPM		= 0.f;	// Original BPM
	m_bFinished		= false;
	m_iSize			= 0;
	m_fScale		= 1.f;
	m_iSamplesToBytes = 0;
}

CSoundTouch::~CSoundTouch()
{

}

bool CSoundTouch::SetSource(IFloopySoundInput *src)
{
	if( IFloopySoundFilter::SetSource(src) )
	{
		SOUNDFORMAT *fmt = GetFormat();
		if(NULL != fmt)
		{
			m_SoundTouch.setSampleRate(fmt->frequency);
			m_SoundTouch.setChannels(fmt->channels);

			m_iSamplesToBytes = ((fmt->bitsPerSample/8) * fmt->channels);

			detectBPM();
		}
		getNewSize();
	}
	return false;
}

int CSoundTouch::Read(BYTE *data, int size)
{
	if(m_fTempoDelta==0.f && m_fPitch==0.f && m_fRate==0.f)
		return IFloopySoundFilter::Read(data, size);

	SOUNDFORMAT *fmt = GetFormat();
	if(fmt->bitsPerSample==0 || fmt->channels==0)
		return EOF;

	int nBytes = 0;

	int len = 0;
	int nSamples = 0;

	BYTE *buffer = new BYTE[size];

	int bytesPerSample = (fmt->bitsPerSample / 8) * fmt->channels;

	if( !m_bFinished )
	{
		do {
			len = IFloopySoundFilter::Read(buffer, size);

			if(EOF != len)
			{
				nSamples = len / bytesPerSample;
				m_SoundTouch.putSamples((const short*)buffer, nSamples);

				do {
					nSamples = (size-nBytes) / bytesPerSample;
					nSamples = m_SoundTouch.receiveSamples((short*)(data+nBytes), nSamples);
					nBytes += nSamples * bytesPerSample;
				} while (nSamples!=0 && nBytes<size);
			}
			else
			{
				m_SoundTouch.flush();
				m_bFinished = true;
			}
			
		} while (len>0 && nBytes<size);
	}

	if(m_bFinished && nBytes<size)
	{
		do {
			nSamples = (size-nBytes) / bytesPerSample;
			nSamples = m_SoundTouch.receiveSamples((short*)(data+nBytes), nSamples);
			nBytes += nSamples * bytesPerSample;
		} while (nSamples!=0 && nBytes<size);
	}

	if(m_bFinished && nBytes==0)
		nBytes = EOF;

	delete buffer;

	return nBytes;
}


void CSoundTouch::MoveTo(int samples)
{
	m_SoundTouch.clear();
	m_bFinished = false;

#ifndef VARIABLE_SIZE
//	if(m_fTempoDelta != 0.f)
//		samples = (int)((float)samples / (1.f - m_fTempoDelta/100.f));
	samples = (int)ceil((float)samples / m_fScale);
#endif

	IFloopySoundFilter::MoveTo(samples);
}

int CSoundTouch::GetSize()
{

#ifdef VARIABLE_SIZE
	if(m_fTempoDelta==0.f && m_fPitch==0.f && m_fRate==0.f)
		return IFloopySoundFilter::GetSize();
	else
		return SIZE_VARIABLE;
#else
//	if(m_fTempoDelta != 0.f)
//		return (int)ceil((float)size - (float)size*m_fTempoDelta/100.f);
//	else
//		return IFloopySoundFilter::GetSize();
	return m_iSize;
#endif
}

void CSoundTouch::Reset()
{
	m_SoundTouch.clear();
	IFloopySoundFilter::Reset();
	m_bFinished = false;
}

void CSoundTouch::Close()
{
	IFloopySoundFilter::Close();
}

void CSoundTouch::detectBPM()
{
	if(NULL == m_source)
		return;

	BPMDetect bpm;

	m_fOrigBPM = bpm.detectBpm( m_source );
	m_source->MoveTo( 0 );

	if (m_fNewBPM>0.f && m_fOrigBPM>0.f)
	{
		// adjust tempo to given bpm
		m_fTempoDelta = (m_fNewBPM / m_fOrigBPM - 1.0f) * 100.0f;
		m_SoundTouch.setTempoChange( m_fTempoDelta );
	}
}

void CSoundTouch::getNewSize()
{
	m_iSize = 0;
	m_fScale = 1;
	if(m_fTempoDelta!=0.f && m_iSamplesToBytes!=0.f)
	{
		int len;
		BYTE buff[512];
		while(EOF != (len=Read(buff, 512)))
		{
			m_iSize += len;
		}
		if(m_iSize > 0)
		{
			m_iSize /= m_iSamplesToBytes;
			int origSize = IFloopySoundFilter::GetSize();
			if(origSize > 0)
				m_fScale = (float)m_iSize / (float)origSize;
		}
	}
	else
		m_iSize = IFloopySoundFilter::GetSize();
}




int CSoundTouch::GetPropertyCount()
{
	return 7;
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
	case 5:
		*value = m_fNewBPM;
		return true;
	case 6:
		*value = m_fOrigBPM;
		return true;
	}
	return false;
}

void CSoundTouch::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_fTempoDelta = value;
		m_fTempo = value;
		m_SoundTouch.setTempoChange(m_fTempoDelta);
		getNewSize();
		break;
	case 1:
		m_fPitch = value;
		m_SoundTouch.setPitchSemiTones(m_fPitch);
		break;
	case 2:
		m_fRate = value;
		m_SoundTouch.setRateChange(m_fRate);
		getNewSize();
		break;
	case 3:
		m_bQuickSeek = (value == 0.f);
		m_SoundTouch.setSetting(SETTING_USE_QUICKSEEK, m_bQuickSeek);
		break;
	case 4:
		m_bUseAAFilter = (value == 0.f);
		m_SoundTouch.setSetting(SETTING_USE_AA_FILTER, m_bUseAAFilter);
		break;
	case 5:
		m_fNewBPM = value;
		detectBPM();
		getNewSize();
		break;
	}
}

float CSoundTouch::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0: return -95.f;
	case 1: return -60.f;
	case 2: return -95.f;
	case 3: return 0.f;
	case 4: return 0.f;
	case 5: return 0.f; // 70.f
	case 6: return 0.f; // 70.f
	}
	return 0.f;
}

float CSoundTouch::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0: return 5000.f;
	case 1: return 60;
	case 2: return 5000.f;
	case 3: return 1.f;
	case 4: return 1.f;
	case 5: return 200.f;
	case 6: return 200.f;
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
	case 5:
		return 0.1f;
	case 3:
	case 4:
		return 1.f;
	case 6:
		return 0.f;
	}
	return 0.f;
}

char *CSoundTouch::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "Tempo";
	case 1: return "Pitch";
	case 2: return "Rate";
	case 3: return "Quick";
	case 4: return "AntiAliasing";
	case 5: return "BPM";
	case 6: return "SrcBPM";
	}
	return NULL;
}

char *CSoundTouch::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Tempo";
	case 1: return "Pitch";
	case 2: return "Rate";
	case 3: return "Quick tempo change algorithm";
	case 4: return "Use anti alias filtering";
	case 5: return "Beats Per Minute";
	case 6: return "Source Tempo";
	}
	return NULL;
}

//char *GetPropertyUnit(int index);
