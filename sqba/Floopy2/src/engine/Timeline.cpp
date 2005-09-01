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
	else
		return FALSE;
}

void CTimeline::SetParamVal(int offset, int index, float value)
{
	tParam *tmp = getParam(offset, index);

	if(tmp)
		tmp->value = value;
	else
		tmp = newParam(offset, index, value);
}

BOOL CTimeline::MoveParam(int offset, int index, int newoffset)
{
	tParam *param = getParam(offset, index);
	if(param)
		return moveParam(param, newoffset);
	else
		return FALSE;
}

BOOL CTimeline::RemoveParam(int offset, int index)
{
	tParam *tmp = getParam(offset, index);
	if(tmp)
	{
		tmp = removeParam( tmp );
		if(m_pTemp == tmp)
			m_pTemp = NULL;
		if(tmp)
		{
			delete tmp;
			return TRUE;
		}
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
/*
	// Pretpostavka je da su ofseti poredjani u rastucem redosledu!
	tParam *tmp = m_pFirst;
	int val = 0;

	if(m_pTemp && (m_pTemp->offset <= offset))
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
*/


	int min = 0;
	tParam *tmp = m_pFirst;

	if(m_pTemp && (m_pTemp->offset <= offset))
		tmp = m_pTemp;

	while(tmp)
	{
		if((tmp->offset > offset) && ((tmp->offset < min) || (min == 0)))
		{
			min = tmp->offset;
			m_pTemp = tmp;
		}
		tmp = tmp->next;
	}
	return min;
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
	while(m_pFirst)
	{
		tParam *tmp = m_pFirst->next;
		delete m_pFirst;
		m_pFirst = tmp;
	}
	m_pFirst = m_pLast = NULL;
}

void CTimeline::MoveAllParamsBetween(int start, int end, int offset)
{
/*
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if(tmp->offset >= start && tmp->offset <= end)
		{
			// move parameter offset bytes or
			// remove it if the offset is -1.
			if(-1 == offset)
				removeParam( tmp );
			else
			{
				tParam *prev = getPrevParam(tmp->offset+offset);
				if( prev )
					insertAfter(prev, tmp);
			}
		}
		tmp = tmp->next;
	}
*/
}














CTimeline::tParam *CTimeline::removeParam(CTimeline::tParam *param)
{
	tParam *prev = param->prev;
	tParam *next = param->next;
	
	if(prev)
		prev->next = next;
	if(next)
		next->prev = prev;
	
	if(m_pFirst == param)
		m_pFirst = next;
	if(m_pLast == param)
		m_pLast = prev;
	
	param->prev = NULL;
	param->next = NULL;
	
	return param;
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

CTimeline::tParam *CTimeline::insertAfter(CTimeline::tParam *prev, CTimeline::tParam *param)
{
	if(prev == param->prev)
		return param;

	if(prev == param)
	{
		prev->offset = param->offset;
		return param;
	}

	if((prev->index == param->index) && (prev->offset == param->offset))
	{
		/*prev->value = param->value;
		prev->next = param->next;
		if(param->next)
			param->next->prev = prev;
		delete param;*/
		//param = prev;
		prev->value = param->value;
		param = removeParam( param );
		if(param)
			delete param;
		return prev;
	}

	tParam *next = prev->next;
	if(next != param)
	{
		/*if(param->prev)
			param->prev->next = param->next;
		if(param->next)
			param->next->prev = param->prev;*/

		/*param->prev->next = param->next;
		prev->prev = param->prev;
		prev->next = param;
		param->prev = prev;*/

		param = removeParam( param );

		param->next = prev->next;
		param->prev = prev;
		prev->next  = param;
		if(param->next)
			param->next->prev = param;

		return param;
	}
	return NULL;
//	assert(NULL != getParam(param->offset, param->index));
}

CTimeline::tParam *CTimeline::getPrevParam(int offset)
{
	int max = 0;
	tParam *prev = m_pFirst;
	
	tParam *tmp = m_pFirst;
	while(tmp)
	{
		if((tmp->offset < offset) && (tmp->offset >= max))
		{
			max = tmp->offset;
			prev = tmp;
		}
		tmp = tmp->next;
	}
	return prev;
}

CTimeline::tParam *CTimeline::newParam(int offset, int index, float value)
{
	tParam * tmp = new tParam;

	memset(tmp, 0, sizeof(tParam));

	tmp->offset	= offset;
	tmp->index	= index;
	tmp->value	= value;

	if(NULL == m_pFirst)
	{
		m_pFirst = m_pLast = tmp;
	}
	else
	{
		tParam *prev = getPrevParam(offset);
		insertAfter(prev, tmp);
	}

	if( !_checkSortOrder() )
	{
		int d=1;
	}

	return tmp;
}

BOOL CTimeline::moveParam(CTimeline::tParam *param, int newoffset)
{
	tParam *prev = getPrevParam(newoffset);
	if(prev)
	{
		param->offset = newoffset;
		insertAfter(prev, param);
		//assert(count == GetCount());
		return TRUE;
	}
	return FALSE;
}

BOOL CTimeline::_checkSortOrder()
{
	tParam *tmp = m_pFirst;
	int offset = 0;
	while(tmp)
	{
		if(tmp->offset < offset)
			return FALSE;
		offset = tmp->offset;
		tmp = tmp->next;
	}
	return TRUE;
}
