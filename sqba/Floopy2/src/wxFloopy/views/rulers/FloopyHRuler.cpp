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


void CFloopyHRuler::OnDrawBG(wxDC &dc)
{
	// Background and frame
	wxSize size = this->GetClientSize();
	int left=0, top=0;
/*
	int xScrollUnits=0, yScrollUnits=0;
	m_owner->GetViewStart(&left, &top);
	m_owner->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
	left *= xScrollUnits;
	int width = m_owner->GetClientSize().GetWidth();
*/
	int width = size.GetWidth();
	int height = size.GetHeight();

	DrawRect3D(dc, wxRect(left, 0, width, height));
	//DrawAquaRect(dc, wxRect(0, 0, width, height), 0);
}
