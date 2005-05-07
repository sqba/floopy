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
	m_nStartAt = m_nStopAt = m_nLength = 0;
}

CInput::~CInput()
{

}

UINT CInput::Read(BYTE *data, UINT size)
{
//	if(-1 == offset)
//		m_nLength = 0;

	UINT len = GetSize();
	if(m_nLength + size > len)
	{
		size = len - m_nLength;
	}
	return IFloopySoundInput::Read(data, size);
/*
	UINT len = 0;
	UINT sizeOrig = size;

	if((m_nLength >= m_nStartAt) && ((m_nLength < m_nStopAt) || (m_nStopAt == 0)))
	{
		if((m_nLength - m_nStartAt) < (int)size)
		{
			// Skip data
			size = m_nLength - m_nStartAt;
			BYTE *tmp = new BYTE[size];
			IFloopySoundInput::Read(data, size);
			delete[] tmp;
			size = sizeOrig - size;
		}

		if(((m_nLength + (int)size) > m_nStopAt) && (m_nStopAt > 0))
		{
			size = m_nStopAt - m_nLength;
		}

		len = IFloopySoundInput::Read(data, size);
	}

	m_nLength += sizeOrig;

	return len;
*/
}

UINT CInput::GetSize()
{
	if(m_nStartAt >= m_nStopAt)
		return 0;

	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));

	int x = (fmt->size / 8) * fmt->channels;

	UINT size = (m_nStopAt - m_nStartAt) / x;

	return size;

	//return IFloopySoundInput::GetSize();
}

void CInput::MoveTo(UINT samples)
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));

	int x = (fmt->size / 8) * fmt->channels;

	IFloopySoundInput::MoveTo((m_nStartAt / x) + samples);
}

void CInput::Reset()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));

	int x = (fmt->size / 8) * fmt->channels;

	IFloopySoundInput::MoveTo(m_nStartAt / x);
}

void  CInput::SetParam(int index, float value)
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));

	int x = (fmt->size / 8) * fmt->channels;

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
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));

	int x = (fmt->size / 8) * fmt->channels;

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
