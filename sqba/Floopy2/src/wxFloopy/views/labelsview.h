// LabelsView.h: interface for the CLabelsView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYCANVAS_H__8E37300D_BB37_4B3D_A520_0278F7DAA9B9__INCLUDED_)
#define AFX_MYCANVAS_H__8E37300D_BB37_4B3D_A520_0278F7DAA9B9__INCLUDED_

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

#include <wx/docview.h>
#include <wx/log.h>

#include "MainView.h"
#include "../engine/tracks.h"

class CLabelsView : public CMainView
{
public:
	CLabelsView(CRulerView*, CTracks*);

	void OnDraw(wxDC& dc);
	void OnMouseEvent(wxMouseEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnSize(wxSizeEvent& event);

	void SetTimelineView(wxScrolledWindow *panel) { m_pTimelineView = panel; }

private:
	wxScrolledWindow *m_pTimelineView;

private:
    DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_MYCANVAS_H__8E37300D_BB37_4B3D_A520_0278F7DAA9B9__INCLUDED_)
