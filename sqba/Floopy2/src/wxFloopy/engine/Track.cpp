// Track.cpp: implementation of the CTrack class.
//
//////////////////////////////////////////////////////////////////////

//#include "Track.h"
#include "tracks.h"
#include <stdio.h>
#include <wx/caret.h>

#define MIN_HEIGHT	15

IMPLEMENT_DYNAMIC_CLASS(CTrack, IFloopyObj)

WX_DEFINE_LIST(RegionList);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrack::CTrack(CTracks *tracks, IFloopySoundInput *input, IFloopySoundInput *parent, int level)
 : IFloopyObj(tracks)
{
	wxLog::AddTraceMask(_T("CTrack"));

	m_nLevel = level;

	m_pInput = CTracks::GetComponent(input, "track");
	//m_pInput = input;
	m_pSource = parent;

	/*if( m_pSource->IsFilter() )
	{
		m_pSource = ((IFloopySoundFilter*)m_pSource)->GetSource();
	}*/

	//m_pInput = GetComponent("track");
	assert(NULL != m_pInput);

	m_bHide = false;


	char *name = input->GetDisplayName();
	if(!name || strlen(name) == 0)
		name = input->GetName();
	
	char *tmp = strrchr(name, '\\');
	if(tmp)
		name = tmp + 1;

	m_name    = name;
	m_height  = MIN_HEIGHT*3;
	m_top     = 0;
	m_pBitmap = new wxBitmap();
	m_pBitmap->LoadFile("res/help.bmp", wxBITMAP_TYPE_BMP);
	m_pBorder = new CBorder(this);
//	m_pDisplay = NULL;

	m_pMenu = new CMenu( this );
	m_pMenu->Append(_T("&New track..."), NULL);
	m_pMenu->Append(_T("&New event..."), NULL);
	m_pMenu->Append(_T("&Properties..."), NULL);
	m_pMenu->Append(_T("&Remove track..."), NULL);

	loadRegions();

	m_pButtonLoop = new CLoopButton(this);
	m_pButtonCache = new CCacheButton(this);

	m_bReset = TRUE;

//	m_pDisplay = new CWaveDisplay(this);

#ifdef _DEBUG	// test only ///////////////////////
//	LoadDisplay(_T("WavDisplay"));
#endif
}

CTrack::~CTrack()
{
	WX_CLEAR_LIST(RegionList, m_regions);
	delete m_pBitmap;
	delete m_pBorder;
	/*if(m_pDisplay) {
		delete m_pDisplay;
		//m_libDisplay.Unload();
	}*/
	delete m_pButtonLoop;
	delete m_pButtonCache;
}

/////////////////////////////////////////////////////////////////////////////
// DrawLabel
//! Draws track's label.
//! \param dc [in] reference to the device context
//! \param rc [in] track label's rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::DrawLabel(wxDC& dc, wxRect& rc)
{
	m_rcLabel = rc;

	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	wxPen pen( *wxMEDIUM_GREY_PEN );
	pen.SetWidth(1);
	dc.SetPen( pen );

	//wxBrush brush(m_colour, (IsSelected() ? wxCROSSDIAG_HATCH : wxSOLID));
	wxBrush brush(GetBGColor(), wxSOLID);
	dc.SetBrush(brush);

	int left, top, width, height;

	if(TRUE)
	{
		// Draw background
		left   = m_nLevel*4+2;
		top    = rc.GetTop()+1;
		width  = rc.GetWidth()-left-3;
		height = m_height-2;
		dc.DrawRoundedRectangle(left, top, width, height, 4);
	}
	else
	{
		// Draw aqua background
		left   = 0;
		top    = rc.GetTop();
		width  = rc.GetWidth()-left;
		height = m_height;
		dc.DrawRectangle(left, top, width, height);
		DrawAquaRect(dc, wxRect(left+1, top+1, width-2, height-2));
	}

	dc.SetTextForeground( GetForeColor() );
	wxFont font = dc.GetFont();
	font.SetWeight(IsSelected() ? wxBOLD : wxNORMAL);
	//font.SetPointSize(m_height / 4);
	font.SetPointSize( 9 );
	dc.SetFont(font);
	//wxFont font(12, wxDEFAULT, wxITALIC, (IsSelected() ? wxBOLD : wxNORMAL));
	//dc.SetFont(font);
	////dc.SetFont((IsSelected() ? *wxITALIC_FONT : *wxNORMAL_FONT));

	// Get text dimensions
	wxCoord w=0, h=0;
	dc.GetTextExtent(m_name, &w, &h);

	/*int ptSize = m_height / 2;
	// Get text dimensions
	wxCoord w=0, h=0;
	do {
		font.SetPointSize(ptSize);
		dc.GetTextExtent(m_name, &w, &h);
		ptSize--;
	} while (w>width && ptSize > 5);*/


	// Draw text
	int x = left + 5;//width/2 - w/2;
	int y = (rc.GetTop() + (m_height/4) - h/2);
	dc.DrawText( m_name, x, y );
	m_rcLabel.SetHeight(m_height);


	int n = m_height/2-2;
	if(n > 20)
		n = 20;
	drawLoopSign(dc,  wxRect(5, top+height-n-2, n, n));
	drawCacheSign(dc, wxRect(n+5+1, top+height-n-2, n, n));


	dc.SetPen(oldpen);
	dc.SetBrush( oldBrush );
}

