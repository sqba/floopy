
#include "engine/tracks.h"

void DrawAquaRect(wxDC& dc, wxRect& rc, int radius = 3)
{
	int left	= rc.GetX();
	int top		= rc.GetTop();
	int height	= rc.GetHeight();
	int width	= rc.GetWidth();
	int right	= left+width;

	if(width <= radius*2)
		return;

	wxPen oldpen = dc.GetPen();

	int border	= 2;//(IsSelected() ? 2 : 1);

	int edge = radius;
	int endEdge = top+radius;
	float rstep = 60.f / ((float)height / 3.f);
	float gstep = 20.f / ((float)height / 3.f);
	float r=180.f, g=190.f, b=225.f;
	for(int y=top+1; y<top+height/3; y++)
	{
		r -= rstep;
		g -= gstep;
		dc.SetPen( wxPen(wxColor(r, g, b), border) );
		if(y<endEdge)
		{
			dc.DrawLine(left+edge, y, right-edge*2, y);
			edge--;
		}
		else
			dc.DrawLine(left, y, right, y);
	}

	edge = 1;
	int startEdge = top+height-radius;
	rstep = 75.f / (((float)height / 3.f) * 2.f);
	gstep = 51.f / (((float)height / 3.f) * 2.f);
	r=111.f, g=161.f, b=225.f;
	for(y=top+height/3; y<top+height-1; y++)
	{
		r += rstep;
		g += gstep;
		dc.SetPen( wxPen(wxColor(r, g, b), border) );
		if(y>=startEdge)
		{
			dc.DrawLine(left+edge, y, right-edge*2, y);
			edge++;
		}
		else
			dc.DrawLine(left, y, right, y);
	}

	dc.SetPen( oldpen );
}
