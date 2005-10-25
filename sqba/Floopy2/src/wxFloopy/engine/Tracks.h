#ifndef Tracks_H
#define Tracks_H


#include <strstrea.h>
#include <wx/log.h>
#include <wx/wxprec.h>
#include <wx/list.h>
#include <wx/listimpl.cpp>
#include <wx/docview.h>
#include <wx/dynlib.h>
#include <wx/thread.h>
#include <wx/timer.h>

#include <wx/dcmemory.h>
#include <wx/bitmap.h>

#include "../../../ifloopy.h"
#include "../FloopyControl.h"
#include "floopyobj.h"
#include "../views/rulerview.h"

#include "objtypes.h"


#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#define MIN_DISTANCE	96


class CTrack;
//class CRegion;
//class CRegionBorder;
//class CParameter;
class CPlayThread;
class CRegionDisplay;



WX_DECLARE_LIST(CTrack, TracksList);



/*! \class CTracks
 *  \brief Main class
 *  \author sqba
 *  \version 0.0
 *  \date 2005
 *  \bug ???.
 *  \warning Still requires a lot of work.
 *
 *  Class represents all tracks...
 */
class CTracks : public IFloopyObj
{
//	DECLARE_DYNAMIC_CLASS(CTracks)

	/*! \class CTimer
	 *  \brief timer
	 *  \author sqba
	 *  \version 0.0
	 *  \date 25/05/2005
	 *
	 *  Used for updating caret position.
	 */
	class CTimer : public wxTimer
	{
	public:
		CTimer() : wxTimer() {}

		void SetParent(CTracks *pTracks) { m_pTracks = pTracks; }
		void Start();
		void Notify();

	private:
		CTracks *m_pTracks;
	};

	/*! \class CTracksBorder
	 *  \brief Tracks border
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  Used for changing the project length.
	 */
	class CBorder : public IFloopyObj
	{
	public:
		CBorder(CTracks *tracks) : IFloopyObj(tracks) {}
		virtual ~CBorder() {}

		int GetType()	{ return FLOOPY_TRACKS_BORDER; }

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZEWE); }
		void Move(int dx, int WXUNUSED(dy));
		void OnMouseEvent(wxMouseEvent& event);
		void DrawBG(wxDC&, wxRect&);
		void DrawFore(wxDC&, wxRect&);

	private:
		inline CTracks *getTracks()	{ return (CTracks*)GetParent(); }
	};

	/*! \class CSelectedEvents
	 *  \brief 
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  
	 */
	class CSelectedEvents : public IFloopyObj
	{
	public:
		CSelectedEvents(CTracks *tracks) : IFloopyObj(tracks) {}
		virtual ~CSelectedEvents() {}

		void Move(int dx, int WXUNUSED(dy));

	private:
		inline CTracks *getTracks()	{ return (CTracks*)GetParent(); }
	};

public:
	CTracks();
	virtual ~CTracks();

	////////////////////////////////////////////////////////////////////////////////
	// IFloopyObj interface
	////////////////////////////////////////////////////////////////////////////////
	int GetType()	{ return FLOOPY_TRACKS; }

	//int GetChildCount()							{ return GetTrackCount(); }
	//IFloopyObj *GetChild(int index)				{ return (IFloopyObj*)GetTrack(index); }

	void DrawBG  (wxDC& dc);
	void DrawFore(wxDC& dc);

	int   GetPropertyCount();
	bool  GetPropertyVal(int, float*);
	void  SetPropertyVal(int, float);
	char *GetPropertyName(int);
	char *GetPropertyDesc(int);
	float GetPropertyMax(int);
	float GetPropertyMin(int);
	char *GetPropertyUnit(int);
	float GetPropertyStep(int);

	int  GetWidth();
	void SetWidth(int width);
	int  GetHeight();


	void DeselectAllTracks();
	void DeselectAllRegions();
	bool RemoveSelectedObjects();
	void UpdateSelectedRegions();


	void SetTimelineView(CRulerView *panel);
	void SetLabelsView(CRulerView *panel)		{ m_pLabelsView = panel; }
	
	CRulerView *GetTimelineView()				{ return m_pTimelineView; };
	CRulerView *GetLabelsView()					{ return m_pLabelsView; };

	void SetCaretPos(int samples);
	int  GetCaretPos();

	int  GetSamplesPerPixel();
	void SetSamplesPerPixel(int);

	float GetLength()							{ return m_length; }
	void  SetLength(float len)					{ m_length = len; }

	int GetTrackCount()							{ return m_tracks.GetCount(); }

	void DrawLabels(wxDC& dc, wxSize size);

	bool RemoveTrack(CTrack *track);

	void MoveSelectedRegions(int dx);

	CTrack *GetTrackAt(int y);
	CTrack *GetTrack(int index);
	IFloopyObj *GetChildAt(int x, int y);
	int GetTrackIndex(CTrack *track);

	void Refresh();
	void Invalidate();

	void Dump(ostream& stream);

	bool Open(char *filename);
	bool Save(char *filename);
	void Clear();

	IFloopySoundInput *GetInput()				{ return m_pEngine; }

	bool OnKeyDown(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	IFloopyObj *GetSelectedObj();
	CTrack *GetSelectedTrack();

	void RefreshTracks(CTrack *track);

	int GetClosestGridPos(int pos);
	int CalcStep(int mindist);

	void Play();
	void Pause();
	void Stop();
	void OnExitThread();
	bool IsPlaying();
	bool IsPaused();

	int GetCursorPosition();
	void SetCursorPosition(int pos);

	void SetStatusText(int pos);

	wxStatusBar *GetStatusBar()					{ return m_pFrame->GetStatusBar(); }
	void SetFrame(wxFrame *pFrame)				{ m_pFrame = pFrame; }

	char *GetFilename()							{ return m_filename; }

	bool GetViewUpdatedWhilePlaying();
	void SetViewUpdatedWhilePlaying(bool bUpdate);

	void CenterView(int sample);

	int GetPixelsPerSecond() { return m_iPixelsPerSecond; }

	static IFloopySoundInput *FindComponentByName(IFloopySoundInput*, char*);
	static char *GetComponentName(IFloopySoundInput*);
	static bool IsFilter(IFloopySoundInput*);

private:
	CTrack *addTrack(IFloopySoundInput*, int);
	bool createEngine(char *plugin);
	void loadTracks(IFloopySoundInput*, int);
	void changeHeight(int dy);
	void init();

private:
	TracksList			m_tracks;
	IFloopySoundEngine	*m_pEngine;
	IFloopySoundMixer	*m_pMaster;
	CRulerView			*m_pTimelineView, *m_pLabelsView;
	CBorder				*m_pBorder;
	wxDynamicLibrary	m_libEngine;
	CPlayThread			*m_pPlayThread;
	int					m_iCursorPosition;
	wxStatusBar			*m_pStatusBar;
	wxFrame				*m_pFrame;
	CTimer				m_Timer;
	int					m_iStartSample;
	char				m_filename[MAX_PATH];
	bool				m_bViewUpdatedWhilePlaying;

	// Properties
	bool	m_bSnapTo;
	int		m_iPixelsPerSecond;
	float	m_length;	// In seconds
};


#endif // #ifndef Tracks_H
