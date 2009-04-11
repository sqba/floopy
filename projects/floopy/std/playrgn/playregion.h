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

	const char *GetName()			{ return "std.playrgn"; }
	const char *GetDescription()	{ return "Start/Stop At Sample"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int   GetParamCount()	{ return 2; }
	void  SetParamVal(int index, float value);
	bool  GetParamVal(int index, float *value);
	const char *GetParamName(int index);
	const char *GetParamDesc(int index);

	int GetSize();
	void MoveTo(int samples);
	void Reset();

	int Read(BYTE *data, int size);

	const char *GetLastErrorDesc() { return NULL; }

private:
	int samplesToBytes();
	void calcVariables();

private:
	int m_offset, m_startOffset, m_endOffset, m_nSourceSize;
	int m_startSample, m_endSample;
	int m_samplesToBytes;
};

#endif // !defined(AFX_PLAYREGION_H__296D45BA_8272_498F_A8B6_E99A25106F8B__INCLUDED_)
