// TimelineView.cpp: implementation of the CTimelineView class.
//
//////////////////////////////////////////////////////////////////////

#include "timelineview.h"
#include "rulers/timeruler.h"
#include "rulers/sampleruler.h"
#include "../engine/track.h"
#include "../engine/region.h"


BEGIN_EVENT_TABLE(CTimelineView, CCaretView)
	EVT_MOUSE_EVENTS(CTimelineView::OnMouseEvent)
	EVT_MOUSEWHEEL(CTimelineView::OnMouseWheel)
	EVT_KEY_DOWN( CTimelineView::OnKeyDown )
END_EVENT_TABLE()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CTimelineView::CTimelineView(CRulerView* parent, wxScrolledWindow *labels, CTracks *tracks)
 : CCaretView(parent, tracks)
{
	m_pObjMenu		= NULL;
	m_bDrag			= false;
	m_pLabelsView	= labels;
	m_pSelectedObj	= NULL;
	m_ptPrev.x = m_ptPrev.y = 0;
//	m_pTracks->SetTimelineView(this);
	
	SetCursor( wxCURSOR_PENCIL );

	m_pParamsDialog = new CControlDlg( tracks );
	m_pPropsDialog = new CControlDlg( tracks );

	SetTopRuler( new CTimeRuler(parent, tracks) );
	SetBottomRuler( new CSampleRuler(parent, tracks) );

	wxLog::AddTraceMask(_T("CTimelineView"));
}

CTimelineView::~CTimelineView()
{
	delete m_pParamsDialog;
	delete m_pPropsDialog;

	/*if(m_pObjMenu) {
		wxMenuBar *menubar = GetFrame()->GetMenuBar();
		menubar->Remove(menubar->GetMenuCount()-1);
	}*/
}

void CTimelineView::OnDraw(wxDC &dc)
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

void CTimelineView::ScrollWindow( int dx, int dy, const wxRect *rect )
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

void CTimelineView::OnKeyDown(wxKeyEvent& event)
{
	int x, y, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	event.m_x += x*xScrollUnits;
	event.m_y += y*yScrollUnits;

	IFloopyObj *obj = m_pTracks->GetSelectedObj();
	if(NULL==obj || !obj->OnKeyDown(event))
		m_pTracks->OnKeyDown(event);
}

