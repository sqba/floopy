// ActionHistory.h: interface for the CActionHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONHISTORY_H__2D7E64BD_E15A_4491_9EC6_6759FE4311BD__INCLUDED_)
#define AFX_ACTIONHISTORY_H__2D7E64BD_E15A_4491_9EC6_6759FE4311BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"
#include "floopyobj.h"

enum action_type {ENABLE_AT, SET_PARAM_AT, RESET_PARAM_AT, MOVE_PARAM};

class CActionHistory
{
private:
	struct tAction
	{
		action_type type;
		tAction *prev;
		tAction *next;
		IFloopy *obj;
		IFloopyObj *caller;
		int offset;
		int index;
		float value;
		float prevValue;
		union {
			int newoffset;
			bool bEnable;
		};
	};

public:
	CActionHistory();
	virtual ~CActionHistory();

	// Izbaciti IFloopyTimeline::MoveAllParamsBetween

	bool EnableAt(IFloopyObj *caller, IFloopy *obj, int offset, bool bEnable);
	bool SetParamAt(IFloopyObj *caller, IFloopy *obj, int offset, int index, float value);
	bool ResetParamAt(IFloopyObj *caller, IFloopy *obj, int offset, int index, float value);
	bool MoveParam(IFloopyObj *caller, IFloopy *obj, int offset, int index, float value, int newoffset);

	bool UndoLastAction();
	bool RedoLastAction();
	bool FileChanged()		{ return m_bChanged; }

	//bool RemoveObject(IFloopyObj *obj);

private:
	void add(CActionHistory::tAction *action);
	bool exec(CActionHistory::tAction *action, bool bRedo);

private:
	tAction *m_pActions;
	tAction *m_pLastAction;
	bool m_bChanged;
	int m_iCount;
};

#endif // !defined(AFX_ACTIONHISTORY_H__2D7E64BD_E15A_4491_9EC6_6759FE4311BD__INCLUDED_)
