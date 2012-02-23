// Performance.cpp: implementation of the CPerformance class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "performance.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPerformance::CPerformance()
{
	m_nFrameSize = m_ReadTime = m_nFrameCount = 0;
}

CPerformance::~CPerformance()
{

}

int CPerformance::Read(BYTE *data, int size)
{
	clock_t start = clock();

	int len = IFloopySoundFilter::Read(data, size);

	clock_t stop = clock();

	m_ReadTime += stop - start;
	m_nFrameCount++;
	m_nFrameSize += len;

	return len;
}

void CPerformance::Close()
{
	IFloopySoundFilter::Close();
}

void CPerformance::MoveTo(int samples)
{
	IFloopySoundFilter::MoveTo( samples );
}

void CPerformance::Reset()
{
	IFloopySoundFilter::Reset();
}

int CPerformance::GetSourceSize()
{
	return IFloopySoundFilter::GetSourceSize();
}

bool CPerformance::Open(char *filename)
{
	return IFloopySoundFilter::Open( filename );
}

int CPerformance::GetSize()
{
	return IFloopySoundFilter::GetSize();
}


int CPerformance::GetPropertyCount()
{
	return 3;
}

const char *CPerformance::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "AvgFrmReadTime";
	case 1: return "AvgFrmSize";
	case 2: return "SamplesPerSecond";
	}
	return NULL;
}

const char *CPerformance::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Average frame read time";
	case 1: return "Average frame size";
	case 2: return "Average read time";
	}
	return NULL;
}

const char *CPerformance::GetPropertyUnit(int index)
{
	switch(index)
	{
	case 0: return "ms";
	case 1: return "samples";
	//case 2: return "samples per second";
	}
	return NULL;
}

bool CPerformance::GetPropertyVal(int index, float *value)
{
	SOUNDFORMAT *fmt = GetFormat();
	if(NULL==fmt || fmt->bitsPerSample==0 || fmt->channels==0 ||
		m_ReadTime==0 || m_nFrameCount==0 || m_nFrameSize==0)
	{
		*value = 0.f;
		return true;
	}

	float avgFrameTimeMs	= (float)m_ReadTime / (float)m_nFrameCount;
	float avgFrameBytes		= (float)m_nFrameSize / (float)m_nFrameCount;
	float samplesToBytes	= (fmt->bitsPerSample / 8.f) * fmt->channels;
	float avgFrameSamples	= avgFrameBytes / samplesToBytes;
	float avgFrameTimeSec	= avgFrameTimeMs / 1000.f;
	float samplesPerSecond	= avgFrameSamples / avgFrameTimeSec;

	switch(index)
	{
	case 0:
		*value = avgFrameTimeMs;
		return true;
	case 1:
		*value = avgFrameSamples;
		return true;
	case 2:
		*value = samplesPerSecond;
		return true;
	}
	return false;
}
