// GridView.cpp: implementation of the CGridView class.
//
//////////////////////////////////////////////////////////////////////

#include "GridView.h"
//#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridView::CGridView(wxWindow* parent, CTracks *tracks)
 : wxScrolledWindow(parent)
{
	m_bDrawVGrid = TRUE;
	m_pTracks    = tracks;

	//SetWindowStyle(wxNO_BORDER);
	SetWindowStyle(wxSIMPLE_BORDER);
	//SetWindowStyle(wxSUNKEN_BORDER);
	SetBackgroundColour(*wxWHITE);
	//SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
}

CGridView::~CGridView()
{

}

void CGridView::OnDraw(wxDC &dc)
{
	wxScrolledWindow::OnDraw(dc);

	if(m_bDrawVGrid) {
		//int pps    = m_pTracks->GetPixelsPerSecond();
		int length = m_pTracks->GetWidth();
		int height = m_pTracks->GetHeight();
		int iStep  = m_pTracks->CalcStep(MIN_DISTANCE);

		wxPen oldpen = dc.GetPen();
		
		for(int n=0; n<length; n+=iStep) {
			bool full = ((n/iStep)%4 == 0);
			dc.SetPen(full ? *wxMEDIUM_GREY_PEN : *wxLIGHT_GREY_PEN);
			dc.DrawLine(n, 0, n, height);
		}
		
		dc.SetPen(oldpen);
	}
}
