// PathCtrl.h: interface for the CPathCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATHCTRL_H__D02B75EF_1C75_4BA5_9B5E_B402DB4371ED__INCLUDED_)
#define AFX_PATHCTRL_H__D02B75EF_1C75_4BA5_9B5E_B402DB4371ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include <wx/region.h>
#include <wx/list.h>

#include "FloopyObj.h"


class CPathItem : public IFloopyObj
{
public:
	CPathItem(IFloopySoundInput *input);
	virtual ~CPathItem();

	void DrawBG  (wxDC &dc, wxRect &rc);
	void DrawFore(wxDC &dc, wxRect &rc);

private:
	IFloopySoundInput *m_pinput;
//	wxRegion m_Region;
};


WX_DECLARE_LIST(CPathItem, ItemList);


class CPathCtrl : public IFloopyObj
{
public:
	CPathCtrl(IFloopySoundInput *input);
	virtual ~CPathCtrl();

	void SetPath(IFloopySoundInput *input);
	void Clear();

	void DrawBG  (wxDC &dc, wxRect &rc);
	void DrawFore(wxDC &dc, wxRect &rc);

private:
	ItemList m_PathList;
};

#endif // !defined(AFX_PATHCTRL_H__D02B75EF_1C75_4BA5_9B5E_B402DB4371ED__INCLUDED_)