/////////////////////////////////////////////////////////////////////////////
// DrawBG
//! Draws track's background.
//! \param dc [in] reference to the device context
//! \param rc [in] track's rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::DrawBG(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	m_top = rc.GetTop();

	if( IsSelected() )
	{
		dc.SetPen( *wxTRANSPARENT_PEN );
		wxBrush brush(wxColor(240, 240, 240), wxSOLID);
		dc.SetBrush(brush);
		dc.DrawRectangle(0, m_top, rc.GetWidth(), GetHeight());

		//wxRect rcBG(0, m_top, rc.GetWidth(), GetHeight());
		//DrawAquaRect(dc, rcBG);
	}
	/*else
	{
		dc.SetPen( *wxTRANSPARENT_PEN );
		wxBrush brush(wxColor(255, 255, 255), wxSOLID);
		dc.SetBrush(brush);
		dc.DrawRectangle(0, m_top, rc.GetWidth(), GetHeight());
	}*/

	dc.SetPen( *wxMEDIUM_GREY_PEN );

	int y = m_top + m_height;
	//dc.DrawLine(0, y, rc.GetWidth(), y);

	wxRect rcBorder(0, y, rc.GetWidth(), m_pBorder->GetHeight());
	m_pBorder->DrawFore(dc, rcBorder);

	dc.SetBrush( oldBrush );
	dc.SetPen( oldpen );
}

/////////////////////////////////////////////////////////////////////////////
// DrawFore
//! Draws track's foreground.
//! \param dc [in] reference to the device context
//! \param rc [in] track's rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::DrawFore(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();

	m_top = rc.GetTop();

	dc.SetBrush( wxBrush(m_color) );

	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		region->DrawBG(dc, rc);
		region->DrawFore(dc, rc);
		node = node->GetNext();
	}

	dc.SetBrush( oldBrush );
}

/////////////////////////////////////////////////////////////////////////////
// DrawPreview
//! Draws track preview (in the preview bar).
//! \param dc [in] reference to the device context
//! \param rc [in] track's preview rectangle
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::DrawPreview(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();
	dc.SetBrush( wxBrush(m_color) );
	dc.SetPen( wxPen(m_color) );

	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		region->DrawPreview(dc, rc);
		node = node->GetNext();
	}

	dc.SetBrush( oldBrush );
	dc.SetPen(oldpen);
}

/////////////////////////////////////////////////////////////////////////////
// HitTest
//! Returns TRUE if the vertical coordinate y is in the track.
//! \param y [in] vertical coordinate
//! \return TRUE if the vertical coordinate y is in the track
/////////////////////////////////////////////////////////////////////////////
bool CTrack::HitTest(int y)
{
	if(m_bHide)
		return false;
	return m_rcLabel.Inside(1, y);
}

