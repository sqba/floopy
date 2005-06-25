// Timeline.h: interface for the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_)
#define AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <memory.h>

#include "../ifloopy.h"

/*
#define TIMELINE_PARAM_ENABLE		-1
#define PARAM_VALUE_ENABLED		1000.f
#define PARAM_VALUE_DISABLED	2000.f
#define TIMELINE_PARAM_MOVETO		-2
//#define TIMELINE_PARAM_RESET		-3
//#define PARAM_VALUE_RESET			-1
*/

struct tParam
{
	int offset;
	int index;
	float value;
	tParam *prev;
	tParam *next;
};

class CTimeline  
{
public:
	CTimeline();
	virtual ~CTimeline();

	void  SetParamVal(int offset, int index, float value);
	float GetParamVal(int offset, int index);
	BOOL Remove(int offset, int index);
	int GetCount();
	int GetNextOffset(int offset);
	tParam *GetParam(int offset, int index);
	int GetPrevOffset(int offset);
	void Clear();
	BOOL MoveParam(int offset, int index, int newoffset);

//	void dump(FILE *fp);

private:
	tParam *getPrevParam(int offset);
	void insertAfter(tParam *ref, tParam *param);
	void remove(tParam *param);

private:
	tParam *m_pFirst, *m_pLast;
};

#endif // !defined(AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_)
