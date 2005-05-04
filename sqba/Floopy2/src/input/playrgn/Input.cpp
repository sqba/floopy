// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"

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

UINT CInput::Read(BYTE *data, UINT size, UINT offset)
{
	if(-1 == offset)
		m_nLength = 0;

	UINT len = 0;
	UINT sizeOrig = size;

	if((m_nLength >= m_nStartAt) && ((m_nLength < m_nStopAt) || (m_nStopAt == 0)))
	{
		if((m_nLength - m_nStartAt) < (int)size)
		{
			// Skip data
			size = m_nLength - m_nStartAt;
			BYTE *tmp = new BYTE[size];
			IFloopySoundInput::Read(data, size, offset);
			delete[] tmp;
			size = sizeOrig - size;
		}

		if(((m_nLength + (int)size) > m_nStopAt) && (m_nStopAt > 0))
		{
			size = m_nStopAt - m_nLength;
		}

		len = IFloopySoundInput::Read(data, size, offset);
	}

	m_nLength += sizeOrig;


	return len;
}

void  CInput::SetParam(int index, float value)
{
	int x = (GetFormat()->size / 8) * GetFormat()->channels;
	if(index == 0)
		m_nStartAt = (int)value * x;
	else if(index == 1)
		m_nStopAt = (int)value * x;
}

float CInput::GetParam(int index)
{
	int x = (GetFormat()->size / 8) * GetFormat()->channels;
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
