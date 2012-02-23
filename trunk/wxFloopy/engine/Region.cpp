// Region.cpp: implementation of the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "../globals.h"
#include "tracks.h"
#include "track.h"
#include "region.h"
#include "parameter.h"
#include "regiondisplay.h"
#include "../util/util.h"
#include "actionhistory.h"

//IMPLEMENT_DYNAMIC_CLASS(CRegion, IFloopyObj)

//WX_DEFINE_LIST(ParameterList);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegion::CRegion(CTrack *track, UINT start, UINT end) : IFloopyObj(track)
{
	m_iStartSample		= start;
	m_iEndSample		= end;

	m_pLeftBorder		= new CBorder(this, true);
	m_pRightBorder		= new CBorder(this, false);

	m_pOffsetBar		= new COffsetBar(this);
	m_bShowOffsetBar	= true;

	m_iPrevStart		= m_iPrevEnd = -1;

	m_bEdit				= false;

	m_iStartOffset		= -1;
	m_iLengthNotLooped	= 0;

	m_pDisplay			= new CRegionDisplay(this);

	m_pParameters		= new CParameters(this);

	createMenu();

	Invalidate();

	wxLog::AddTraceMask(_T("CRegion"));
}

CRegion::~CRegion()
{
	delete m_pLeftBorder;
	delete m_pRightBorder;
	delete m_pOffsetBar;
	//delete m_pMenu; // !Deleted in ~IFloopyObj()
//	WX_CLEAR_LIST(ParameterList, m_Parameters);
	if(m_pDisplay)
		delete m_pDisplay;
	delete m_pParameters;
}

void CRegion::createMenu()
{
	m_pMenu = new CMenu( this );
	m_pMenu->Append(_T("&Copy"), NULL);
	m_pMenu->Append(_T("&Remove"), remove);
	m_pMenu->Append(_T("&Properties"), NULL);
	m_pMenu->Append(_T("&Set track source"), NULL);
}

