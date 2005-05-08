// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"

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
	int len = IFloopySoundInput::GetSize() * samplesToBytes();

	if((m_nStopAt > 0) && (len > m_nStopAt))
		len = m_nStopAt;

	if((m_nPosition + size) > len)
		size = len - m_nPosition;

	int read = IFloopySoundInput::Read(data, size);

	m_nPosition += read;

	return read;
}

int CInput::GetSize()
{
	int size = IFloopySoundInput::GetSize();

	if((m_nStartAt < m_nStopAt))
		size = (m_nStopAt - m_nStartAt) / samplesToBytes();

	return size;
}

void CInput::MoveTo(int samples)
{
//	int sizeo = IFloopySoundInput::GetSize();
//	int size = GetSize();
	m_nPosition = m_nStartAt + samples * samplesToBytes();
	IFloopySoundInput::MoveTo((m_nStartAt / samplesToBytes()) + samples);
}

void CInput::Reset()
{
	MoveTo(0);
}

void  CInput::SetParam(int index, float value)
{
	int x = samplesToBytes();

	if(index == 0)
	{
		m_nStartAt = (int)value * x;
		IFloopySoundInput::MoveTo((int)value);
	}
	else if(index == 1)
	{
		//m_nStopAt = (int)value * x;
		int val = (int)value * x;
		int max = IFloopySoundInput::GetSize() * x;
		m_nStopAt = (val > max ? max : val);
	}
}

float CInput::GetParam(int index)
{
	int x = samplesToBytes();

	if(index == 0)
		return (float)m_nStartAt / x;
	else if(index == 1)
		return (float)m_nStopAt / x;
	else
		return 0.f;
}

char *CInput::GetParamName(int index)
{
	if(index == 0)
		return "StartAt";
	else if(index == 1)
		return "StopAt";
	else
		return NULL;
}

char *CInput::GetParamDesc(int index)
{
	if(index == 0)
		return "Start At Sample";
	else if(index == 1)
		return "Stop At Sample";
	else
		return NULL;
}

int CInput::samplesToBytes()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
