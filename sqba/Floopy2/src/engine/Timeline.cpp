// Timeline.cpp: implementation of the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#include "Timeline.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeline::CTimeline()
{
	first = last = NULL;
}

CTimeline::~CTimeline()
{
	tParam *tmp;
	while(first)
	{
		tmp = first->next;
		delete first;
		first = tmp;
	}
}

void CTimeline::Set(UINT offset, int index, float value)
{
	tParam *tmp = Find(offset, index);
	if(!tmp)
	{
		tmp = new tParam;
		memset(tmp, 0, sizeof(tParam));
		if(!first)
		{
			first = last = tmp;
		}
		else
		{
			last->next = tmp;
			tmp->prev = last;
			last = tmp;
		}
	}
	tmp->offset = offset;
	tmp->index = index;
	tmp->value = value;
}

tParam *CTimeline::GetParam(UINT offset, int index)
{
	return Find(offset, index);
}

float CTimeline::Get(UINT offset, int index)
{
	tParam *tmp = Find(offset, index);
	return(tmp ? tmp->value : 0.f);
}

UINT CTimeline::GetLastOffset(UINT offset)
{
	//UINT val = offset;
	tParam *tmp = first;
	UINT max = 0;
	while(tmp)
	{
		if(tmp->offset < offset)
		{
			if(tmp->offset > max)
				max = tmp->offset;
			/*if((val == offset) || (tmp->offset < val))
			{
				val = tmp->offset;
			}*/
		}
		tmp = tmp->next;
	}
	//return (val != offset ? val : 0);
	return max;
}

UINT CTimeline::GetNextOffset(UINT offset)
{
	UINT val = offset;
	tParam *tmp = first;
	while(tmp)
	{
		if(tmp->offset > offset)
		{
			if((val == offset) || (tmp->offset < val))
			{
				val = tmp->offset;
			}
		}
		tmp = tmp->next;
	}
	return (val != offset ? val : 0);
}

void CTimeline::Remove(UINT offset, int index)
{
	tParam *tmp = Find(offset, index);
	if(tmp)
	{
		tParam *p = tmp->prev;
		tParam *n = tmp->next;
		if(p) p->next = n;
		if(n) n->prev = p;
	}
}

int CTimeline::GetCount()
{
	int len = 0;
	tParam *tmp = first;
	while(tmp)
	{
		if(tmp->index >= 0)
			len++;
		tmp = tmp->next;
	}
	return len;
}

tParam *CTimeline::Find(UINT offset, int index)
{
	tParam *tmp = first;
	while(tmp)
	{
		if((tmp->offset == offset) && (tmp->index == index))
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

//void CTimeline::Sort()