void CRegion::Remove()
{
	Refresh();

	if(m_bEdit)
	{
		IFloopySoundInput *track = getTrack()->GetTrack();
		assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
		assert( track->ResetParamAt(m_iPrevEnd,   TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
		float value = 0.f;
		if(track->GetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, &value))
			assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, value) );
	}
	else
	{
		IFloopySoundInput *track = getTrack()->GetTrack();
		assert( track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
		assert( track->ResetParamAt(m_iEndSample,   TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
		float value = 0.f;
		if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
			assert( track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value) );
	}

	if(m_iEndSample - m_iStartSample > getTracks()->GetSamplesPerPixel())
	{
		Invalidate();
		Refresh();

		getTrack()->InvalidateRegions( this );
	}
}

void CRegion::remove(IFloopyObj *evt)
{
//	CRegion *region = (CRegion*)evt;
//	CTrack *track = region->getTrack();
//	track->RemoveRegion( region );
}

/**
 * Returns the original source length, before the looping component.
 */
int CRegion::getLengthNotLooped()
{
	int len = getTrack()->GetSource()->GetSize();
	if( getTrack()->IsLooped() )
	{
		IFloopySoundFilter *loop = (IFloopySoundFilter*)getTrack()->FindComponentByName("loop");
		if(loop)
		{
			IFloopySoundInput *src = loop->GetSource();
			if(src)
				len = src->GetSize();
		}
	}
	return len;
}
/*
void CRegion::drawFrame(wxDC& dc, wxRect& rc)
{
	int start = getStartOffset();
	if(start == -1)
		start = 0;
	int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();
	start /= nSamplesPerPixel;

	int origLen = getLengthNotLooped() / nSamplesPerPixel;

	int loopCount = floor((float)rc.GetWidth() / (float)origLen);

//	dc.DrawRoundedRectangle(rc.GetLeft(), rc.GetTop(), rc.GetWidth(), rc.GetHeight(), 3);

	int left = rc.GetLeft();
	int right = left + rc.GetWidth();

	int y1 = rc.GetTop();
	int y2 = rc.GetBottom();

	dc.DrawLine(left, y1, left, y2);	// Vertical left

	int prev = rc.GetLeft();
	for(int i=0; i<=loopCount; i++)
	{
		int l = origLen;

		if(i==0 && start!=0)
			l = origLen-start;

		int x1 = prev;
		int x2 = prev + l;

		if(prev+origLen < right)
		{
			dc.DrawLine(x1, y1, x2-3, y1);	// Horizontal top
			dc.DrawLine(x2-3, y1, x2, y1+6);// Diagonal down
			dc.DrawLine(x2, y1+6, x2, y1);	// Vertical up

			dc.DrawLine(x1, y2, x2-3, y2);	// Horizontal bottom
			dc.DrawLine(x2-3, y2, x2, y2-6);// Diagonal up
			dc.DrawLine(x2, y2-6, x2, y2);	// Vertical down

			prev += l;
		}
		else
		{
			dc.DrawLine(x1, y1, right, y1);	// Horizontal top
			dc.DrawLine(x1, y2, right, y2);	// Horizontal bottom
		}

	}

	dc.DrawLine(right, y1, right, y2);	// Vertical right
}
*/
/*
void CRegion::drawFrame(wxDC& dc, wxRect& rc)
{
	int start = m_iStartOffset;
	if(start == -1)
	{
		dc.DrawRoundedRectangle(rc.GetLeft(), rc.GetTop(), rc.GetWidth(), rc.GetHeight(), 3);
		return;
	}

	int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();
	start /= nSamplesPerPixel;

	// Ovo nije dovoljno precizno!!!
	int origLen = ceil((float)m_iLengthNotLooped / (float)nSamplesPerPixel);

	int loopCount = floor((float)rc.GetWidth() / (float)origLen);

	int left  = rc.GetLeft();
	int right = left + rc.GetWidth();

	int y1 = rc.GetTop();
	int y2 = rc.GetBottom();

	int n=0;
	wxPoint *points = new wxPoint[(loopCount+1)*6*2+4];
	wxPoint *pointsBottom = new wxPoint[(loopCount+1)*6];
	int b=0;

	// Vertical left
	points[n++] = wxPoint(left, y2);
	points[n++] = wxPoint(left, y1);

	int prev = rc.GetLeft();

	int width = rc.GetWidth();
	int end = start+width;
	//int pos = left-start;

	for(int x=left; x<=right; x++)
	{
		int l=3;

		int x1 = prev;
		int x2 = prev + l;

		if(x>left && (x+start)%origLen==0)
		// Trick: x % a = x & (a - 1) for binary numbers
		//if((i & (origLen-1)) == 0)
		{
			// Horizontal top
			points[n++] = wxPoint(x1, y1);
			points[n++] = wxPoint(x2-3, y1);
			// Diagonal down
			points[n++] = wxPoint(x2-3, y1);
			points[n++] = wxPoint(x2, y1+6);
			// Vertical up
			points[n++] = wxPoint(x2, y1+6);
			points[n++] = wxPoint(x2, y1);

			// Horizontal bottom
			pointsBottom[b++] = wxPoint(x1, y2);
			pointsBottom[b++] = wxPoint(x2-3, y2);
			// Diagonal up
			pointsBottom[b++] = wxPoint(x2-3, y2);
			pointsBottom[b++] = wxPoint(x2, y2-6);
			// Vertical down
			pointsBottom[b++] = wxPoint(x2, y2-6);
			pointsBottom[b++] = wxPoint(x2, y2);

			prev = x2;
		}
		else if(x == right-1)
		{
			// Horizontal top
			points[n++] = wxPoint(x1, y1);
			points[n++] = wxPoint(right, y1);

			// Horizontal bottom
			pointsBottom[b++] = wxPoint(x1, y2);
			pointsBottom[b++] = wxPoint(right, y2);
		}
	}

	// Vertical right
	points[n++] = wxPoint(right, y1);
	points[n++] = wxPoint(right, y2);

	for(int i=b; i>0; i--)
		points[n++] = pointsBottom[i-1];

	dc.DrawPolygon(n, points);

	delete points;
	delete pointsBottom;
}
*/

/**
 * Draws region frame with loop end markers.
 * @param dc
 * @param rc
 * @return void
 */
void CRegion::drawFrame(wxDC& dc, wxRect& rc)
{
	int start = m_iStartOffset;
	if(start==-1 || SIZE_VARIABLE==m_iLengthNotLooped)
	{
		dc.DrawRoundedRectangle(rc.GetLeft(), rc.GetTop(), rc.GetWidth(), rc.GetHeight(), 3);
		return;
	}

	int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();
	start /= nSamplesPerPixel;

	// Ovo nije dovoljno precizno!!!
	int origLen = ceil((float)m_iLengthNotLooped / (float)nSamplesPerPixel);

	int loopCount = floor((float)rc.GetWidth() / (float)origLen);

	int left  = rc.GetLeft();
	int right = left + rc.GetWidth();

	int y1 = rc.GetTop();
	int y2 = rc.GetBottom();

	int n=0;
	wxPoint *points			= new wxPoint[(loopCount+1)*6*2+4];
	wxPoint *pointsBottom	= new wxPoint[(loopCount+1)*6];
	int b=0;

	// Vertical left
	points[n++] = wxPoint(left, y2);
	points[n++] = wxPoint(left, y1);

	int prev = rc.GetLeft();
	for(int i=0; i<=loopCount; i++)
	{
		int l = origLen;

		if(i==0 && start!=0)
			l = origLen - start;

		if(l < 0)
			l = 0;

		int x1 = prev;
		int x2 = prev + l;

		if(x2<right && x1!=x2)
		{
			// Horizontal top
			points[n++] = wxPoint(x1, y1);
			points[n++] = wxPoint(x2-3, y1);
			// Diagonal down
			points[n++] = wxPoint(x2-3, y1);
			points[n++] = wxPoint(x2, y1+6);
			// Vertical up
			points[n++] = wxPoint(x2, y1+6);
			points[n++] = wxPoint(x2, y1);

			// Horizontal bottom
			pointsBottom[b++] = wxPoint(x1, y2);
			pointsBottom[b++] = wxPoint(x2-3, y2);
			// Diagonal up
			pointsBottom[b++] = wxPoint(x2-3, y2);
			pointsBottom[b++] = wxPoint(x2, y2-6);
			// Vertical down
			pointsBottom[b++] = wxPoint(x2, y2-6);
			pointsBottom[b++] = wxPoint(x2, y2);

			prev = x2-1;
		}
		else
		{
			// Horizontal top
			points[n++] = wxPoint(x1, y1);
			points[n++] = wxPoint(right, y1);

			// Horizontal bottom
			pointsBottom[b++] = wxPoint(x1, y2);
			pointsBottom[b++] = wxPoint(right, y2);
		}

	}

	// Vertical right
	points[n++] = wxPoint(right, y1);
	points[n++] = wxPoint(right, y2);

	for(int i=b; i>0; i--)
		points[n++] = pointsBottom[i-1];

	dc.DrawPolygon(n, points);

	delete points;
	delete pointsBottom;
}

/////////////////////////////////////////////////////////////////////////////
// DrawBG
//! Draws region's background.
//! \param dc [in] reference to the device context
//! \param rc [in] track's rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::DrawBG(wxDC& dc, wxRect& rc)
{
	int left=0, right=0;
	calcPos(&left, &right);

	int width = right - left;

	m_rcRegion = rc;
	m_rcRegion.SetX(left);
	m_rcRegion.SetWidth(width);

	wxPen oldpen = dc.GetPen();
	wxBrush oldbrush = dc.GetBrush();

	//int border = (IsSelected() ? 2 : 1);
	int border = 1;

	//dc.SetPen(wxPen(*wxGREY, border));
	//dc.SetPen(wxPen((GetReset()?*wxBLACK:*wxLIGHT_GREY), border));
	dc.SetPen( *wxGREY_PEN );

	//wxBrush brush(GetColor(), wxSOLID);
	wxBrush brush(GetBGColor(), wxSOLID);

	if(DRAW_TRANSPARENT_REGION)
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
	else
		dc.SetBrush(brush);

	int top    = rc.GetTop() + 1;
	int height = rc.GetHeight() - 2;

	if(m_bShowOffsetBar && this->GetHeight()>2*m_pOffsetBar->GetHeight())
	{
		int offsetBarHeight = m_pOffsetBar->GetHeight();
		wxRect rcOffsetBar(left, top, width, offsetBarHeight);
		m_pOffsetBar->DrawBG(dc, rcOffsetBar);

		top += offsetBarHeight;
		height -= offsetBarHeight;
	}

	wxRect rcFrame(left, top, width, height);
	drawFrame(dc, rcFrame);
	//dc.DrawRoundedRectangle(left, top, width, height, 3);
	//DrawAquaRect(dc, wxRect(left+1, top+1, width-2, height-2), 3);

	dc.SetPen(oldpen);
	dc.SetBrush(oldbrush);

	///////////////////////////////////////////////////////
	dc.SetPen(wxPen(*wxLIGHT_GREY));
	//CWaveDisplay *disp = (CWaveDisplay*)getTrack()->GetDisplay();
	CRegionDisplay *disp = m_pDisplay;
	if(disp && IsDrawPreviewOn())
	{
	    wxRect rc(left+border, top+border, width-border*2, height-border*2);
		disp->DrawBG(dc, rc);
	}
	dc.SetPen(oldpen);
	///////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////
// DrawFore
//! Draws region's foreground.
//! \param dc [in] reference to the device context
//! \param rc [in] track's rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::DrawFore(wxDC& dc, wxRect& rc)
{
	int left=0, right=0;
	calcPos(&left, &right);
	int width	= right - left;
//	int border	= (IsSelected() ? 2 : 1);
	int top		= rc.GetTop() + 1;
	int height	= rc.GetHeight() - 2;

	if(m_bShowOffsetBar && this->GetHeight()>2*m_pOffsetBar->GetHeight())
	{
		int offsetBarHeight = m_pOffsetBar->GetHeight();
		wxRect rcOffsetBar(left, top, width, offsetBarHeight);
		m_pOffsetBar->DrawFore(dc, rcOffsetBar);
		top += offsetBarHeight;
		height -= offsetBarHeight;
	}

	wxRect rce(left, top, width, height);

	///////////////////////////////////////////////////////
	wxPen oldpen = dc.GetPen();
	//wxColor color = GetColor();
	wxColor color = GetForeColor();

//	if(DRAW_TRANSPARENT_REGION)
//		color.Set(color.Red()/2, color.Green()/2, color.Blue()/2);

	dc.SetPen(wxPen(color, 1));
	//CWaveDisplay *disp = (CWaveDisplay*)getTrack()->GetDisplay();
	CRegionDisplay *disp = m_pDisplay;
	if(disp && IsDrawPreviewOn())
		//disp->DrawRegion(this, dc, rce);
		disp->DrawFore(dc, rce);
	dc.SetPen(oldpen);
	///////////////////////////////////////////////////////

	if(!m_bEdit)
		m_pParameters->DrawFore(dc, rce);
//	drawParametersFore(dc, rce);
}

wxColor CRegion::GetBGColor()
{
	//wxColor color(255, 255, 255);

	wxColor color = getTrack()->GetColor();

	//if( IsSelected() )
	//	color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());

	return color;
}

wxColor CRegion::GetForeColor()
{
	//wxColor color = getTrack()->GetColor();

	wxColor color(0, 0, 100);

	//if( IsSelected() )
	//	color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());

	//if( !IsSelected() )
	//	color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());

	return color;
}

/////////////////////////////////////////////////////////////////////////////
// HitTest
//! Returns true if the coordinates are in the region's rectangle.
//! \param x [in] horizontal coordinate
//! \param y [in] vertical coordinate
//! \return true if the coordinates are in the region's rectangle
/////////////////////////////////////////////////////////////////////////////
bool CRegion::HitTest(int x, int y)
{
	int left=0, right=0;
	calcPos(&left, &right);
	int width = right - left;
	wxRect rc(left, getTrack()->GetTop()-1, width, GetHeight()+2);
	return rc.Inside(x, y);
}

/////////////////////////////////////////////////////////////////////////////
// Move
//! Moves the region horizontally for the dx offset.
//! \param dx [in] offset in pixels
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::Move(int dx, int WXUNUSED(dy))
{
	Refresh(); // Old rectangle

	int left=0, right=0;
	calcPos(&left, &right);

	left  += dx;
	right += dx;

	getTrack()->CheckIntersections(this, left, right, false);
	// if not, return...

	if( (left < 0) || (right > getTrack()->GetWidth()) )
		return;

	startEdit();

	int spp = getTracks()->GetSamplesPerPixel();

	m_iStartSample = left  * spp;
	m_iEndSample   = right * spp;

	Refresh(); // New rectangle
}

/////////////////////////////////////////////////////////////////////////////
// Refresh
//! Redraws the region.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::Refresh()
{
	wxScrolledWindow *panel = getTracks()->GetTimelineView();
	if(panel)
	{
		wxRect rc;
		GetRect(rc);
		panel->RefreshRect(rc);
	}
}

void CRegion::Invalidate()
{
	m_iStartOffset = getStartOffset();
	m_iLengthNotLooped = getLengthNotLooped();

	///////////////////////////////////////////////////////
	CRegionDisplay *disp = m_pDisplay;
	if(disp && IsDrawPreviewOn())
		//m_pDisplay->Run();
		disp->LoadPeaks();
	///////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////
// GetRect
//! Calculates region's rectangle.
//! \param rc [out] reference to the rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::GetRect(wxRect& rc)
{
//	rc = m_rcRegion;
//	rc.Inflate(2, 0);

	int left=0, right=0;
	calcPos(&left, &right);

	int width = right - left;
	rc = wxRect(left, GetTop(), width, GetHeight());

	CRulerView *panel = getTracks()->GetTimelineView();
	if(panel)
	{
		/*int x, y, xScrollUnits, yScrollUnits;
		panel->GetViewStart(&x, &y);
		panel->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
		rc.Offset(-x*xScrollUnits, -y*yScrollUnits);*/

		int x, y;
		//panel->CalcUnscrolledPosition( 0, 0, &x, &y );
		panel->GetViewStart2(&x, &y);
		rc.Offset( -x, -y );

		rc.Inflate(2, 0);
	}
}

IFloopyObj *CRegion::GetChildAt(int x, int y)
{
	int left=0, right=0;
	calcPos(&left, &right);

	if(x<left || x>right)
		return NULL;

	if( HitTest(x, y) )
	{
		if( m_bShowOffsetBar &&
			this->GetHeight()>2*m_pOffsetBar->GetHeight() &&
			y>=GetTop() &&
			y<GetTop() + m_pOffsetBar->GetHeight())
			return m_pOffsetBar;
		if(left == x)
			return m_pLeftBorder;
		else if(right == x+1)
			return m_pRightBorder;
		else
		{
			IFloopyObj *obj = m_pParameters->GetChildAt(x, y);
			if(obj)
				return obj;
			/*ParameterList::Node *node = m_Parameters.GetFirst();
			while (node)
			{
				CParameter *param = (CParameter*)node->GetData();
				//if(param->HitTest(x, y))
				//	return param->GetChildAt(x, y);
				IFloopyObj *obj = param->GetChildAt(x, y);
				if(obj)
					return obj;
				node = node->GetNext();
			}*/
			return this;
		}
	}
	else
		return NULL;
}

IFloopyObj *CRegion::GetSelectedObj()
{
	IFloopyObj *obj = m_pParameters->GetSelectedObj();
	if(obj)
		return obj;
	/*ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->IsSelected())
			return param;
		node = node->GetNext();
	}*/
	return ( IsSelected() ? this : NULL);
}

/////////////////////////////////////////////////////////////////////////////
// Resize
//! Resizes the region.
//! \param dl [out] number of pixels to move the left side (start)
//! \param dr [out] number of pixels to move the right side (end)
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::Resize(int dl, int dr)
{
	int left=0, right=0;
	calcPos(&left, &right);

	if(0 != dl)
	{
		left += + dl;
		getTrack()->CheckIntersections(this, left, right, false);
		if( 0 <= left )
		{
			startEdit();
			Refresh(); // delete background
			int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();
			m_iStartSample = left * nSamplesPerPixel;
			Refresh();
		}
	}
	else if(0 != dr)
	{
		right += dr;
		getTrack()->CheckIntersections(this, left, right, false);
		if( (0 <= right) && (right <= getTrack()->GetWidth()) )
		{
			startEdit();
			Refresh(); // delete background
			int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();
			m_iEndSample   = right * nSamplesPerPixel;
			Refresh();
		}
	}
}
/*
void CRegion::Update()
{
	bool bRefresh = true;

	IFloopySoundInput *track = getTrack()->GetTrack();

	CActionHistory *actionHistory = getTracks()->GetActionHistory();

	if(m_iStartSample < 0)
		m_iStartSample = 0;

	bool bOffset	= true;
	int len1		= m_iPrevEnd - m_iPrevStart;
	int len2		= m_iEndSample - m_iStartSample;
	bool bResize	= (len2 > len1);

	/////////////////////////////////////////////////////////////////////////////////////////
	//if((m_iPrevStart != m_iStartSample) && (m_iPrevEnd != m_iEndSample))
	//	track->MoveAllParamsBetween(m_iStartSample, m_iEndSample, m_iStartSample-m_iPrevStart);
	/////////////////////////////////////////////////////////////////////////////////////////

	if((m_iPrevStart >= 0) && (m_iPrevStart != m_iStartSample))
	{
		//if( !track->MoveParam(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED, m_iStartSample) )
		if( !actionHistory->MoveParam(this, track, m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED, m_iStartSample) )
		{
			//assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
			//track->EnableAt(m_iStartSample, true);
			actionHistory->ResetParamAt(this, track, m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED);
			actionHistory->EnableAt(this, track, m_iStartSample, true);
		}

		float value = 0;
		if(track->GetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, &value))
		{
			//if( !track->MoveParam(m_iPrevStart, TIMELINE_PARAM_MOVETO, value, m_iStartSample) )
			if( !actionHistory->MoveParam(this, track, m_iPrevStart, TIMELINE_PARAM_MOVETO, value, m_iStartSample) )
			{
				//assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, value) );
				//track->SetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value);
				actionHistory->ResetParamAt(this, track, m_iPrevStart, TIMELINE_PARAM_MOVETO, value);
				actionHistory->SetParamAt(this, track, m_iStartSample, TIMELINE_PARAM_MOVETO, value);
			}

			//assert( getReset( m_iStartSample ) );
		}
		else
			bOffset = false;
	}

	if((m_iPrevEnd >= 0.f) && (m_iPrevEnd != m_iEndSample))
	{
		//if( !track->MoveParam(m_iPrevEnd, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED, m_iEndSample) )
		if( !actionHistory->MoveParam(this, track, m_iPrevEnd, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED, m_iEndSample) )
		{
			//assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
			//track->EnableAt(m_iEndSample, false);
			actionHistory->ResetParamAt(this, track, m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED);
			actionHistory->EnableAt(this, track, m_iStartSample, false);
		}
	}

	m_iPrevStart = m_iPrevEnd = -1;

	m_bEdit = false;

//	if(!bOffset || bResize)
	{
		Invalidate();
		Refresh();
	}

	getTrack()->InvalidateRegions( this );

	getTracks()->SetChanged( true );

	getTracks()->SetViewUpdatedWhilePlaying(true);
}
*/
bool CRegion::setStartPos(int prevPos, int newPos)
{
//	if((prevPos >= 0) && (prevPos != newPos))
	{
		IFloopySoundInput *track = getTrack()->GetTrack();

		if( !track->MoveParam(prevPos, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED, newPos) )
		{
			if( !track->ResetParamAt(prevPos, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) )
				return false;
			track->EnableAt(newPos, true);
		}

		/////////////////////////////////////////////////////////////////////////////////////////
		//if((m_iPrevStart != m_iStartSample) && (m_iPrevEnd != m_iEndSample))
		//	track->MoveAllParamsBetween(m_iStartSample, m_iEndSample, m_iStartSample-m_iPrevStart);
		/////////////////////////////////////////////////////////////////////////////////////////

		float value = 0;
		if(track->GetParamAt(prevPos, TIMELINE_PARAM_MOVETO, &value))
		{
			if( !track->MoveParam(prevPos, TIMELINE_PARAM_MOVETO, value, newPos) )
			{
				if( !track->ResetParamAt(prevPos, TIMELINE_PARAM_MOVETO, value) )
					return false;
				track->SetParamAt(newPos, TIMELINE_PARAM_MOVETO, value);
			}
		}

		m_iStartSample = newPos;

		return true;
	}
	return false;
}

bool CRegion::setEndPos(int prevPos, int newPos)
{
//	if((prevPos >= 0.f) && (prevPos != newPos))
	{
		IFloopySoundInput *track = getTrack()->GetTrack();

		if( !track->MoveParam(prevPos, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED, newPos) )
		{
			if( !track->ResetParamAt(prevPos, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) )
				return false;
			track->EnableAt(newPos, false);
		}

		m_iEndSample = newPos;

		return true;
	}
	return false;
}

void CRegion::Update()
{
//	bool bRefresh = true;

//	IFloopySoundInput *track = getTrack()->GetTrack();

	CActionHistory *actionHistory = getTracks()->GetActionHistory();

	if(m_iStartSample < 0)
		m_iStartSample = 0;

//	bool bOffset	= true;
//	bool bResize	= (m_iPrevEnd-m_iPrevStart > m_iEndSample-m_iStartSample);

	//setStartPos(m_iPrevStart, m_iStartSample);
	//setEndPos(m_iPrevEnd, m_iEndSample);
	if(m_iPrevEnd-m_iPrevStart == m_iEndSample-m_iStartSample)
	{
		// Move whole region
		actionHistory->MoveParam(this, this, m_iPrevStart, 0, 0.f, m_iStartSample);
	}
	else
	{
		// Move start
		if((m_iPrevStart >= 0) && (m_iPrevStart != m_iStartSample))
			actionHistory->MoveParam(this, this, m_iPrevStart, 1, 0.f, m_iStartSample);

		// Move end
		if((m_iPrevEnd >= 0.f) && (m_iPrevEnd != m_iEndSample))
			actionHistory->MoveParam(this, this, m_iPrevEnd, 2, 0.f, m_iEndSample);
	}

	m_iPrevStart = m_iPrevEnd = -1;

	m_bEdit = false;

//	if(!bOffset || bResize)
	{
		Invalidate();
		Refresh();
	}

	getTrack()->InvalidateRegions( this );

	getTracks()->SetChanged( true );

	getTracks()->SetViewUpdatedWhilePlaying(true);
}



void CRegion::CancelUpdate()
{
	m_iStartSample	= m_iPrevStart;
	m_iEndSample	= m_iPrevEnd;
	m_iPrevStart	= m_iPrevEnd = -1;
}

bool CRegion::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode() )
	{
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
	case '>':
	case '.':
		Move(+1, 0);
		Update();
		Refresh();
		return true;
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
	case '<':
	case ',':
		Move(-1, 0);
		Update();
		Refresh();
		return true;
	case WXK_UP:
	case WXK_NUMPAD_UP:
	case '+':
		getTrack()->SetHeight( getTrack()->GetHeight() + 1 );
		return true;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
	case '-':
		getTrack()->SetHeight( getTrack()->GetHeight() - 1 );
		return true;
	case WXK_DELETE:
		getTrack()->RemoveRegion( this );
		return true;
	case 'r':
	case 'R':
		SetReset( !GetReset() );
		return true;
	case'o':
	case'O':
		{
			int offset = getTrack()->GetTracks()->GetCaretPos() - m_iStartSample;
			if(offset > 0)
			{
				float value = 0.f;
				if(getTrack()->GetTrack()->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
					offset += (int)value;
				SetStartOffset( offset );
				Update();
				Refresh();
			}
		break;
		}
		return true;
	case 'p':
	case 'P':
		SetDrawPreview( !IsDrawPreviewOn() );
		//Invalidate();
		//Refresh();
		return true;
	default:
		return getTrack()->OnKeyDown(event);
		/*{
			IFloopyObj *obj = GetSelectedObj();
			if(obj && obj->OnKeyDown(event))
				return true;
		}*/
	}

	return false;
}

bool CRegion::OnMouseEvent(wxMouseEvent& event)
{
	bool bResult = false;
	if( event.LeftDown() )
	{
		if( !event.ShiftDown() && !IsSelected())
			getTracks()->DeselectAllRegions();
		if( !IsSelected() )
			Select();
//		m_pParamsDialog->Update();
//		m_pPropsDialog->Update();
		bResult = true;
	}
	else
		bResult = m_pParameters->OnMouseEvent(event);
	IFloopyObj::OnMouseEvent(event);
	return bResult;
}

void CRegion::startEdit()
{
	if(!m_bEdit)
	{
		m_iPrevStart = m_iStartSample;
		m_iPrevEnd   = m_iEndSample;

		m_bEdit = true;
	}
}

void CRegion::calcPos(int *left, int *right)
{
	int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();

	float fLeft  = (float)m_iStartSample / (float)nSamplesPerPixel;
	float fRight = (float)m_iEndSample   / (float)nSamplesPerPixel;

	*left  = floor( fLeft );
	*right = floor( fRight );

	/*if(*left > *right)
	{
		int tmp = *right;
		*right = *left;
		*left = tmp;
	}*/
}

int CRegion::GetLeft()
{
	return m_iStartSample / getTracks()->GetSamplesPerPixel();
}

int CRegion::GetRight()
{
	return m_iEndSample / getTracks()->GetSamplesPerPixel();
}

int CRegion::GetWidth()
{
	return (m_iEndSample - m_iStartSample) / getTracks()->GetSamplesPerPixel();
}
/*
bool CRegion::isAfterTrack(IFloopySoundInput *obj)
{
	bool bAfterTrack = false; // Is obj source of track?
	IFloopySoundInput *track = getTrack()->GetTrack();
	IFloopySoundInput *tmp = track;

	while(tmp)
	{
		if(obj == tmp)
			return true;

		int type = tmp->GetType();
		if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
			tmp = ((IFloopySoundFilter*)tmp)->GetSource();
		else
			tmp = NULL;
	}

	return false;
}
*/
void CRegion::ShowObjectParameters(IFloopySoundInput *obj, bool show)
{
	if(!show)
		//removeParameters( obj );
		m_pParameters->RemoveInput( obj );
	else
		//loadParameters( obj );
		m_pParameters->LoadInput( obj );
	Refresh();
}
/*
void CRegion::removeParameters(IFloopySoundInput *obj)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->GetInput()==obj)
		{
			ParameterList::Node *tmp = node->GetNext();
			if(m_Parameters.DeleteNode(node))
			{
				delete param;
				node = tmp;
			}
			else
				node = node->GetNext();
		}
		else
			node = node->GetNext();
	}
}

bool CRegion::paramsLoaded(IFloopySoundInput *obj)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->GetInput()==obj)
			return true;
		node = node->GetNext();
	}
	return false;
}

void CRegion::loadParameters(IFloopySoundInput *obj)
{
	if(NULL == obj)
		return;

	// Check if object exists
	if( paramsLoaded(obj) )
		return;

	bool bAfterTrack = isAfterTrack(obj);

//	WX_CLEAR_LIST(ParameterList, m_Parameters);

	for(int index=0; index<obj->GetParamCount(); index++)
	{
		m_Parameters.Append( new CParameter(this, obj, index, bAfterTrack) );
	}
}

void CRegion::drawParametersFore(wxDC& dc, wxRect& rc)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		param->DrawFore(dc, rc);
		node = node->GetNext();
	}
}
*/
bool CRegion::GetReset()
{
//	return m_bReset;

	return getReset( m_iStartSample );

	/*float value = 0.f;
	if(getTrack()->GetTrack()->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		return value==0.f;
	else
		return false;*/
}

bool CRegion::HasStartOffset()
{
	float value = 0.f;
	IFloopySoundInput *input = getTrack()->GetTrack();
	return(input->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value));
}

bool CRegion::getReset(int sample)
{
	float value = 0.f;
	IFloopySoundInput *input = getTrack()->GetTrack();
	if(input->GetParamAt(sample, TIMELINE_PARAM_MOVETO, &value))
		return value==0.f;
	else
		return false;
}

int CRegion::GetStartOffset()
{
	return getStartOffset();
}

int CRegion::getStartOffset()
{
	float value = 0;
	IFloopySoundInput *input = getTrack()->GetTrack();
	if(input->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		return (int)value;
	else
	{
		CRegion *prev = getPrevRegion();
		if(NULL != prev)
		{
			int dist = GetStartPos() - prev->GetEndPos();
			return prev->GetEndOffset() + dist;
		}
		else
			return GetStartPos();
	}
	return -1;
}

void CRegion::SetReset(bool bReset)
{
//	m_bReset = bReset;


	if(bReset)
		SetStartOffset(0);
	else
	{
		IFloopySoundInput *track = getTrack()->GetTrack();

		CActionHistory *actionHistory = getTracks()->GetActionHistory();

		float value = 0;
		if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
			//track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value);
			actionHistory->ResetParamAt(this, track, m_iStartSample, TIMELINE_PARAM_MOVETO, value);

		Invalidate();
		Refresh();
	}
}

void CRegion::SetStartOffset(int sample)
{
/*
	int spp = getTrack()->GetTracks()->GetSamplesPerPixel();
	int pps = getTrack()->GetTracks()->GetPixelsPerSecond();
	int pix = sample / spp;
	int sec = pix / pps;
	//wxString csLabel("00:00:000");
	//formatTime(sec, csLabel);
*/

	IFloopySoundInput *track = getTrack()->GetTrack();

	CActionHistory *actionHistory = getTracks()->GetActionHistory();

	float value = 0;
	if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		//track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value);
		actionHistory->ResetParamAt(this, track, m_iStartSample, TIMELINE_PARAM_MOVETO, value);

	//track->SetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, sample);
	actionHistory->SetParamAt(this, track, m_iStartSample, TIMELINE_PARAM_MOVETO, sample);

	Invalidate();
	Refresh();

	if(m_iEndSample - m_iStartSample > getTrack()->GetTracks()->GetSamplesPerPixel())
		getTrack()->InvalidateRegions( this );
}

void CRegion::Select(bool selected)
{
	if(!selected)
	{
		/*ParameterList::Node *node = m_Parameters.GetFirst();
		while (node)
		{
			CParameter *param = (CParameter*)node->GetData();
			if(param)
				param->Select(false);
			node = node->GetNext();
		}*/
		m_pParameters->DeselectAll();
	}

	IFloopyObj::Select(selected);

	Refresh();
}
/*
void CRegion::DeselectAllParameters(CParameter *caller)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param!=caller)
			param->Select(false);
		node = node->GetNext();
	}
}
*/
int CRegion::GetCaretPos()
{
	int pos = getTrack()->GetCaretPos();

	/*float value = 0;
	int offset = 0;
	IFloopySoundInput *track = getTrack()->GetTrack();
	if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		pos -= m_iStartSample + (int)value;*/

	/*if( GetReset() )
	{
		return getTrack()->GetCaretPos();
	}*/

	return pos;
}

wxColor CRegion::GetColor()
{
	return getTrack()->GetColor();
}

int CRegion::GetHeight()
{
	return getTrack()->GetHeight() - 2;
}

int CRegion::GetTop()
{
	return getTrack()->GetTop() + 1;
}

IFloopySoundInput *CRegion::GetInput()
{
	return getTrack()->GetInput();
}

CTracks *CRegion::getTracks()
{
	return (CTracks*)getTrack()->GetParent();
}

CRegion *CRegion::getPrevRegion()
{
	CTrack	*track	= getTrack();
	CRegion	*prev	= NULL;
	int start = GetStartPos();
	int max = 0;

	for(int i=0; i<track->GetRegionCount(); i++)
	{
		CRegion *tmp = track->GetRegion(i);
		int end = tmp->GetEndPos();
		if(end<start && end>=max)
		{
			prev = tmp;
			max = end;
		}
	}

	return prev;
}

int CRegion::GetEndOffset()
{
	int len = GetEndPos() - GetStartPos();
	if(m_iStartOffset >= 0)
		return m_iStartOffset + len;
	else
	{
		CRegion *prev = getPrevRegion();
		if(NULL != prev)
		{
			int dist = GetStartPos() - prev->GetEndPos();
			return prev->GetEndOffset() + dist + len;
		}
		else
		{
			return GetEndPos();
		}
	}
}

bool CRegion::RemoveSelectedObjects()
{
	return false;
}



int CRegion::GetPropertyCount()
{
	return 1;
}

bool CRegion::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)IsDrawPreviewOn();
		return true;
	}
	return false;
}

