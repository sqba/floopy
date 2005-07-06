// Timeline.cpp: implementation of the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "Timeline.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeline::CTimeline()
{
	m_pFirst = m_pLast = m_pTemp = NULL;
}

CTimeline::~CTimeline()
{
	Clear();
}

BOOL CTimeline::GetParamVal(int offset, int index, float *value)
{
	tParam *tmp = getParam(offset, index);
	if(tmp)
	{
		*value = tmp->value;
		return TRUE;
	}
	return FALSE;
}

void CTimeline::SetParamVal(int offset, int index, float value)
{
	tParam *tmp = getParam(offset, index);

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
		}
	}

	tmp->offset = offset;
	tmp->index = index;
	tmp->value = value;

//	assert( NULL != GetParam(offset, index) );
}

BOOL CTimeline::MoveParam(int offset, int index, int newoffset)
{
	//int count = GetCount();
	tParam *tmp = getParam(offset, index);
	if(tmp)
	{
		tParam *prev = getPrevParam(newoffset);
		if(prev)
		{
			tmp->offset = newoffset;
			insertAfter(prev, tmp);
			//assert(count == GetCount());
			return TRUE;
		}
//		assert(FALSE);
	}
//	assert(FALSE);
	//assert(count == GetCount());
	return FALSE;
}

BOOL CTimeline::RemoveParam(int offset, int index)
{
	tParam *tmp = getParam(offset, index);
	if(tmp)
	{
		removeParam( tmp );
		return TRUE;
	}
	return FALSE;
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

int CTimeline::GetPrevOffset(int offset, int index)
{
	int max = 0;
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if((index == tmp->index) && (tmp->offset < offset) && (tmp->offset > max))
		{
			max = tmp->offset;
		}
		tmp = tmp->next;
	}
	return max;
}

int CTimeline::GetNextOffset(int offset)
{
	// Pretpostavka je da su ofseti poredjani u rastucem redosledu!
	tParam *tmp = m_pFirst;
	int val = 0;

	if(m_pTemp && m_pTemp->offset <= offset)
		tmp = m_pTemp;

	while(tmp)
	{
		if(tmp->offset > offset)
		{
			m_pTemp = tmp;
			val = tmp->offset;
			break;
		}
		tmp = tmp->next;
	}
	return val;
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

int CTimeline::GetStartOffset()
{
	int offset = 0;
	int tmp = 0;
	float value = 0.f;

	do {
		if( GetParamVal(tmp, TIMELINE_PARAM_ENABLE, &value) )
		{
			if(value == PARAM_VALUE_ENABLED)
			{
				offset = tmp;
				break;
			}
		}
	} while((tmp=GetNextOffset(tmp)) > 0);

	return offset;
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

void CTimeline::removeParam(tParam *param)
{
	tParam *prev = param->prev;
	tParam *next = param->next;
	if(prev) prev->next = next;
	if(next) next->prev = prev;
	delete param;
	//memset(param, 0, sizeof(tParam));
}

CTimeline::tParam *CTimeline::getParam(int offset, int index)
{
	tParam *tmp = m_pFirst;

	if(m_pTemp && m_pTemp->offset <= offset)
		tmp = m_pTemp;

	while(tmp)
	{
		if((tmp->offset == offset) && (tmp->index == index))
			return tmp;
		tmp = tmp->next;
	}
	return NULL;
}

void CTimeline::insertAfter(tParam *prev, tParam *param)
{
	if(prev == param)
	{
		prev->offset = param->offset;
		return;
	}

	if((prev->index == param->index) && (prev->offset == param->offset))
	{
		prev->value = param->value;
		//delete param;
		//param = prev;
		return;
	}

	tParam *next = prev->next;
	if(next != param)
	{
		if(param->prev)
			param->prev->next = param->next;
		if(param->next)
			param->next->prev = param->prev;

		prev->next  = param;
		param->prev = prev;
		param->next = next;
	}
//	assert(NULL != getParam(param->offset, param->index));
}

CTimeline::tParam *CTimeline::getPrevParam(int offset)
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

