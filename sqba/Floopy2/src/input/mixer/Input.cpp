// Mixer.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <time.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	count = 0;

	buffsize = 0;
	buffers = NULL;

	lengths = NULL;
	lensize = 0;

#ifdef _DEBUG_TIMER_
	framesize=speed=frames=0;
#endif // _DEBUG_TIMER_
}

CInput::~CInput()
{
	for(int i=0; i<count; i++)
	{
		delete inputs[i];
	}

	if(NULL != buffers)
		delete[] buffers;

	if(NULL != lengths)
		delete[] lengths;
}

int CInput::AddSource(IFloopySoundInput *src)
{
	if(count < MAX_INPUTS)
	{
		if(0==count)
			m_source = src;
		inputs[count++] = src;
		return count;
	}
	return 0;
}

void CInput::RemoveSource(IFloopySoundInput *src)
{
	/*for(int i=0; i<count; i++)
	{
		if(src == inputs[i])
		{
		}
	}*/
	/*if(index < count)
	{
		IFloopySoundInput *tmp = inputs[index];
		tmp
		return inputs[index];
	}*/
}

IFloopySoundInput *CInput::GetSource(int index)
{
	/*for(int i=0; i<count; i++)
	{
		if(index == i)
			return inputs[i];
	}*/
	if(index < count)
		return inputs[index];
	return NULL;
}

int CInput::GetInputCount()
{
	return count;
}

UINT CInput::Read(BYTE *data, UINT size)
{
	if(lensize != count)
	{
		if(NULL != lengths)
			delete[] lengths;
		lengths = new UINT[count];
		lensize = count;
	}

	if(count>0)
	{
		WAVFORMAT *fmt = GetFormat();
		assert((fmt->size > 0) && (fmt->channels > 0));

		if(buffsize != (count*(int)size))
		{
			if(NULL != buffers)
				delete[] buffers;
			buffsize = count*size;
			buffers = new BYTE[buffsize];
		}
		memset(buffers, 0, buffsize);
		BYTE *pbuffers = buffers;

		// Fill source buffers;
		for(int i=0; i<count; i++)
		{
			lengths[i] = inputs[i]->Read(pbuffers, size);
			pbuffers += size;
		}

		MixBuffers(buffers, count, data, size);
	}

	UINT result = 0;
	for(int i=0; i<count; i++)
	{
		if(lengths[i] > result)
			result = lengths[i];
	}

	return result; 
}

void CInput::MixBuffers(BYTE *buffers, int buffcount, BYTE *output, UINT size)
{
#ifdef _DEBUG_TIMER_
	clock_t start = clock();
#endif // _DEBUG_TIMER_

	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	
	int step = fmt->size / 8;
	int numsamples = size/step;

	// For 16 bit samples only!!!
	short int *in  = (short int*)buffers;
	short int *out = (short int*)output;

	// For each sample
	for(int i=0; i<numsamples; i++)
	{
		// For each source
		for(int n=0; n<buffcount*numsamples; n+=numsamples)
		{
			*out += *(in+n+i) / buffcount;
		}
		out++; // Move to next sample
	}

#ifdef _DEBUG_TIMER_
	speed += clock() - start;
	framesize += numsamples;
	frames++;
#endif // _DEBUG_TIMER_
}

/*
// Maybe this one is faster? Check out...
void CInput::MixBuffers(BYTE *buffers, int buffcount, BYTE *output, UINT size)
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	
	int step = fmt->size / 8;
	int numsamples = size/step;

	// For 16 bit samples only!!!
	short int *in  = (short int*)buffers;
	short int *out = (short int*)output;

	// For each channel
	for(int i=0; i<buffcount*numsamples; i+=numsamples)
	{
		short int *channel = in+i;
		out = (short int*)output;

		for(int n=0; n<numsamples; n++)
		{
			*(out++) += *(channel++) / buffcount;
		}
	}
}
*/
void CInput::Close()
{
	for(int i=0; i<count; i++)
	{
		inputs[i]->Close();
	}

#ifdef _DEBUG_TIMER_
	printf("Average frame mixing time (%d samples): %f ms\n",
		framesize/frames, (float)speed/(float)frames);
#endif // _DEBUG_TIMER_
}

void CInput::MoveTo(UINT samples)
{
	for(int i=0; i<count; i++)
	{
		inputs[i]->MoveTo(samples);
	}
}

void CInput::Reset()
{
	for(int i=0; i<count; i++)
	{
		inputs[i]->Reset();
	}
}

DWORD CInput::GetSize()
{
	DWORD size = 0;
	for(int i=0; i<count; i++)
	{
		if(inputs[i]->GetSize() > size)
			size = inputs[i]->GetSize();
	}
	return size;
}
/*
WAVFORMAT *CInput::GetFormat()
{
	WAVFORMAT *fmt = IFloopySoundInput::GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	for(int i=0; i<count; i++)
	{
		WAVFORMAT *tmp = inputs[i]->GetFormat();
		if(tmp->freq > 0)
		{
			fmt = tmp;
			break;
		}
	}
	return fmt;
}
*/