CRegion *CTrack::AddNewRegionAt(int left)
{
	wxLogTrace(_T("CTrack"), _T("Adding new region"));

	int nSamplesPerPixel = GetTracks()->GetSamplesPerPixel();
	int start = left * nSamplesPerPixel;
	int end   = start + nSamplesPerPixel;

	// Dodati novi event u input
	m_pInput->EnableAt(start, TRUE);
	m_pInput->EnableAt(end, FALSE);

	CRegion *region = new CRegion( this, start, end );
	try {
		region->SetReset( GetReset() );
		m_regions.Append( region );
		region->Refresh();

		/*if( !GetReset() )
		{
			Invalidate();
			Refresh();
		}*/
	} catch(...) {
		wxLogTrace(_T("CTrack"), _T("AddNewRegionAt exception"));
		return NULL;
	}
	return region;
}

CRegion *CTrack::AddRegion(int start, int end)
{
	wxLogTrace(_T("CTrack"), _T("Adding new region"));
	CRegion *region = new CRegion( this, start, end );
	try {
		//region->SetReset( GetReset() );

		m_regions.Append( region );
		/*if( !GetReset() )
		{
			Invalidate();
			Refresh();
		}*/
	} catch(...) {
		wxLogTrace(_T("CTrack"), _T("AddRegion exception"));
		return NULL;
	}
	return region;
}

bool CTrack::RemoveRegion(CRegion *region)
{
	wxLogTrace(_T("CTrack"), _T("Removing region"));
	try {
		//region->Refresh();
		region->Remove();
		if(m_regions.DeleteObject( region ))
			delete region;
	} catch(...) {
		wxLogTrace(_T("CTrack"), _T("RemoveRegion exception"));
		return FALSE;
	}
	return TRUE;
}

IFloopyObj *CTrack::GetChildAt(int x, int y)
{
	if(x > GetWidth())
		return NULL;

	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *event = (CRegion*)node->GetData();
		if(event->HitTest(x, y))
			return event->GetChildAt(x, y);
		node = node->GetNext();
	}

	//if((y >= m_top) && (y < m_top+m_height) && (x < GetWidth()))
	if((y >= m_top) && (y < m_top+m_height))
		return this;
	else if(y == m_top+m_height)
		return m_pBorder;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// DeselectAllRegions
//! Deselects all track's events.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::DeselectAllRegions()
{
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		//if( region->IsSelected() )
		//	region->Refresh();
		region->Select(FALSE);
		node = node->GetNext();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Refresh
//! Redraws the track and it's label.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::Refresh()
{
	wxScrolledWindow *panel = GetTracks()->GetTracksView();

	int xOrig=0, yOrig=0;
	panel->GetViewStart(&xOrig, &yOrig);

	int xScrollUnits=0, yScrollUnits=0;
	panel->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	xOrig *= xScrollUnits;
	yOrig *= yScrollUnits;

	m_top -= yOrig;

	if(panel) {
		int width = panel->GetVirtualSize().GetWidth();
		wxRect rc(0, m_top, width, m_height+2);
		panel->RefreshRect(rc);
	}

	panel = GetTracks()->GetLabelsView();
	if(panel)
	{
		wxSize size = panel->GetSize();
		wxRect rc(0, m_top, size.GetWidth(), m_height);
		panel->RefreshRect(rc);
	}
}

void CTrack::Invalidate()
{
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		region->Invalidate();
		node = node->GetNext();
	}
}

/////////////////////////////////////////////////////////////////////////////
// MoveSelectedRegions
//! Horizontally moves all selected events for the dx offset.
//! \param dx [in] offset in pixels
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::MoveSelectedRegions(int dx)
{
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		if( region->IsSelected() )
		{
			//region->Refresh();
			region->Move(dx, 0);
			//region->Refresh();
		}
		node = node->GetNext();
	}
}

void CTrack::UpdateSelectedRegions()
{
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		if( region->IsSelected() )
			region->Update();
		node = node->GetNext();
	}
}

void CTrack::RemoveSelectedObjects()
{
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		node = node->GetNext();
		if( region->IsSelected() )
			RemoveRegion( region );
	}
}

