// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__C7D6984D_7FB9_43B3_AD16_041165A37CD1__INCLUDED_)
#define AFX_INPUT_H__C7D6984D_7FB9_43B3_AD16_041165A37CD1__INCLUDED_

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

	char *GetName()			{ return "playrgn"; }
	char *GetDescription()	{ return "Start/Stop At Sample"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()	{ return 2; }
	void  SetParam(int index, float value);
	float GetParam(int index);
	char *GetParamName(int index);
	char *GetParamDesc(int index);
	int   GetParamIndex(char *name);

	int GetSize();
	void MoveTo(int samples);
	void Reset();

	int Read(BYTE *data, int size);

private:
	int samplesToBytes();

private:
	int m_nPosition, m_nStartAt, m_nStopAt, m_nSourceSize;
};

#endif // !defined(AFX_INPUT_H__C7D6984D_7FB9_43B3_AD16_041165A37CD1__INCLUDED_)
