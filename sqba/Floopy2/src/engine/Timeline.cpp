// Timeline.cpp: implementation of the CTimeline class.
//
//////////////////////////////////////////////////////////////////////

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

bool CTimeline::GetParamVal(int offset, int index, float *value)
{
	tParam *tmp = getParam(offset, index);
	if(tmp)
	{
		*value = tmp->value;
		return true;
	}
	else
		return false;
}

void CTimeline::SetParamVal(int offset, int index, float value)
{
	tParam *tmp = getParam(offset, index);

	if(tmp)
		tmp->value = value;
	else
		tmp = newParam(offset, index, value);
}

bool CTimeline::MoveParam(int offset, int index, float value, int newoffset)
{
	tParam *param = getParam(offset, index, value);
	if(param)
		return moveParam(param, newoffset);
	else
		return false;
}

bool CTimeline::MoveAllParamsBetween(int start, int end, int offset)
{
	// Isto kao MoveParam samo shto se umesto param->next
	// uzima lastParam->next

	tParam *startParam	= getFirstParamAt(start);
	tParam *endParam	= getLastParamAt(end);
	tParam *nextParam	= getFirstParamAt(start+offset);

	if(!startParam || !endParam)
		return false;

	tParam *tmp = startParam;
	while(tmp != endParam->next)
	{
		tmp->offset += offset;
		tmp = tmp->next;
	}

	if(startParam == nextParam)
		return false;

	if(startParam->prev)
		startParam->prev->next = endParam->next;
	if(endParam->next)
		endParam->next->prev = startParam->prev->next;

	tmp = nextParam->next;
	startParam->prev = nextParam;
	nextParam->next = startParam;
	nextParam->prev = endParam;
	endParam->next = tmp;

	return true;
}

bool CTimeline::RemoveParam(int offset, int index, float value)
{
	tParam *tmp = getParam(offset, index, value);
	if(tmp)
	{
		tmp = removeParam( tmp );
		if(m_pTemp == tmp)
			m_pTemp = NULL;
		if(tmp)
		{
			delete tmp;
			return true;
		}
	}
	return false;
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

int CTimeline::GetNextOffset(int offset, int index)
{
	int min = 0;
	tParam *tmp = m_pFirst;

	if(m_pTemp && m_pTemp->offset<=offset && m_pTemp->index==index)
		tmp = m_pTemp;

	while(tmp)
	{
		if((index == tmp->index) && (tmp->offset > offset) && ((tmp->offset < min) || (min == 0)))
		{
			min = tmp->offset;
			m_pTemp = tmp;
		}
		tmp = tmp->next;
	}
	return min;
}

int CTimeline::GetNextOffset(int offset)
{
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

bool CTimeline::EnableAt(int offset, bool bEnable)
{
	float value = bEnable ? PARAM_VALUE_ENABLED : PARAM_VALUE_DISABLED;
	SetParamVal(offset, TIMELINE_PARAM_ENABLE, value);
	return true;
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

CTimeline::tParam *CTimeline::getParam(int offset, int index, float value)
{
	tParam *tmp = m_pFirst;

	if(m_pTemp && m_pTemp->offset <= offset)
		tmp = m_pTemp;

	while(tmp)
	{
		if((tmp->offset == offset) && (tmp->index == index) && (tmp->value == value))
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
		prev->value = param->value;
		param = removeParam( param );
		if(param)
			delete param;
		return prev;
	}

	tParam *next = prev->next;
	if(next != param)
	{
		param = removeParam( param );

		param->next = prev->next;
		param->prev = prev;
		prev->next  = param;
		if(param->next)
			param->next->prev = param;

		return param;
	}

	//assert(NULL != getParam(param->offset, param->index));

	return NULL;
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

	//assert( _checkSortOrder() );

	return tmp;
}

bool CTimeline::moveParam(CTimeline::tParam *param, int newoffset)
{
	tParam *prev = getPrevParam(newoffset);
	if(prev)
	{
		param->offset = newoffset;
		insertAfter(prev, param);
		//assert(count == GetCount());
		return true;
	}
	return false;
}

CTimeline::tParam *CTimeline::getFirstParamAt(int offset)
{
	int o = 0;
	CTimeline::tParam *tmp = m_pFirst;

	while(tmp && tmp->offset<offset)
		tmp = tmp->next;

	return tmp ? tmp->prev : NULL;
}

CTimeline::tParam *CTimeline::getLastParamAt(int offset)
{
	int o = 0;
	CTimeline::tParam *tmp = m_pLast;

	while(tmp && tmp->offset>offset)
		tmp = tmp->prev;

	return tmp ? tmp->next : NULL;
}

bool CTimeline::_checkSortOrder()
{
	tParam *tmp = m_pFirst;
	int offset = 0;
	while(tmp)
	{
		if(tmp->offset < offset)
			return false;
		offset = tmp->offset;
		tmp = tmp->next;
	}
	return true;
}
