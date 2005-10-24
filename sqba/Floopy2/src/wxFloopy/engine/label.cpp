
#include "tracks.h"
#include "track.h"
#include "label.h"
#include "../util/util.h"



/////////////////////////////////////////////////////////////////////
// CLoopButton functions
/////////////////////////////////////////////////////////////////////
void CLabel::CLoopButton::DrawFore(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	wxPen pen( getTrack()->IsLooped() ? *wxBLACK : *wxLIGHT_GREY );
	pen.SetWidth(1);
	dc.SetPen( pen );

	wxBrush brush(getTrack()->GetColor(), wxSOLID);
	dc.SetBrush(brush);

	int left   = rc.GetX();
	int top    = rc.GetTop();
	int width  = rc.GetWidth();
	int height = rc.GetHeight();

	dc.DrawCircle(left+width/2, top+height/2, height/3);

	dc.SetPen(oldpen);
	dc.SetBrush( oldBrush );
}




/////////////////////////////////////////////////////////////////////
// CCacheButton functions
/////////////////////////////////////////////////////////////////////
void CLabel::CCacheButton::DrawFore(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	wxPen pen( *wxLIGHT_GREY );
	pen.SetWidth(1);
	dc.SetPen( pen );

	//wxBrush brush(m_color, (IsSelected() ? wxCROSSDIAG_HATCH : wxSOLID));
	wxBrush brush(getTrack()->GetColor(), wxSOLID);
	dc.SetBrush(brush);

	// Draw background
	int left   = rc.GetX();
	int top    = rc.GetTop();
	int width  = rc.GetWidth();
	int height = rc.GetHeight();

	//dc.DrawCircle(left+width/2, top+height/2, height/3);

	dc.SetPen(oldpen);
	dc.SetBrush( oldBrush );
}






/////////////////////////////////////////////////////////////////////
// CLabel functions
/////////////////////////////////////////////////////////////////////

CLabel::CLabel(CTrack *track) : IFloopyObj(track)
{
//	m_pButtonLoop = new CLoopButton(track);
//	m_pButtonCache = new CCacheButton(track);
	m_pPathCtrl = new CPathCtrl(this, track->GetInput());
//	m_pPathCtrl = new CPathCtrl(track->GetSource());
}

CLabel::~CLabel()
{
//	delete m_pButtonLoop;
//	delete m_pButtonCache;
	delete m_pPathCtrl;
}

