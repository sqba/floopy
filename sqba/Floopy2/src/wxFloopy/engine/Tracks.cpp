// Tracks.cpp: implementation of the CTracks class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <wx/wx.h>
#include <wx/caret.h>

#include "Tracks.h"
#include "../../../IFloopy.h"

IMPLEMENT_DYNAMIC_CLASS(CTracks, IFloopyObj)

WX_DEFINE_LIST(TracksList);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTracks::CTracks() : IFloopyObj(NULL)
{
	wxLog::AddTraceMask(_T("CTracks"));

	m_pTracksView = NULL;
	m_pLabelsView = NULL;
//	m_hres   = 2756; // samples per pixel
	m_pps    = 2;	// pixels per second
//	m_bpm    = 120;	// beats per minute
	m_length = 60;	// seconds

	m_pBorder = new CBorder(this);

	m_pEngine = NULL;
	m_pMixer = NULL;

	createEngine("engine");

	m_bSnapTo = TRUE;

	m_pPlayThread = NULL;
}

CTracks::~CTracks()
{
	WX_CLEAR_LIST(TracksList, m_tracks);
	delete m_pBorder;
	if(m_pPlayThread)
		delete m_pPlayThread;

	if(NULL != m_pEngine)
	{
		//m_plugin->Close();
		delete m_pEngine;

		m_libEngine.Unload();
	}
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
		if( !track->IsHidden() )
		{
			track->DrawBG(dc, rc);
			rc.Offset(0, track->GetHeight());
		}
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
		if( !track->IsHidden() )
		{
			track->DrawFore(dc, rc);
			rc.Offset(0, track->GetHeight());
		}
		node = node->GetNext();
	}
}

/////////////////////////////////////////////////////////////////////////////
// DrawLabels
//! Draws track labels.
//! \param dc [in] reference to the device context
//! \param size [in] labels panel dimensions
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::DrawLabels(wxDC& dc, wxSize size)
{
	wxFont oldFont = dc.GetFont();

	wxFont fixedFont(12, wxDEFAULT, wxITALIC, wxBOLD);
	dc.SetFont(fixedFont);

	wxRect rc(0, 0, size.GetWidth(), size.GetHeight());

	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if( !track->IsHidden() )
		{
			track->DrawLabel(dc, rc);
			rc.Offset(0, track->GetHeight());
		}
		node = node->GetNext();
	}

	dc.SetFont(oldFont);
}

/////////////////////////////////////////////////////////////////////////////
// DrawPreview
//! Draws tracks preview (in the preview bar).
//! \param dc [in] reference to the device context
//! \param size [in] preview window dimensions
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::DrawPreview(wxDC& dc, wxSize size)
{
/*	//int n = m_tracks.GetCount();

	int n = 0;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if(track->GetRegionCount() > 0 && !track->IsHidden())
			n++;
		node = node->GetNext();
	}

	if(n == 0)
		return;


	int h = size.GetHeight() / n;
	int w = size.GetWidth();
	int t = 0;//(size.GetHeight() - (h * n)) / 2;
	int y = t > 0 ? t : 0;
	int trackwidth = m_pTracksView->GetVirtualSize().GetWidth();

	if(h < 1)
		h = 1;

	node = m_tracks.GetFirst();
	while (node)
	{
		wxRect rc(0, y, w, h);
		CTrack *track = (CTrack*)node->GetData();
		if(track->GetRegionCount() > 0 && !track->IsHidden())
		{
			track->DrawPreview(dc, rc, pps);
			y += h;
		}
		node = node->GetNext();
	}*/
}

CTrack *CTracks::AddTrack(IFloopySoundInput *input, int level)
{
	wxLogTrace(_T("CTracks"), _T("Adding new track"));
	UINT r=0, g=0, b=0;
	input->GetColor(&r, &g, &b);
	CTrack *track = new CTrack( this, input, level, wxColor(r, g, b) );
	try {
		m_tracks.Append( track );
	} catch(...) {
		wxLogTrace(_T("CTracks"), _T("AddTrack exception"));
		return NULL;
	}
	Refresh();
	return track;
}

