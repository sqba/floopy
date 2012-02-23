// Wrapper.h: interface for the CWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRAPPER_H__925CEBE7_34E0_4993_9D3D_CF9D635A9D60__INCLUDED_)
#define AFX_WRAPPER_H__925CEBE7_34E0_4993_9D3D_CF9D635A9D60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"

#include "CVSTHost.h"

class CWrapper : public IFloopySoundFilter
{
public:
	CWrapper();
	virtual ~CWrapper();

	char *GetName()			{ return "vsthost.wrapper"; }
	char *GetDescription()	{ return "VST plugin wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	bool ReadSourceIfDisabled()	{ return false; }

	SOUNDFORMAT *GetFormat();

	bool Open(char *filename);
	int Read(BYTE *data, int size);
	void Reset();
	void MoveTo(int offset);

	int   GetParamCount();
	float GetParamMin(int);
	float GetParamMax(int);
	char *GetParamUnit(int);
	char *GetParamName(int);
	char *GetParamDesc(int);
	float GetParamStep(int);
	void  SetParamVal(int, float);
	bool  GetParamVal(int, float*);

private:

private:
	CVSTHost *m_pHost;
	float *m_pOutputs[2];
	float *m_pInputs[2];
	int m_nBuffSize;
	char m_szLastError[100];
	int m_nEffect;
};

#endif // !defined(AFX_WRAPPER_H__925CEBE7_34E0_4993_9D3D_CF9D635A9D60__INCLUDED_)
