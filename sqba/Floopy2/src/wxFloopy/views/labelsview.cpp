// CLabelsView.cpp: implementation of the CLabelsView class.
//
//////////////////////////////////////////////////////////////////////

#include "LabelsView.h"


BEGIN_EVENT_TABLE( CLabelsView, wxScrolledWindow )
	EVT_SIZE( CLabelsView::OnSize )
	EVT_MOUSE_EVENTS( CLabelsView::OnMouseEvent )
	EVT_KEY_DOWN( CLabelsView::OnKeyDown )
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLabelsView::CLabelsView(wxWindow* parent, CTracks *tracks)
 : wxScrolledWindow(parent)
{
	m_tracks = tracks;

	m_tracks->SetLabelsView(this);

	SetWindowStyle( wxSIMPLE_BORDER );
	//SetWindowStyle( wxSUNKEN_BORDER );

	//SetBackgroundColour(*wxLIGHT_GREY);
	SetBackgroundColour( *wxWHITE );
	//SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_MENU ) );

	//wxLog::AddTraceMask(_T("CLabelsView"));
}

void CLabelsView::OnDraw(wxDC& dc)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTracksView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	int y = m_pTracksView->GetScrollPos(wxVERTICAL) * yScrollUnits;
	dc.SetDeviceOrigin( 0, -y );

	m_tracks->DrawLabels(dc, GetVirtualSize());
}

void CLabelsView::OnMouseEvent(wxMouseEvent& event)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTracksView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	int y = m_pTracksView->GetScrollPos(wxVERTICAL);
	y *= yScrollUnits;

	CTrack *track = m_tracks->GetTrackAt(event.GetY() + y);

	if( event.ButtonDown(wxMOUSE_BTN_LEFT) ) {
		CTrack *track = m_tracks->GetTrackAt(event.GetY() + y);
		if(track) {
			m_tracks->DeselectAllTracks();
			track->Select();
			track->Refresh();
			m_pTracksView->SetFocus();
			//m_tracks->SetCaretPos( m_tracks->GetCaretPos() );
		}
	}

	if( event.LeftDClick() ) {
		if(track) {
			track->Hide( !track->IsHidden() );
			m_tracks->Refresh();
		}
	}

	if(track)
		SetCursor( wxCURSOR_HAND );
	else
		SetCursor( *wxSTANDARD_CURSOR );
}

void CLabelsView::OnKeyDown(wxKeyEvent& event)
{
	int x, y, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	IFloopyObj *obj = m_tracks->GetChildAt(event.GetX() + x*xScrollUnits,
											event.GetY() + y*yScrollUnits);

	if(obj) {
		if(obj->IsKindOf(CLASSINFO(CTrack))) {
			CTrack *track = (CTrack*)obj;
			int height = track->GetHeight();

			wxASSERT(height > 0 && height < 65535);

			switch (event.GetKeyCode() )
			{
			case '+':
				height += 1;
				break;
			case '-':
				height -= 1;
				break;
			default:
				return;
			}

			track->SetHeight( height );
		}
	}
}

void CLabelsView::OnSize(wxSizeEvent& WXUNUSED(event))
{
	Refresh();
}