bool CTracks::RemoveTrack(CTrack *track)
{
	wxLogTrace(_T("CTracks"), _T("Removing track"));

	try {
		if(m_tracks.DeleteObject( track )) {
			Refresh();
			delete track;
		}
	} catch(...) {
		wxLogTrace(_T("CTracks"), _T("RemoveTrack exception"));
		return FALSE;
	}
	return TRUE;
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
		if( track->IsSelected() )
			track->Refresh();
		track->Select(FALSE);
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

void CTracks::RemoveSelectedObjects()
{
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		node = node->GetNext();
		track->RemoveSelectedObjects();
		if( track->IsSelected() )
			RemoveTrack( track );
	}
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

	if( m_pTracksView )
	{
		m_pTracksView->SetScrollbars(ppu, ppu, noUnitsX+1, noUnitsY+1);
		m_pTracksView->Refresh();
	}

	if( m_pLabelsView )
	{
		/*int width=0, height=0;
		m_pLabelsView->GetSize(&width, &height);
		m_pLabelsView->SetVirtualSize(width, noUnitsY+1);*/

		m_pLabelsView->Refresh();
	}
}

/////////////////////////////////////////////////////////////////////////////
// SetPixelsPerSecond
//! Sets new horizontal resolution (pixels per second).
//! \param pps [in] offset in pixels
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::SetPixelsPerSecond(int pps)
{
	if( 1 <= pps )
	{
		int x=0, y=0;
		
		int width1=0, height1=0;
		m_pTracksView->GetVirtualSize(&width1, &height1);

		// Get previous caret position
		int xc1=0, yc1=0;
		wxCaret *caret = m_pTracksView->GetCaret();
		caret->GetPosition(&x, &y);
		m_pTracksView->CalcUnscrolledPosition(x, y, &xc1, &yc1);

		// Get previous view position
		int xs1 = m_pTracksView->GetScrollPos(wxHORIZONTAL);
		int ys1 = m_pTracksView->GetScrollPos(wxVERTICAL);
		int xScrollUnits, yScrollUnits;
		m_pTracksView->GetScrollPixelsPerUnit( &xScrollUnits, &yScrollUnits );


		m_pps = pps;

		Refresh();


		int width2=0, height2=0;
		m_pTracksView->GetVirtualSize(&width2, &height2);

		// Move caret
		x = (int)(((float)width2  / (float)width1)  * (float)xc1);
		y = (int)(((float)height2 / (float)height1) * (float)yc1);
		int xxc2=0, yyc2=0;
		m_pTracksView->CalcScrolledPosition(x, y, &xxc2, &yyc2);
		caret->Move(xxc2, yyc2);

		// Move view
		x = (int)(((float)width2  / (float)width1)  * (float)xs1);
		y = (int)(((float)height2 / (float)height1) * (float)ys1);
		m_pTracksView->Scroll(x, y);
	}
}

int CTracks::GetPixelsPerBeat()
{
	return 0;//(int)(((float)m_bpm / 60.f) * (float)GetPixelsPerSecond());
}

