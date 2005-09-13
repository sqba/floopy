// Mixer.cpp: implementation of the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "mixer.h"
#include <math.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMixer::CMixer()
{
	m_nInputCount = 0;

	m_nBuffSize = 0;
	m_pBuffers = NULL;

	m_nLengths = NULL;
	m_nLengthsSize = 0;

	for(int i=0; i<MAX_INPUTS; i++)
	{
		m_pInputs[i] = NULL;
	}

#ifdef _DEBUG_TIMER_
	m_bDebugTimer = true;
	m_nFrameSize=m_dwSpeed=m_nFrameCount=0;
#endif // _DEBUG_TIMER_
}

CMixer::~CMixer()
{
	for(int i=0; i<m_nInputCount; i++)
	{
		if(m_pInputs[i])
			delete m_pInputs[i];
	}

	if(NULL != m_pBuffers)
		delete[] m_pBuffers;

	if(NULL != m_nLengths)
		delete[] m_nLengths;
}

int CMixer::AddSource(IFloopySoundInput *src)
{
	if(m_nInputCount < MAX_INPUTS)
	{
		if(0==m_nInputCount)
			m_source = src;
		//m_pInputs[m_nInputCount++] = src;
		for(int i=0; i<MAX_INPUTS; i++)
		{
			if(NULL == m_pInputs[i])
			{
				m_pInputs[i] = src;
				m_nInputCount++;
				return m_nInputCount;
			}
		}
	}
	return -1;
}

bool CMixer::RemoveSource(IFloopySoundInput *src)
{
	for(int i=0; i<m_nInputCount; i++)
	{
		if(src == m_pInputs[i])
		{
			m_pInputs[i] = NULL;
			m_nInputCount--;
			return true;
		}
	}
	return false;
}

IFloopySoundInput *CMixer::GetSource(int index)
{
	if(index < m_nInputCount)
		return m_pInputs[index];
	return NULL;
}

int CMixer::GetInputCount()
{
	return m_nInputCount;
}

int CMixer::Read(BYTE *data, int size)
{
	if(m_nLengthsSize != m_nInputCount)
	{
		if(NULL != m_nLengths)
			delete[] m_nLengths;
		m_nLengths = new int[m_nInputCount];
		m_nLengthsSize = m_nInputCount;
	}

	if(m_nInputCount>0)
	{
		SOUNDFORMAT *fmt = GetFormat();
		assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));

		if(m_nBuffSize != (m_nInputCount*(int)size))
		{
			if(NULL != m_pBuffers)
				delete[] m_pBuffers;
			m_nBuffSize = m_nInputCount*size;
			m_pBuffers = new BYTE[m_nBuffSize];
		}
		assert(m_nBuffSize > 0);
		memset(m_pBuffers, 0, m_nBuffSize);
		BYTE *pBuffers = m_pBuffers;

		// Fill source m_pBuffers;
		for(int i=0; i<m_nInputCount; i++)
		{
			if(m_pInputs[i])
			{
				m_nLengths[i] = m_pInputs[i]->Read(pBuffers, size);
				pBuffers += size;
			}
		}

		mixBuffers(m_pBuffers, m_nInputCount, data, size);
	}

	int result = 0;
	for(int i=0; i<m_nInputCount; i++)
	{
		if(m_nLengths[i] > result)
			result = m_nLengths[i];
	}

	return result; 
}

void CMixer::mixBuffers(BYTE *pBuffers, int buffCount, BYTE *output, int size)
{
#ifdef _DEBUG_TIMER_
	clock_t start = 0;
	if(m_bDebugTimer)
		start = clock();
#endif // _DEBUG_TIMER_

	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	
	int step = fmt->bitsPerSample / 8;
	int numsamples = size/step;

	int maxSample = (int)pow(2, fmt->bitsPerSample) / 2 - 1;

	// For 16 bit samples only!!!
	short int *in  = (short int*)pBuffers;
	short int *out = (short int*)output;

	// For each sample
	for(int i=0; i<numsamples; i++)
	{
		// For each source
		for(int n=0; n<buffCount*numsamples; n+=numsamples)
		{
			short int sample = *(in+n+i);
			
			//if(abs(sample) > buffCount)
			if(sample != 0)
			{
				//int tmp = (int)*out + (int)((float)sample / (float)buffCount);
				int tmp = (int)*out + (int)sample;

				if(tmp > maxSample || tmp < -maxSample)
					tmp = (short int)(sample > 0 ? +1 : -1) * maxSample;

				*out = (short int)tmp;
			}

			//*out += *(in+n+i) / buffCount;
		}
		out++; // Move to next sample
	}

#ifdef _DEBUG_TIMER_
	if(m_bDebugTimer)
	{
		m_dwSpeed += clock() - start;
		m_nFrameSize += numsamples;
		m_nFrameCount++;
	}
#endif // _DEBUG_TIMER_
}

