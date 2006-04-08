// Tracks.cpp: implementation of the CTracks class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <wx/wx.h>
#include <wx/caret.h>

#include "tracks.h"
#include "track.h"
#include "label.h"
#include "playthread.h"
#include "../views/timelineview.h"
#include "../../../IFloopy.h"
#include "ActionHistory.h"

typedef IFloopySoundEngine* (*CreateProc)(HMODULE);
#define PROC_NAME	"CreateSoundEngine"
#define PLUG_EXT	".dll"

//IMPLEMENT_DYNAMIC_CLASS(CTracks, IFloopyObj)

WX_DEFINE_LIST(TracksList);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracks::CTracks() : IFloopyObj(NULL)
{
	m_iCursorPosition	= 0;
	m_length			= 120;		// seconds
	m_iPixelsPerSecond	= 32;
	m_pMaster			= NULL;
	m_pEngine			= NULL;
	m_pTimelineView		= NULL;
	m_pLabelsView		= NULL;
	m_pFrame			= NULL;
	m_bSnapTo			= true;
	m_bChanged			= false;
	m_bViewUpdated		= false;
	m_bScrollView		= false;
	m_bDrawPreview		= false;	// Default

	memset(m_filename, 0, sizeof(m_filename));

	m_Timer.SetParent( this );

	m_pBorder			= new CBorder(this);
	m_pPlayThread		= new CPlayThread(this);
	m_pActionHistory	= new CActionHistory();

	createEngine("engine");

	wxLog::AddTraceMask(_T("CTracks"));
}

CTracks::~CTracks()
{
	WX_CLEAR_LIST(TracksList, m_tracks);

	delete m_pBorder;

	if(NULL != m_pPlayThread)
	{
		if(m_pPlayThread->IsPlaying())
			m_pPlayThread->Stop();
		delete m_pPlayThread;
	}

	if(NULL != m_pEngine)
	{
		delete m_pEngine;
		m_libEngine.Unload();
	}

	delete m_pActionHistory;
}

/////////////////////////////////////////////////////////////////////////////
// DrawBG
//! Draws track backgrounds.
//! \param dc [in] reference to the device context
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::DrawBG(wxDC& dc)
{
	wxRect rc(0, 0, GetWidth(), GetHeight());

	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		int height = track->GetHeight();
		rc.SetHeight( height );
		track->DrawBG(dc, rc);
		rc.Offset(0, height);
		node = node->GetNext();
	}
}

/////////////////////////////////////////////////////////////////////////////
// DrawFore
//! Draws track foregrounds.
//! \param dc [in] reference to the device context
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::DrawFore(wxDC& dc)
{
	wxRect rc(0, 0, GetWidth(), GetHeight());

	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		int height = track->GetHeight();
		rc.SetHeight( height );
		track->DrawFore(dc, rc);
		rc.Offset(0, height);
		node = node->GetNext();
	}
}

CTrack *CTracks::addTrack(IFloopySoundInput *input, int level)
{
	wxLogTrace(_T("CTracks"), _T("Adding new track"));

	CTrack *track = new CTrack( this, input, level );
	UINT r=255, g=255, b=255;
	input = track->GetInput();
	if( !input->GetColor(&r, &g, &b) )
	{
		int n = m_tracks.GetCount();
		if(n > 0)
		{
			CTrack *pTrack = GetTrack(n-1);
			IFloopySoundInput *pInput = pTrack->GetInput();
			if( pInput && pTrack->GetInput()->GetColor(&r, &g, &b) )
			{
				r -= 5;
				g -= 5;
				b -= 5;
			}
		}
	}
	track->SetColor(wxColor(r, g, b));
	m_tracks.Append( track );
	Refresh();
	return track;
}

bool CTracks::RemoveTrack(CTrack *track)
{
	wxLogTrace(_T("CTracks"), _T("Removing track"));

	try
	{
		if( m_pMaster->RemoveSource(track->GetInput()) )
		{
			if(m_tracks.DeleteObject( track ))
			{
				Refresh();
				delete track;
				return true;
			}
		}
		else
			wxLogTrace(_T("CTracks"), _T("RemoveTrack error: track input not found!"));
	}
	catch(...)
	{
		wxLogTrace(_T("CTracks"), _T("RemoveTrack exception"));
	}
	return false;
}

