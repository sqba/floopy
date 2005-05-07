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

UINT CInput::Read(BYTE *data, UINT size)
{
	UINT len = GetSize();

	if((m_nPosition + size) > len)
		size = len - m_nPosition;

	return IFloopySoundInput::Read(data, size);
}

UINT CInput::GetSize()
{
	UINT size = 0;

	if(m_nStartAt < m_nStopAt)
		size = (m_nStopAt - m_nStartAt) / samplesToBytes();

	return size;
}

void CInput::MoveTo(UINT samples)
{
	IFloopySoundInput::MoveTo((m_nStartAt / samplesToBytes()) + samples);
}

void CInput::Reset()
{
	IFloopySoundInput::MoveTo(0);
}

void  CInput::SetParam(int index, float value)
{
	int x = samplesToBytes();

	if(index == 0)
	{
		m_nStartAt = (int)value * x;
		IFloopySoundInput::MoveTo((UINT)value);
	}
	else if(index == 1)
		m_nStopAt = (int)value * x;
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
	assert((fmt->size > 0) && (fmt->channels > 0));
	return (fmt->size / 8) * fmt->channels;
}
