// FloopyView.cpp: implementation of the CFloopyView class.
//
//////////////////////////////////////////////////////////////////////

#include "FloopyView.h"


BEGIN_EVENT_TABLE(CFloopyView, wxSplitterWindow)
//	EVT_KEY_DOWN( CFloopyView::OnKeyDown )
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFloopyView::CFloopyView(wxWindow* parent, CTracks *tracks)
 : wxSplitterWindow(parent, -1, wxDefaultPosition, wxSize(600, 400), wxSP_3D|wxSP_LIVE_UPDATE)
{
	m_pTracks = tracks;

	m_pLabelsView = new CRulerView(this);
	CLabelsView *pLabelsView = new CLabelsView(m_pLabelsView, m_pTracks);
	m_pLabelsView->SetView( pLabelsView );

	m_pTracksView = new CRulerView(this);
	CTracksView *pTracksView = new CTracksView(m_pTracksView, m_pLabelsView, m_pTracks);
	m_pTracksView->SetView( pTracksView );

	pLabelsView->SetTracksView(m_pTracksView);

	m_pTracks->SetTracksView( m_pTracksView );
	m_pTracks->SetLabelsView( m_pLabelsView );

    SplitVertically(m_pLabelsView, m_pTracksView, 100);
}

CFloopyView::~CFloopyView()
{

}

bool CFloopyView::Open(char *filename)
{
	if( m_pTracks->Open(filename) )
	{
		wxString str;
		str.Printf("Floopy! - %s", filename);
		SetTitle( str );
		//m_pTracksView->RefreshRulers();
		m_pTracksView->SetFocus();

		UINT r=0, g=0, b=0;
		IFloopySoundEngine *engine = (IFloopySoundEngine*)m_pTracks->GetInput();
		if( engine->GetColor(&r, &g, &b) )
		{
			wxColor color = wxColor(r, g, b);

			m_pLabelsView->SetBackgroundColour( color );
			m_pTracksView->SetBackgroundColour( color );
		}
		return true;
	}
	return false;
}

/*
void CFloopyView::OnKeyDown(wxKeyEvent& event)
{
	event.Skip();
}
*/