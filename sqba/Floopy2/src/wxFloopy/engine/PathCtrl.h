// PathCtrl.h: interface for the CPathCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHCTRL_H__D02B75EF_1C75_4BA5_9B5E_B402DB4371ED__INCLUDED_)
#define AFX_PATHCTRL_H__D02B75EF_1C75_4BA5_9B5E_B402DB4371ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <wx/region.h>
#include <wx/list.h>

#include "FloopyObj.h"

class CPathCtrl;

class CPathItem : public IFloopyObj
{
public:
	CPathItem(CPathCtrl *parent, IFloopySoundInput *input, bool first);
	virtual ~CPathItem();

	void DrawBG  (wxDC &dc, wxRect &rc);
	void DrawFore(wxDC &dc, wxRect &rc);

	void Select(bool selected);

public:
//	wxRect m_rc;
	wxRegion *m_pRegion;

private:
	IFloopySoundInput *m_pInput;
	bool m_bFirst;
};


WX_DECLARE_LIST(CPathItem, ItemList);


class CPathCtrl : public IFloopyObj
{
public:
	CPathCtrl(IFloopyObj parent, IFloopySoundInput *input);
	virtual ~CPathCtrl();

	void SetPath(IFloopySoundInput *input);
	void Clear();

	void DrawBG  (wxDC &dc, wxRect &rc);
	void DrawFore(wxDC &dc, wxRect &rc);

	IFloopyObj *GetChildAt(int x, int y);
	int GetChildCount();
	IFloopyObj *GetChild(int index);
	void Move(int dx, int dy);
	IFloopyObj *GetSelectedObj();

private:
	ItemList m_PathList;
	wxRect m_rc;
};

#endif // !defined(AFX_PATHCTRL_H__D02B75EF_1C75_4BA5_9B5E_B402DB4371ED__INCLUDED_)
