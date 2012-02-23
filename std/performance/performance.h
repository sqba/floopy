// Performance.h: interface for the CPerformance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PERFORMANCE_H__A67BDCC0_9CAA_47F7_A142_51B43D646520__INCLUDED_)
#define AFX_PERFORMANCE_H__A67BDCC0_9CAA_47F7_A142_51B43D646520__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#include <stdio.h>

#include "../../ifloopy.h"

class CPerformance : public IFloopySoundFilter
{
public:
	CPerformance();
	virtual ~CPerformance();

	const char *GetName()			{ return "std.performance"; }
	const char *GetDescription()	{ return "Performance measuring"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int Read(BYTE *data, int size);
	void MoveTo(int samples);
	void Reset();
	int GetSourceSize();
	bool Open(char *filename);
	int GetSize();
	void Close();

	bool CanReadSourceIfDisabled()	{ return true; }


	int   GetPropertyCount();
	//bool  GetPropertyIndex(char *name, int *index);
	bool  GetPropertyVal(int index, float *value);
	//void  SetPropertyVal(int index, float value);
	//float GetPropertyMin(int index);
	//float GetPropertyMax(int index);
	//float GetPropertyStep(int index);
	const char *GetPropertyName(int index);
	const char *GetPropertyDesc(int index);
	const char *GetPropertyUnit(int index);

	const char *GetLastErrorDesc() { return NULL; }

private:
	clock_t	m_ReadTime;
	int		m_nFrameCount;
	int		m_nFrameSize;
};

#endif // !defined(AFX_PERFORMANCE_H__A67BDCC0_9CAA_47F7_A142_51B43D646520__INCLUDED_)
