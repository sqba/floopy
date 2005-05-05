// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
//	m_pos = 0;
//	m_offset = 0;
	m_nLoops = 0;
	m_nMaxLoops = 5;
}

CInput::~CInput()
{

}

UINT CInput::Read(BYTE *data, UINT size)
{
//	if(-1 == offset)
//		m_nLoops = 0;

	UINT len = IFloopySoundInput::Read(data, size);
//	int x = (GetFormat()->size / 8) * GetFormat()->channels;
//	m_pos += len / x;
//	UINT l = IFloopySoundInput::GetSize();
	if(len < size && len > 0)
	{
		if(m_nLoops < m_nMaxLoops-1)
		{
			size -= len;
			IFloopySoundInput::Reset();
			len += IFloopySoundInput::Read(data+len, size);
			m_nLoops++;
		}
//		else
//			return len;
	}
	/*else if(m_pos >= IFloopySoundInput::GetSize())
	{
		m_pos = 0;
		IFloopySoundInput::Reset();
		m_nLoops++;
		return 0;
	}*/
//	return size;
	return len;
}

void CInput::SetParam(int index, float value)
{ 
	m_nMaxLoops = (int)value;
	Reset();
}

DWORD CInput::GetSize()
{
	return (NULL != m_source ? m_source->GetSize()*m_nMaxLoops : 0);
}

void CInput::MoveTo(UINT samples)
{
//	WAVFORMAT *fmt = GetFormat();
//	m_offset = samples * ( (fmt->size / 8) * fmt->channels );

	UINT size = IFloopySoundInput::GetSize();

	if(samples > size)
	{
		int s = samples / size;
		m_nLoops = s;
		if(s>0)
		{
			samples -= (s * size);
		}
		IFloopySoundInput::Reset();
	}
	IFloopySoundInput::MoveTo(samples);
//	m_pos = samples;
}

void CInput::Reset()
{
//	m_pos = 0;
//	m_offset = 0;
	m_nLoops=0;
	IFloopySoundInput::Reset();
}
