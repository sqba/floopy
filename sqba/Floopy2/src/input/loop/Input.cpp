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
	m_nMaxLoops = -1;  // -1 - infinite?
}

CInput::~CInput()
{

}

int CInput::Read(BYTE *data, int size)
{
	if((m_nMaxLoops > 0) && (m_nLoops >= m_nMaxLoops))
		return 0;

	int len = 0;
	int origSize = size;
	
	int srclen = IFloopySoundInput::GetSize();
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

int CInput::GetSize()
{
	if(m_nMaxLoops > 0)
		return (NULL != m_source ? m_source->GetSize()*m_nMaxLoops : 0);
	else
		return -1;
}

void CInput::MoveTo(int samples)
{
	int size = IFloopySoundInput::GetSize();

	if((samples > (size * m_nMaxLoops)) && (m_nMaxLoops > 0))
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
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