void CTracks::init()
{
	wxLogTrace(_T("CTracks"), _T("Initialization"));
}

/////////////////////////////////////////////////////////////////////////////
// GetHeight
//! Returns total track height.
//! \return integer, total track height
/////////////////////////////////////////////////////////////////////////////
int CTracks::GetHeight()
{
	int height = 0;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		height += track->GetHeight();
		node = node->GetNext();
	}
	return height;
}

CTrack *CTracks::GetTrackAt(int y)
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if(track->HitTest(y))
			return track;
		node = node->GetNext();
	}
	return NULL;
}

int CTracks::GetTrackIndex(CTrack *track)
{
	int index = 0;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *tmp = (CTrack*)node->GetData();
		if(tmp == track)
			return index;
		node = node->GetNext();
		index++;
	}
	return -1;
}

CTrack *CTracks::GetTrack(int index)
{
	int count = 0;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if(count == index)
			return track;
		node = node->GetNext();
		count++;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// DeselectAllTracks
//! Deselects all tracks.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::DeselectAllTracks()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		track->Select(false);
		node = node->GetNext();
	}
}

/////////////////////////////////////////////////////////////////////////////
// DeselectAllRegions
//! Deselects all events in all tracks.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::DeselectAllRegions()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		track->DeselectAllRegions();
		node = node->GetNext();
	}
}

IFloopyObj *CTracks::GetChildAt(int x, int y)
{
	if(x > GetWidth())
		return NULL;

	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		IFloopyObj *obj = track->GetChildAt(x, y);
		if(NULL != obj)
			return obj;
		else if(x == GetWidth())
			return m_pBorder;
		//else if(track->HitTest(y) && (x < GetWidth()))
		else if(track->HitTest(y))
			return track;
		node = node->GetNext();
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// MoveSelectedRegions
//! Horizontally moves all selected events in all tracks for the dx offset.
//! \param dx [in] offset in pixels
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::MoveSelectedRegions(int dx)
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		track->MoveSelectedRegions(dx);
		node = node->GetNext();
	}
}

void CTracks::UpdateSelectedRegions()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		track->UpdateSelectedRegions();
		node = node->GetNext();
	}
}

bool CTracks::RemoveSelectedObjects()
{
	bool bResult = false;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		node = node->GetNext();
		if( !track->RemoveSelectedObjects() && track->IsSelected() )
		{
			if( RemoveTrack(track) )
			{
				GetStatusBar()->SetStatusText("Track removed", 0);
				bResult = true;
			}
			else
				GetStatusBar()->SetStatusText("Failed to remove track", 0);
		}
	}
	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// Refresh
//! Redraws all tracks and their labels.
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::Refresh()
{
	int ppu = 10; // ??
	int noUnitsX = GetWidth()  / ppu;
	int noUnitsY = GetHeight() / ppu;

	if( m_pTimelineView )
	{
		m_pTimelineView->SetScrollbars(ppu, ppu, noUnitsX+1, noUnitsY+1);
		m_pTimelineView->Refresh();
	}

	if( m_pLabelsView )
		m_pLabelsView->Refresh();

	SetCaretPos( GetCaretPos() );
}

void CTracks::Invalidate()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		track->Invalidate();
		node = node->GetNext();
	}
//	SetCaretPos( GetCaretPos() );
}

int CTracks::GetSamplesPerPixel()
{
	if(m_pEngine)
	{
		SOUNDFORMAT *fmt = m_pEngine->GetFormat();
		int freq = fmt->frequency;
		return freq / m_iPixelsPerSecond;
	}
	return 0;
}

void CTracks::SetSamplesPerPixel(int spp)
{
	if( spp < 1 )
		return;

	int pos = GetCaretPos();

	SOUNDFORMAT *fmt = m_pEngine->GetFormat();
	int freq = fmt->frequency;
	m_iPixelsPerSecond = freq / spp;

	Invalidate();
	Refresh();

	SetCaretPos( pos );

	CenterView( pos );
}

