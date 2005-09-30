// TimeRuler.cpp: implementation of the CSampleRuler class.
//
//////////////////////////////////////////////////////////////////////

#include "SampleRuler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleRuler::CSampleRuler(CRulerView *parent, CTracks *tracks) : CHorizontalRuler( parent )
{
	m_pTracks = tracks;

	SetWindowStyle(wxNO_BORDER);
}

CSampleRuler::~CSampleRuler()
{

}

void CSampleRuler::OnDraw(wxDC &dc)
{
	drawFrame( dc );

	wxSize size = this->GetClientSize();

	int spp      = m_pTracks->GetSamplesPerPixel();
	int iMiddle  = size.GetHeight()/2;
	int iStep    = m_pTracks->CalcStep( MIN_DISTANCE );

	int iLineTop = 0;

	int iLineBottom1 = iMiddle - 1;
	int iLineBottom2 = iMiddle - (iMiddle/3)*2;
	int iLineBottom = iLineBottom1;

	dc.SetFont( *wxSWISS_FONT );

	int w=0, h=0;
	wxString csLabel("Sample");
	dc.GetTextExtent(csLabel, &w, &h);
	int iTextTop = iLineBottom - (h - h/3);

	for(int x=0; x<m_pTracks->GetWidth(); x+=iStep)
	{
		//if((x/iStep)%4 == 0)
		//Optimization: x%a == x&(a-1) for binary numbers
		if(((x/iStep)&3) == 0)
		{
			int sample = x * spp;
			csLabel.Printf("%d", sample);
			iLineBottom = iLineBottom1;
			dc.DrawText(csLabel, x+4, iTextTop);
		}
		else
		{
			iLineBottom = iLineBottom2;
		}


		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(x, iLineTop, x, iLineBottom);

		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(x+1, iLineTop, x+1, iLineBottom);
	}
}

/////////////////////////////////////////////////////////////////////////////
// drawFrame
//! Draws top and left white lines and bottom and right grey lines.
/////////////////////////////////////////////////////////////////////////////
void CSampleRuler::drawFrame(wxDC &dc)
{
	wxSize size = this->GetClientSize();

	int width = m_pTracks->GetWidth();
	int height = size.GetHeight();

	dc.SetPen(*wxWHITE_PEN);
	dc.DrawLine(0, 0, width, 0);				// top
	dc.DrawLine(0, 0, 0, size.GetHeight());		// left

	dc.SetPen(*wxMEDIUM_GREY_PEN);
	dc.DrawLine(width-1, 0, width-1, height);	// right
	dc.DrawLine(0, height-1, width, height-1);	// bottom
}
