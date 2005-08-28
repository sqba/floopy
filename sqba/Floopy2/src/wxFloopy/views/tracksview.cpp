// TracksView.cpp: implementation of the CTracksView class.
//
//////////////////////////////////////////////////////////////////////

#include "TracksView.h"
//#include "../FloopyDoc.h"


BEGIN_EVENT_TABLE(CTracksView, CCaretView)
	EVT_MOUSE_EVENTS(CTracksView::OnMouseEvent)
	EVT_KEY_DOWN( CTracksView::OnKeyDown )
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CTracksView::CTracksView(wxWindow* parent, wxScrolledWindow *labels, CTracks *tracks)
 : CCaretView(parent, tracks)
{
	m_pObjMenu		= NULL;
	m_bDrag			= FALSE;
	m_pLabelsView	= labels;
	m_pTracks		= tracks;
	m_pSelectedObj	= NULL;
	m_ptPrev.x = m_ptPrev.y = 0;
	m_pTracks->SetTracksView(this);
	
	SetCursor( wxCURSOR_PENCIL );

	//SetBackgroundColour(*wxLIGHT_GREY);
	//SetWindowStyle(wxSIMPLE_BORDER);

	wxLog::AddTraceMask(_T("CTracksView"));
}

CTracksView::~CTracksView()
{
	/*if(m_pObjMenu) {
		wxMenuBar *menubar = GetFrame()->GetMenuBar();
		menubar->Remove(menubar->GetMenuCount()-1);
	}*/
}

void CTracksView::OnDraw(wxDC &dc)
{
	m_pTracks->DrawBG(dc);		// Draw track backgrounds
	CCaretView::OnDraw(dc);		// Draw grid
	m_pTracks->DrawFore(dc);	// Draw track foregrounds
	// Draw the rest of the window
/*
	int width, height;
	GetClientSize(&width, &height);
	int x=0, y=0, x1=0, y1=0, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	//CalcScrolledPosition(x, y, &x1, &y1);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	dc.DrawRectangle(x*xScrollUnits, y*yScrollUnits, width, 20);
*/
/*
	int x=0, y=0, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	dc.DrawRectangle(0, y*yScrollUnits, m_pTracks->GetWidth(), 20);
*/
}

void CTracksView::ScrollWindow( int dx, int dy, const wxRect *rect )
{
	CCaretView::ScrollWindow( dx, dy, rect );
/*
	int width, height;
	GetClientSize(&width, &height);
	int x=0, y=0, x1=0, y1=0, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	//CalcScrolledPosition(x, y, &x1, &y1);
//	RefreshRect(wxRect(x, y, width, 20));
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	RefreshRect(wxRect(x*xScrollUnits, y*yScrollUnits, width, 20));
*/
/*
	int x=0, y=0, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	RefreshRect(wxRect(0, y*yScrollUnits, m_pTracks->GetWidth(), 20));
*/
	if(0 != dy)
		m_pLabelsView->ScrollWindow( 0, dy, NULL );
}

void CTracksView::OnKeyDown(wxKeyEvent& event)
{
	IFloopyObj *tmp = m_pTracks->GetSelectedObj();
	if(tmp)
		tmp->OnKeyDown(event);
	else
	{
		switch (event.GetKeyCode() )
		{
		case WXK_RIGHT:
		case WXK_NUMPAD_RIGHT:
			/*{
				wxCaret *caret = GetCaret();
				int x=0, y=0;
				caret->GetPosition(&x, &y);
				caret->Move( x+1, y );

			}*/
			m_pTracks->SetCaretPos( m_pTracks->GetCaretPos() + m_pTracks->GetSamplesPerPixel() );
			break;
		case WXK_LEFT:
		case WXK_NUMPAD_LEFT:
			/*{
				wxCaret *caret = GetCaret();
				int x=0, y=0;
				caret->GetPosition(&x, &y);
				caret->Move( x-1, y );
			}*/
			m_pTracks->SetCaretPos( m_pTracks->GetCaretPos() - m_pTracks->GetSamplesPerPixel() );
			break;
		default:
			m_pTracks->OnKeyDown(event);
		}
	}
}

