// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
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

UINT CInput::Read(BYTE *data, UINT size, UINT offset)
{
	if(-1 == offset)
		m_nLoops = 0;

	UINT len = IFloopySoundInput::Read(data, size, offset);
	if(len < size)
	{
		if(m_nLoops < m_nMaxLoops-1)
		{
			size -= len;
			// Moze li se ovde umesto offset-a pozvati npr. Reset()?
			//len = IFloopySoundInput::Read(data+len, size, -1);
			// Proba:
			IFloopySoundInput::Reset();
			len = IFloopySoundInput::Read(data+len, size, offset);
			// A ako radi, da li se offset parametar moze potpuno izbaciti?
			m_nLoops++;
		}
		else
			return len;
	}
	return size;
}
