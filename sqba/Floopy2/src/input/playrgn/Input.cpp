// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	m_nStartAt = m_nStopAt = m_nPosition = 0;
}

CInput::~CInput()
{

}

int CInput::Read(BYTE *data, int size)
{
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

	m_nPosition += read;

	return read;
}

int CInput::GetSize()
{
	//int size = IFloopySoundInput::GetSize();
	int size = m_nSourceSize / samplesToBytes();

	int start = m_nStartAt;
	int stop = (m_nStopAt > 0 ? m_nStopAt : size*samplesToBytes());
	size = (stop - start) / samplesToBytes();

	return size;
}

void CInput::MoveTo(int samples)
{
	m_nPosition = m_nStartAt + samples * samplesToBytes();
	IFloopySoundInput::MoveTo((m_nStartAt / samplesToBytes()) + samples);
	m_nSourceSize = IFloopySoundInput::GetSize() * samplesToBytes();
}

void CInput::Reset()
{
	m_nPosition = m_nStartAt;
	IFloopySoundInput::Reset();
	m_nSourceSize = IFloopySoundInput::GetSize() * samplesToBytes();
}

void  CInput::SetParam(int index, float value)
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
		m_nStopAt = (stop > max ? max : stop);
	}
}

float CInput::GetParam(int index)
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

char *CInput::GetParamName(int index)
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

char *CInput::GetParamDesc(int index)
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

int CInput::GetParamIndex(char *name)
{
	if(0==strcmpi(name, "StartAt"))
		return 0;
	if(0==strcmpi(name, "StopAt"))
		return 1;
	return -1;
}

int CInput::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
