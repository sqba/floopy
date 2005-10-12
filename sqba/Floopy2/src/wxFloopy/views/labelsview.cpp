// CLabelsView.cpp: implementation of the CLabelsView class.
//
//////////////////////////////////////////////////////////////////////

#include "LabelsView.h"
#include "../engine/track.h"


BEGIN_EVENT_TABLE( CLabelsView, CMainView )
	EVT_SIZE( CLabelsView::OnSize )
	EVT_MOUSE_EVENTS( CLabelsView::OnMouseEvent )
	EVT_KEY_DOWN( CLabelsView::OnKeyDown )
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLabelsView::CLabelsView(CRulerView* parent, CTracks *tracks) : CMainView(parent, tracks)
{
//	m_pTracks->SetLabelsView(this);

	//wxLog::AddTraceMask(_T("CLabelsView"));

	//SetTopRuler(new CFloopyHRuler(parent, tracks));
	GetTopRuler()->SetWindowStyle( wxRAISED_BORDER );
}

void CLabelsView::OnDraw(wxDC& dc)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTimelineView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	
	int y = m_pTimelineView->GetScrollPos(wxVERTICAL) * yScrollUnits;
	dc.SetDeviceOrigin( 0, -y );

	m_pTracks->DrawLabels(dc, GetVirtualSize());
}

void CLabelsView::OnMouseEvent(wxMouseEvent& event)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTimelineView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	int y = m_pTimelineView->GetScrollPos(wxVERTICAL) * yScrollUnits;
	CTrack *track = m_pTracks->GetTrackAt(event.GetY() + y);

	if( event.ButtonDown(wxMOUSE_BTN_LEFT) )
	{
		m_pTracks->DeselectAllTracks();

		if(track)
			track->Select();

		m_pTimelineView->SetFocus();
	}

	if( event.LeftDClick() )
	{
		if(track)
			track->Select();

		m_pTimelineView->SetFocus();
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
		if(obj->GetType() == FLOOPY_TRACK)
		{
			CTrack *track = (CTrack*)obj;
			if(track)
			{
				switch (event.GetKeyCode() )
				{
				case '+':
					track->SetHeight( track->GetHeight()+1 );
					//m_pTimelineView->SetFocus();
					break;
				case '-':
					track->SetHeight( track->GetHeight()-1 );
					//m_pTimelineView->SetFocus();
					break;
				default:
					return;
				}
			}
		}
	}
}

void CLabelsView::OnSize(wxSizeEvent& WXUNUSED(event))
{
	Refresh();
}
