// PlayRegion.cpp: implementation of the CPlayRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "playregion.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayRegion::CPlayRegion()
{
	m_startOffset = m_endOffset = m_offset = 0;
//	m_startSample = m_endSample = 0;
}

CPlayRegion::~CPlayRegion()
{

}

int CPlayRegion::Read(BYTE *data, int size)
{
	if((m_endOffset > 0) && (m_offset >= m_endOffset))
		return EOF;

//	m_startOffset = m_startSample * samplesToBytes();
//	m_endOffset = m_endSample * samplesToBytes();

	//int len = IFloopySoundFilter::GetSize() * samplesToBytes();
	int len = m_offset + m_nSourceSize;

	if((m_endOffset > 0) && (len > m_endOffset))
		len = m_endOffset;

	if((m_offset + size) > len)
	{
		assert(len >= m_offset);
		size = len - m_offset;
	}

	int read = IFloopySoundFilter::Read(data, size);
	if(EOF == read)
		return EOF;

	m_offset += read;

	return read;
}

int CPlayRegion::GetSize()
{
	//int size = IFloopySoundFilter::GetSize();
	int size = m_nSourceSize / samplesToBytes();

	int start = m_startOffset;
	int stop = (m_endOffset > 0 ? m_endOffset : size*samplesToBytes());
	size = (stop - start) / samplesToBytes();

	return size;

//	return m_endSample - m_startSample;
}

void CPlayRegion::MoveTo(int samples)
{
	m_offset = m_startOffset + samples * samplesToBytes();
	IFloopySoundFilter::MoveTo((m_startOffset / samplesToBytes()) + samples);
	m_nSourceSize = IFloopySoundFilter::GetSize() * samplesToBytes();
}

void CPlayRegion::Reset()
{
	m_offset = m_startOffset;
	IFloopySoundFilter::Reset();
	m_nSourceSize = IFloopySoundFilter::GetSize() * samplesToBytes();
}

void  CPlayRegion::SetParamVal(int index, float value)
{
	switch(index)
	{
	case 0:
//		m_startSample = (int)value;
		m_startOffset = (int)value * samplesToBytes();
		Reset();
		IFloopySoundFilter::MoveTo((int)value);
		break;
	case 1:
		int stop = (int)value * samplesToBytes();
		int size = IFloopySoundFilter::GetSize();
		int max = size * samplesToBytes();
		if(stop == 0)
			m_endOffset = max;
		else
			m_endOffset = (stop > max ? max : stop);
//		m_endSample = (int)value;// > size ? size : (int)value;
	}
}

BOOL CPlayRegion::GetParamVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)m_startOffset / samplesToBytes();
		return TRUE;
	case 1:
		*value = (float)m_endOffset / samplesToBytes();
		return TRUE;
	}
	return FALSE;
}

char *CPlayRegion::GetParamName(int index)
{
	switch(index)
	{
	case 0:
		return "StartAt";
	case 1:
		return "StopAt";
	default:
		return NULL;
	}
}

char *CPlayRegion::GetParamDesc(int index)
{
	switch(index)
	{
	case 0:
		return "Start At Sample";
	case 1:
		return "Stop At Sample";
	default:
		return NULL;
	}
}

int CPlayRegion::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
