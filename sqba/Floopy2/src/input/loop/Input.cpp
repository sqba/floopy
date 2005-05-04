// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
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
	if(len < size)
	{
		if(m_nLoops < m_nMaxLoops-1)
		{
			size -= len;
			// Moze li se ovde umesto offset-a pozvati npr. Reset()?
			//len = IFloopySoundInput::Read(data+len, size, -1);
			// Proba:
			IFloopySoundInput::Reset();
			len = IFloopySoundInput::Read(data+len, size);
			// A ako radi, da li se offset parametar moze potpuno izbaciti?
			m_nLoops++;
		}
		else
			return len;
	}
	return size;
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
	IFloopySoundInput::MoveTo(samples);
}

void CInput::Reset()
{
	m_nLoops=0;
	IFloopySoundInput::Reset();
}
