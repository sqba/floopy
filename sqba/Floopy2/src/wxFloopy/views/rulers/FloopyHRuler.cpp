// FloopyHRuler.cpp: implementation of the CFloopyHRuler class.
//
//////////////////////////////////////////////////////////////////////

#include "FloopyHRuler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFloopyHRuler::CFloopyHRuler(CRulerView *parent, CTracks *tracks)
 : CHorizontalRuler( parent )
{
	m_pTracks = tracks;

	SetWindowStyle(wxNO_BORDER);
}

CFloopyHRuler::~CFloopyHRuler()
{

}


void CFloopyHRuler::OnDraw(wxDC &dc)
{
	// Background and frame
	wxSize size = this->GetClientSize();
	int width = m_pTracks->GetWidth();
	int height = size.GetHeight();
	DrawRect3D(dc, wxRect(0, 0, width, height));
	//DrawAquaRect(dc, wxRect(0, 0, width, height), 0);
}
