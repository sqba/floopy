// TimeRuler.cpp: implementation of the CTimeRuler class.
//
//////////////////////////////////////////////////////////////////////

#include "TimeRuler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTimeRuler::CTimeRuler(CRulerView *parent, CTracks *tracks) : CHorizontalRuler( parent )
{
	m_pTracks = tracks;

	SetWindowStyle(wxNO_BORDER);
}

CTimeRuler::~CTimeRuler()
{

}

void CTimeRuler::OnDraw(wxDC &dc)
{
	drawFrame( dc );

	wxSize size = this->GetClientSize();

	int pix      = m_pTracks->GetPixelsPerSecond();
	int iMiddle  = size.GetHeight()/2;
	int iStep    = m_pTracks->CalcStep( MIN_DISTANCE );

	int iLineTop1 = iMiddle + 1;
	int iLineTop2 = iMiddle + (iMiddle/3)*2;

	int iLineTop = 0;
	int iLineBottom = size.GetHeight();

	int right = m_pTracks->GetWidth();

	dc.SetFont( *wxSWISS_FONT );

	int w=0, h=0;
	wxString csLabel("00:00:000");
	dc.GetTextExtent(csLabel, &w, &h);
	int iTextTop = iLineTop1 - (h - h/3);

	for(int x=0; x<right; x+=iStep)
	{
		//if((x/iStep)%4 == 0)
		//Optimization: x%a == x&(a-1) for binary numbers
		if(((x/iStep)&3) == 0)
		{
			iLineTop = iLineTop1;

			if(x+4+w < right)
			{
				float fSeconds = (float)x / (float)pix;
				formatTime(fSeconds, csLabel);
				dc.DrawText(csLabel, x+4, iTextTop);
			}
		}
		else
		{
			iLineTop = iLineTop2;
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
void CTimeRuler::drawFrame(wxDC &dc)
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

/////////////////////////////////////////////////////////////////////////////
// formatTime
//! Formats a float representing time in seconds into string (CString).
//! @param fSec [in] time in seconds
//! @param csTime [out] reference to the wxString class.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTimeRuler::formatTime(float fSec, wxString &csTime)
{
	float min = fSec>60.f ? fSec/60.f : 0.f;
	float sec = min>1.f ? fSec-min*60.f : fSec;
	float ms  = sec*1000.f - (int)sec*1000;

	csTime.Printf("%2.2d:%2.2d:%3.3d", (int)min, (int)sec, (int)ms);
}
