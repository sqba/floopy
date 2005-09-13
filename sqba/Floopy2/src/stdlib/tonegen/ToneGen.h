// ToneGen.h: interface for the CToneGen class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FLOOPY_TONEGEN_H_
#define _FLOOPY_TONEGEN_H_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../../ifloopy.h"


#define M_PI 3.14159265358979323846


class CToneGen : public IFloopySoundInput
{
public:
	CToneGen();
	virtual ~CToneGen();

	int  GetSize()					{ return -1; }
	int  Read(BYTE *data, int size);
	void MoveTo(int samples)		{ if(samples==0) m_angle = 0.0; }
	void Reset()					{ m_angle = 0.0; }

	char *GetName()					{ return "stdlib.tonegen"; }
	char *GetDescription()			{ return "Tone generator"; }
	char *GetVersion()				{ return "0.1"; }
	char *GetAuthor()				{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParamVal(int, float);
	bool  GetParamVal(int, float*);
	char *GetParamName(int index)	{ return(index==0 ? "Frequency" : NULL); }
	char *GetParamDesc(int index)	{ return(index==0 ? "Tone Frequency" : NULL); }
	float GetParamMax(int index)	{ return(index==0 ? 20000.f : 0.f); }
	float GetParamMin(int index)	{ return 0.f; }
	char *GetParamUnit(int index)	{ return(index==0 ? "Hz" : NULL); }

private:
	int m_BytesToSamples;
	double m_freq, m_step, m_angle, m_max;
};

#endif // _FLOOPY_TONEGEN_H_