/*
// Maybe this one is faster? Check out...
void CMixer::mixBuffers(BYTE *m_pBuffers, int buffm_nInputCount, BYTE *output, int size)
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	
	int step = fmt->size / 8;
	int numsamples = size/step;

	// For 16 bit samples only!!!
	short int *in  = (short int*)m_pBuffers;
	short int *out = (short int*)output;

	// For each channel
	for(int i=0; i<buffm_nInputCount*numsamples; i+=numsamples)
	{
		short int *channel = in+i;
		out = (short int*)output;

		for(int n=0; n<numsamples; n++)
		{
			*(out++) += *(channel++) / buffm_nInputCount;
		}
	}
}
*/
void CMixer::Close()
{
	for(int i=0; i<m_nInputCount; i++)
	{
		if(m_pInputs[i])
			m_pInputs[i]->Close();
	}
	m_nInputCount = 0;

#ifdef _DEBUG_TIMER_
	if(m_bDebugTimer)
	{
		if(m_dwSpeed>0 && m_nFrameCount>0)
		{
			float afmt = (float)m_dwSpeed / (float)m_nFrameCount;
			float afsz = (float)m_nFrameSize / (float)m_nFrameCount;
			float amr = afsz * (float)sampleSize() / (afmt / 1000.f);
			if(amr < 1024.f)
				printf("Mixing rate:\t%.2f b/sec\n", amr);
			else if(amr > 1024.f*1024.f)
				printf("Mixing rate:\t%.2f Mb/sec\n", amr / (1024.f*1024.f));
			else
				printf("Mixing rate:\t%.2f Kb/sec\n", amr / 1024.f);
			printf("Average frame mixing time:\t%f ms\n", afmt);
			printf("Average frame size:\t\t%.2f samples\n", afsz);
		}
	}
	m_nFrameSize=m_dwSpeed=m_nFrameCount=0;
#endif // _DEBUG_TIMER_
}

void CMixer::MoveTo(int samples)
{
	for(int i=0; i<m_nInputCount; i++)
	{
		if(m_pInputs[i])
			m_pInputs[i]->MoveTo(samples);
	}
}

void CMixer::Reset()
{
	for(int i=0; i<m_nInputCount; i++)
	{
		if(m_pInputs[i])
			m_pInputs[i]->Reset();
	}
	if(m_nBuffSize > 0)
		memset(m_pBuffers, 0, m_nBuffSize);
}

int CMixer::GetSize()
{
	int size = 0;
	for(int i=0; i<m_nInputCount; i++)
	{
		if(m_pInputs[i])
		{
			int tmp = m_pInputs[i]->GetSize();
			if(tmp > size)
				size = tmp;
		}
	}
	return size;
}

#ifdef _DEBUG_TIMER_
void CMixer::SetParamVal(int index, float value)
{
	if(index == 0)
		m_bDebugTimer = (value==1.f);
}

bool CMixer::GetParamVal(int index, float *value)
{
	float afmt = (float)m_dwSpeed / (float)m_nFrameCount;
	float afsz = (float)m_nFrameSize / (float)m_nFrameCount;
	float amr = afsz * sampleSize() / (afmt / 1000.f);

	switch(index)
	{
	case -333:
		*value = (m_bDebugTimer ? 1.f : 0.f);
		return true;
	case -334:
		*value = afmt;
		return true;
	case -335:
		*value = afsz;
		return true;
	case -336:
		*value = amr;
		return true;
	}

	return false;
}

char *CMixer::GetParamName(int index)
{
	switch(index)
	{
	case -333:
		return "timer";
	case -334:
		return "mixtime";
	case -335:
		return "m_nFrameSize";
	case -336:
		return "mixrate";
	}
	return NULL;
}

char *CMixer::GetParamDesc(int index)
{
	switch(index)
	{
	case -333:
		return "Debug Timer On/Off (1/0)";
	case -334:
		return "Average frame mixing time (in ms)";
	case -335:
		return "Average frame size (in bytes)";
	case -336:
		return "Average mixing rate (in bytes)";
	}
	return NULL;
}
#endif // _DEBUG_TIMER_
/*
SOUNDFORMAT *CMixer::GetFormat()
{
	SOUNDFORMAT *fmt = IFloopySoundInput::GetFormat();
	//assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	if(fmt->frequency > 0)
		return fmt;
	for(int i=0; i<m_nInputCount; i++)
	{
		SOUNDFORMAT *tmp = m_pInputs[i]->GetFormat();
		if(tmp->frequency > 0)
		{
			fmt = tmp;
			break;
		}
	}
	return fmt;
}
*/

int CMixer::sampleSize()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert(fmt->bitsPerSample > 0);
	return fmt->bitsPerSample / 8;
}
/*
int CMixer::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
*/