/////////////////////////////////////////////////////////////////////////////
// SetTimelineView
//! Sets the tracks display panel.
//! \param panel [in] Pointer to the tracks display panel (wxScrolledWindow)
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::SetTimelineView(CRulerView *panel)
{
	m_pTimelineView = panel;
	Refresh();
}

int CTracks::GetWidth()
{
	return m_length * (float)m_iPixelsPerSecond;
}

void CTracks::SetWidth(int width)
{
	m_length = width / (float)m_iPixelsPerSecond;

	Refresh();
}

void CTracks::changeHeight(int dy)
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		track->SetHeight(track->GetHeight() + dy);
		node = node->GetNext();
	}
	SetCaretPos( GetCaretPos() );
}

void CTracks::Dump(ostream& stream)
{
	/*stream << _T("CTracks") << '\n';

	if( m_pBorder )
		stream << m_pBorder << '\n';

	stream << '\n';*/
}







bool CTracks::createEngine(char *plugin)
{
	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);

	bool result = false;

	if( m_libEngine.Load(filename) )
	{
		CreateProc func = (CreateProc)m_libEngine.GetSymbol(_T(PROC_NAME));
		if ( !func )
		{
			wxLogTrace(_T("CTrack"), _T("CreateDisplay function not found!"));
		}
		else {
			try
			{
				m_pEngine = func( NULL );
				SetEngine(m_pEngine);
				result = true;
			}
			catch(...)
			{
				wxLogTrace(_T("CEngine"), _T("CreateDisplay failed!"));
				m_libEngine.Unload();
			}
		}
	}

	delete filename;

	return result;
}

bool CTracks::Open(char *filename)
{
	bool result = false;

//	((CTimelineView*)m_pTimelineView)->Init();

	::wxSetCursor( *wxHOURGLASS_CURSOR );

	if(m_pEngine)
	{
		if(m_pEngine->Open(filename))
		{
			m_pMaster = NULL;
			memset(m_filename, 0, sizeof(m_filename));
			strcpy(m_filename, filename);
			Clear();
			loadTracks(m_pEngine, 0);
			SOUNDFORMAT *fmt = m_pEngine->GetFormat();
			float freq = fmt->frequency;
			m_pEngine->Reset();
			m_length = (float)m_pEngine->GetSize() / freq;
			m_length += 2; // seconds
			Refresh();
			result = true;
		}
		else
		{
			IFloopySoundFilter *track = (IFloopySoundFilter*)m_pEngine->CreateTrack(filename);
			if(track)
			{
				if(!m_pMaster)
				{
					m_pMaster = (IFloopySoundMixer*)m_pEngine->CreateInput("stdlib.mixer");
					m_pMaster->Enable(true);
					m_pEngine->SetSource( m_pMaster );
				}
				track->SetDisplayName(filename, strlen(filename));
				if( m_pMaster->AddSource(track) > -1 )
				{
					addTrack(track, 0);
					Refresh();
					result = true;
				}
				else
				{
					wxString err;
					err.Printf("Mixer supports maximum %d tracks!", m_tracks.GetCount());
					(void)wxMessageBox(err, _T("Mixer error"), wxICON_EXCLAMATION);
					result = true;
				}
			}
		}
	}

	::wxSetCursor( *wxSTANDARD_CURSOR );

	SetCaretPos( 0 );

	m_pTimelineView->SetFocus();

	return result;
}

bool CTracks::Save(char *filename)
{
	if(m_pEngine)
		return m_pEngine->Save(filename);
	return false;
}

void CTracks::RefreshTracks(CTrack *startTrack)
{
	bool bStart = false;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if(track == startTrack)
			bStart = true;
		else if(bStart)
			track->Refresh();
		node = node->GetNext();
	}
}

