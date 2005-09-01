// CLabelsView.cpp: implementation of the CLabelsView class.
//
//////////////////////////////////////////////////////////////////////

#include "LabelsView.h"


BEGIN_EVENT_TABLE( CLabelsView, CMainView )
	EVT_SIZE( CLabelsView::OnSize )
	EVT_MOUSE_EVENTS( CLabelsView::OnMouseEvent )
	EVT_KEY_DOWN( CLabelsView::OnKeyDown )
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLabelsView::CLabelsView(wxWindow* parent, CTracks *tracks) : CMainView(parent, tracks)
{
	m_pTracks->SetLabelsView(this);

	//wxLog::AddTraceMask(_T("CLabelsView"));
}

void CLabelsView::OnDraw(wxDC& dc)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTracksView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	
	int y = m_pTracksView->GetScrollPos(wxVERTICAL) * yScrollUnits;
	dc.SetDeviceOrigin( 0, -y );

	m_pTracks->DrawLabels(dc, GetVirtualSize());
}

void CLabelsView::OnMouseEvent(wxMouseEvent& event)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTracksView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	int y = m_pTracksView->GetScrollPos(wxVERTICAL) * yScrollUnits;
	CTrack *track = m_pTracks->GetTrackAt(event.GetY() + y);

	if( event.ButtonDown(wxMOUSE_BTN_LEFT) )
	{
		m_pTracks->DeselectAllTracks();

		if(track)
		{
			track->Select();
			track->Refresh();
		}

		m_pTracksView->SetFocus();
	}

	if( event.LeftDClick() )
	{
		if(track)
		{
			track->Hide( !track->IsHidden() );
			m_pTracks->Refresh();
		}

		m_pTracksView->SetFocus();
	}

	SetCursor( track ? wxCURSOR_HAND : *wxSTANDARD_CURSOR );
}

void CLabelsView::OnKeyDown(wxKeyEvent& event)
{
	int x, y, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	IFloopyObj *obj = m_pTracks->GetChildAt(event.GetX() + x*xScrollUnits,
											event.GetY() + y*yScrollUnits);

	if(obj)
	{
		if(obj->IsKindOf(CLASSINFO(CTrack)))
		{
			CTrack *track = (CTrack*)obj;
			
			int height = track->GetHeight();

			//wxASSERT(height > 0 && height < 65535);

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