void CTracksView::OnMouseEvent(wxMouseEvent& event)
{
	try {
	int x, y, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	IFloopyObj *obj = m_pTracks->GetChildAt(event.GetX() + x*xScrollUnits,
											event.GetY() + y*yScrollUnits);

	if( event.Dragging() && m_pSelectedObj ) {
		wxRect rc = this->GetRect();
		rc.Offset(-rc.GetX(), -rc.GetY());
		if(!rc.Inside(event.GetX(), event.GetY()))
			return;

		if(m_pSelectedObj->IsKindOf(CLASSINFO(CRegion))) {
			if(0 != m_ptPrev.x) {
				int dx = event.GetX() - m_ptPrev.x;
				m_pTracks->MoveSelectedRegions(dx);
			}
		} else { // Border
			if(0 != m_ptPrev.x) {
				int dx = event.GetX() - m_ptPrev.x;
				int dy = event.GetY() - m_ptPrev.y;
				m_pSelectedObj->Move(dx, dy);
			}
		}
		if(obj) { // Outside of tracks
			m_ptPrev.x = event.GetX();
			m_ptPrev.y = event.GetY();
		}
		//RefreshRulers();
		return;
	}
	else if(obj)
	{
		SetCursor( obj->GetCursor() );

		wxMenu *mnu = obj->GetMenu();

		if(mnu) {
			if(event.RightUp()) {
				PopupMenu(mnu, event.GetPosition());
				return;
			}
		}

		if(event.LeftDown()) {
			m_pSelectedObj = obj;

			if(obj->IsKindOf(CLASSINFO(CTrack))) {
				CTrack *track = (CTrack*)obj;
				wxCaret *caret = GetCaret();
				caret->Show(FALSE);

				int x=0;
				caret->GetPosition(&x, NULL);

				int xScrollUnits=0, yScrollUnits=0;
				GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
				int xOrig=0, yOrig=0;
				GetViewStart(&xOrig, &yOrig);
				xOrig *= xScrollUnits;
				yOrig *= yScrollUnits;
			
				if( track->IsSelected() ) {
					caret->SetSize(1, track->GetHeight());
					caret->Move( x-xOrig, track->GetTop()-yOrig );
				} else {
					m_pTracks->DeselectAllTracks();
					m_pTracks->DeselectAllRegions();

					caret->SetSize(1, GetSize().GetHeight());
					caret->Move( x-xOrig, 0 );
				}
				caret->Show(TRUE);

				///////////////////////////////////////////
				// Add new region
				int x1 = event.GetX() + xOrig;
				CRegion *region = track->AddNewRegionAt(x1);
				m_pSelectedObj = region->GetBorder(FALSE);
				region->Refresh();
				///////////////////////////////////////////
			} else if(obj->IsKindOf(CLASSINFO(CRegion))) {
				if( !event.ShiftDown() && !m_bDrag ) {
					m_pTracks->DeselectAllRegions();
				} else
					m_bDrag = TRUE;

				m_pSelectedObj->Select();
				m_pSelectedObj->Refresh();

				return; // Don't move the caret
			}
		}

		if( event.LeftUp() && m_pSelectedObj ) {
			if(m_pSelectedObj->IsKindOf(CLASSINFO(CTrack))) {
				m_bDrag = FALSE;

			} else if(m_pSelectedObj->IsKindOf(CLASSINFO(CRegion))) {
				m_pTracks->UpdateSelectedRegions();
			} else if(m_pSelectedObj->IsKindOf(CLASSINFO(CRegion::CBorder))) {
				CRegion *region = (CRegion*)m_pSelectedObj->GetParent();
				CTrack *trk = (CTrack*)region->GetParent();
				if(region->GetWidth() <= 2)
					trk->RemoveRegion( region );
				else
					region->Update();
				m_pSelectedObj = NULL;
			}
		}
	}
	else
	{
		SetCursor( *wxSTANDARD_CURSOR );
		if(event.LeftDown())
		{
			m_pTracks->DeselectAllRegions();
			m_pTracks->DeselectAllTracks();

			wxCaret *caret = GetCaret();
			caret->SetSize(1, GetSize().GetHeight());
			caret->Move( event.GetX(), 0 );
		}
	}

	m_ptPrev.x = event.GetX();
	m_ptPrev.y = event.GetY();

	} catch(...)
	{
		wxLogTrace(_T("CTracksView"), _T("OnMouseEvent exception"));
	}

	CCaretView::OnMouseEvent(event);

	if(event.LeftDown())
		m_pTracks->SetCursorPosition(m_pTracks->GetCaretPos());
}
