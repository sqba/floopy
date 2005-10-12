// TimelineView.h: interface for the CTimelineView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCANVAS_H__5756201B_EA96_4005_A65A_EB12C24993BC__INCLUDED_)
#define AFX_GRIDCANVAS_H__5756201B_EA96_4005_A65A_EB12C24993BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

//#include <wx/dragimag.h>
//#include <wx/generic/dragimgg.h>

//#include <wx/docview.h>

//#include "labelsview.h"
#include "caretview.h"
#include "../engine/engine.h"
#include "../dialogs/controldlg.h"

class CTimelineView : public CCaretView
{
    DECLARE_EVENT_TABLE()

public:
	CTimelineView(CRulerView *parent, wxScrolledWindow *labels, CTracks *tracks);
	virtual ~CTimelineView();

	void OnDraw(wxDC& dc);
	void ScrollWindow( int dx, int dy, const wxRect *rect );
    void OnMouseEvent(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	//void OnKeyUp(wxKeyEvent& event);

	void ShowParamsDlg();
	void ShowPropertiesDlg();

private:
	CControlDlg			*m_pParamsDialog;
	CControlDlg			*m_pPropsDialog;

protected:
	wxPoint				m_ptPrev;
	IFloopyObj			*m_pSelectedObj;
	wxScrolledWindow	*m_pLabelsView;
	bool				m_bDrag;
	wxMenu				*m_pObjMenu;
};

#endif // !defined(AFX_GRIDCANVAS_H__5756201B_EA96_4005_A65A_EB12C24993BC__INCLUDED_)