void CTimelineView::OnMouseEvent(wxMouseEvent& event)
{
	try {

		//int n = event.GetWheelRotation();
		//int d = event.GetWheelDelta();

	int x, y, xScrollUnits, yScrollUnits;
	GetViewStart(&x, &y);
	GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	IFloopyObj *obj = m_pTracks->GetChildAt(event.GetX() + x*xScrollUnits,
											event.GetY() + y*yScrollUnits);

	if( event.Dragging() && m_pSelectedObj )
	{
		wxRect rc = this->GetRect();
		rc.Offset(-rc.GetX(), -rc.GetY());
		if(!rc.Inside(event.GetX(), event.GetY()))
			return;

		if(m_pSelectedObj->GetType() == FLOOPY_REGION)
		{
			if(0 != m_ptPrev.x)
			{
				int dx = event.GetX() - m_ptPrev.x;
				m_pTracks->MoveSelectedRegions(dx);
			}
		}
		else // Border
		{
			if(0 != m_ptPrev.x)
			{
				int dx = event.GetX() - m_ptPrev.x;
				int dy = event.GetY() - m_ptPrev.y;
				m_pSelectedObj->Move(dx, dy);
			}
		}
		if(obj) // Outside of tracks
		{
			m_ptPrev.x = event.GetX();
			m_ptPrev.y = event.GetY();
		}
		//RefreshRulers();
		return;
	}
	else if(obj)
	{
		SetCursor( obj->GetCursor() );

		if(event.LeftDown())
		{
			m_pSelectedObj = obj;

			switch( obj->GetType() )
			{
			case FLOOPY_TRACK:
				{
					CTrack *track = (CTrack*)obj;
				
					if( !track->IsSelected() )
						m_pTracks->DeselectAllTracks();
					
					m_pTracks->DeselectAllRegions();

					m_pTracks->SetCaretPos(m_pTracks->GetCaretPos());

					int xScrollUnits=0, yScrollUnits=0;
					GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
					int xOrig=0, yOrig=0;
					GetViewStart(&xOrig, &yOrig);
					xOrig *= xScrollUnits;
					yOrig *= yScrollUnits;

					///////////////////////////////////////////
					// Add new region
					int x1 = event.GetX() + xOrig;
					CRegion *region = track->AddNewRegionAt(x1);
					m_pSelectedObj = region->GetBorder(false);
					//region->Refresh();
					///////////////////////////////////////////

					break;
				}
			case FLOOPY_REGION:
				{
					if( !event.ShiftDown() && !obj->IsSelected())
						m_pTracks->DeselectAllRegions();
					if( !obj->IsSelected() )
						obj->Select();

					m_pParamsDialog->Update();
					m_pPropsDialog->Update();

					return; // Don't move the caret
				}
			default:
				if( !obj->IsSelected() )
				{
					m_pTracks->DeselectAllTracks();
					m_pTracks->DeselectAllRegions();
					obj->Select();
				}
				return;
			}
		}
		else if(event.RightUp())
		{
			wxMenu *mnu = obj->GetMenu();
			if(mnu)
			{
				PopupMenu(mnu, event.GetPosition());
				return;
			}
		}
		else if( event.LeftUp() && m_pSelectedObj )
		{
			switch( m_pSelectedObj->GetType() )
			{
			case FLOOPY_TRACK:
				m_bDrag = false;
				break;
			case FLOOPY_REGION:
				m_pTracks->UpdateSelectedRegions();
				break;
			case FLOOPY_REGION_BORDER:
				{
					CRegion *region = (CRegion*)m_pSelectedObj->GetParent();
					
					if(region->GetWidth() <= 2)
					{
						CTrack *track = (CTrack*)region->GetParent();
						track->RemoveRegion( region );
					}
					else
						region->Update();

					m_pParamsDialog->Update();
					m_pPropsDialog->Update();

					m_pSelectedObj = NULL;
				}
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

			m_pTracks->SetCaretPos(m_pTracks->GetCaretPos());
		}
	}

	m_ptPrev.x = event.GetX();
	m_ptPrev.y = event.GetY();

	}
	catch(...)
	{
		wxLogTrace(_T("CTimelineView"), _T("OnMouseEvent exception"));
	}

	CCaretView::OnMouseEvent(event);

	if(event.LeftDown())
		m_pTracks->SetCursorPosition(m_pTracks->GetCaretPos());
}

void CTimelineView::OnMouseWheel(wxMouseEvent& event)
{
	// EVT_MOUSE_EVENTS( CTimelineView::OnMouseEvent )
	
	// Zameniti OnMouseEvent pojedinacnim eventima

	// EVT_LEFT_DOWN( CTimelineView::OnLeftDown )
	// EVT_LEFT_UP( CTimelineView::OnLeftUp )
	// EVT_LEFT_DCLICK( CTimelineView::OnLeftDClick )
	// EVT_MIDDLE_DOWN( CTimelineView::OnMiddleDown )
	// EVT_MIDDLE_UP( CTimelineView::OnMiddleUp )
	// EVT_MIDDLE_DCLICK( CTimelineView::OnMiddleDClick )
	// EVT_RIGHT_DOWN( CTimelineView::OnRightDown )
	// EVT_RIGHT_UP( CTimelineView::OnRightUp )
	// EVT_RIGHT_DCLICK( CTimelineView::OnRightDClick )
	// EVT_MOTION( CTimelineView::OnMouseMove )
	// EVT_ENTER_WINDOW( CTimelineView::OnEnterWindow )
	// EVT_LEAVE_WINDOW( CTimelineView::OnLeaveWindow )
	// EVT_MOUSEWHEEL( CTimelineView::OnMouseWheel )
}

void CTimelineView::ShowParamsDlg()
{
	IFloopyObj *obj = m_pTracks->GetSelectedObj();
	if(NULL == obj)
		obj = m_pTracks;

	m_pParamsDialog->InitParams( obj );
	m_pParamsDialog->Show( true );
}

void CTimelineView::ShowPropertiesDlg()
{
	IFloopyObj *obj = m_pTracks->GetSelectedObj();
	if(NULL == obj)
		obj = m_pTracks;

	m_pPropsDialog->InitProps( obj );
	m_pPropsDialog->Show( true );
}
