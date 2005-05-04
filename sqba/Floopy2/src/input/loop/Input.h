// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__8424BDB3_B751_4EBE_B07D_55F804CFC065__INCLUDED_)
#define AFX_INPUT_H__8424BDB3_B751_4EBE_B07D_55F804CFC065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../../ifloopy.h"

class CInput : public IFloopySoundInput
{
public:
	CInput();
	virtual ~CInput();

	UINT Read(BYTE *data, UINT size);

	char *GetName()			{ return "loop"; }
	char *GetDescription()	{ return "Looping component"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParam(int index, float value)
	{ 
		m_nMaxLoops = (int)value;
		m_nLoops=0;
		IFloopySoundInput::Reset();
	}
	float GetParam(int index)		{ return (float)m_nMaxLoops; }
	char *GetParamName(int index)	{ return "loopcount"; }
	char *GetParamDesc(int index)	{ return "Loop Count"; }

private:
	int m_nLoops, m_nMaxLoops;
};

#endif // !defined(AFX_INPUT_H__8424BDB3_B751_4EBE_B07D_55F804CFC065__INCLUDED_)
