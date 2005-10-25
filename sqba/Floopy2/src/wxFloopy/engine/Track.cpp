// Track.cpp: implementation of the CTrack class.
//
//////////////////////////////////////////////////////////////////////

//#include "Track.h"
#include <stdio.h>
#include "track.h"
#include "tracks.h"
#include "region.h"
#include "label.h"
#include "../util/util.h"
#include <wx/caret.h>

#define MIN_HEIGHT	15

//IMPLEMENT_DYNAMIC_CLASS(CTrack, IFloopyObj)

WX_DEFINE_LIST(RegionList);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrack::CTrack(CTracks *tracks, IFloopySoundInput *input, int level)
 : IFloopyObj(tracks)
{
	wxLog::AddTraceMask(_T("CTrack"));

	m_nLevel = level;

	m_pInput = input;
	m_pTrack = CTracks::FindComponentByName(input, "track");


	m_pSource = input;

	// Check if input is mono to stereo converter:
	// if it is then draw mono source.
	if( CTracks::IsFilter(input) )
	{
		char *tmp = CTracks::GetComponentName(input);
		if(0==strcmpi(tmp, "mono2stereo"))
			m_pSource = ((IFloopySoundFilter*)input)->GetSource();
	}

/*
	m_pSource = CTracks::FindComponentByName(input, "mono2stereo");
	if(NULL == m_pSource)
		m_pSource = input;
	else
		m_pSource = ((IFloopySoundFilter*)m_pSource)->GetSource();
*/

	assert(NULL != m_pTrack);


	char *name = input->GetDisplayName();
	if(!name || strlen(name) == 0)
		name = input->GetName();
	
	char *tmp = strrchr(name, '\\');
	if(tmp)
		name = tmp + 1;

	/*m_height  = MIN_HEIGHT*3;
	int index = 0;
	if(m_pTrack->GetPropertyIndex("height", &index))
	{
		float val = 0;
		if(m_pTrack->GetPropertyVal(index, &val))
			m_height = (int)val;
	}*/

	m_name    = name;
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

	m_pLabel = new CLabel( this );

	m_bReset = true;

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
	delete m_pLabel;
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
		dc.DrawRectangle(0, m_top+1, rc.GetWidth(), GetHeight());

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

	int y = m_top + GetHeight() - m_pBorder->GetHeight();
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

CRegion *CTrack::AddNewRegionAt(int left)
{
	wxLogTrace(_T("CTrack"), _T("Adding new region"));

	int nSamplesPerPixel = GetTracks()->GetSamplesPerPixel();
	int start = left * nSamplesPerPixel;
	int end   = start + nSamplesPerPixel;

	// Dodati novi event u input
	m_pTrack->EnableAt(start, true);
	m_pTrack->EnableAt(end, false);

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
		if(region->GetEndPos() - region->GetStartPos() > GetTracks()->GetSamplesPerPixel())
			InvalidateRegions( region );
		if(m_regions.DeleteObject( region ))
			delete region;
	} catch(...) {
		wxLogTrace(_T("CTrack"), _T("RemoveRegion exception"));
		return false;
	}
	return true;
}

IFloopyObj *CTrack::GetChildAt(int x, int y)
{
	int bottom = m_top + GetHeight();

	if(y<m_top || y>bottom)
		return NULL;

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

	if(y>=m_top && y<bottom)
		return this;
	else if(y == bottom)
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
		region->Select(false);
		node = node->GetNext();
	}
}


