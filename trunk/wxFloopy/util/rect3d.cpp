

#include <wx/settings.h>
#include "util.h"


/////////////////////////////////////////////////////////////////////////////
// DrawRect3D
//! Draws top and left white lines and bottom and right grey lines.
/////////////////////////////////////////////////////////////////////////////
void DrawRect3D(wxDC& dc, wxRect& rc)
{
	int left	= rc.GetLeft();
	int top		= rc.GetTop();
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();
	int right	= left + width;
	int bottom	= top + height;

	wxPen oldpen = dc.GetPen();
	wxBrush oldbrush = dc.GetBrush();

	// Background
	dc.SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), wxSOLID) );
	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle(left, top, width, height);

	dc.SetPen(*wxWHITE_PEN);
	dc.DrawLine(left, top, right, top);				// top
	dc.DrawLine(left, top, left, bottom);			// left

	dc.SetPen(*wxMEDIUM_GREY_PEN);
	dc.DrawLine(right-1, top, right-1, bottom);		// right
	dc.DrawLine(left, bottom-1, right, bottom-1);	// bottom

	dc.SetPen( oldpen );
	dc.SetBrush( oldbrush );
}
