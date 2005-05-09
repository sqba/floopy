// Timeline.h: interface for the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_)
#define AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

#define TIMELINE_PARAM		-1
#define PARAM_ENABLE	1000.f
#define PARAM_DISABLE	2000.f

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

	void Set(int offset, int index, float value);
	float Get(int offset, int index);
	void Remove(int offset, int index);
	int GetCount();
	int GetNextOffset(int offset);
	tParam *GetParam(int offset, int index);
	int GetPrevOffset(int offset);

	void dump(FILE *fp);

private:
	tParam *Find(int offset, int index);
//	tParam *GetPrevParam(int offset);

private:
	tParam *first, *last;
};

#endif // !defined(AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_)
