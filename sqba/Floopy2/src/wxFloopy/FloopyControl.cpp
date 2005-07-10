// FloopyControl.cpp: implementation of the CFloopyControl class.
//
//////////////////////////////////////////////////////////////////////

#include "FloopyControl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFloopyControl::CFloopyControl(IFloopyObj *parent) : IFloopyObj(parent)
{

}

CFloopyControl::~CFloopyControl()
{

}

void CFloopyControl::DrawBG(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	wxPen pen( *wxLIGHT_GREY );
	pen.SetWidth(2);
	dc.SetPen( pen );

	wxBrush brush(GetParent()->GetColour(), wxSOLID);
	dc.SetBrush(brush);

	dc.DrawRoundedRectangle(rc.GetX(), rc.GetTop(),
		rc.GetWidth(), rc.GetHeight(), 2);

	dc.SetPen(oldpen);
	dc.SetBrush( oldBrush );
}
