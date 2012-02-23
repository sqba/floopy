
#include "rulerview.h"
#include <wx/dcclient.h>
#include <wx/settings.h>




//////////////////////////////////////////////////////////////////////
// CRulerView
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( CRulerView, wxScrolledWindow )
	EVT_PAINT( CRulerView::OnPaint )
	EVT_SIZE( CRulerView::OnSize )
//	EVT_KEY_DOWN( CRulerView::OnKeyDown )
//	EVT_KEY_UP( CRulerView::OnKeyUp )
//	EVT_ERASE_BACKGROUND( CRulerView::OnEraseBackground )
END_EVENT_TABLE()

CRulerView::CRulerView(wxWindow* parent) : wxScrolledWindow(parent, -1)
{
//	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));

	m_pView = NULL;

	m_pTopRuler = m_pBottomRuler = NULL;

	m_pTopRuler		= new CHorizontalRuler( this );
	//m_pBottomRuler	= new CHorizontalRuler( this );
}

CRulerView::~CRulerView()
{
	if(m_pTopRuler)
		delete m_pTopRuler;
//	if(m_pLeftRuler)
//		delete m_pLeftRuler;
//	if(m_pRightRuler)
//		delete m_pRightRuler;
	if(m_pBottomRuler)
		delete m_pBottomRuler;
	if(m_pView)
		delete m_pView;
}


void CRulerView::SetView(wxWindow* view)
{
	m_pView = view;
	m_pView->Reparent( this );
}


void CRulerView::ScrollWindow( int dx, int dy, const wxRect *rect )
{
	if(m_pTopRuler)
		m_pTopRuler->ScrollWindow( dx, 0, rect );
//	if(m_pLeftRuler)
//		m_pLeftRuler->ScrollWindow( 0, dy, rect );
//	if(m_pRightRuler)
//		m_pRightRuler->ScrollWindow( 0, dy, rect );
	if(m_pBottomRuler)
		m_pBottomRuler->ScrollWindow( dx, 0, rect );
	if(m_pView)
		m_pView->ScrollWindow( dx, dy, rect );
}

void CRulerView::OnSize( wxSizeEvent& event )
{
	int cw, ch;
	GetClientSize( &cw, &ch );
	//GetVirtualSize( &cw, &ch );
	//cw = GetSize().GetWidth();

	int viewTop = 0;
	int viewBottom = ch;

	if ( m_pTopRuler && m_pTopRuler->IsShown() )
	{
		int height = m_pTopRuler->GetSize().GetHeight();
		m_pTopRuler->SetSize( 0, 0, cw, height);
		viewTop = height;
		viewBottom -= height;
	}

	if (m_pBottomRuler && m_pBottomRuler->IsShown() )
	{
		int height = m_pBottomRuler->GetSize().GetHeight();
		m_pBottomRuler->SetSize( 0, ch-height, cw, height);
		viewBottom -= height;
	}

	if (m_pView && m_pView->IsShown() )
		m_pView->SetSize( 0, viewTop, cw, viewBottom);

    event.Skip();
}

wxCaret *CRulerView::GetCaret() const
{
	if(m_pView)
		return m_pView->GetCaret();
	else
		return NULL;
}

void CRulerView::SetTopRuler(CHorizontalRuler *ruler)
{
	if(m_pTopRuler)
		delete m_pTopRuler;
	m_pTopRuler = ruler;
	m_pTopRuler->Reparent( this );
}

void CRulerView::SetBottomRuler(CHorizontalRuler *ruler)
{
	if(m_pBottomRuler)
		delete m_pBottomRuler;
	m_pBottomRuler = ruler;
	m_pBottomRuler->Reparent( this );
}

void CRulerView::GetViewStart2(int* x, int* y) const
{
	wxScrolledWindow::CalcUnscrolledPosition( 0, 0, x, y );
	if(m_pTopRuler)
		*y -= m_pTopRuler->GetSize().GetHeight();

//	wxScrolledWindow::GetViewStart(x, y);
//	if(m_pTopRuler)
//		*y += m_pTopRuler->GetSize().GetHeight();
}

void CRulerView::SetBackgroundColour(wxColour& color)
{
	if(m_pView)
		m_pView->SetBackgroundColour(color);
	else
		wxScrolledWindow::SetBackgroundColour(color);
}

void CRulerView::SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY, int noUnitsX, int noUnitsY, int xPos, int yPos, bool noRefresh)
{
	if(m_pTopRuler)
		noUnitsY += m_pTopRuler->GetSize().GetHeight() / pixelsPerUnitY;
	if(m_pBottomRuler)
		noUnitsY += m_pBottomRuler->GetSize().GetHeight() / pixelsPerUnitY;
	wxScrolledWindow::SetScrollbars(pixelsPerUnitX, pixelsPerUnitY, noUnitsX, noUnitsY, xPos, yPos, noRefresh);
}

