
#include "engine/tracks.h"

void DrawAquaRect(wxDC& dc, wxRect& rc)
{
	int top		= rc.GetTop();
	int height	= rc.GetHeight();
	int width	= rc.GetWidth();

	int border	= 2;//(IsSelected() ? 2 : 1);

	float rstep = 60.f / ((float)height / 3.f);
	float gstep = 20.f / ((float)height / 3.f);
	float r=180.f, g=190.f, b=225.f;
	for(int y=top; y<top+height/3; y++)
	{
		r -= rstep;
		g -= gstep;
		dc.SetPen( wxPen(wxColour(r, g, b), border) );
		dc.DrawLine(0, y, width, y);
	}

	rstep = 75.f / (((float)height / 3.f) * 2.f);
	gstep = 51.f / (((float)height / 3.f) * 2.f);
	r=111.f, g=161.f, b=225.f;
	for(y=top+height/3; y<top+height; y++)
	{
		r += rstep;
		g += gstep;
		dc.SetPen( wxPen(wxColour(r, g, b), border) );
		dc.DrawLine(0, y, width, y);
	}
}
