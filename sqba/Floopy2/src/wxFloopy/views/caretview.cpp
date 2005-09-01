// CaretView.cpp: implementation of the CCaretView class.
//
//////////////////////////////////////////////////////////////////////

#include "CaretView.h"


BEGIN_EVENT_TABLE(CCaretView, CGridView)
	EVT_MOUSE_EVENTS(CCaretView::OnMouseEvent)
	EVT_SIZE(CCaretView::OnSize)
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaretView::CCaretView(wxWindow* parent, CTracks *tracks)
 : CGridView(parent, tracks)
{
	wxLog::AddTraceMask(_T("CCaretView"));

	m_bSelection   = FALSE;
	m_iSelStart    = m_iSelEnd      = -1;
	m_ptSelStart.x = m_ptSelStart.y = -1;
	m_ptSelEnd.x   = m_ptSelEnd.y   = -1;

	wxCaret *caret = new wxCaret(this, 1, 500);
	SetCaret( caret );
	caret->Show();
}

CCaretView::~CCaretView()
{

}

void CCaretView::ScrollWindow( int dx, int dy, const wxRect *rect )
{
	CGridView::ScrollWindow( dx, dy, rect );

	wxCaret *caret = GetCaret();
	if(NULL != caret)
	{
		int x=0, y=0;
		caret->GetPosition(&x, &y);
		caret->Move( x+dx, y+dy );
	}

	m_iSelStart += dx;
	m_iSelEnd   += dx;
}

void CCaretView::OnMouseEvent(wxMouseEvent& event)
{
	//CGridView::OnMouseEvent( event );

	/*if( event.IsButton() ) {
		wxCaret *caret = GetCaret();
		if(NULL != caret) {
			caret->Move( event.GetX(), 0 );
			//GetParent()->Refresh();
			RefreshRulers();
		}
	}*/

	if( event.LeftDown() )
	{
		//wxLogTrace(_T("CCaretView"), _T("ButtonDown x=%d y=%d"), event.GetX(), event.GetY());
		m_ptSelStart = event.GetPosition();
			
		//m_iSelStart = m_ptSelStart.x;
		//m_iSelEnd = -1;

		wxCaret *caret = GetCaret();
		if(NULL != caret)
		{
			int y=0;

			/*int xOrig=0, yOrig=0;
			GetViewStart(&xOrig, &yOrig);

			int xScrollUnits=0, yScrollUnits=0;
			GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

			xOrig *= xScrollUnits;
			yOrig *= yScrollUnits;*/

			caret->GetPosition(NULL, &y);
			//caret->Move( event.GetX()-xOrig, y-yOrig );
			caret->Move( event.GetX(), y );
			//GetParent()->Refresh();
//			RefreshRulers();
		}
	}
	else if( event.Dragging() )
	{
		//wxLogTrace(_T("CCaretView"), _T("Dragging() x=%d y=%d"), event.GetX(), event.GetY());
		//m_ptSelStart, m_ptSelEnd
		m_ptSelEnd = event.GetPosition();
		m_bSelection = TRUE;
		Refresh();
	}
	else if( event.LeftUp() )
	{
		//wxLogTrace(_T("CCaretView"), _T("ButtonUp x=%d y=%d"), event.GetX(), event.GetY());
		m_ptSelEnd = event.GetPosition();
		if(m_bSelection)
		{
			m_bSelection = FALSE;
			m_iSelStart = m_ptSelStart.x;
			m_iSelEnd = m_ptSelEnd.x;
		}
		else
		{
			m_iSelStart = m_iSelEnd = -1;
//			Refresh();
		}
	}
}

void CCaretView::OnSize(wxSizeEvent& event)
{
	CGridView::OnSize( event );

	wxCaret *caret = GetCaret();
	if(NULL != caret)
	{

		int height = m_pTracks->GetHeight();
		if(height > 0)
			caret->SetSize(1, height);
		else
		{
			wxSize size = GetClientSize();
			caret->SetSize(1, size.GetHeight());
		}
	}
}
/*
void CCaretView::OnDraw(wxDC& dc)
{
	CGridView::OnDraw(dc);

	// Draw selection
	int x, y, w;

	if(-1 != m_iSelStart) {
		x = m_iSelStart;
		w = m_iSelEnd - x;
	} else if(m_bSelection) {
		x = m_ptSelStart.x;
		w = m_ptSelEnd.x - x;
	} else
		return;

	int xOrig=0, yOrig=0;
	GetViewStart(&xOrig, &yOrig);

	int xScrollUnits=0, yScrollUnits=0;
    GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	//wxRect rc = GetRect();
	wxSize size = GetClientSize();

	y = yOrig*yScrollUnits;

	dc.SetBrush(*wxTRANSPARENT_BRUSH);
	dc.DrawRectangle(x + xOrig*xScrollUnits, y, w, size.GetHeight());
}
*/