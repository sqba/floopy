// Region.cpp: implementation of the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "tracks.h"

//IMPLEMENT_DYNAMIC_CLASS(CRegion, IFloopyObj)

WX_DEFINE_LIST(ParameterList);

//#define TRANSPARENT_BACKGROUND

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegion::CRegion(CTrack *track, UINT startSample, UINT endSample)
 : IFloopyObj(track)
{
	m_iStartSample	= startSample;
	m_iEndSample	= endSample;

	m_pLeftBorder	= new CBorder(this, true);
	m_pRightBorder	= new CBorder(this, false);

	m_pOffsetBar	= new COffsetBar(this);
	m_bShowOffsetBar = true;

	m_iPrevStart	= m_iPrevEnd = -1;

	m_bEdit			= false;

	m_iStartOffset	= 0;
	m_iLengthNotLooped = 0;

	m_bDrawPreview = true;

	createMenu();

	wxLog::AddTraceMask(_T("CRegion"));

	m_pDisplay = new CRegionDisplay(this);

	Invalidate();
}

CRegion::~CRegion()
{
	delete m_pLeftBorder;
	delete m_pRightBorder;
	delete m_pOffsetBar;
	//delete m_pMenu; // !Deleted in ~IFloopyObj()
	WX_CLEAR_LIST(ParameterList, m_Parameters);
	if(m_pDisplay)
		delete m_pDisplay;
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
		IFloopySoundInput *track = getTrack()->GetInput();
		assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
		assert( track->ResetParamAt(m_iPrevEnd,   TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
		float value = 0.f;
		if(track->GetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, &value))
			assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, value) );
	}
	else
	{
		IFloopySoundInput *track = getTrack()->GetInput();
		assert( track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
		assert( track->ResetParamAt(m_iEndSample,   TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
		float value = 0.f;
		if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
			assert( track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value) );
	}

	if(m_iEndSample - m_iStartSample > getTrack()->GetTracks()->GetSamplesPerPixel())
	{
		Invalidate();
		Refresh();

		getTrack()->InvalidateRegions( this );
	}
}

void CRegion::remove(IFloopyObj *evt)
{
	CRegion *region = (CRegion*)evt;
	CTrack *track = region->getTrack();
	track->RemoveRegion( region );
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
	for(int i=0; i<=loopCount; i++)
	{
		int l = origLen;

		if(i==0 && start!=0)
			l = origLen-start;
		
		int x1 = prev;
		int x2 = prev+l;

		if(prev+origLen < right)
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

	for(i=b; i>0; i--)
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

#ifndef TRANSPARENT_BACKGROUND
		dc.SetBrush(brush);
#else
		dc.SetBrush(*wxTRANSPARENT_BRUSH);
#endif

	int top    = rc.GetTop() + 1;
	int height = rc.GetHeight() - 2;

	if(m_bShowOffsetBar && this->GetHeight()>2*m_pOffsetBar->GetHeight())
	{
		int offsetBarHeight = m_pOffsetBar->GetHeight();
		wxRect rcOffsetBar(left, rc.GetTop(), width, offsetBarHeight);
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
	if(disp && m_bDrawPreview)
		disp->DrawBG(dc, wxRect(left+border, top+border, width-border*2, height-border*2));
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
	int width = right - left;
	int border = (IsSelected() ? 2 : 1);
	int top    = rc.GetTop();
	int height = rc.GetHeight();

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

#ifndef TRANSPARENT_BACKGROUND
	//color.Set(color.Red()/2, color.Green()/2, color.Blue()/2);
#endif
	dc.SetPen(wxPen(color, 1));
	//CWaveDisplay *disp = (CWaveDisplay*)getTrack()->GetDisplay();
	CRegionDisplay *disp = m_pDisplay;
	if(disp && m_bDrawPreview)
		//disp->DrawRegion(this, dc, rce);
		disp->DrawFore(dc, rce);
	dc.SetPen(oldpen);
	///////////////////////////////////////////////////////

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
	wxRect rc(left, getTrack()->GetTop(), width, GetHeight());
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
	wxScrolledWindow *panel = getTracks()->GetRegionsView();
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
	if(disp && m_bDrawPreview)
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

	CRulerView *panel = getTracks()->GetRegionsView();
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
			ParameterList::Node *node = m_Parameters.GetFirst();
			while (node)
			{
				CParameter *param = (CParameter*)node->GetData();
				//if(param->HitTest(x, y))
				//	return param->GetChildAt(x, y);
				IFloopyObj *obj = param->GetChildAt(x, y);
				if(obj)
					return obj;
				node = node->GetNext();
			}
			return this;
		}
	}
	else
		return NULL;
}

IFloopyObj *CRegion::GetSelectedObj()
{
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

void CRegion::Update()
{
	bool bRefresh = true;

	IFloopySoundInput *track = getTrack()->GetInput();

	if(m_iStartSample < 0)
		m_iStartSample = 0;

	/////////////////////////////////////////////////////////////////////////////////////////
	//if((m_iPrevStart != m_iStartSample) && (m_iPrevEnd != m_iEndSample))
	//	track->MoveAllParamsBetween(m_iStartSample, m_iEndSample, m_iStartSample-m_iPrevStart);
	/////////////////////////////////////////////////////////////////////////////////////////

	if((m_iPrevStart >= 0) && (m_iPrevStart != m_iStartSample))
	{
		if( !track->MoveParam(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED, m_iStartSample) )
		{
			assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
			track->EnableAt(m_iStartSample, true);
		}

		float value = 0;
		if(getTrack()->GetInput()->GetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, &value))
		{
			if( !track->MoveParam(m_iPrevStart, TIMELINE_PARAM_MOVETO, value, m_iStartSample) )
			{
				assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, value) );
				getTrack()->GetInput()->SetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value);
			}

			//assert( getReset( m_iStartSample ) );
		}
	}

	if((m_iPrevEnd >= 0.f) && (m_iPrevEnd != m_iEndSample))
	{
		if( !track->MoveParam(m_iPrevEnd, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED, m_iEndSample) )
		{
			assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
			track->EnableAt(m_iEndSample, false);
		}
	}

	Invalidate();
	Refresh();

	m_iPrevStart = m_iPrevEnd = -1;

	m_bEdit = false;

	getTrack()->InvalidateRegions( this );

	getTracks()->SetChanged( true );

	getTracks()->SetViewUpdatedWhilePlaying(true);
}

