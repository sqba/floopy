// PlayRegion.h: interface for the CPlayRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYREGION_H__296D45BA_8272_498F_A8B6_E99A25106F8B__INCLUDED_)
#define AFX_PLAYREGION_H__296D45BA_8272_498F_A8B6_E99A25106F8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include "../../ifloopy.h"

class CPlayRegion : public IFloopySoundFilter
{
public:
	CPlayRegion();
	virtual ~CPlayRegion();

	char *GetName()			{ return "stdlib.playrgn"; }
	char *GetDescription()	{ return "Start/Stop At Sample"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()	{ return 2; }
	void  SetParamVal(int index, float value);
	BOOL  GetParamVal(int index, float *value);
	char *GetParamName(int index);
	char *GetParamDesc(int index);

	int GetSize();
	void MoveTo(int samples);
	void Reset();

	int Read(BYTE *data, int size);

private:
	int samplesToBytes();

private:
	int m_offset, m_startOffset, m_endOffset, m_nSourceSize;
//	int m_startSample, m_endSample;
};

#endif // !defined(AFX_PLAYREGION_H__296D45BA_8272_498F_A8B6_E99A25106F8B__INCLUDED_)