void CLabel::DrawBG(wxDC& dc, wxRect& rc)
{
	CTrack *track = getTrack();
	m_rcLabel = rc;

	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	wxPen pen( *wxMEDIUM_GREY_PEN );
	pen.SetWidth(1);
	dc.SetPen( pen );

	//wxBrush brush(m_color, (IsSelected() ? wxCROSSDIAG_HATCH : wxSOLID));
	wxBrush brush(track->GetBGColor(), wxSOLID);
	dc.SetBrush(brush);

	int left, top, width, height;

	if(true)
	{
		// Draw background
		left   = 1;//m_nLevel*4+2;
		top    = rc.GetTop()+1;
		width  = rc.GetWidth()-1;//left-3;
		height = rc.GetHeight()-2;
		//dc.DrawRoundedRectangle(left, top, width, height, 4);
		//DrawRect3D(dc, wxRect(left, top, width, height));
		DrawRect3D(dc, rc);
	}
	else
	{
		// Draw aqua background
		left   = 0;
		top    = rc.GetTop();
		width  = rc.GetWidth()-left;
		height = rc.GetHeight();
		//dc.DrawRectangle(left, top, width, height);
		DrawAquaRect(dc, wxRect(left+1, top+1, width-2, height-2), 4);
	}

	dc.SetTextForeground( track->GetForeColor() );
	wxFont font = dc.GetFont();
	font.SetWeight(IsSelected() ? wxBOLD : wxNORMAL);
	//font.SetPointSize(GetHeight() / 4);
	font.SetPointSize( 9 );
	dc.SetFont(font);
	//wxFont font(12, wxDEFAULT, wxITALIC, (IsSelected() ? wxBOLD : wxNORMAL));
	//dc.SetFont(font);
	////dc.SetFont((IsSelected() ? *wxITALIC_FONT : *wxNORMAL_FONT));

	// Get text dimensions
	wxCoord w=0, h=0;
	wxString csName;
	track->GetName(csName);
	dc.GetTextExtent(csName, &w, &h);

	/*int ptSize = GetHeight() / 2;
	// Get text dimensions
	wxCoord w=0, h=0;
	do {
		font.SetPointSize(ptSize);
		dc.GetTextExtent(m_name, &w, &h);
		ptSize--;
	} while (w>width && ptSize > 5);*/


	// Draw text
	int x = left + 5;//width/2 - w/2;
	int y = (rc.GetTop() + (rc.GetHeight()/4) - h/2);
	dc.DrawText( csName, x, y );
	m_rcLabel.SetHeight(rc.GetHeight());

/*
	int n = rc.GetHeight()/2-2;
	if(n > 20)
		n = 20;
	drawLoopSign(dc,  wxRect(5, top+height-n-2, n, n));
	drawCacheSign(dc, wxRect(n+5+1, top+height-n-2, n, n));
*/
	wxRect rcTmp = rc;
	rcTmp.SetY(top+height - height/3);
	rcTmp.SetHeight(height/3);
	m_pPathCtrl->DrawBG(dc, rcTmp);


	dc.SetPen(oldpen);
	dc.SetBrush( oldBrush );
}

void CLabel::DrawFore(wxDC& dc, wxRect& rc)
{
	int top    = rc.GetTop()+1;
	int height = rc.GetHeight()-2;

	wxRect rcTmp = rc;
	rcTmp.SetY(top+height - height/3);
	rcTmp.SetHeight(height/3);
	m_pPathCtrl->DrawFore(dc, rcTmp);
}

/////////////////////////////////////////////////////////////////////////////
// HitTest
//! Returns true if the vertical coordinate y is in the track.
//! \param y [in] vertical coordinate
//! \return true if the vertical coordinate y is in the track
/////////////////////////////////////////////////////////////////////////////
bool CLabel::HitTest(int y)
{
	return m_rcLabel.Inside(1, y);
}

void CLabel::drawLoopSign(wxDC& dc, wxRect& rc)
{
//	m_pButtonLoop->DrawBG(dc, rc);
//	m_pButtonLoop->DrawFore(dc, rc);
}

void CLabel::drawCacheSign(wxDC& dc, wxRect& rc)
{
//	m_pButtonCache->DrawBG(dc, rc);
//	m_pButtonCache->DrawFore(dc, rc);
}

void CLabel::Select(bool selected)
{
	IFloopyObj::Select( selected );
	GetParent()->Select( selected );
	Refresh();
}

IFloopyObj *CLabel::GetChildAt(int x, int y)
{
	IFloopyObj *obj = m_pPathCtrl->GetChildAt(x, y);

	if(NULL != obj)
		return obj;
/*
	CTrack *track = (CTrack*)GetParent();
	int bottom = track->GetTop() + track->GetHeight();
	if(y>=bottom-1 && y<=bottom+1)
		return track->GetChildAt(x, y);
*/
	return this;
}

int CLabel::GetChildCount()
{
	return 1;
}

IFloopyObj *CLabel::GetChild(int index)
{
	return m_pPathCtrl;
}

void CLabel::Move(int dx, int dy)
{

}

IFloopyObj *CLabel::GetSelectedObj()
{
	IFloopyObj *obj = m_pPathCtrl->GetSelectedObj();

	if(NULL != obj)
		return obj;

	if(NULL==obj && GetParent()->IsSelected())
		return this;

	return NULL;
}