void CTracks::loadTracks(IFloopySoundInput *input, int level)
{
	if(!input)
		return;

	if(input->GetType() == TYPE_FLOOPY_SOUND_MIXER)
	{
		m_pMaster = (IFloopySoundMixer*)input;

		int count = m_pMaster->GetInputCount();
		
		for(int i=0; i<count; i++)
		{
			IFloopySoundInput *track = m_pMaster->GetSource(i);
			addTrack(track, 0);
		}

		return;
	}
	else if( IsFilter(input) )
	{
		loadTracks(((IFloopySoundFilter*)input)->GetSource(), ++level);
	}
}

void CTracks::Clear()
{
	if(m_pMaster)
		m_pMaster->Close();
	WX_CLEAR_LIST(TracksList, m_tracks);
	Refresh();
}

bool CTracks::OnKeyDown(wxKeyEvent& event)
{
	switch( event.GetKeyCode() )
	{
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
	case '<':
	case ',':
		SetCaretPos( GetCaretPos() - GetSamplesPerPixel() );
		return true;
	case '-':
		SetSamplesPerPixel( GetSamplesPerPixel()*2 );
		return true;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
	case '>':
	case '.':
		SetCaretPos( GetCaretPos() + GetSamplesPerPixel() );
		return true;
	case '+':
		SetSamplesPerPixel( GetSamplesPerPixel()/2 );
		return true;
	case WXK_UP:
	case WXK_NUMPAD_UP:
		changeHeight( +1 );
		return true;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		changeHeight( -1 );
		return true;
	case WXK_DELETE:
		RemoveSelectedObjects();
		//RefreshRulers();
		return true;
	case WXK_F5:
	case WXK_SPACE:
		Play();
		return true;
	case WXK_PRIOR:
	case WXK_PAGEUP:
	case WXK_NUMPAD_PAGEUP:
		{
			CTrack *track = GetSelectedTrack();
			if(track && track->IsSelected())
			{
				int index = GetTrackIndex(track);
				if(index > 0)
				{
					track = GetTrack(index-1);
					if(track)
					{
						DeselectAllTracks();
						track->Select();
						//track->Refresh();
					}
				}
			}
		}
		return true;
	case WXK_NEXT:
	case WXK_PAGEDOWN:
	case WXK_NUMPAD_PAGEDOWN:
		{
			CTrack *track = GetSelectedTrack();
			if(track && track->IsSelected())
			{
				int index = GetTrackIndex(track);
				if(index < (int)m_tracks.GetCount())
				{
					track = GetTrack(index+1);
					if(track)
					{
						DeselectAllTracks();
						track->Select();
						//track->Refresh();
					}
				}
			}
		}
		return true;
	case WXK_HOME:
		SetCaretPos( 0 );
		CenterView( 0 );
		return true;
	case WXK_END:
		return true;
	case 'p':
	case 'P':
		SetDrawPreview( !IsDrawPreviewOn() );
		//Invalidate();
		//Refresh();
		return true;
	case 'z':
	case 'Z':
		if( event.ControlDown() )
		{
			if( event.ShiftDown() )
				m_pActionHistory->RedoLastAction();
			else 
				m_pActionHistory->UndoLastAction();
		}
		return true;
	default:
		/*{
			IFloopyObj *obj = GetSelectedObj();
			if(obj && obj->OnKeyDown(event))
				return true;
		}*/
		/*if(event.GetKeyCode() < m_tracks.GetCount())
		{
			DeselectAllTracks();
			CTrack *track = GetTrack( event.GetKeyCode() );
			if(track)
			{
				track->Select();
				return true;
			}
		}*/
		return false;
	}
}

bool CTracks::OnMouseEvent(wxMouseEvent& event)
{
	return IFloopyObj::OnMouseEvent(event);
}

IFloopyObj *CTracks::GetSelectedObj()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		IFloopyObj *tmp = track->GetSelectedObj();
		if(tmp)
			return tmp;
		node = node->GetNext();
	}
	return NULL;
}

CTrack *CTracks::GetSelectedTrack()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if(track && track->IsSelected())
			return track;
		node = node->GetNext();
	}
	return NULL;
}

