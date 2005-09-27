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

	//SetWindowStyle(wxSIMPLE_BORDER);
	//SetWindowStyle(wxRAISED_BORDER);
	SetWindowStyle(wxNO_BORDER);
	//SetWindowStyle(wxSUNKEN_BORDER);
	//SetWindowStyle(wxDOUBLE_BORDER);
}

CTimeRuler::~CTimeRuler()
{

}

void CTimeRuler::OnDraw(wxDC &dc)
{
	//dc.SetFont(wxGetApp().m_canvasFont);
	//wxFont font(8,wxDEFAULT,wxNORMAL,wxNORMAL);
	dc.SetFont(*wxSWISS_FONT);
	//dc.SetPen(*wxBLACK_PEN);
	//wxCoord nCharHeight = dc.GetCharHeight();
	//wxCoord nCharWidth  = dc.GetCharWidth();

	//wxRect rc = GetRect();

	wxSize size = this->GetClientSize();

	wxString csLabel;
	int pix      = m_pTracks->GetPixelsPerSecond();
	int iLineTop = 0, iLineBottom = 0;
	int iMiddle  = size.GetHeight()/2;
	int iStep    = m_pTracks->CalcStep( MIN_DISTANCE );

	//wxPoint pos = GetPosition();
	wxSize vSize = GetSize();

	for(int x=0 ; x<m_pTracks->GetWidth() ; x+=iStep)
	{
		//if(x>=(-m_xOrigin)) {
			if((x/iStep)%4 == 0) {
				float fPos = (float)x / (float)pix;
				FormatTime(fPos, csLabel);
				iLineTop = iMiddle + 1;
				iLineBottom = size.GetHeight();//rc.GetBottom();
				//dc.DrawText(csLabel, x+4, iMiddle-nCharHeight/2);
				dc.DrawText(csLabel, x+4, 0);
			}
			else {
				iLineTop = iMiddle + 5;
				iLineBottom = size.GetHeight();//rc.GetBottom();
			}


			dc.SetPen(*wxMEDIUM_GREY_PEN);
			dc.DrawLine(x, iLineTop, x, iLineBottom);

			dc.SetPen(*wxWHITE_PEN);
			dc.DrawLine(x + 1, iLineTop, x + 1, iLineBottom);
		//}
	}

	dc.SetPen(*wxMEDIUM_GREY_PEN);
	dc.DrawLine(0, size.GetHeight()-2, m_pTracks->GetWidth(), size.GetHeight()-2);

	dc.SetPen(*wxWHITE_PEN);
	dc.DrawLine(0, size.GetHeight()-1, m_pTracks->GetWidth(), size.GetHeight()-1);
}

/////////////////////////////////////////////////////////////////////////////
// FormatTime
//! Formats a float representing time in seconds into string (CString).
//! @param fSec [in] time in seconds
//! @param csTime [out] reference to the wxString class.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTimeRuler::FormatTime(float fSec, wxString &csTime)
{
	int min = (int)(fSec / 60);
	int sec = (int)(fSec - (min * 60));
	int ms  = (int)((fSec - (float)((min * 60) + sec)) * 1000);

	csTime.Printf("%2.2d:%2.2d:%3.3d", min, sec, ms);
}
