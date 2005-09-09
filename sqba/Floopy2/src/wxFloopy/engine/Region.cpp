// Region.cpp: implementation of the CRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"

IMPLEMENT_DYNAMIC_CLASS(CRegion, IFloopyObj)

WX_DEFINE_LIST(ParameterList);

//#define TRANSPARENT_BACKGROUND

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegion::CRegion(CTrack *track, UINT startSample, UINT endSample)
 : IFloopyObj(track)
{
	m_iStartSample = startSample;
	m_iEndSample   = endSample;

	m_pLeftBorder  = new CBorder(this, TRUE);
	m_pRightBorder = new CBorder(this, FALSE);

	m_iPrevStart = m_iPrevEnd = -1;

	m_bEdit = FALSE;

	createMenu();

	wxLog::AddTraceMask(_T("CRegion"));

	//loadParameters( getTrack()->GetInput() );
//	loadParameters( getTrack()->GetComponent("volume") );

//	m_bReset = FALSE;

	m_pDisplay = new CRegionDisplay(this);
//	m_pDisplay->Run();
}

CRegion::~CRegion()
{
	delete m_pLeftBorder;
	delete m_pRightBorder;
//	delete m_pMenu; // Deleted in ~IFloopyObj()
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
}

void CRegion::remove(IFloopyObj *evt)
{
	CRegion *region = (CRegion*)evt;
	CTrack *track = region->getTrack();
	track->RemoveRegion( region );
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

	int top    = GetTop();
	int height = GetHeight();

	dc.DrawRoundedRectangle(left, top, width, height, 3);

	dc.SetPen(oldpen);
	dc.SetBrush(oldbrush);

	///////////////////////////////////////////////////////
	dc.SetPen(wxPen(*wxLIGHT_GREY));
	//CWaveDisplay *disp = (CWaveDisplay*)getTrack()->GetDisplay();
	CRegionDisplay *disp = m_pDisplay;
	if(disp)
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
	int top    = GetTop();
	int height = GetHeight();
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
	if(disp)
		//disp->DrawRegion(this, dc, rce);
		disp->DrawFore(dc, rce);
	dc.SetPen(oldpen);
	///////////////////////////////////////////////////////

//	drawParametersFore(dc, rce);
}

wxColour CRegion::GetBGColor()
{
	//wxColor color(255, 255, 255);

	wxColor color = getTrack()->GetColor();

	//if( IsSelected() )
	//	color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());

	return color;
}

wxColour CRegion::GetForeColor()
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
// DrawPreview
//! Draws region preview (in the preview bar).
//! \param dc [in] reference to the device context
//! \param rc [in] track's preview rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CRegion::DrawPreview(wxDC& dc, wxRect& rc)
{
	/*SOUNDFORMAT *fmt = getTrack()->GetInput()->GetFormat();
	float freq = fmt->frequency;//44100.f;
	float fStart = (float)m_iStartSample / freq;
	float fEnd   = (float)m_iEndSample   / freq;

	int left  = fStart * (float)pps;
	int right = fEnd * (float)pps;
	int width = right - left;
	dc.DrawRectangle(left, rc.GetTop(), width, rc.GetHeight());*/
}

/////////////////////////////////////////////////////////////////////////////
// HitTest
//! Returns TRUE if the coordinates are in the region's rectangle.
//! \param x [in] horizontal coordinate
//! \param y [in] vertical coordinate
//! \return TRUE if the coordinates are in the region's rectangle
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

	getTrack()->CheckIntersections(this, left, right, FALSE);
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
	wxScrolledWindow *panel = getTracks()->GetTracksView();
	if(panel)
	{
		wxRect rc;
		GetRect(rc);
		panel->RefreshRect(rc);
	}
}

void CRegion::Invalidate()
{
	///////////////////////////////////////////////////////
	CRegionDisplay *disp = m_pDisplay;
	if(disp)
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
	int left=0, right=0;
	calcPos(&left, &right);

	int width = right - left;
	rc = wxRect(left, GetTop(), width, GetHeight());

	wxScrolledWindow *panel = getTracks()->GetTracksView();
	if(panel)
	{
		int x, y, xScrollUnits, yScrollUnits;
		panel->GetViewStart(&x, &y);
		panel->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
		rc.Offset(-x*xScrollUnits, -y*yScrollUnits);
		rc.Inflate(2, 0);
	}
}