int CTracks::GetClosestGridPos(int pos)
{
	if(m_bSnapTo)
	{
		int nStep	= CalcStep(MIN_DISTANCE);
		int nLine	= (int)ceil( pos / nStep );
		int result	= nLine * nStep;

		return result;
	}
	else
		return pos;
}

int CTracks::CalcStep(int mindist)
{
	SOUNDFORMAT *fmt = m_pEngine->GetFormat();
	int freq	= fmt->frequency;
	int pps		= m_iPixelsPerSecond;
	int res		= (pps / mindist);

	if((res % 2 == 0) || (res % 4 == 0))
		res = res;
	else
		res = res - 1;
	res = (res <= 0 ? 1 : res);

	int iStep = pps / res;

	if(iStep == 0)
		iStep = 1;

	while(iStep < mindist)
		iStep *= 2;

	iStep /= 4;

	return iStep;
}

void CTracks::OnExitThread()
{
	m_Timer.Stop();
	// Postaviti nekako kursor na prvobitnu poziciju.
	// A to ne moze iz drugog thread-a!

	GetStatusBar()->SetStatusText("Finished playing", 0);
	m_pPlayThread = new CPlayThread(this);
}

void CTracks::Play()
{
	if(NULL != m_pPlayThread)
	{
		if( m_pPlayThread->IsPaused() )
			m_pPlayThread->Pause();
		else if( m_pPlayThread->IsPlaying() )
		{
			Stop();
			return;
		}
		else if(wxTHREAD_NO_ERROR == m_pPlayThread->Create())
		{
			m_iStartSample = GetCaretPos();
			m_pPlayThread->Play( m_iStartSample );
			GetStatusBar()->SetStatusText("Playing", 0);
		}

		m_Timer.Start();
	}
}

void CTracks::Pause()
{
	if(NULL != m_pPlayThread)
	{
		if(m_pPlayThread->IsPaused())
			GetStatusBar()->SetStatusText("Playing", 0);
		else
			GetStatusBar()->SetStatusText("Paused", 1);
		m_pPlayThread->Pause();
	}
}

void CTracks::Stop()
{
	if(NULL != m_pPlayThread)
	{
		m_pPlayThread->Stop();

		m_Timer.Stop();

		SetCaretPos( m_iStartSample );
		SetCursorPosition( m_iStartSample );

		GetStatusBar()->SetStatusText("Idle", 1);
	}
}

void CTracks::SetCaretPos(int samples)
{
	wxCaret *caret = m_pTimelineView->GetCaret();
	if(NULL == caret)
		return;

	if(samples < 0)
		samples = 0;

	int x = samples / GetSamplesPerPixel();
	int y = 0;

	int xc1, yc1;

	int height = this->GetHeight();

	///////////////////////////////////////////////
	// Mora da se popravi vertikalno skrolovanje
	//m_pTimelineView->GetClientSize(NULL, &height);
	///////////////////////////////////////////////

	IFloopyObj *obj = GetSelectedTrack();
	if( obj )
	{
		CTrack *pTrack = (CTrack*)obj;
		height = pTrack->GetHeight();
		y = pTrack->GetTop();
	}

	m_pTimelineView->CalcScrolledPosition(x, y, &xc1, &yc1);

	if(height > 0)
	{
		caret->SetSize(1, height);
		caret->Move( xc1, yc1 );
	}

	caret->Show(true);
}

int CTracks::GetCaretPos()
{
	wxCaret *caret = m_pTimelineView->GetCaret();
	if(NULL == caret)
		return 0;

	int x=0, y=0, xc1=0, yc1=0;
	caret->GetPosition(&x, &y);
	m_pTimelineView->CalcUnscrolledPosition(x, y, &xc1, &yc1);
	return xc1 * GetSamplesPerPixel();
}

// Positions the view so that the sample is in the middle (if possible).
void CTracks::CenterView(int sample)
{
	int xScrollUnits=0, yScrollUnits=0;
	m_pTimelineView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );

	int width=0, height=0;
	m_pTimelineView->GetClientSize(&width, &height);

	int x = sample/GetSamplesPerPixel() - width/2;

	m_pTimelineView->Scroll(x/xScrollUnits, 0);
}

