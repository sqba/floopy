// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__060C33B4_59B2_4E1F_9128_82DD17E998AB__INCLUDED_)
#define AFX_INPUT_H__060C33B4_59B2_4E1F_9128_82DD17E998AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define M_PI 3.14159265358979323846

#include <windows.h>
#include "../../ifloopy.h"

class CInput : public IFloopySoundInput
{
public:
	CInput();
	virtual ~CInput();

	UINT Read(BYTE *data, UINT size, UINT offset);

	char *GetName()			{ return "tonegen"; }
	char *GetDescription()	{ return "Tone generator"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParam(int index, float value) { freq = value; }
	float GetParam(int index)		{ return (float)freq; }
	char *GetParamName(int index)	{ return "Frequency"; }
	char *GetParamDesc(int index)	{ return "Tone Frequency"; }

private:
	int atoi(char *s);

	double freq;
	double dangle;
	double angle;
};

#endif // !defined(AFX_INPUT_H__060C33B4_59B2_4E1F_9128_82DD17E998AB__INCLUDED_)
