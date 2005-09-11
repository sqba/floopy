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

	char *GetName()			{ return "stdlib.performance"; }
	char *GetDescription()	{ return "Performance measuring"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int Read(BYTE *data, int size);
	//void MoveTo(int samples);
	//void Reset();
	//int GetSourceSize();
	//BOOL Open(char *filename);
	//int GetSize();
	//int GetPos();
	void Close();

	BOOL ReadSourceIfDisabled()	{ return TRUE; }


	int   GetPropertyCount();
	BOOL  GetPropertyIndex(char *name, int *index);
	BOOL  GetPropertyVal(int index, float *value);
	//void  SetPropertyVal(int index, float value);
	//float GetPropertyMin(int index);
	//float GetPropertyMax(int index);
	//float GetPropertyStep(int index);
	char *GetPropertyName(int index);
	char *GetPropertyDesc(int index);
	//char *GetPropertyUnit(int index);

private:
	void formatBytes(int bytes, char *str);
	void printResults();

private:
	clock_t	m_ReadTime;
	int		m_nFrameCount;
	int		m_nFrameSize;
};

#endif // !defined(AFX_PERFORMANCE_H__A67BDCC0_9CAA_47F7_A142_51B43D646520__INCLUDED_)
