// Mixer.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <time.h>
#include <stdio.h>

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
}

void CInput::RemoveSource(int index)
{
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
#ifdef _DEBUG_TIMER_
void CInput::Close()
{
	printf("Average frame mixing time (%d samples): %f ms\n",
		framesize/frames, (float)speed/(float)frames);
}
#endif // _DEBUG_TIMER_