/////////////////////////////////////////////////////////////////////////////
// SetHeight
//! Sets track height.
//! \param height [in] new track height in pixels
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::SetHeight(int height)
{
	if( MIN_HEIGHT <= height )
	{
		m_height = height - m_pBorder->GetHeight();

		Refresh();
		GetTracks()->RefreshTracks(this);

		GetTracks()->SetCaretPos(GetTracks()->GetCaretPos());
	}
}

void CTrack::Move(int dx, int dy)
{
	if( 0 != dx)
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// LoadDisplay
//! Creates an event displaying object from the dynamic library.
//! \param strType [in] Library name only, no extension!
//! \return bool
/////////////////////////////////////////////////////////////////////////////
bool CTrack::LoadDisplay(wxString strType)
{
	bool result = FALSE;
	//wxDynamicLibrary dlib;

	/*if( m_libDisplay.Load(strType) ) {
		typedef IFloopyObj* (*funcType)(void);
		//typedef void (*funcType)(IFloopyObj**);
		funcType func = (funcType)m_libDisplay.GetSymbol(_T("CreateDisplay"));
		if ( !func ) {
			wxLogTrace(_T("CTrack"), _T("CreateDisplay function not found!"));
		} else {
			try {
				m_pDisplay = func();
				//func( &m_pDisplay );
				result = TRUE;
			} catch(...) {
				wxLogTrace(_T("CTrack"), _T("CreateDisplay failed!"));
				m_libDisplay.Unload();
			}
		}
	}*/
	return result;
}

void CTrack::Dump(ostream& stream)
{
	/*stream << _T("CTrack") << '\n';

	if( m_pBitmap )
		stream << m_pBitmap << '\n';
	if( m_pBorder )
		stream << m_pBorder << '\n';
	if( m_pDisplay )
		stream << m_pDisplay << '\n';

	stream << '\n';*/
}

/*
void CTrack::RefreshLabel(wxPanel *panel)
{
	wxSize size = panel->GetSize();
	wxRect rc(0, m_top, size.GetWidth(), size.GetHeight());
	panel->RefreshRect(rc);
}
*/

/*
void CTrack::Select(bool sel)
{
	m_bSelected = sel;
	if(sel)
		wxLogTrace(_T("CTrack"), _T("Selected track: %s"), m_name);
}
*/
void CTrack::loadRegions()
{
	int start=-1, end=-1;
	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	float freq = (float)fmt->frequency;

	m_pInput->Reset();

	int offset=0;
	do {
		m_pInput->MoveTo(offset);

		float paramVal = 0.f;
		if(m_pInput->GetParamVal(TIMELINE_PARAM_ENABLE, &paramVal))
		{
			if((paramVal == PARAM_VALUE_ENABLED) && (start == -1))
				start = offset;
			else if((paramVal == PARAM_VALUE_DISABLED) && (start >= 0))
			{
				end = offset;
				AddRegion(start, end);
				start = end = -1;
			}
		}

		offset = m_pInput->GetNextOffset(offset);
	} while (offset > 0);

	if(end == -1 && start >= 0)
	{
		end = m_pInput->GetSize();
		AddRegion(start, end);
	}
}

bool CTrack::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode() )
	{
	case WXK_UP:
	case WXK_NUMPAD_UP:
		SetHeight( GetHeight() + 1 );
		return true;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		SetHeight( GetHeight() - 1 );
		return true;
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
		{
			int pos = GetTracks()->GetCaretPos();
			pos -= GetTracks()->GetSamplesPerPixel();
			GetTracks()->SetCaretPos( pos );
			break;
		}
		return true;
	case '-':
		GetTracks()->SetSamplesPerPixel( GetTracks()->GetSamplesPerPixel()*2 );
		return true;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
		{
			int pos = GetTracks()->GetCaretPos();
			pos += GetTracks()->GetSamplesPerPixel();
			GetTracks()->SetCaretPos( pos );
			break;
		}
		return true;
	case '+':
		GetTracks()->SetSamplesPerPixel( GetTracks()->GetSamplesPerPixel()/2 );
		return true;
	case WXK_DELETE:
		GetTracks()->RemoveTrack( this );
		return true;
	case 'l':
	case 'L':
		SetLooped( !IsLooped() );
		return true;
	case 'r':
	case 'R':
		SetReset( !GetReset() );
		return true;
	case 'v':
	case 'V':
		SetReverse( !IsReverse() );
		return true;
	case WXK_F5:
	case WXK_SPACE:
		if(GetTracks()->IsPlaying())
			GetTracks()->Stop();
		else
			GetTracks()->Play();
		return true;
	case WXK_END:
	case WXK_RETURN:
		if(GetTracks()->IsPlaying())
			GetTracks()->Stop();
		return true;
	default:
		return GetTracks()->OnKeyDown(event);
	}
		
	return false;
}

