// Timeline.h: interface for the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FLOOPY_TIMELINE_H_
#define _FLOOPY_TIMELINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <memory.h>

#include "../ifloopy.h"


class CTimeline  
{
private:
	struct tParam
	{
		int offset;
		int index;
		float value;
		tParam *prev;
		tParam *next;
	};

public:
	CTimeline();
	virtual ~CTimeline();

	void SetParamVal(int offset, int index, float value);
	BOOL GetParamVal(int offset, int index, float *value);
	BOOL RemoveParam(int offset, int index);
	int GetCount();
	int GetNextOffset(int offset);
	int GetPrevOffset(int offset);
	void Clear();
	BOOL MoveParam(int offset, int index, int newoffset);
	int GetPrevOffset(int offset, int index);
	int GetStartOffset();

//	void dump(FILE *fp);

private:
	tParam *getParam(int offset, int index);
	tParam *getPrevParam(int offset);
	void insertAfter(CTimeline::tParam *prev, CTimeline::tParam *param);
	void removeParam(CTimeline::tParam *param);

private:
	tParam *m_pFirst, *m_pLast, *m_pTemp;
};

#endif // _FLOOPY_TIMELINE_H_