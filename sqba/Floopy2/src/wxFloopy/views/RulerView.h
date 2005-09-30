// RulerView.h: interface for the CRulerView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RULERVIEW_H__8CD1001A_E7AE_4447_BE97_B5872A600074__INCLUDED_)
#define AFX_RULERVIEW_H__8CD1001A_E7AE_4447_BE97_B5872A600074__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/window.h>
#include <wx/scrolwin.h>


class CHorizontalRuler;


class CRulerView : public wxScrolledWindow  
{
    DECLARE_EVENT_TABLE()

public:
	CRulerView(wxWindow* parent);
	virtual ~CRulerView();

	void SetView(wxWindow* view);
	wxWindow *GetView() { return m_pView; }
	void SetTopRuler(CHorizontalRuler *ruler);

	void OnSize( wxSizeEvent& event );
//	void OnKeyDown(wxKeyEvent& event);
	void ScrollWindow( int dx, int dy, const wxRect *rect );
	wxCaret *GetCaret() const;
//	void GetClientSize(int* width, int* height) const;
//	void CalcScrolledPosition( int x, int y, int *xx, int *yy) const;
//	void GetViewStart(int* x, int* y) const;
	void GetViewStart2(int* x, int* y) const;
	void SetBackgroundColour(wxColour& color);

private:
	wxWindow *m_pView;
	CHorizontalRuler *m_pTopRuler;
	CHorizontalRuler *m_pBottomRuler;
};


class CHorizontalRuler : public wxWindow  
{
    DECLARE_EVENT_TABLE()

public:
	CHorizontalRuler(CRulerView* parent);
	virtual ~CHorizontalRuler();

	virtual void OnDraw(wxDC& dc) {}

private:
	void OnPaint( wxPaintEvent& WXUNUSED(event) );
//	void OnMouseEvent( wxMouseEvent& event );
//	void OnMouseWheel( wxMouseEvent& event );
//	void OnKeyDown( wxKeyEvent& event );
//	void OnKeyUp( wxKeyEvent& event );
//	void OnSize( wxSizeEvent& event );

private:
	CRulerView *m_owner;
};


class CView : public wxWindow  
{
    DECLARE_EVENT_TABLE()

public:
	CView(CRulerView* parent);
	virtual ~CView();

	void SetTopRuler(CHorizontalRuler *ruler);

	virtual void OnDraw(wxDC& dc) {}
//	void OnKeyDown(wxKeyEvent& event);

	void GetViewStart(int* x, int* y) const;
	void GetScrollPixelsPerUnit(int* xUnit, int* yUnit) const;

private:
	void OnPaint( wxPaintEvent& WXUNUSED(event) );

private:
	CRulerView *m_owner;
};


#endif // !defined(AFX_RULERVIEW_H__8CD1001A_E7AE_4447_BE97_B5872A600074__INCLUDED_)