IFloopyObj *CTrack::GetSelectedObj()
{
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *event = (CRegion*)node->GetData();
		IFloopyObj *tmp = event->GetSelectedObj();
		if(tmp)
			return tmp;
		node = node->GetNext();
	}
	if(IsSelected())
		return this;
	return NULL;
}

void CTrack::OnMouseEvent(wxMouseEvent& event)
{
	/*if( event.LeftDown() )
	{
		wxScrolledWindow *panel = getTracks()->GetPanel();
		wxCaret *caret = panel->GetCaret();
		caret->Show(FALSE);

		int x=0;
		caret->GetPosition(&x, NULL);

		int xScrollUnits=0, yScrollUnits=0;
		panel->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );
		int xOrig=0, yOrig=0;
		panel->GetViewStart(&xOrig, &yOrig);
		xOrig *= xScrollUnits;
		yOrig *= yScrollUnits;
	
		if( IsSelected() ) {
			caret->SetSize(1, GetHeight());
			caret->Move( x-xOrig, GetTop()-yOrig );
		} else {
			getTracks()->DeselectAllTracks();
			getTracks()->DeselectAllRegions();

			caret->SetSize(1, panel->GetSize().GetHeight());
			caret->Move( x-xOrig, 0 );
		}
		caret->Show(TRUE);

		///////////////////////////////////////////
		// Add new event
		int x1 = event.GetX() + xOrig;
		CRegion *evt = AddEventAt(x1);
		//m_pSelectedObj = evt->GetBorder(FALSE);
		evt->Refresh();
		///////////////////////////////////////////
	}

	if( event.LeftUp() )
	{
	}*/
}







/**
 * Checks if the event intersects any other event in the track.
 * @param pEvent1 Pointer to the event being moved ir resized.
 * @param left New left coordinate being tested.
 * @param right New right coordinate being tested.
 * @param bResize
 * @return void.
 */
void CTrack::CheckIntersections(CRegion *pEvent1, int &left, int &right, bool bResize)
{
	enum Direction
	{
		dir_none,
		dir_left,
		dir_right
	} iDir = dir_none, iLastDir = dir_none;

	if((left == 0) || (right == GetTracks()->GetWidth()))
		return;

	int width = right - left;

	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *pEvent2 = (CRegion*)node->GetData();

		if (pEvent2 != pEvent1)
		{
			int nRes = 0;
			int l1 = left,  l2 = pEvent2->GetLeft();
			int r1 = right, r2 = pEvent2->GetRight();

			if((l2 >= l1) && (l2 <= r1))
				nRes = r1 - l2;		// pEvent2 starts after
			else if((l1 >= l2) && (l1 <= r2))
				nRes = -(r2 - l1);	// pEvent2 starts before

			iDir = (nRes > 0 ? dir_left : (nRes < 0 ? dir_right : dir_none));

			if(iDir != dir_none)
			{
				if(!bResize) // Event is being translated
				{
					if (iDir == dir_left)
					{
						left  = (iLastDir == dir_right ? r2 : l2-width);
						right = (iLastDir == dir_right ? r2+width : l2);
					}
					else if (iDir == dir_right)
					{
						left  = (iLastDir == dir_left ? l2-width : r2);
						right = (iLastDir == dir_left ? l2 : r2+width);
					}
				}
				else		// Event is being resized
				{
					if (iDir == dir_left)
						right = l2;
					else if (iDir == dir_right)
						left  = r2;
					break;
				}
			}

			iLastDir = iDir;
		}

		node = node->GetNext();
	}
	