int CTracks::GetCursorPosition()
{
	return m_pPlayThread->GetPosition();
}

void CTracks::SetStatusText(int samples)
{
	float seconds = 0.f;
	SOUNDFORMAT *fmt = m_pEngine->GetFormat();
	if(fmt && fmt->frequency>0)
		seconds = (float)samples / (float)fmt->frequency;

	wxString csTime;
	float min = seconds>60.f ? seconds/60.f : 0.f;
	float sec = min>1.f ? seconds-min*60.f : seconds;
	float ms  = sec*1000.f - (int)sec*1000;
	csTime.Printf("%2.2d:%2.2d:%3.3d", (int)min, (int)sec, (int)ms);

	wxString str;
	str.Printf("%d samples / %s", samples, csTime);

	GetStatusBar()->SetStatusText(str, 1);
}

void CTracks::SetCursorPosition(int samples)
{
	m_iCursorPosition = samples;

	SetStatusText( samples );

	if(IsPlaying())
	{
		m_pPlayThread->SetStartPos(samples);
		SetViewUpdatedWhilePlaying(true);
	}
}

bool CTracks::IsPlaying()
{
	return m_pPlayThread ? m_pPlayThread->IsPlaying() : false;
}

bool CTracks::IsPaused()
{
	return m_pPlayThread ? m_pPlayThread->IsPaused() : false;
}

bool CTracks::GetViewUpdatedWhilePlaying()
{
	return m_bViewUpdated;
}

void CTracks::SetViewUpdatedWhilePlaying(bool bUpdate)
{
	if( IsPlaying() )
		m_bViewUpdated = bUpdate;
}


char *CTracks::GetComponentName(IFloopySoundInput *src)
{
	if(NULL == src)
		return NULL;
	char *tmp = src->GetName();
	char *name = strrchr(tmp, '.');
	if(NULL == name)
		name = tmp;
	else
		name++;
	return name;
}

IFloopySoundInput *CTracks::FindComponentByName(IFloopySoundInput *src, char *name)
{
	while(src)
	{
		char *tmp = GetComponentName(src);
		if(0==strcmpi(tmp, name))
			return src;

		if( IsFilter(src) )
			src = ((IFloopySoundFilter*)src)->GetSource();
		else
			src = NULL;
	}
	return NULL;
}

bool CTracks::IsFilter(IFloopySoundInput *src)
{
	if(NULL == src)
		return false;
	int type = src->GetType();
	return(type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

void CTracks::SetDrawPreview(bool bDrawPreview)
{
//	IFloopyObj *obj = GetSelectedObj();
//	if(obj)
//		obj->SetDrawPreview(bDrawPreview);
//	else
		IFloopyObj::SetDrawPreview(bDrawPreview);
}


bool CTracks::IsChanged()
{
	return m_pActionHistory->FileChanged();
	//return IFloopyObj::IsChanged();
}




int CTracks::GetPropertyCount()
{
	return 6;
}

bool CTracks::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)IsDrawPreviewOn();
		return true;
	case 1:
		*value = (float)GetSamplesPerPixel();
		return true;
	case 2:
		*value = (float)GetLength();
		return true;
	case 3:
		*value = (float)m_bSnapTo;
		return true;
	case 4:
		*value = (float)GetCaretPos();
		return true;
	case 5:
		*value = (float)m_bScrollView;
		return true;
	}
	return false;
}

void CTracks::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		SetDrawPreview( value != 0.f );
		return;
	case 1:
		SetSamplesPerPixel((int)value);
		return;
	case 2:
		SetLength((int)value);
		return;
	case 3:
		m_bSnapTo = (value==0.f ? false : true);
		return;
	case 4:
		SetCaretPos((int)value);
		return;
	case 5:
		m_bScrollView = (value != 0.f);
		return;
	}
}

char *CTracks::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "DrawPreview";
	case 1: return "SamplesPerPixel";
	case 2: return "Length";
	case 3: return "SnapToGrid";
	case 4: return "CaretPos";
	case 5: return "ScrollWhilePlaying";
	}
	return NULL;
}

