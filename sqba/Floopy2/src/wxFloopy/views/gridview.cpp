// GridView.cpp: implementation of the CGridView class.
//
//////////////////////////////////////////////////////////////////////

#include "GridView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridView::CGridView(CRulerView* parent, CTracks *tracks) : CMainView(parent, tracks)
{
	m_bDrawVGrid = true;
}

CGridView::~CGridView()
{

}

void CGridView::OnDraw(wxDC &dc)
{
//	CMainView::OnDraw(dc);

	if(m_bDrawVGrid)
	{
		int length = m_pTracks->GetWidth();
		int height = m_pTracks->GetHeight();
		int iStep  = m_pTracks->CalcStep(MIN_DISTANCE);

		wxPen oldpen = dc.GetPen();
		
		for(int n=0; n<length; n+=iStep)
		{
			bool full = ((n/iStep)%4 == 0);
			dc.SetPen(full ? *wxMEDIUM_GREY_PEN : *wxLIGHT_GREY_PEN);
			dc.DrawLine(n, 0, n, height);
		}
		
		dc.SetPen(oldpen);
	}
}
