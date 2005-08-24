// ToneGen.cpp: implementation of the CToneGen class.
//
//////////////////////////////////////////////////////////////////////


#include <math.h>
#include "tonegen.h"


CToneGen::CToneGen() : IFloopySoundInput()
{
	m_format.format = WAVE_FORMAT_PCM;

	int ch  = m_format.channels			= 2;
	int fr  = m_format.frequency		= 44100;
	int bps = m_format.bitsPerSample	= 16;

	m_freq = m_step = m_angle = 0.0;
	m_max  = (pow(2, bps) - 1.0) / 2.0;	// Maximum sample value

	m_BytesToSamples = (bps / 8) * ch;
}

CToneGen::~CToneGen()
{

}

int CToneGen::Read(BYTE *data, int size)
{
	short int *sample_buffer = (short*)data;
	int numsamples = size / m_BytesToSamples;
	int x = numsamples * m_format.channels;
	short int *s = sample_buffer;

	while (x--)
	{
		int i;
		double d = sin(m_angle) * m_max;
		__asm {
			fld d
			fistp i
		}
		*(s++) = i;
		m_angle += m_step;
	}

	return size;
}

BOOL CToneGen::GetParamVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)m_freq;
		return TRUE;
	}
	return FALSE;
}

void CToneGen::SetParamVal(int index, float value)
{
	if(index == 0)
	{
		m_freq = value;

		double ch = (double)m_format.channels;
		double fr = (double)m_format.frequency;
		m_step = M_PI * ch * m_freq / fr / 2.0;
	}
}