CRegion *CTrack::GetRegion(int index)
{
	int count = 0;
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		if(count == index)
			return region;
		node = node->GetNext();
		count++;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Refresh
//! Redraws the track and it's label.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::Refresh()
{
	CRulerView *panel = GetTracks()->GetTimelineView();
/*
	int xOrig=0, yOrig=0;
	panel->GetViewStart(&xOrig, &yOrig);

	int xScrollUnits=0, yScrollUnits=0;
	panel->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	xOrig *= xScrollUnits;
	yOrig *= yScrollUnits;

	m_top -= yOrig;
*/
	int x, y;
	panel->GetViewStart2(&x, &y);
	int top = m_top - y;

	if(panel)
	{
		int width = panel->GetVirtualSize().GetWidth();
		wxRect rc(0, top, width, GetHeight()+2);
		panel->RefreshRect(rc);
	}

	panel = GetTracks()->GetLabelsView();
	if(panel)
	{
		wxSize size = panel->GetSize();
		wxRect rc(0, top, size.GetWidth(), GetHeight());
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

bool CTrack::RemoveSelectedObjects()
{
	bool bResult = false;
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		node = node->GetNext();
		if( region->IsSelected() )
		{
			if( !region->RemoveSelectedObjects() )
				RemoveRegion( region );
			bResult = true;
		}
	}
	return bResult;
}

int CTrack::GetHeight()
{
	int height = MIN_HEIGHT*3;
	int index = 0;
	if(m_pTrack->GetPropertyIndex("height", &index))
	{
		float val = 0;
		if(m_pTrack->GetPropertyVal(index, &val))
			height = (int)val;
	}

	return height + m_pBorder->GetHeight();
}

/////////////////////////////////////////////////////////////////////////////
// SetHeight
//! Sets track height.
//! \param height [in] new track height in pixels
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTrack::SetHeight(int height)
{
	height -= m_pBorder->GetHeight();
	if( MIN_HEIGHT <= height )
	{
		int index = 0;
		if(m_pTrack->GetPropertyIndex("height", &index))
		{
			m_pTrack->SetPropertyVal(index, (float)height);

			Refresh();
			
			GetTracks()->RefreshTracks(this);

			GetTracks()->SetCaretPos(GetTracks()->GetCaretPos());
		}
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
	bool result = false;
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
				result = true;
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
	SOUNDFORMAT *fmt = m_pTrack->GetFormat();
	float freq = (float)fmt->frequency;

	m_pTrack->Reset();

	int offset=0;
	do {
		m_pTrack->MoveTo(offset);

		float paramVal = 0.f;
		if(m_pTrack->GetParamVal(TIMELINE_PARAM_ENABLE, &paramVal))
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

		offset = m_pTrack->GetNextOffset(offset);
	} while (offset > 0);

	if(end == -1 && start >= 0)
	{
		end = m_pTrack->GetSize();
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
	case 'p':
	case 'P':
		SetDrawPreview( !IsDrawPreviewOn() );
		Invalidate();
		Refresh();
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
		CRegion *region = (CRegion*)node->GetData();
		IFloopyObj *tmp = region->GetSelectedObj();
		if(tmp)
			return tmp;
		node = node->GetNext();
	}
	if(IsSelected())
		return this;
	return NULL;
}

bool CTrack::OnMouseEvent(wxMouseEvent& event)
{
	bool bResult = false;
	/*if(event.LeftDown())
	{
		if( !IsSelected() )
			m_pTracks->DeselectAllTracks();
		m_pTracks->DeselectAllRegions();
		m_pTracks->SetCaretPos(m_pTracks->GetCaretPos());
		CRegion *region = AddNewRegionAt(x);
		m_pSelectedObj = region->GetBorder(false);
		bResult = true;
	}*/
	IFloopyObj::OnMouseEvent(event);
	return bResult;
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

bool CTrack::IsLooped()
{
	IFloopySoundFilter *loop = (IFloopySoundFilter*)FindComponentByName("loop");
	if(loop)
		return !loop->GetBypass();
	return false;
}

void CTrack::SetLooped(bool bLooped)
{
	IFloopySoundFilter *loop = (IFloopySoundFilter*)FindComponentByName("loop");
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
		if(m_pTrack->GetType() == TYPE_FLOOPY_SOUND_TRACK)
		{
			IFloopySoundInput *src = ((IFloopySoundFilter*)m_pTrack)->GetSource();
			if(src->GetSize() == SIZE_INFINITE)
				return; // No need

			IFloopySoundEngine *engine = (IFloopySoundEngine*)GetTracks()->GetInput();
			IFloopySoundFilter *loop = (IFloopySoundFilter*)engine->CreateInput("stdlib.loop");
			if(loop)
			{
				loop->EnableAt(0, true);
				loop->SetBypass(false);

				loop->SetSource( src );
				((IFloopySoundFilter*)m_pTrack)->SetSource( loop );

				Invalidate();
				Refresh();
			}
		}
	}
}

bool CTrack::IsReverse()
{
	IFloopySoundFilter *reverse = (IFloopySoundFilter*)FindComponentByName("reverse");
	if(reverse)
		return !reverse->GetBypass();
	return false;
}

void CTrack::SetReverse(bool bReverse)
{
	IFloopySoundFilter *reverse = (IFloopySoundFilter*)FindComponentByName("reverse");
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
		if(m_pTrack->GetType() == TYPE_FLOOPY_SOUND_TRACK)
		{
			IFloopySoundInput *src = ((IFloopySoundFilter*)m_pTrack)->GetSource();
			while(src && src->GetSize() == SIZE_INFINITE)
			{
				/*int t = src->GetType();
				if(t == TYPE_FLOOPY_SOUND_FILTER || t == TYPE_FLOOPY_SOUND_MIXER || t == TYPE_FLOOPY_SOUND_TRACK)
					src = ((IFloopySoundFilter*)src)->GetSource();
				else
					src = NULL;*/
				int type = src->GetType();
				if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
					src = ((IFloopySoundFilter*)src)->GetSource();
				else
					src = NULL;
			}

			if(!src)
				return; // No need

			IFloopySoundEngine *engine = GetTracks()->GetEngine();
			assert(engine != NULL);
			if(NULL != engine)
			{
				IFloopySoundFilter *reverse = (IFloopySoundFilter*)engine->CreateInput("stdlib.reverse");
				if(NULL != reverse)
				{
					reverse->EnableAt(0, true);
					reverse->SetBypass(false);

					reverse->SetSource( src );
					((IFloopySoundFilter*)m_pTrack)->SetSource( reverse );

					Invalidate();
					Refresh();
				}
			}
		}
	}
}

bool CTrack::GetReset()
{
	return m_bReset;

	/*if(m_pTrack->GetType() == TYPE_FLOOPY_SOUND_TRACK)
	{
		float value =0.f;
		if(m_pTrack->GetParamAt(0, 0, &value))
			return value != 0.f;
	}
	return false;*/
}

void CTrack::SetReset(bool bReset)
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

IFloopySoundInput *CTrack::FindComponentByName(char *name)
{
	return CTracks::FindComponentByName(m_pTrack, name);
}

void CTrack::InvalidateRegions(CRegion *start)
{
	int startOffset = start->GetStartPos();
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		if(region && (region->GetStartPos()>startOffset) && !region->HasStartOffset() )
		{
			region->Invalidate();
			region->Refresh();
		}
		node = node->GetNext();
	}
}

wxColor CTrack::GetColor()
{
	/*UINT r, g, b;
	if( m_pTrack->GetColor(&r, &g, &b) )
		return wxColor(r, g, b);
	else*/
		return m_color;
}

void CTrack::SetColor(wxColor color)
{
	m_color = color;
	if(m_pTrack)
		m_pTrack->SetColor(color.Red(), color.Green(), color.Blue());
}

wxColor CTrack::GetBGColor()
{
	wxColor color = GetColor();
	if( IsSelected() )
		color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());
	return color;
}

