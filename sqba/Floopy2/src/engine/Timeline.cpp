// Timeline.cpp: implementation of the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#include "Timeline.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeline::CTimeline()
{
	m_pFirst = m_pLast = NULL;
}

CTimeline::~CTimeline()
{
	Clear();
}

void CTimeline::SetParamVal(int offset, int index, float value)
{
	tParam *tmp = GetParam(offset, index);
	if(!tmp)
	{
		tmp = new tParam;
		memset(tmp, 0, sizeof(tParam));
		if(!m_pFirst)
		{
			m_pFirst = m_pLast = tmp;
		}
		else
		{
			tParam *prev = getPrevParam(offset);
			insertAfter(prev, tmp);

			//insertAfter(m_pLast, tmp);

			//m_pLast->next = tmp;
			//tmp->prev = m_pLast;
			//m_pLast = tmp;
		}
	}
	tmp->offset = offset;
	tmp->index = index;
	tmp->value = value;
}

float CTimeline::GetParamVal(int offset, int index)
{
	tParam *tmp = GetParam(offset, index);
	return(tmp ? tmp->value : 0.f);
}

int CTimeline::GetPrevOffset(int offset)
{
	int max = 0;
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if((tmp->offset < offset) && (tmp->offset > max))
		{
			max = tmp->offset;
		}
		tmp = tmp->next;
	}
	return max;
}

int CTimeline::GetNextOffset(int offset)
{
	tParam *tmp = m_pFirst;
	int max = offset;

	while(tmp)
	{
		if(tmp->offset > max)
			max = tmp->offset;
		tmp = tmp->next;
	}

	int val = max;
	tmp = m_pFirst;
	while(tmp)
	{
		if((tmp->offset > offset) && (tmp->offset < val))
			val = tmp->offset;
		tmp = tmp->next;
	}
	return (val != offset ? val : 0);
}

BOOL CTimeline::Remove(int offset, int index)
{
	tParam *tmp = GetParam(offset, index);
	if(tmp)
	{
		tParam *p = tmp->prev;
		tParam *n = tmp->next;
		if(p) p->next = n;
		if(n) n->prev = p;
		//delete tmp;
		memset(tmp, 0, sizeof(tParam));
		return TRUE;
	}
	return FALSE;
}

int CTimeline::GetCount()
{
	int len = 0;
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if(tmp->index >= 0)
			len++;
		tmp = tmp->next;
	}
	return len;
}

tParam *CTimeline::GetParam(int offset, int index)
{
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if((tmp->offset == offset) && (tmp->index == index))
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

void CTimeline::Clear()
{
	tParam *tmp;
	while(m_pFirst)
	{
		tmp = m_pFirst->next;
		delete m_pFirst;
		m_pFirst = tmp;
	}
	m_pFirst = m_pLast = NULL;
}

void CTimeline::insertAfter(tParam *ref, tParam *param)
{
	tParam *tmp = ref->next;
	ref->next   = param;
	param->prev = ref;
	param->next = tmp;
}

tParam *CTimeline::getPrevParam(int offset)
{
	int max = 0;
	tParam *prev = m_pFirst;
	
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if((tmp->offset < offset) && (tmp->offset > max))
		{
			max = tmp->offset;
			prev = tmp;
		}
		tmp = tmp->next;
	}
	return prev;
}


/*
void CTimeline::dump(FILE *fp)
{
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		fprintf(fp, "%d\t%d\t%f\n", tmp->offset, tmp->index, tmp->value);
		tmp = tmp->next;
	}
}

int CTimeline::GetPrevOffset(int offset)
{
	//int val = offset;
	tParam *tmp = m_pFirst;
	int max = 0;
	while(tmp)
	{
		if(tmp->offset < offset)
		{
			if(tmp->offset > max)
				max = tmp->offset;
			//if((val == offset) || (tmp->offset < val))
			//{
			//	val = tmp->offset;
			//}
		}
		tmp = tmp->next;
	}
	//return (val != offset ? val : 0);
	return max;
}

int CTimeline::GetNextOffset(int offset)
{
	int val = offset;
	tParam *tmp = m_pFirst;
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

void CTimeline::Sort()
{
	tParam liTmp;
	int n=GetCount(), i, j, l;
	for(l = 1, i = n-1; l && (i>0); i--) {
		for(j = l = 0; j < i; j++) {
			if(CountJokers(m_LineInfo[j].data) > CountJokers(m_LineInfo[j+1].data)) {
				liTmp.type = m_LineInfo[j].type;
				strcpy(liTmp.data, m_LineInfo[j].data);
				liTmp.index = m_LineInfo[j].index;

				m_LineInfo[j].type = m_LineInfo[j+1].type;
				strcpy(m_LineInfo[j].data, m_LineInfo[j+1].data);
				m_LineInfo[j].index = m_LineInfo[j+1].index;

				m_LineInfo[j+1].type = liTmp.type;
				strcpy(m_LineInfo[j+1].data, liTmp.data);
				m_LineInfo[j+1].index = liTmp.index;

				l = 1;
			}
		}
	}
}
*/