void CRegion::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		SetDrawPreview( value != 0.f );
		return;
	}
}

const char *CRegion::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "DrawPreview";
	}
	return NULL;
}

const char *CRegion::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Draw region previews";
	}
	return NULL;
}

float CRegion::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0: return 0.f;
	}
	return 0.f;
}

float CRegion::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0: return 1.f;
	}
	return 0.f;
}

const char *CRegion::GetPropertyUnit(int index)
{
	//switch(index)
	//{
	//case 0: return "Db";
	//}
	return NULL;
}

float CRegion::GetPropertyStep(int index)
{
	switch(index)
	{
	case 0: return 1.f;
	}
	return 0.f;
}






/////////////////////////////////////////////////////////////////////
// CBorder functions
/////////////////////////////////////////////////////////////////////
void CRegion::CBorder::Move(int dx, int WXUNUSED(dy))
{
	if(m_bLeft)
		getRegion()->Resize(dx, 0);
	else
		getRegion()->Resize(0, dx);
}



























































/**
 * Called by CActionHistory.
 * Moves or resizes region depending of the index parameter.
 * @param offset
 * @param index 0:translation, 1:start position, 2:end position
 * @param value not used
 * @param newoffset New position
 * @return true if succesfull
 */
bool CRegion::MoveParam(int offset, int index, float value, int newoffset)
{
	switch(index)
	{
	case 0:
		{
			Refresh();
			if( !setStartPos(offset, newoffset) )
				return false;
			int len = m_iEndSample-m_iStartSample;
			return setEndPos(offset+len, newoffset+len);
		}
		return true;
	case 1:
		Refresh();
		return setStartPos(offset, newoffset);
	case 2:
		Refresh();
		return setEndPos(offset, newoffset);
	}
	return false;
}

int CRegion::GetParamCount()
{
	return 0;
}

bool CRegion::GetParamVal(int index, float *value)
{
	return false;
}

void CRegion::SetParamVal(int index, float value)
{

}

float CRegion::GetParamMin(int index)
{
	return 0.f;
}

float CRegion::GetParamMax(int index)
{
	return 0.f;
}

float CRegion::GetParamStep(int index)
{
	return 0.f;
}

const char *CRegion::GetParamName(int index)
{
	return NULL;
}

const char *CRegion::GetParamDesc(int index)
{
	return NULL;
}

const char *CRegion::GetParamUnit(int index)
{
	return NULL;
}