char *CTracks::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Draw region previews";
	case 1: return "Samples per pixel";
	case 2: return "Project length";
	case 3: return "Snap to grid";
	case 4: return "Caret position";
	case 5: return "Scroll view while playing";
	}
	return NULL;
}

float CTracks::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0: return 0.f;
	case 1: return 1.f;
	case 2: return 1.f;
	case 3: return 0.f;
	case 4: return 0.f;
	case 5: return 0.f;
	}
	return 0.f;
}

float CTracks::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0: return 1.f;
	case 1: return 65535.f; //?
	case 2: return 3600.f;
	case 3: return 1.f;
	case 4: return 65535.f; //?
	case 5: return 1.f;
	}
	return 0.f;
}

char *CTracks::GetPropertyUnit(int index)
{
	switch(index)
	{
	//case 0: return "Db";
	case 1: return "sec";
	case 2: return "On/Off";
	case 5: return "On/Off";
	}
	return NULL;
}

float CTracks::GetPropertyStep(int index)
{
	switch(index)
	{
	case 0: return 1.f;
	case 1: return 1.f;
	case 2: return 1.f;
	case 3: return 1.f;
	case 4: return 1.f;
	case 5: return 1.f;
	}
	return 0.f;
}




/////////////////////////////////////////////////////////////////////
// CTimer functions
/////////////////////////////////////////////////////////////////////
void CTracks::CTimer::Start()
{
	wxTimer::Start(1, wxTIMER_CONTINUOUS);
}

void CTracks::CTimer::Notify()
{
	int pos = m_pTracks->GetCursorPosition();
	m_pTracks->SetCaretPos( pos );
	m_pTracks->SetStatusText( pos );
	
	// Mozda je bolje skrolovati konstantnom brzinom
	// i povremeno vrsiti korekcije
	if( m_pTracks->ScrollWhilePlaying() )
		m_pTracks->CenterView( pos );
}





/////////////////////////////////////////////////////////////////////
// CBorder functions
/////////////////////////////////////////////////////////////////////
void CTracks::CBorder::Move(int dx, int WXUNUSED(dy))
{
	getTracks()->SetWidth(getTracks()->GetWidth() + dx);
}

bool CTracks::CBorder::OnMouseEvent(wxMouseEvent& event)
{
	bool bResult = false;
	if(event.Dragging() && (0 != m_ptPrev.x))
	{
		int dx = event.GetX() - m_ptPrev.x;
		int dy = event.GetY() - m_ptPrev.y;
		Move(dx, dy);
		bResult = true;
	}
	IFloopyObj::OnMouseEvent(event);
	return bResult;
}

void CTracks::CBorder::DrawBG(wxDC& dc, wxRect& rc)
{

}

void CTracks::CBorder::DrawFore(wxDC& dc, wxRect& rc)
{

}



/////////////////////////////////////////////////////////////////////////////
// DrawLabels
//! Draws track labels.
//! \param dc [in] reference to the device context
//! \param size [in] labels panel dimensions
//! \return void
/////////////////////////////////////////////////////////////////////////////
/*void CTracks::DrawLabels(wxDC& dc, wxSize size)
{
	wxFont oldFont = dc.GetFont();

	wxFont fixedFont(12, wxDEFAULT, wxITALIC, wxBOLD);
	dc.SetFont(fixedFont);

	wxRect rc(0, 0, size.GetWidth(), size.GetHeight());

	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		CLabel *label = track->GetLabel();
		int height = track->GetHeight();
		rc.SetHeight( height );
		label->DrawBG(dc, rc);
		label->DrawFore(dc, rc);
		rc.Offset(0, height);
		node = node->GetNext();
	}

	dc.SetFont(oldFont);
}*/


/*
/////////////////////////////////////////////////////////////////////
// CSelectedEvents functions
/////////////////////////////////////////////////////////////////////
void CTracks::CSelectedEvents::Move(int dx, int WXUNUSED(dy))
{
	getTracks()->MoveSelectedRegions(dx);
}
*/