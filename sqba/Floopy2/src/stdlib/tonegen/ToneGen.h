// ToneGen.h: interface for the CToneGen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TONEGEN_H__7D7076F8_D00F_4027_B960_138F92765E48__INCLUDED_)
#define AFX_TONEGEN_H__7D7076F8_D00F_4027_B960_138F92765E48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define M_PI 3.14159265358979323846

#include "../../ifloopy.h"

#include <string.h>

class CToneGen : public IFloopySoundInput
{
public:
	CToneGen();
	virtual ~CToneGen();

	int Read(BYTE *data, int size);
	void Reset() { angle = 0.0; }

	char *GetName()			{ return "stdlib.tonegen"; }
	char *GetDescription()	{ return "Tone generator"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParamVal(int index, float value) { if(index == 1) freq = value; }
	BOOL  GetParamVal(int index, float *value);
	char *GetParamName(int index)	{ return "Frequency"; }
	char *GetParamDesc(int index)	{ return "Tone Frequency"; }

	void MoveTo(int samples)		{ if(samples==0) angle = 0.0; }

	int GetSize() { return -1; }

private:
	int atoi(char *s);

	double freq;
	double dangle;
	double angle;

	char name[20];
};

#endif // !defined(AFX_TONEGEN_H__7D7076F8_D00F_4027_B960_138F92765E48__INCLUDED_)
