// Performance.cpp: implementation of the CPerformance class.
//
//////////////////////////////////////////////////////////////////////

#include "Performance.h"

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
	//printResults();
}




void CPerformance::formatBytes(int bytes, char *str)
{
	if(bytes < 1024)
		sprintf(str, "%d b", bytes);
	else if(bytes > 1024 * 1024)
		sprintf(str, "%.2f Mb", (float)bytes / (1024.f*1024.f));
	else
		sprintf(str, "%.2f Kb", (float)bytes / 1024.f);
}

void CPerformance::printResults()
{
	SOUNDFORMAT *fmt = GetFormat();
	//assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int samplesToBytes = (fmt->bitsPerSample / 8) * fmt->channels;

	printf("%s (%s)\n", GetName(), GetDisplayName());

	float avgFrameTimeMs  = (float)m_ReadTime / (float)m_nFrameCount;
	float avgFrameBytes   = (float)m_nFrameSize / (float)m_nFrameCount;
	float avgFrameSamples = avgFrameBytes / (float)samplesToBytes;
	float avgFrameTimeSec = (float)avgFrameTimeMs / 1000.f;

	printf(" Average frame time: %f ms (%f sec)\n",
		avgFrameTimeMs, avgFrameTimeSec);

	char tmp[100] = {0};
	formatBytes((int)avgFrameBytes, tmp);

	printf(" Average frame size: %d samples (%s)\n",
		(int)avgFrameSamples, tmp);

	float readTimeBytes   = avgFrameBytes / avgFrameTimeSec;
	float readTimeSamples = avgFrameSamples / avgFrameTimeSec;
	memset(tmp, 0, sizeof(tmp));
	formatBytes((int)readTimeBytes, tmp);
	printf(" Average read time:  %.2f samples/sec (%s/sec)\n",
		readTimeSamples, tmp);

	m_nFrameSize = m_ReadTime = m_nFrameCount = 0;
}


int CPerformance::GetPropertyCount()
{
	return 3;
}

bool CPerformance::GetPropertyIndex(char *name, int *index)
{
	return false;
}

bool CPerformance::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)m_ReadTime;
		return true;
	case 1:
		*value = (float)m_nFrameSize;
		return true;
	case 2:
		*value = (float)m_nFrameCount;
		return true;
	}
	return false;
}

char *CPerformance::GetPropertyName(int index)
{
	switch(index)
	{
	case 0:
		return "ReadTime";
	case 1:
		return "FrameSize";
	case 2:
		return "FrameCount";
	}
	return NULL;
}

char *CPerformance::GetPropertyDesc(int index)
{
	return NULL;
}
