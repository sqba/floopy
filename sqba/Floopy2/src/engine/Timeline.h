// Timeline.h: interface for the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FLOOPY_TIMELINE_H_
#define _FLOOPY_TIMELINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	bool GetParamVal(int offset, int index, float *value);
	bool RemoveParam(int offset, int index, float value);
	int GetCount();
	int GetNextOffset(int offset);
	int GetPrevOffset(int offset);
	void Clear();
	bool MoveParam(int offset, int index, float value, int newoffset);
	int GetPrevOffset(int offset, int index);
	int GetNextOffset(int offset, int index);
	int GetStartOffset();

	bool MoveAllParamsBetween(int start, int end, int offset);

	void EnableAt(int offset, bool bEnable);

private:
	tParam *getParam(int offset, int index);
	tParam *getParam(int offset, int index, float value);
	tParam *getPrevParam(int offset);
	tParam *insertAfter(CTimeline::tParam *prev, CTimeline::tParam *param);
	CTimeline::tParam *removeParam(CTimeline::tParam *param);
	tParam *newParam(int offset, int index, float value);
	bool moveParam(CTimeline::tParam *param, int newoffset);
	tParam *getFirstParamAt(int offset);
	tParam *getLastParamAt(int offset);

	bool _checkSortOrder();

private:
	tParam *m_pFirst, *m_pLast, *m_pTemp;
};

#endif // _FLOOPY_TIMELINE_H_