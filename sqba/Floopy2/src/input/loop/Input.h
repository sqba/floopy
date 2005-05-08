// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__8424BDB3_B751_4EBE_B07D_55F804CFC065__INCLUDED_)
#define AFX_INPUT_H__8424BDB3_B751_4EBE_B07D_55F804CFC065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <assert.h>
#include "../../ifloopy.h"

class CInput : public IFloopySoundInput
{
public:
	CInput();
	virtual ~CInput();

	int Read(BYTE *data, int size);

	char *GetName()			{ return "loop"; }
	char *GetDescription()	{ return "Looping component"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParam(int index, float value);
	float GetParam(int index)		{ return (float)m_nMaxLoops; }
	char *GetParamName(int index)	{ return "loopcount"; }
	char *GetParamDesc(int index)	{ return "Loop Count"; }

	int GetSize();
	void MoveTo(int samples);
	void Reset();

private:
	int samplesToBytes();

private:
	int m_nLoops, m_nMaxLoops;
	int m_nPosition;//, m_pos;
};

#endif // !defined(AFX_INPUT_H__8424BDB3_B751_4EBE_B07D_55F804CFC065__INCLUDED_)