wxColor CTrack::GetForeColor()
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

	bool bPlaying = false;

	if(pTracks->IsPlaying())
	{
		bPlaying = !pTracks->IsPaused();
		pTracks->Stop();
	}

	if(selected)
		pTracks->SetCaretPos( caretpos );

	if(bPlaying)
		pTracks->Play();
}

int CTrack::GetCaretPos()
{
	int pos = GetTracks()->GetCaretPos();

	float value = 0;
	int offset = 0;
	if(m_pTrack->GetParamAt(pos, TIMELINE_PARAM_MOVETO, &value))
		pos -= (int)value;
	else
	{
		int prev = m_pTrack->GetPrevOffset( pos, TIMELINE_PARAM_MOVETO );
		if(m_pTrack->GetParamAt(prev, TIMELINE_PARAM_MOVETO, &value))
		{
			pos -= prev + (int)value;
		}
	}
		
	return pos;
}

bool CTrack::HitTest(int y)
{
	return (y>m_top && y<m_top+GetHeight());
}

int CTrack::GetWidth()
{
	return GetTracks()->GetWidth();
}

void CTrack::ShowObjectParameters(IFloopySoundInput *obj, bool show)
{
	UpdateSelectedRegions();
	RegionList::Node *node = m_regions.GetFirst();
	while (node)
	{
		CRegion *region = (CRegion*)node->GetData();
		if(region)
		{
			region->ShowObjectParameters(obj, show);
			region->Refresh();
		}
		node = node->GetNext();
	}
}



int CTrack::GetPropertyCount()
{
	return 1;
}

bool CTrack::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)IsDrawPreviewOn();
		return true;
	}
	return false;
}

void CTrack::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		SetDrawPreview( value != 0.f );
		return;
	}
}

char *CTrack::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "DrawPreview";
	}
	return NULL;
}

char *CTrack::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Draw region previews";
	}
	return NULL;
}

float CTrack::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0: return 0.f;
	}
	return 0.f;
}

float CTrack::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0: return 1.f;
	}
	return 0.f;
}

char *CTrack::GetPropertyUnit(int index)
{
	//switch(index)
	//{
	//case 0: return "Db";
	//}
	return NULL;
}

float CTrack::GetPropertyStep(int index)
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
	dc.DrawRectangle(0, y, rc.GetWidth(), height);

	dc.SetPen( oldpen );
}