IFloopyObj *CRegion::GetChildAt(int x, int y)
{
	int left=0, right=0;
	calcPos(&left, &right);

	if( HitTest(x, y) )
	{
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
	if( IsSelected() )
		return this;
	else
	{
		return NULL;
	}
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
		getTrack()->CheckIntersections(this, left, right, FALSE);
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
		getTrack()->CheckIntersections(this, left, right, FALSE);
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
	BOOL bRefresh = TRUE;

	IFloopySoundInput *track = getTrack()->GetInput();

	if(m_iStartSample < 0)
		m_iStartSample = 0;

//	float value=0.f;

	if((m_iPrevStart >= 0) && (m_iPrevStart != m_iStartSample))
	{
		//assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE) );
		//track->EnableAt(m_iStartSample, TRUE);
//		ddump();
//		assert( track->GetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, &value) );
//		assert( track->MoveParam(m_iPrevStart, TIMELINE_PARAM_ENABLE, m_iStartSample) );
//		assert( track->GetParamAt(m_iStartSample, TIMELINE_PARAM_ENABLE, &value) );
		/*if( !track->MoveParam(m_iPrevStart, TIMELINE_PARAM_ENABLE, m_iStartSample) )
		{
			m_iStartSample = m_iPrevStart;
			m_iPrevStart = m_iPrevEnd = -1;
			m_bEdit = FALSE;
			getTrack()->Refresh();
			return;
		}*/
		if( !track->MoveParam(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED, m_iStartSample) )
		{
			assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_ENABLED) );
			track->EnableAt(m_iStartSample, TRUE);
		}

		//float paramVal=0.f;
		//track->GetParamAt(0, TIMELINE_PARAM_ENABLE, &paramVal);

		/////////////////////////////////////////////////////////////////////////////////////////
		//track->MoveAllParamsBetween(m_iStartSample, m_iEndSample, m_iStartSample-m_iPrevStart);
		/////////////////////////////////////////////////////////////////////////////////////////

//		if( getReset( m_iPrevStart ) )
		float value = 0;
		if(getTrack()->GetInput()->GetParamAt(m_iPrevStart, TIMELINE_PARAM_MOVETO, &value))
		{
			bRefresh = FALSE;

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
		//assert( track->ResetParamAt(m_iPrevEnd, TIMELINE_PARAM_ENABLE) );
		//track->EnableAt(m_iEndSample, FALSE);
//		ddump();
//		assert( track->GetParamAt(m_iPrevEnd, TIMELINE_PARAM_ENABLE, &value) );
//		assert( track->MoveParam(m_iPrevEnd, TIMELINE_PARAM_ENABLE, m_iEndSample) );
//		assert( track->GetParamAt(m_iEndSample, TIMELINE_PARAM_ENABLE, &value) );
		/*if( track->MoveParam(m_iPrevEnd, TIMELINE_PARAM_ENABLE, m_iEndSample) )
		{
			m_iEndSample = m_iPrevEnd;
			m_iPrevStart = m_iPrevEnd = -1;
			m_bEdit = FALSE;
			getTrack()->Refresh();
			return;
		}*/
		if( !track->MoveParam(m_iPrevEnd, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED, m_iEndSample) )
		{
			assert( track->ResetParamAt(m_iPrevStart, TIMELINE_PARAM_ENABLE, PARAM_VALUE_DISABLED) );
			track->EnableAt(m_iEndSample, FALSE);
		}
	}

	//if((m_iPrevEnd-m_iPrevStart) != (m_iEndSample-m_iStartSample))

//	if( !GetReset() )
	if( bRefresh )
	{
		Invalidate();
		Refresh();
	}
//	else
//	{
		// Resize
//		Invalidate();
		///////////////////////////////////////////////////////
		/*CRegionDisplay *disp = m_pDisplay;
		if(disp)
			disp->LoadPeaks();*/
		///////////////////////////////////////////////////////
//		Refresh();
//	}
	// Move params!!!

	/*wxLogTrace(_T("CRegion"), _T("m_iPrevStart=%d m_iStartSample=%d"),
		m_iPrevStart, m_iStartSample);
	wxLogTrace(_T("CRegion"), _T("m_iPrevEnd=%d m_iEndSample=%d"),
		m_iPrevEnd, m_iEndSample);*/

	m_iPrevStart = m_iPrevEnd = -1;

	m_bEdit = FALSE;

	getTrack()->InvalidateRegions( this );

	getTracks()->SetChanged( TRUE );

	getTracks()->SetViewUpdatedWhilePlaying(TRUE);
}
/*
void CRegion::ddump()
{
	IFloopySoundInput *track = getTrack()->GetInput();
	int offset = 0;
	do
	{
		float value = 0.f;
		if(track->GetParamAt(offset, TIMELINE_PARAM_ENABLE, &value))
			wxLogTrace(_T("CRegion"), _T("%d\t%s"), offset, value==PARAM_VALUE_ENABLED?"ON":"OFF");

		//for(int index=0; index<track->GetParamCount(); index++)
		//{
		//	float value = 0;
		//	if(track->GetParamAt(offset, index, &value))
		//		wxLogTrace(_T("CRegion"), _T("%d\t%d:%.3f"), offset, index, value);
		//}
		offset = track->GetNextOffset(offset);
	} while(offset > 0);
}
*/
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
		//	m_bDrag = TRUE;

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

		m_bEdit = TRUE;
	}
}

void CRegion::calcPos(int *left, int *right)
{
	int nSamplesPerPixel = getTracks()->GetSamplesPerPixel();

	*left  = m_iStartSample / nSamplesPerPixel;
	*right = m_iEndSample   / nSamplesPerPixel;

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

BOOL CRegion::GetReset()
{
//	return m_bReset;

	return getReset( m_iStartSample );

	/*float value = 0.f;
	if(getTrack()->GetInput()->GetParamAt(m_iStartSample, TIMELINE_PARAM_MOVETO, &value))
		return value==0.f;
	else
		return FALSE;*/
}

BOOL CRegion::getReset(int sample)
{
	float value = 0.f;
	if(getTrack()->GetInput()->GetParamAt(sample, TIMELINE_PARAM_MOVETO, &value))
		return value==0.f;
	else
		return FALSE;
}

void CRegion::SetReset(BOOL bReset)
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

	getTrack()->InvalidateRegions( this );
}

void CRegion::Select(bool selected)
{
	IFloopyObj::Select(selected);

	Refresh();
}







/////////////////////////////////////////////////////////////////////
// CBorder functions
/////////////////////////////////////////////////////////////////////
void CRegion::CBorder::Move(int dx, int WXUNUSED(dy))
{
	if(m_bLeft) {
		getRegion()->Resize(dx, 0);
	} else {
		getRegion()->Resize(0, dx);
	}
	// Too slow
	//getRegion()->Invalidate();
	//getRegion()->Refresh();
}

void CRegion::CBorder::DrawBG(wxDC& dc, wxRect& rc)
{

}

void CRegion::CBorder::DrawFore(wxDC& dc, wxRect& rc)
{

}