//	assert(left>0 && right>0);
}

void CTrack::drawLoopSign(wxDC& dc, wxRect& rc)
{
	m_pButtonLoop->DrawBG(dc, rc);
	m_pButtonLoop->DrawFore(dc, rc);
}

void CTrack::drawCacheSign(wxDC& dc, wxRect& rc)
{
	m_pButtonCache->DrawBG(dc, rc);
	m_pButtonCache->DrawFore(dc, rc);
}

BOOL CTrack::IsLooped()
{
	IFloopySoundFilter *loop = (IFloopySoundFilter*)GetComponent("loop");
	if(loop)
		return !loop->GetBypass();
	return FALSE;
}

void CTrack::SetLooped(BOOL bLooped)
{
	IFloopySoundFilter *loop = (IFloopySoundFilter*)GetComponent("loop");
	if(loop)
	{
		//loop->ClearAllParams();
		//loop->EnableAt(0, bLooped);
		loop->SetBypass( !bLooped );
		Invalidate();
		Refresh();
	}
	else if(bLooped)
	{
		if(m_pInput->GetType() == TYPE_FLOOPY_SOUND_TRACK)
		{
			IFloopySoundInput *src = ((IFloopySoundFilter*)m_pInput)->GetSource();
			if(src->GetSize() == SIZE_INFINITE)
				return; // No need

			IFloopySoundEngine *engine = GetTracks()->GetEngine();
			IFloopySoundFilter *loop = (IFloopySoundFilter*)engine->CreateInput("stdlib.loop");
			if(loop)
			{
				loop->EnableAt(0, TRUE);
				loop->SetBypass(FALSE);

				loop->SetSource( src );
				((IFloopySoundFilter*)m_pInput)->SetSource( loop );

				Invalidate();
				Refresh();
			}
		}
	}
}

BOOL CTrack::IsReverse()
{
	IFloopySoundFilter *reverse = (IFloopySoundFilter*)GetComponent("reverse");
	if(reverse)
		return !reverse->GetBypass();
	return FALSE;
}

void CTrack::SetReverse(BOOL bReverse)
{
	IFloopySoundFilter *reverse = (IFloopySoundFilter*)GetComponent("reverse");
	if(reverse)
	{
		//loop->ClearAllParams();
		//loop->EnableAt(0, bLooped);
		reverse->SetBypass( !bReverse );
		Invalidate();
		Refresh();
	}
	else if(bReverse)
	{
		if(m_pInput->GetType() == TYPE_FLOOPY_SOUND_TRACK)
		{
			IFloopySoundInput *src = ((IFloopySoundFilter*)m_pInput)->GetSource();
			while(src && src->GetSize() == SIZE_INFINITE)
			{
				int t = src->GetType();
				if(t == TYPE_FLOOPY_SOUND_FILTER || t == TYPE_FLOOPY_SOUND_MIXER || t == TYPE_FLOOPY_SOUND_TRACK)
					src = ((IFloopySoundFilter*)src)->GetSource();
				else
					src = NULL;
			}

			if(!src)
				return; // No need

			IFloopySoundEngine *engine = GetTracks()->GetEngine();
			IFloopySoundFilter *reverse = (IFloopySoundFilter*)engine->CreateInput("stdlib.reverse");
			if(reverse)
			{
				reverse->EnableAt(0, TRUE);
				reverse->SetBypass(FALSE);

				reverse->SetSource( src );
				((IFloopySoundFilter*)m_pInput)->SetSource( reverse );

				Invalidate();
				Refresh();
			}
		}
	}
}

BOOL CTrack::GetReset()
{
	return m_bReset;

	/*if(m_pInput->GetType() == TYPE_FLOOPY_SOUND_TRACK)
	{
		float value =0.f;
		if(m_pInput->GetParamAt(0, 0, &value))
			return value != 0.f;
	}
	return FALSE;*/
}

void CTrack::SetReset(BOOL bReset)
{
	m_bReset = bReset;

	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		region->SetReset(bReset);
		node = node->GetNext();
	}
}

