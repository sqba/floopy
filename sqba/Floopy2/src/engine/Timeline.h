// Timeline.h: interface for the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_)
#define AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#define TIMELINE_PARAM		-1
#define PARAM_ENABLE	1000.f
#define PARAM_DISABLE	2000.f

struct tParam
{
	UINT offset;
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

	void Set(UINT offset, int index, float value);
	float Get(UINT offset, int index);
	void Remove(UINT offset, int index);
	int GetCount();
	UINT GetNextOffset(UINT offset);
	tParam *GetParam(UINT offset, int index);
	UINT GetLastOffset(UINT offset);

private:
	tParam *Find(UINT offset, int index);

private:
	tParam *first, *last;
};

#endif // !defined(AFX_TIMELINE_H__E99D9CE2_478E_4EFF_807B_852DB0AF9322__INCLUDED_)