void CRegion::CancelUpdate()
{
	m_iStartSample = m_iPrevStart;
	m_iEndSample = m_iPrevEnd;
	m_iPrevStart = m_iPrevEnd = -1;
}

bool CRegion::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode() )
	{
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
		Move(+1, 0);
		Update();
		Refresh();
		return true;
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
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
				if(getTrack()->GetInput()->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
					offset += (int)value;
				SetStartOffset( offset );
				Update();
				Refresh();
			}
		break;
		}
		return true;
	default:
		return getTrack()->OnKeyDown(event);
	}
	
	return false;
}

void CRegion::OnMouseEvent(wxMouseEvent& event)
{
	if( event.Dragging() && (0 != m_ptPrev.x) ) {
		int dx = event.GetX() - m_ptPrev.x;
		getTracks()->MoveSelectedRegions(dx);
		//return;
	}

	if( event.LeftDown() )
	{
		//if(!event.ControlDown())
		//if( !event.ShiftDown() && !m_bDrag ) {
		if( !event.ShiftDown() ) {
			//getTracks()->DeselectAllTracks();
			getTracks()->DeselectAllRegions();
		} //else
		//	m_bDrag = true;

		//m_pSelectedObj->Select();
		//m_pSelectedObj->Refresh();

		return; // Don't move the caret
	}

	if( event.LeftUp() )
	{
		Update();
	}

	IFloopyObj::OnMouseEvent(event);
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

void CRegion::loadParameters(IFloopySoundInput *obj)
{
	if(NULL == obj)
		return;

	WX_CLEAR_LIST(ParameterList, m_Parameters);

	for(int index=0; index<obj->GetParamCount(); index++)
	{
		m_Parameters.Append( new CParameter(this, obj, index) );
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

bool CRegion::GetReset()
{
//	return m_bReset;

	return getReset( m_iStartSample );

	/*float value = 0.f;
	if(getTrack()->GetInput()->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		return value==0.f;
	else
		return false;*/
}

bool CRegion::getReset(int sample)
{
	float value = 0.f;
	if(getTrack()->GetInput()->GetParamAt(sample, TIMELINE_PARAM_MOVETO, &value))
		return value==0.f;
	else
		return false;
}

int CRegion::getStartOffset()
{
	float value = 0;
	IFloopySoundInput *track = getTrack()->GetInput();
	if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		return (int)value;
	return -1;
}

void CRegion::SetReset(bool bReset)
{
//	m_bReset = bReset;


	if(bReset)
		SetStartOffset(0);
	else
	{
		IFloopySoundInput *track = getTrack()->GetInput();

		float value = 0;
		if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
			track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value);

		Invalidate();
		Refresh();
	}
}

void CRegion::SetStartOffset(int sample)
{
	IFloopySoundInput *track = getTrack()->GetInput();

	float value = 0;
	if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		track->ResetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, value);

	track->SetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, sample);

	Invalidate();
	Refresh();

	if(m_iEndSample - m_iStartSample > getTrack()->GetTracks()->GetSamplesPerPixel())
		getTrack()->InvalidateRegions( this );
}

