// ActionHistory.cpp: implementation of the CActionHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "ActionHistory.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActionHistory::CActionHistory()
{
	m_pActions		= NULL;
	m_pLastAction	= NULL;
}

CActionHistory::~CActionHistory()
{
	tAction *action = m_pActions;
	while(action)
	{
		tAction *tmp = action;
		action = action->next;
		delete tmp;
	}
}

bool CActionHistory::EnableAt(IFloopyObj *caller, IFloopy *obj, int offset, bool bEnable)
{
	if( obj->EnableAt(offset, bEnable) )
	{
		tAction *action	= new tAction;
		action->type	= ENABLE_AT;
		action->caller	= caller;
		action->obj		= obj;
		action->offset	= offset;
		action->bEnable	= bEnable;
		add( action );

		return true;
	}
	return false;
}

bool CActionHistory::SetParamAt(IFloopyObj *caller, IFloopy *obj, int offset, int index, float value)
{
	float prevValue = 0.f;
	obj->GetParamAt(offset, index, &prevValue);
	if( obj->SetParamAt(offset, index, value) )
	{
		tAction *action	= new tAction;
		action->type	= SET_PARAM_AT;
		action->caller	= caller;
		action->obj		= obj;
		action->offset	= offset;
		action->index	= index;
		//action->value	= value;
		action->value	= prevValue;
		add( action );

		return true;
	}
	return false;
}

bool CActionHistory::ResetParamAt(IFloopyObj *caller, IFloopy *obj, int offset, int index, float value)
{
	float prevValue = 0.f;
	if( obj->GetParamAt(offset, index, &prevValue) )
	{
		if( obj->ResetParamAt(offset, index, value) )
		{
			tAction *action	= new tAction;
			action->type	= RESET_PARAM_AT;
			action->caller	= caller;
			action->obj		= obj;
			action->offset	= offset;
			action->index	= index;
			//action->value	= value;
			action->value	= prevValue;
			add( action );

			return true;
		}
	}
	return false;
}

bool CActionHistory::MoveParam(IFloopyObj *caller, IFloopy *obj, int offset, int index, float value, int newoffset)
{
	if( obj->MoveParam(offset, index, value, newoffset) )
	{
		tAction *action		= new tAction;
		action->type		= MOVE_PARAM;
		action->caller		= caller;
		action->obj			= obj;
		action->offset		= offset;
		action->index		= index;
		action->value		= value;
		action->newoffset	= newoffset;
		add( action );

		return true;
	}
	return false;
}

bool CActionHistory::UndoLastAction()
{
	if(NULL!=m_pLastAction && exec(m_pLastAction))
	{
		m_pLastAction = m_pLastAction->next;
		return true;
	}
	return false;
}

bool CActionHistory::RedoLastAction()
{
	if(NULL!=m_pLastAction && NULL!=m_pLastAction->prev)
	{
		if( exec( m_pLastAction->prev ) )
		{
			m_pLastAction = m_pLastAction->prev;
			return true;
		}
	}
	return false;
}

void CActionHistory::add(tAction *action)
{
	action->prev = NULL;
	if(NULL == m_pActions)
		action->next = NULL;
	else
	{
		//m_pLastAction = action->next = m_pActions;
		action->next = m_pActions;
		m_pActions->prev = action;
	}
	m_pLastAction = action;
	m_pActions = action;
}

bool CActionHistory::exec(tAction *a)
{
	bool bResult = false;
	if(NULL != a)
	{
		switch(a->type)
		{
		case ENABLE_AT:
			bResult = a->obj->EnableAt(a->offset, !a->bEnable);
			break;
		case SET_PARAM_AT:
			bResult = a->obj->SetParamAt(a->offset, a->index, a->value);
			break;
		case RESET_PARAM_AT:
			//bResult = a->obj->ResetParamAt(a->offset, a->index, a->value);
			bResult = a->obj->SetParamAt(a->offset, a->index, a->value);
			break;
		case MOVE_PARAM:
			bResult = a->obj->MoveParam(a->newoffset, a->index, a->value, a->offset);
			break;
		}
		if(bResult)
		{
			a->caller->Invalidate();
			a->caller->Refresh();
		}
	}
	return bResult;
}
