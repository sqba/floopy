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
	m_nStartAt = m_nStopAt = m_nPosition = 0;
}

CPlayRegion::~CPlayRegion()
{

}

int CPlayRegion::Read(BYTE *data, int size)
{
	if((m_nStopAt > 0) && (m_nPosition >= m_nStopAt))
		return EOF;

	//int len = IFloopySoundInput::GetSize() * samplesToBytes();
	int len = m_nPosition + m_nSourceSize;

	if((m_nStopAt > 0) && (len > m_nStopAt))
		len = m_nStopAt;

	if((m_nPosition + size) > len)
	{
		assert(len >= m_nPosition);
		size = len - m_nPosition;
	}

	int read = IFloopySoundInput::Read(data, size);
	if(EOF == read)
		return EOF;

	m_nPosition += read;

	return read;
}

int CPlayRegion::GetSize()
{
	//int size = IFloopySoundInput::GetSize();
	int size = m_nSourceSize / samplesToBytes();

	int start = m_nStartAt;
	int stop = (m_nStopAt > 0 ? m_nStopAt : size*samplesToBytes());
	size = (stop - start) / samplesToBytes();

	return size;
}

void CPlayRegion::MoveTo(int samples)
{
	m_nPosition = m_nStartAt + samples * samplesToBytes();
	IFloopySoundInput::MoveTo((m_nStartAt / samplesToBytes()) + samples);
	m_nSourceSize = IFloopySoundInput::GetSize() * samplesToBytes();
}

void CPlayRegion::Reset()
{
	m_nPosition = m_nStartAt;
	IFloopySoundInput::Reset();
	m_nSourceSize = IFloopySoundInput::GetSize() * samplesToBytes();
}

void  CPlayRegion::SetParam(int index, float value)
{
	switch(index)
	{
	case 0:
		m_nStartAt = (int)value * samplesToBytes();
		Reset();
		IFloopySoundInput::MoveTo((int)value);
		break;
	case 1:
		int stop = (int)value * samplesToBytes();
		int size = IFloopySoundInput::GetSize();
		int max = size * samplesToBytes();
		if(stop == 0)
			m_nStopAt = max;
		else
			m_nStopAt = (stop > max ? max : stop);
	}
}

float CPlayRegion::GetParam(int index)
{
	switch(index)
	{
	case 0:
		return (float)m_nStartAt / samplesToBytes();
	case 1:
		return (float)m_nStopAt / samplesToBytes();
	default:
		return 0.f;
	}
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

int CPlayRegion::GetParamIndex(char *name)
{
	if(0==strcmpi(name, "StartAt"))
		return 0;
	if(0==strcmpi(name, "StopAt"))
		return 1;
	return -1;
}

int CPlayRegion::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