/*
void CRulerView::CalcScrolledPosition( int x, int y, int *xx, int *yy) const
{
	wxScrolledWindow::CalcScrolledPosition(x, y, xx, yy);
	if(m_pTopRuler)
		yy += m_pTopRuler->GetSize().GetHeight();
}
*/
/*
void CRulerView::GetClientSize(int* width, int* height) const
{
	if(m_pView)
		m_pView->GetClientSize(width, height);
}
*/
/*
void CRulerView::OnKeyDown(wxKeyEvent& event)
{
	event.Skip(); // Neko mi maznjava neke evente npr. keyleft, keyright...
}
*/














//////////////////////////////////////////////////////////////////////
// CHorizontalRuler
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( CHorizontalRuler, wxWindow )
	EVT_PAINT( CHorizontalRuler::OnPaint )
//	EVT_SIZE( CRulerView::OnSize )
//	EVT_KEY_DOWN( CRulerView::OnKeyDown )
//	EVT_KEY_UP( CRulerView::OnKeyUp )
//	EVT_ERASE_BACKGROUND( CRulerView::OnEraseBackground )
END_EVENT_TABLE()

CHorizontalRuler::CHorizontalRuler(CRulerView* parent)
 : wxWindow(parent, -1, wxDefaultPosition, wxSize(100, 20), wxNO_BORDER)
{
	m_owner = parent;

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
}

CHorizontalRuler::~CHorizontalRuler()
{

}

void CHorizontalRuler::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);

	// NO - don't do this because it will set both the x and y origin
	// coords to match the parent scrolled window and we just want to
	// set the x coord  - MB
	//
	// m_owner->PrepareDC( dc );

	int x, y;
	m_owner->CalcUnscrolledPosition( 0, 0, &x, &y );
	dc.SetDeviceOrigin( -x, 0 );

	this->OnDrawBG( dc );

	this->OnDraw( dc );
}
/*
void CHorizontalRuler::OnDraw(wxDC& dc)
{
	for(int x=0; x<500; x+=11)
	{
		dc.DrawLine(x, 0, x, 20);
	}
}

  void CHorizontalRuler::OnMouseEvent( wxMouseEvent& event )
{
    //m_owner->ProcessColLabelMouseEvent( event );
}

void CHorizontalRuler::OnMouseWheel( wxMouseEvent& event )
{
    m_owner->GetEventHandler()->ProcessEvent(event);
}

// This seems to be required for wxMotif otherwise the mouse
// cursor must be in the cell edit control to get key events
//
void CHorizontalRuler::OnKeyDown( wxKeyEvent& event )
{
    if ( !m_owner->GetEventHandler()->ProcessEvent( event ) ) event.Skip();
}

void CHorizontalRuler::OnKeyUp( wxKeyEvent& event )
{
    if ( !m_owner->GetEventHandler()->ProcessEvent( event ) ) event.Skip();
}
*/

















//////////////////////////////////////////////////////////////////////
// CView
//////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( CView, wxWindow )
	EVT_PAINT( CView::OnPaint )
//	EVT_KEY_DOWN( CView::OnKeyDown )
END_EVENT_TABLE()

CView::CView(CRulerView* parent) : wxWindow(parent, -1)
{
	m_owner = parent;
}

CView::~CView()
{

}

void CView::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);

	// NO - don't do this because it will set both the x and y origin
	// coords to match the parent scrolled window and we just want to
	// set the x coord  - MB
	//
	// m_owner->PrepareDC( dc );

	int x, y;
	m_owner->CalcUnscrolledPosition( 0, 0, &x, &y );
	dc.SetDeviceOrigin( -x, -y );

	this->OnDraw( dc );
}

void CView::GetViewStart(int* x, int* y) const
{
	m_owner->GetViewStart(x, y);
}

void CView::GetScrollPixelsPerUnit(int* xUnit, int* yUnit) const
{
	m_owner->GetScrollPixelsPerUnit(xUnit, yUnit);
}

void CView::SetTopRuler(CHorizontalRuler *ruler)
{
	m_owner->SetTopRuler( ruler );
}

void CView::SetBottomRuler(CHorizontalRuler *ruler)
{
	m_owner->SetBottomRuler( ruler );
}
/*
void CView::OnKeyDown(wxKeyEvent& event)
{
	event.Skip(); // Neko mi maznjava neke evente npr. keyleft, keyright...
}
*/