void CRegion::Select(bool selected)
{
	IFloopyObj::Select(selected);

	Refresh();
}

int CRegion::GetCaretPos()
{
	int pos = getTrack()->GetCaretPos();

	/*float value = 0;
	int offset = 0;
	IFloopySoundInput *track = getTrack()->GetInput();
	if(track->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		pos -= m_iStartSample + (int)value;*/

	/*if( GetReset() )
	{
		return getTrack()->GetCaretPos();
	}*/
		
	return pos;
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







/////////////////////////////////////////////////////////////////////
// COffsetBar functions
/////////////////////////////////////////////////////////////////////
void CRegion::COffsetBar::Move(int dx, int WXUNUSED(dy))
{
	// Move start offset
	CRegion *pRegion = getRegion();
	CTracks *pTracks = (CTracks*)pRegion->GetParent()->GetParent();
	int start = pRegion->GetStartOffset();
	int spp = pTracks->GetSamplesPerPixel();
	int newOffset = start - dx*spp;
	if(newOffset >= 0)
	{
		pRegion->SetStartOffset( newOffset );
		pRegion->Invalidate();
		pRegion->Refresh();
	}
}

void CRegion::COffsetBar::DrawBG(wxDC &dc, wxRect &rc)
{
	wxBrush oldbrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	dc.SetBrush( wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), wxSOLID) );

	int left = rc.GetLeft();
	int top = rc.GetTop()+1;
	int width = rc.GetWidth();
	int height = rc.GetHeight();
	int right = left + width;
	int bottom = rc.GetBottom();

	dc.SetPen( *wxTRANSPARENT_PEN );
	dc.DrawRectangle(left, top, width, this->GetHeight());

	dc.SetPen(*wxWHITE_PEN);
	dc.DrawLine(left, top, right, top);			// top
	dc.DrawLine(left, top, left, bottom);		// left

	dc.SetPen(*wxMEDIUM_GREY_PEN);
	dc.DrawLine(right-1, top, right-1, bottom);	// right
	dc.DrawLine(left, bottom-1, right, bottom-1);	// bottom

	dc.SetBrush( oldbrush );
	dc.SetPen( oldpen );
}

void CRegion::COffsetBar::DrawFore(wxDC &dc, wxRect &rc)
{
	CRegion *pRegion = getRegion();
	CTracks *pTracks = (CTracks*)pRegion->GetParent()->GetParent();

	int pix      = pTracks->GetPixelsPerSecond();
	int iStep    = pTracks->CalcStep( MIN_DISTANCE );

	int left = rc.GetLeft();
	int top = rc.GetTop();
	int right = left + rc.GetWidth();
	int bottom = top + this->GetHeight();
	int height = this->GetHeight();
	int iMiddle  = top + height/2;

	int iLineTop1 = iMiddle + 1;
	int iLineTop2 = iMiddle + (height/3);

	int iLineTop = top;
	int iLineBottom = top + height;

	dc.SetFont( *wxSWISS_FONT );

	int w=0, h=0;
	wxString csLabel("00:00:000");
	dc.GetTextExtent(csLabel, &w, &h);
	int iTextTop = iLineTop1 - (h - h/3);

	int start = pRegion->GetStartOffset();
	int spp = pTracks->GetSamplesPerPixel();
	start /= spp;

	int pos = start;

	for(int x=left; x<right; x+=iStep)
	{
		//if((x/iStep)%4 == 0)
		//Optimization: x%a == x&(a-1) for binary numbers
		if((((x-left)/iStep)&3) == 0)
		{
			iLineTop = iLineTop1;

			if(x+4+w < right)
			{
				float fSeconds = (float)pos / (float)pix;
				formatTime(fSeconds, csLabel);
				dc.DrawText(csLabel, x+4, iTextTop);
			}
		}
		else
		{
			iLineTop = iLineTop2;
		}

		pos++;


		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(x, iLineTop, x, iLineBottom);

		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(x+1, iLineTop, x+1, iLineBottom);
	}
}

void CRegion::COffsetBar::formatTime(float fSec, wxString &csTime)
{
	float min = fSec>60.f ? fSec/60.f : 0.f;
	float sec = min>1.f ? fSec-(int)min*60 : fSec;
	float ms  = sec*1000.f - (int)sec*1000;

	csTime.Printf("%2.2d:%2.2d:%3.3d", (int)min, (int)sec, (int)ms);
}
