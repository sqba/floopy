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
	m_samplesToBytes = 0;
}

CPlayRegion::~CPlayRegion()
{

}

int CPlayRegion::Read(BYTE *data, int size)
{
	if((m_endOffset > 0) && (m_offset >= m_endOffset))
		return EOF;

//	m_startOffset = m_startSample * m_samplesToBytes;
//	m_endOffset = m_endSample * m_samplesToBytes;

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
	calcVariables();

	if(m_samplesToBytes == 0)
		return 0;

	//int size = IFloopySoundFilter::GetSize();
//	int size = m_nSourceSize / samplesToBytes();

	int start = m_startOffset;
//	int stop = (m_endOffset > 0 ? m_endOffset : size*m_samplesToBytes);
	int stop = (m_endOffset > 0 ? m_endOffset : m_nSourceSize);
	int size = (stop - start) / m_samplesToBytes;

	return size;

//	return m_endSample - m_startSample;
}

void CPlayRegion::MoveTo(int samples)
{
	calcVariables();

	m_offset = m_startOffset + samples * m_samplesToBytes;
	IFloopySoundFilter::MoveTo(m_startSample + samples);
//	IFloopySoundFilter::MoveTo((m_startOffset / m_samplesToBytes) + samples);
//	m_nSourceSize = IFloopySoundFilter::GetSize() * m_samplesToBytes;
}

void CPlayRegion::Reset()
{
	calcVariables();

	m_offset = m_startOffset;
	IFloopySoundFilter::Reset();
//	m_nSourceSize = IFloopySoundFilter::GetSize() * m_samplesToBytes;
}

void  CPlayRegion::SetParamVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_startSample = (int)value;
//		m_startOffset = (int)value * m_samplesToBytes;
		Reset();
		IFloopySoundFilter::MoveTo( (int)value );
		break;
	case 1:
		/*int stop = (int)value * m_samplesToBytes;
		int size = IFloopySoundFilter::GetSize();
		int max = size * m_samplesToBytes;
		if(stop == 0)
			m_endOffset = max;
		else
			m_endOffset = (stop > max ? max : stop);*/

		int size = IFloopySoundFilter::GetSize();
		m_endSample = (int)value > size ? size : (int)value;
	}

	calcVariables();
}

bool CPlayRegion::GetParamVal(int index, float *value)
{
	calcVariables();

	switch(index)
	{
	case 0:
		*value = (float)m_startSample;//m_startOffset / m_samplesToBytes;
		return true;
	case 1:
		*value = (float)m_endSample;//m_endOffset   / m_samplesToBytes;
		return true;
	}
	return false;
}

const char *CPlayRegion::GetParamName(int index)
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

const char *CPlayRegion::GetParamDesc(int index)
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

void CPlayRegion::calcVariables()
{
	SOUNDFORMAT *fmt = GetFormat();
	if((fmt->bitsPerSample > 0) && (fmt->channels > 0))
	{
		m_samplesToBytes = samplesToBytes();

		m_startOffset = m_startSample * m_samplesToBytes;
		m_endOffset   = m_endSample   * m_samplesToBytes;

		m_nSourceSize = IFloopySoundFilter::GetSize() * m_samplesToBytes;
	}
}
