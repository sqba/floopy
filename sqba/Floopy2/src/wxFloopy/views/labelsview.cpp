// CLabelsView.cpp: implementation of the CLabelsView class.
//
//////////////////////////////////////////////////////////////////////

#include "LabelsView.h"
#include "../engine/track.h"
#include "../engine/label.h"


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

	wxSize size = GetVirtualSize();

//	m_pTracks->DrawLabels(dc, size);

	wxFont oldFont = dc.GetFont();

	wxFont fixedFont(12, wxDEFAULT, wxITALIC, wxBOLD);
	dc.SetFont(fixedFont);

	wxRect rc(0, 0, size.GetWidth(), size.GetHeight());

	for(int i=0; i<m_pTracks->GetTrackCount(); i++)
	{
		CTrack *track = m_pTracks->GetTrack(i);
		CLabel *label = track->GetLabel();
		int height = track->GetHeight();
		rc.SetHeight( height );
		label->DrawBG(dc, rc);
		label->DrawFore(dc, rc);
		rc.Offset(0, height);
	}

	dc.SetFont(oldFont);
}

void CLabelsView::OnMouseEvent(wxMouseEvent& event)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTimelineView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	int y = m_pTimelineView->GetScrollPos(wxVERTICAL) * yScrollUnits;
	CTrack *track = m_pTracks->GetTrackAt(event.GetY() + y);

	if( event.ButtonDown(wxMOUSE_BTN_LEFT) )
	{
		if(track)
		{
			CLabel *label = track->GetLabel();
			IFloopyObj *obj = label->GetChildAt(event.GetX(), event.GetY() + y);
			if(obj)
			{
				if(obj == label)
				{
					m_pTracks->DeselectAllTracks();
					obj->Select();
				}
				else
				{
					obj->Select( !obj->IsSelected() );
					Refresh(); // obj->Select should do this
				}
			}
			else
			{
				m_pTracks->DeselectAllTracks();
				track->Select();
			}
		}

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
