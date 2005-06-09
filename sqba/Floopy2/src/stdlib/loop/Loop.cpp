// Loop.cpp: implementation of the CLoop class.
//
//////////////////////////////////////////////////////////////////////

#include "loop.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoop::CLoop()
{
	m_nPosition = 0;
//	m_nLoops = 0;
//	m_nMaxLoops = -1;  // -1 - infinite?
}

CLoop::~CLoop()
{

}

int CLoop::Read(BYTE *data, int size)
{
//	if((m_nMaxLoops > 0) && (m_nLoops >= m_nMaxLoops))
//		return 0;

	int len = 0;
/*	int origSize = size;
	
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
		Reset();
//		m_nPosition = 0;
//		m_nLoops++;
		len += IFloopySoundInput::Read(data, size);
		m_nPosition += len;
	}
	else
	{*/
		len = IFloopySoundInput::Read(data, size);

		//if(EOF == len)
		//	return len;

		if(len < size)
		{
			// We have reached the end
			Reset();
			int l = IFloopySoundInput::Read(data+len, size-len);

			if(EOF == l)
				return l;

			len += l;
		}
		m_nPosition += len;
//	}

	return len;
}
/*
void CLoop::SetParam(int index, float value)
{ 
	m_nMaxLoops = (int)value;
	Reset();
}
*/
int CLoop::GetSize()
{
//	if(m_nMaxLoops > 0)
//		return (NULL != m_source ? m_source->GetSize()*m_nMaxLoops : 0);
//	else
		return -1;
}

void CLoop::MoveTo(int samples)
{
	int size = IFloopySoundInput::GetSize();

	/*if((m_nMaxLoops > 0) && (samples > (size * m_nMaxLoops)))
	{
		Reset();
		m_nLoops = m_nMaxLoops;
		return;
	}*/

	if(samples > size)
	{
		//m_nLoops = samples / size;
		/*int n = samples % size;
		float a = (float)samples / (float)size;
		int b = (int)samples / (int)size;
		float c = a - (float)b;
		//samples -= (m_nLoops * size);
		int x = samples - (samples % size);
		samples = size * (samples % size) / 10;*/
		//samples = size - (samples % size);
		samples = samples % size;
		IFloopySoundInput::Reset();
	}
	IFloopySoundInput::MoveTo(samples);

//	m_nPosition = samples * samplesToBytes();
}

/*
void CLoop::MoveTo(int samples)
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
*/
void CLoop::Reset()
{
	m_nPosition = 0;
//	m_nLoops=0;
	IFloopySoundInput::Reset();
}

int CLoop::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
