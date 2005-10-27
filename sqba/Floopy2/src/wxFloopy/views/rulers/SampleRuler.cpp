// TimeRuler.cpp: implementation of the CSampleRuler class.
//
//////////////////////////////////////////////////////////////////////

#include "SampleRuler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSampleRuler::CSampleRuler(CRulerView *parent, CTracks *tracks)
 : CFloopyHRuler( parent, tracks )
{

}

CSampleRuler::~CSampleRuler()
{

}

void CSampleRuler::OnDraw(wxDC &dc)
{
	// Background and frame
	wxSize size = this->GetClientSize();
	//int width = m_pTracks->GetWidth();
	//int height = size.GetHeight();
	//DrawRect3D(dc, wxRect(0, 0, width, height));
	CFloopyHRuler::OnDraw(dc);

	int spp      = m_pTracks->GetSamplesPerPixel();
	int iMiddle  = size.GetHeight()/2;
	int iStep    = m_pTracks->CalcStep( MIN_DISTANCE );

	int iLineTop = 0;

	int iLineBottom1 = iMiddle - 1;
	int iLineBottom2 = iMiddle - (iMiddle/3)*2;
	int iLineBottom = iLineBottom1;

	int right = m_pTracks->GetWidth();

	wxFont oldFont = dc.GetFont();

	dc.SetFont( *wxSWISS_FONT );

	int w=0, h=0;
	wxString csLabel("000000");
	dc.GetTextExtent(csLabel, &w, &h);
	int iTextTop = iLineBottom - h/3;

	for(int x=0; x<right; x+=iStep)
	{
		//if((x/iStep)%4 == 0)
		//Optimization: x%a == x&(a-1) for binary numbers
		if(((x/iStep)&3) == 0)
		{
			iLineBottom = iLineBottom1;

			if(x+4+w < right)
			{
				int sample = x * spp;
				csLabel.Printf("%d", sample);
				dc.DrawText(csLabel, x+4, iTextTop);
			}
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

	dc.SetFont(oldFont);
}
