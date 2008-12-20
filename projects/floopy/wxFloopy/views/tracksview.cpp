// TracksView.cpp: implementation of the CTracksView class.
//
//////////////////////////////////////////////////////////////////////

#include "tracksview.h"


//BEGIN_EVENT_TABLE(CTracksView, wxSplitterWindow)
//	EVT_KEY_DOWN( CTracksView::OnKeyDown )
//END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracksView::CTracksView(wxWindow* parent, CTracks *tracks)
 : wxSplitterWindow(parent, -1, wxDefaultPosition, wxSize(600, 400), wxSP_3D|wxSP_LIVE_UPDATE)
// : wxSplitterWindow(parent, -1, wxDefaultPosition, wxSize(600, 400), wxSP_NOBORDER|wxSP_LIVE_UPDATE)
{
	m_pTracks = tracks;

	m_pLabelsView = new CRulerView(this);
	CLabelsView *pLabelsView = new CLabelsView(m_pLabelsView, m_pTracks);
	m_pLabelsView->SetView( pLabelsView );

	m_pTimelineView = new CRulerView(this);
	CTimelineView *pTimelineView = new CTimelineView(m_pTimelineView, m_pLabelsView, m_pTracks);
	m_pTimelineView->SetView( pTimelineView );

	pLabelsView->SetTimelineView(m_pTimelineView);

	m_pTracks->SetTimelineView( m_pTimelineView );
	m_pTracks->SetLabelsView( m_pLabelsView );

    SplitVertically(m_pLabelsView, m_pTimelineView, 100);
}

CTracksView::~CTracksView()
{

}

void CTracksView::ShowParamsDlg()
{
	((CTimelineView*)m_pTimelineView->GetView())->ShowParamsDlg();
}

void CTracksView::ShowPropertiesDlg()
{
	((CTimelineView*)m_pTimelineView->GetView())->ShowPropertiesDlg();
}

void CTracksView::SetBackgroundColour(wxColour& color)
{
	m_pLabelsView->SetBackgroundColour( color );
	m_pTimelineView->SetBackgroundColour( color );
	wxWindow::SetBackgroundColour( color );
}

/*
void CTracksView::OnKeyDown(wxKeyEvent& event)
{
	event.Skip();
}
*/
