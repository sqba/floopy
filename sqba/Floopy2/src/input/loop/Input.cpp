// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	m_nPosition = 0;
	m_nLoops = 0;
	m_nMaxLoops = 1;
}

CInput::~CInput()
{

}

UINT CInput::Read(BYTE *data, UINT size)
{
	if(m_nLoops >= m_nMaxLoops)
		return 0;

	UINT len = 0;
	UINT origSize = size;
	
	UINT srclen = IFloopySoundInput::GetSize();
	srclen *= samplesToBytes();

	if((m_nPosition + size) > srclen)
	{
		if(srclen > m_nPosition)
		{
			size = srclen - m_nPosition;
			len = IFloopySoundInput::Read(data, size);
			size = origSize - size;
		}
		IFloopySoundInput::Reset();
		m_nPosition = 0;
		m_nLoops++;
		len += IFloopySoundInput::Read(data, size);
		m_nPosition += len;
	}
	else
	{
		len = IFloopySoundInput::Read(data, size);
		m_nPosition += len;
	}

	return len;
}

void CInput::SetParam(int index, float value)
{ 
	m_nMaxLoops = (int)value;
	Reset();
}

UINT CInput::GetSize()
{
	return (NULL != m_source ? m_source->GetSize()*m_nMaxLoops : 0);
}

void CInput::MoveTo(UINT samples)
{
	UINT size = IFloopySoundInput::GetSize();

	if(samples > (size * m_nMaxLoops))
	{
		Reset();
		m_nLoops = m_nMaxLoops;
		return;
	}

	if(samples > size)
	{
		int s = samples / size;
		m_nLoops = s;
		if(s>0)
			samples -= (s * size);
		Reset();
	}
	IFloopySoundInput::MoveTo(samples);

	m_nPosition = samples * samplesToBytes();
}

void CInput::Reset()
{
	m_nPosition = 0;
	m_nLoops=0;
	IFloopySoundInput::Reset();
}

int CInput::samplesToBytes()
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	return (fmt->size / 8) * fmt->channels;
}
