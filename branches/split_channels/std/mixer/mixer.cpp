// Mixer.cpp: implementation of the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "mixer.h"
#include <math.h>
#include <assert.h>

#define SAMPLE	short int

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMixer::CMixer()
{
	m_nInputCount	= 0;
	m_nBuffSize		= 0;
	m_pBuffers		= NULL;
	m_nLengths		= NULL;
	m_nLengthsSize	= 0;
	m_pFirst		= NULL;
}

CMixer::~CMixer()
{
	inputChannel *channel = m_pFirst;
	while(channel)
	{
		inputChannel *tmp = channel;
		channel = channel->next;
		delete tmp;
	}

	if(NULL != m_pBuffers)
		delete[] m_pBuffers;

	if(NULL != m_nLengths)
		delete[] m_nLengths;
}

int CMixer::AddSource(IFloopySoundInput *src)
{
	inputChannel *tmp = m_pFirst;
	inputChannel *prev = NULL;

	while(tmp)
	{
		prev = tmp;
		tmp = tmp->next;
	}

	tmp = new inputChannel();
	tmp->input = src;
	tmp->next = NULL;

	if(prev)
		prev->next = tmp;

	if(NULL == m_pFirst)
	{
		m_pFirst = tmp;
		m_source = src;
	}

	m_nInputCount++;

	return m_nInputCount-1;
}

bool CMixer::RemoveSource(IFloopySoundInput *src)
{
	inputChannel *tmp = m_pFirst;
	inputChannel *prev = NULL;

	while(tmp && tmp->input!=src)
	{
		prev = tmp;
		tmp = tmp->next;
	}

	if(tmp)
	{
		if(prev)
			prev->next = tmp->next;
		if(m_pFirst == tmp)
			m_pFirst = tmp->next;
		delete tmp;
		m_nInputCount--;
		return true;
	}

	return false;
}

IFloopySoundInput *CMixer::GetSource(int index)
{
	inputChannel *tmp = m_pFirst;
	int count = 0;
	while(tmp)
	{
		if(index == count)
			return tmp->input;
		tmp = tmp->next;
		count++;
	}

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
		memset(m_pBuffers, 0, m_nBuffSize);	// Fill with silence instead!
		BYTE *pBuffer = m_pBuffers;

		// Fill source m_pBuffers;
		inputChannel *channel = m_pFirst;
		int i=0;
		while(channel)
		{
			m_nLengths[i++] = channel->input->Read(pBuffer, size);
			pBuffer += size;
			channel = channel->next;
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

int CMixer::Read2(BYTE **data, int channels, int samples)
{
	return 0;
}

void CMixer::mixBuffers(BYTE *pBuffers, int buffCount, BYTE *output, int size)
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));

	int step = fmt->bitsPerSample / 8;
	int numsamples = size/step;

	int maxSample = (int)pow(2, fmt->bitsPerSample) / 2 - 1;

	// For 16 bit samples only!!!
	SAMPLE *in  = (SAMPLE*)pBuffers;
	SAMPLE *out = (SAMPLE*)output;

	// For each sample
	for(int i=0; i<numsamples; i++)
	{
		// For each source
		for(int n=0; n<buffCount*numsamples; n+=numsamples)
		{
			SAMPLE sample = *(in+n+i);

			if(sample != 0)
			{
				int tmp = (int)*out + (int)sample;

				if(tmp>maxSample || tmp<-maxSample)
					tmp = (SAMPLE)(sample > 0 ? +1 : -1) * maxSample;

				*out = (SAMPLE)tmp;
			}
		}
		out++; // Move to next sample
	}
}

void CMixer::Close()
{
	inputChannel *channel = m_pFirst;
	while(channel)
	{
		inputChannel *tmp = channel;
		channel->input->Close();
		channel = tmp->next;
		delete tmp;
	}

	m_pFirst = NULL;

	m_nInputCount = 0;
}

void CMixer::MoveTo(int samples)
{
	inputChannel *channel = m_pFirst;
	while(channel)
	{
		channel->input->MoveTo(samples);
		channel = channel->next;
	}
}

void CMixer::Reset()
{
	inputChannel *channel = m_pFirst;
	while(channel)
	{
		channel->input->Reset();
		channel = channel->next;
	}

	if(m_nBuffSize > 0)
		memset(m_pBuffers, 0, m_nBuffSize);
}

int CMixer::GetSize()
{
	int size = 0;

	inputChannel *channel = m_pFirst;
	while(channel)
	{
		int s = channel->input->GetSize();
		if(s > size)
			size = s;
		channel = channel->next;
	}

	return size;
}