int CTracks::GetPixelsPerSecond()
{
	return m_pps;
	/*if(m_pEngine)
	{
		SOUNDFORMAT *fmt = m_pEngine->GetFormat();
		int freq = fmt->frequency;
		return freq / m_hres; // m_hres = samples per pixel
	}
	return 0;*/
}
/*
void CTracks::SetPixelsPerSample(int pps)
{
	if(m_pEngine)
	{
		SOUNDFORMAT *fmt = m_pEngine->GetFormat();
		int freq = fmt->frequency;
		m_pps = pps / freq;
	}
}

int CTracks::GetPixelsPerSample()
{
	if(m_pEngine)
	{
		SOUNDFORMAT *fmt = m_pEngine->GetFormat();
		int freq = fmt->frequency;
		return m_pps * freq;
	}
	return 0;
}
*/
int CTracks::GetSamplesPerPixel()
{
//	return m_hres;
	if(m_pEngine)
	{
		SOUNDFORMAT *fmt = m_pEngine->GetFormat();
		int freq = fmt->frequency;
		return freq / m_pps;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// SetTracksView
//! Sets the tracks display panel.
//! \param panel [in] Pointer to the tracks display panel (wxScrolledWindow)
//! \return void
/////////////////////////////////////////////////////////////////////////////
void CTracks::SetTracksView(wxScrolledWindow *panel)
{
	m_pTracksView = panel;
	Refresh();
}

void CTracks::SetWidth(int width)
{
	m_length = width / (float)m_pps;
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
	//Refresh();
}

void CTracks::Dump(ostream& stream)
{
	/*stream << _T("CTracks") << '\n';

	if( m_pBorder )
		stream << m_pBorder << '\n';

	stream << '\n';*/
}







typedef IFloopySoundEngine* (*CreateProc)(HMODULE);
#define PROC_NAME "CreateSoundEngine"
#define PLUG_EXT ".dll"

BOOL CTracks::createEngine(char *plugin)
{
	char *filename = new char[strlen(plugin) + 5];
	strcpy(filename, plugin);
	strcat(filename, PLUG_EXT);

	bool result = FALSE;
	//wxDynamicLibrary dlib;

	if( m_libEngine.Load(filename) ) {
		//typedef void (*funcType)(IFloopyObj**);
		CreateProc func = (CreateProc)m_libEngine.GetSymbol(_T(PROC_NAME));
		if ( !func ) {
			wxLogTrace(_T("CTrack"), _T("CreateDisplay function not found!"));
			//sprintf(m_szLastError, "Error: %s not found in %s.\n", PROC_NAME, filename);
		} else {
			try {
				m_pEngine = func( NULL );
				//func( &m_pDisplay );
				result = TRUE;
			} catch(...) {
				wxLogTrace(_T("CEngine"), _T("CreateDisplay failed!"));
				m_libEngine.Unload();
			}
		}
	}

	delete filename;

	return result;
}

BOOL CTracks::Open(char *filename)
{
	if(m_pEngine)
	{
		if(m_pEngine->Open(filename))
		{
			Clear();
			loadTracks(m_pEngine, 0);
			//loadTracks(m_pEngine->GetSource(), 0);
			SOUNDFORMAT *fmt = m_pEngine->GetFormat();
			float freq = fmt->frequency;
			m_pEngine->Reset();
			m_length = (float)m_pEngine->GetSize() / freq;
			//RefreshRulers();
			m_pPlayThread = new CPlayThread(this);
			Refresh();
			m_pMixer = getMixer();
			return TRUE;
		}
		else
		{
			SOUNDFORMAT *fmt = m_pEngine->GetFormat();
			IFloopySoundInput *input = m_pEngine->CreateInput(filename, *fmt);
			if(input)
			{
				if(!m_pMixer)
				{
					m_pMixer = (IFloopySoundMixer*)m_pEngine->CreateInput("stdlib.mixer");
					m_pMixer->Enable(TRUE);
					m_pEngine->SetSource(m_pMixer);
				}
				IFloopySoundFilter *track = (IFloopySoundFilter*)m_pEngine->CreateInput("stdlib.track");
				track->SetSource(input);
				track->SetDisplayName(filename, strlen(filename));
				m_pMixer->AddSource(track);
				AddTrack(track, 0);
				Refresh();
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CTracks::Save(char *filename)
{
	if(m_pEngine)
		return m_pEngine->Save(filename);
	return FALSE;
}

void CTracks::RefreshTracks(CTrack *startTrack)
{
	BOOL bStart = FALSE;
	TracksList::Node *node = m_tracks.GetFirst();
	while (node)
	{
		CTrack *track = (CTrack*)node->GetData();
		if(track == startTrack)
			bStart = TRUE;
		else if(bStart)
			track->Refresh();
		node = node->GetNext();
	}
}

void CTracks::loadTracks(IFloopySoundInput *input, int level)
{
	if(!input)
		return;


	if(input->GetType() == TYPE_FLOOPY_SOUND_TRACK)
		AddTrack(input, 0);


	if(input->GetType() == TYPE_FLOOPY_SOUND_MIXER)
	{
		IFloopySoundMixer *mixer = (IFloopySoundMixer*)input;

		int count = mixer->GetInputCount();
		
		for(int i=0; i<count; i++)
		{
			loadTracks(mixer->GetSource(i), ++level);
		}
	}
	else if( input->IsFilter() )
	{
		loadTracks(((IFloopySoundFilter*)input)->GetSource(), ++level);
	}
}

void CTracks::Clear()
{
	if(m_pPlayThread)
	{
		delete m_pPlayThread;
		m_pPlayThread = NULL;
	}
	WX_CLEAR_LIST(TracksList, m_tracks);
	Refresh();
}

void CTracks::OnKeyDown(wxKeyEvent& event)
{
	int pps = GetPixelsPerSecond();

	switch (event.GetKeyCode() )
	{
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
	case '-':
		pps /= 2;
		break;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
	case '+':
		pps *= 2;
		break;
	case WXK_UP:
	case WXK_NUMPAD_UP:
		changeHeight( +1 );
		break;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		changeHeight( -1 );
		break;
	case WXK_DELETE:
		RemoveSelectedObjects();
		//RefreshRulers();
		return;
	default:
		return;
	}

	SetPixelsPerSecond( pps );
	//RefreshRulers();
}

void CTracks::OnMouseEvent(wxMouseEvent& event)
{
	IFloopyObj::OnMouseEvent(event);
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
	return this;
}

int CTracks::GetClosestGridPos(int pos)
{
	if(m_bSnapTo)
	{
		int ppb = GetPixelsPerBeat();
		
		int nStep    = CalcStep(MIN_DISTANCE);
		int nLine = (int)ceil( pos / nStep );
		int result = nLine * nStep;

		return result;
	}
	else
		return pos;
}

int CTracks::CalcStep(int mindist)
{
	int res = (m_pps / mindist);
	if((res % 2 == 0) || (res % 4 == 0))
		res = res;
	else
		res = res - 1;
	res = (res <= 0 ? 1 : res);

	int iStep = m_pps / res;

	if(iStep == 0) // Ovde nastaje interesantan efekat: Kada je iStep = 0 i debager
		iStep = 1; // se zaustavi, moze se debagovati samo masinski kod!

	while(iStep < mindist)
		iStep *= 2;

	iStep /= 4;

	return iStep;
}

void CTracks::Play()
{
	if(NULL == m_pPlayThread)
		return;

	m_pPlayThread->Play( GetCaretPos() );
}

void CTracks::Pause()
{
	if(NULL == m_pPlayThread)
		return;

	m_pPlayThread->Pause();
}

void CTracks::Stop()
{
	if(NULL == m_pPlayThread)
		return;

	m_pPlayThread->Stop();
}

void CTracks::SetCaretPos(int samples)
{
	int x=0, y=0;
	int xc1=0, yc1=0;
	wxCaret *caret = m_pTracksView->GetCaret();
	caret->GetPosition(&x, &y);
	m_pTracksView->CalcUnscrolledPosition(x, y, &xc1, &yc1);

	x += samples/GetSamplesPerPixel();
	m_pTracksView->CalcScrolledPosition(x, y, &xc1, &yc1);
	caret->Move(xc1, yc1);
}

int CTracks::GetCaretPos()
{
	int x=0, y=0;
	int xc1=0, yc1=0;
	wxCaret *caret = m_pTracksView->GetCaret();
	caret->GetPosition(&x, &y);
	m_pTracksView->CalcUnscrolledPosition(x, y, &xc1, &yc1);
	return xc1 * GetSamplesPerPixel();
}

IFloopySoundMixer *CTracks::getMixer()
{
	IFloopySoundInput *tmp = m_pEngine->GetSource();
	while(tmp)
	{
		switch(tmp->GetType())
		{
		case TYPE_FLOOPY_SOUND_MIXER:
			return (IFloopySoundMixer*)tmp;
		case TYPE_FLOOPY_SOUND_INPUT:
			return NULL;
		default:
			tmp = (IFloopySoundFilter*)((IFloopySoundFilter*)tmp)->GetSource();
		}
		/*if(tmp->GetType() == TYPE_FLOOPY_SOUND_MIXER)
			return (IFloopySoundMixer*)tmp;
		else if(tmp->GetType() != TYPE_FLOOPY_SOUND_INPUT)
			tmp = (IFloopySoundFilter*)((IFloopySoundFilter*)tmp)->GetSource();
		else
			tmp = NULL;*/
	}
	return NULL;
}