IFloopySoundInput *CTrack::GetComponent(char *name)
{
	return CTracks::GetComponent(m_pInput, name);

	/*IFloopySoundInput *src = m_pInput;
	while(src)
	{
		char *tmp = strrchr(src->GetName(), '.');
		if(NULL == tmp)
			tmp = src->GetName();
		else
			tmp++;
		if(0==strcmpi(tmp, name))
			return src;

		int type = src->GetType();
		switch(type)
		{
		case TYPE_FLOOPY_SOUND_FILTER:
		case TYPE_FLOOPY_SOUND_MIXER:
		case TYPE_FLOOPY_SOUND_TRACK:
		{
			src = ((IFloopySoundFilter*)src)->GetSource();
			break;
		}
		default:
			src = NULL;
		}
	}
	return NULL;*/
}

void CTrack::InvalidateRegions(CRegion *start)
{
//	BOOL bStart = FALSE;
	int startOffset = start->GetStartOffset();
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
//		if(start == region)
//			bStart = TRUE;
//		else if( bStart && !region->GetReset() )
		if( (region->GetStartOffset() > startOffset) && (!region->GetReset()) )
		{
			// Skip if there was a reset in between
			region->Invalidate();
			region->Refresh();
		}
		node = node->GetNext();
	}

	/*RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		if( start != region && !region->GetReset() )
		{
			region->Invalidate();
			region->Refresh();
		}
		node = node->GetNext();
	}*/
}

wxColour CTrack::GetColor()
{
	/*UINT r, g, b;
	if( m_pInput->GetColor(&r, &g, &b) )
		return wxColor(r, g, b);
	else*/
		return m_color;
}

void CTrack::SetColor(wxColour color)
{
	m_color = color;
	if(m_pInput)
		m_pInput->SetColor(color.Red(), color.Green(), color.Blue());
}

wxColour CTrack::GetBGColor()
{
	wxColor color = GetColor();
	if( IsSelected() )
		color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());
	return color;
}

wxColour CTrack::GetForeColor()
{
	wxColor color = GetColor();
	if( !IsSelected() )
		color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());
	return color;
}

void CTrack::Select(bool selected)
{
	IFloopyObj::Select(selected);

	if(!selected)
		DeselectAllRegions();

	Refresh();

	CTracks *pTracks = GetTracks();

	int caretpos = pTracks->GetCaretPos();

	BOOL bPlaying = FALSE;

	if(pTracks->IsPlaying())
	{
		bPlaying = TRUE;
		pTracks->Stop();
	}

	pTracks->SetCaretPos( caretpos );

	if(bPlaying)
		pTracks->Play();
}


/////////////////////////////////////////////////////////////////////
// CBorder functions
/////////////////////////////////////////////////////////////////////
void CTrack::CBorder::Move(int WXUNUSED(dx), int dy)
{
	int height = getTrack()->GetHeight();
	getTrack()->SetHeight(height + dy);
}

void CTrack::CBorder::DrawBG(wxDC& dc, wxRect& rc)
{

}

int CTrack::CBorder::GetHeight()
{
	return 1;
}

void CTrack::CBorder::DrawFore(wxDC& dc, wxRect& rc)
{
	wxPen oldpen = dc.GetPen();

	dc.SetPen( *wxMEDIUM_GREY_PEN );

	int top = rc.GetTop();
	int height = rc.GetHeight();

	int y = top + height;
	dc.DrawLine(0, y, rc.GetWidth(), y);

	dc.SetPen( oldpen );
}



/////////////////////////////////////////////////////////////////////
// CLoopButton functions
/////////////////////////////////////////////////////////////////////
void CTrack::CLoopButton::DrawFore(wxDC& dc, wxRect& rc)
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
void CTrack::CCacheButton::DrawFore(wxDC& dc, wxRect& rc)
{
	wxBrush oldBrush = dc.GetBrush();
	wxPen oldpen = dc.GetPen();

	wxPen pen( *wxLIGHT_GREY );
	pen.SetWidth(1);
	dc.SetPen( pen );

	//wxBrush brush(m_colour, (IsSelected() ? wxCROSSDIAG_HATCH : wxSOLID));
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
