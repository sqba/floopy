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


#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#define MIN_DISTANCE	96


class CTrack;
class CRegion;
class CRegionBorder;
class CParameter;
class CPlayThread;
class CRegionDisplay;


struct Peak
{
	short int prev;
	short int value;
	int pos;			// Sample offset
};


WX_DECLARE_LIST(CTrack, TracksList);
WX_DECLARE_LIST(CRegion, RegionList);
WX_DECLARE_LIST(CParameter, ParameterList);
WX_DECLARE_OBJARRAY(Peak, PeaksArray);




// Utility function
void DrawAquaRect(wxDC& dc, wxRect& rc, int radius);




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
    DECLARE_DYNAMIC_CLASS(CTracks)

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

	void SetTracksView(wxScrolledWindow *panel);
	void SetLabelsView(wxScrolledWindow *panel) { m_pLabelsView = panel; }
	
	wxScrolledWindow *GetTracksView()			{ return m_pTracksView; };
	wxScrolledWindow *GetLabelsView()			{ return m_pLabelsView; };

	void SetCaretPos(int samples);
	int  GetCaretPos();

	int GetSamplesPerPixel();
	void SetSamplesPerPixel(int);

	float GetLength()							{ return m_length; }
	void SetLength(float len)					{ m_length = len; }

	int GetWidth();
	void SetWidth(int width);
	int GetHeight();

	void DrawBG  (wxDC& dc);
	void DrawFore(wxDC& dc);
	void DrawLabels(wxDC& dc, wxSize size);
	void DrawPreview(wxDC& dc, wxSize size);

	CTrack *AddTrack(IFloopySoundInput *input, IFloopySoundInput *parent, int level);
	bool RemoveTrack(CTrack *track);

	void DeselectAllTracks();
	void DeselectAllRegions();

	void MoveSelectedRegions(int dx);
	void RemoveSelectedObjects();
	void UpdateSelectedRegions();

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

	IFloopySoundEngine *GetEngine()				{ return m_pEngine; }

	int GetClosestGridPos(int pos);
	int CalcStep(int mindist);

	void Play();
	void Pause();
	void Stop();
	void OnExitThread();
	bool IsPlaying();

	int GetCursorPosition();
	void SetCursorPosition(int pos);
	void SetStatusText(int pos);

	wxStatusBar *GetStatusBar()					{ return m_pFrame->GetStatusBar(); }
	void SetFrame(wxFrame *pFrame)				{ m_pFrame = pFrame; }

	void SetChanged(bool bChanged)				{ m_bChanged = bChanged; }
	bool IsChanged()							{ return m_bChanged; }

	char *GetFilename()							{ return m_filename; }

	bool GetViewUpdatedWhilePlaying();
	void SetViewUpdatedWhilePlaying(bool bUpdate);

	void CenterView(int sample);

	static IFloopySoundInput *GetComponent(IFloopySoundInput *src, char *name);

private:
	IFloopySoundMixer *getMixer();
	bool createEngine(char *plugin);
	void loadTracks(IFloopySoundInput *input, IFloopySoundInput *parent, int level);
	void changeHeight(int dy);
	void init();

private:
	int					m_iPixelsPerSecond;
	bool				m_bInit;
	float				m_length;	// In seconds
	TracksList			m_tracks;
	wxScrolledWindow	*m_pTracksView, *m_pLabelsView;
	CBorder				*m_pBorder;

	IFloopySoundEngine	*m_pEngine;
	IFloopySoundMixer	*m_pMixer;
	wxDynamicLibrary	m_libEngine;

	bool m_bSnapTo;

	CPlayThread			*m_pPlayThread;

	int					m_iCursorPosition;
	wxStatusBar			*m_pStatusBar;
	wxFrame				*m_pFrame;
	bool				m_bChanged;

	CTimer				m_Timer;
	int					m_iStartSample;

	char				m_filename[MAX_PATH];

	bool				m_bViewUpdatedWhilePlaying;
};

class CTrack : public IFloopyObj  
{
    DECLARE_DYNAMIC_CLASS(CTrack)

	/*! \class CBorder
	 *  \brief 
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  
	 */
	class CBorder : public IFloopyObj
	{
	public:
		CBorder(CTrack *track) : IFloopyObj(track) {}
		virtual ~CBorder() {}

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZENS); }
		void Move(int WXUNUSED(dx), int dy);
		void DrawBG(wxDC&, wxRect&);
		void DrawFore(wxDC&, wxRect&);
		int GetHeight();

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

	/*! \class CLoopButton
	 *  \brief 
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  
	 */
	class CLoopButton : public CFloopyControl
	{
	public:
		CLoopButton(CTrack *track) : CFloopyControl(track) {}
		virtual ~CLoopButton() {}

		void DrawFore(wxDC& dc, wxRect& rc);

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

	/*! \class CCacheButton
	 *  \brief 
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  
	 */
	class CCacheButton : public CFloopyControl
	{
	public:
		CCacheButton(CTrack *track) : CFloopyControl(track) {}
		virtual ~CCacheButton() {}

		void DrawFore(wxDC& dc, wxRect& rc);

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

public:
	CTrack() {}
	CTrack(CTracks *tracks, IFloopySoundInput *input, IFloopySoundInput *parent, int level);
	virtual ~CTrack();

	void DrawLabel(wxDC& dc, wxRect& rc);
	void DrawBG   (wxDC& dc, wxRect& rc);
	void DrawFore (wxDC& dc, wxRect& rc);
	void DrawPreview(wxDC& dc, wxRect& rc);

	bool GetName(wxString& name)	{ name = _T("Track"); return true; }
	int GetWidth()					{ return GetTracks()->GetWidth(); }
	int GetHeight();//					{ return m_height + m_pBorder->GetHeight(); }
	int GetTop()					{ return m_top; }
	void SetHeight(int height);
	wxString GetName()				{ return m_name; }
	wxBitmap *GetBitmap()			{ return m_pBitmap; }
	wxCursor GetCursor()			{ return wxCursor(wxCURSOR_PENCIL); }

	CRegion *AddRegion(int start, int end);
	CRegion *AddNewRegionAt(int left);
	bool RemoveRegion(CRegion *event);

	CTracks *GetTracks()			{ return (CTracks*)GetParent(); }

	bool HitTest(int y);

	IFloopyObj *GetChildAt(int x, int y);
	IFloopyObj *GetSelectedObj();

	void DeselectAllRegions();

	void Refresh();
	void Invalidate();

	void Move(int dx, int dy);

	void MoveSelectedRegions(int dx);
	void RemoveSelectedObjects();
	void UpdateSelectedRegions();

	void Dump(ostream& stream);

	IFloopySoundInput *GetInput()	{ return m_pInput; }
	IFloopySoundInput *GetSource()	{ return m_pSource; }

	bool OnKeyDown(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	int GetRegionCount()			{ return m_regions.GetCount(); }

	void CheckIntersections(CRegion *pRegion1, int &left, int &right, bool bResize);

	bool IsHidden()					{ return m_bHide; } // ->Collapse/expand
	void Hide(bool bHide)			{ m_bHide = bHide; }

	bool IsLooped();
	void SetLooped(bool bLooped);

	bool IsReverse();
	void SetReverse(bool bReverse);

	bool GetReset();
	void SetReset(bool bReset);

	void InvalidateRegions(CRegion *start);

	wxColour GetBGColor();
	wxColour GetForeColor();

	IFloopySoundInput *GetComponent(char *name);

	void Select(bool selected=true);

	wxColour GetColor();
	void SetColor(wxColour color);

	int GetCaretPos();//	{ return GetTracks()->GetCaretPos(); }

private:
	bool LoadDisplay(wxString strType);
	void loadRegions();
	void drawLoopSign(wxDC& dc, wxRect& rc);
	void drawCacheSign(wxDC& dc, wxRect& rc);

private:
	int			m_nLevel;
	bool		m_bHide;
	RegionList	m_regions;
	wxRect		m_rcLabel;
	int			m_top;
//	int			m_height;
	wxString	m_name;
	wxBitmap	*m_pBitmap;
	CBorder		*m_pBorder;
	IFloopySoundInput *m_pInput;

	CLoopButton		*m_pButtonLoop;
	CCacheButton	*m_pButtonCache;

	bool		m_bReset;

	IFloopySoundInput *m_pSource;
};

class CRegion : public IFloopyObj
{
    DECLARE_DYNAMIC_CLASS(CRegion)

public:
	/*! \class CBorder
	 *  \brief Region's left or right border
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *  \bug Too many bugs at this point.
	 *  \warning Requires a lot of work.
	 *
	 *  Class represents event's left or right border...
	 */
	class CBorder : public IFloopyObj
	{
	public:
		CBorder(CRegion *event, bool left) : IFloopyObj(event) { m_bLeft = left;}
		virtual ~CBorder() {}

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZEWE); }

		void Move(int dx, int WXUNUSED(dy));
		void DrawBG(wxDC&, wxRect&);
		void DrawFore(wxDC&, wxRect&);

	private:
		inline CRegion  *getRegion()	{ return (CRegion*)GetParent(); }
		bool m_bLeft; // Left or right, start or end
		CRegion *m_pRegion;
	};

public:
	CRegion() {}
	CRegion(CTrack *track, UINT startSample, UINT endSample);
	virtual ~CRegion();

	void DrawBG  (wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);
	void DrawPreview(wxDC& dc, wxRect& rc);

	int GetLeft();
	int GetRight();
	int GetWidth();

	void GetRect(wxRect& rc);

	wxCursor GetCursor() { return wxCursor(wxCURSOR_HAND); }
	wxColour GetColor()	 { return getTrack()->GetColor(); }

	IFloopyObj *GetBorder(bool left) { return(left ? m_pLeftBorder : m_pRightBorder); }
	IFloopyObj *GetSelectedObj();// { return IsSelected() ? this : NULL; }
	IFloopyObj *GetChildAt(int x, int y);

	void Move(int dx, int WXUNUSED(dy));
	bool HitTest(int x, int y);
	void Refresh();
	void Invalidate();
	void Resize(int dl, int dr);
	void Update();
	void CancelUpdate();
	void Remove();

	bool OnKeyDown(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	int GetStartOffset()	{ return m_iStartSample; }
	int GetEndOffset()		{ return m_iEndSample; }

	int GetHeight()	{ return getTrack()->GetHeight() - 2; }
	int GetTop()	{ return getTrack()->GetTop() + 1; }

	bool GetReset();
	void SetReset(bool bReset);

	void SetStartOffset(int sample);

	wxColour GetBGColor();
	wxColour GetForeColor();

	void Select(bool selected=true);

	IFloopySoundInput *GetInput()				{ return getTrack()->GetInput(); }
	int GetCaretPos();

private:
	static void remove(IFloopyObj *event);
	void createMenu();
	void startEdit();
	void calcPos(int *left, int *right);
	inline CTrack  *getTrack()	{ return (CTrack*)GetParent(); }
	inline CTracks *getTracks()	{ return (CTracks*)getTrack()->GetParent(); }
	void loadParameters(IFloopySoundInput *obj);
	void drawParametersFore(wxDC& dc, wxRect& rc);
	bool getReset(int sample);

private:
	bool			m_bEdit;
	int				m_iStartSample, m_iEndSample, m_iPrevStart, m_iPrevEnd;
	wxRect			m_rcRegion;
	CBorder			*m_pLeftBorder, *m_pRightBorder;
	ParameterList	m_Parameters;
	CRegionDisplay	*m_pDisplay;
};


class CParameter : public IFloopyObj
{
    DECLARE_DYNAMIC_CLASS(CParameter)

public:
	class CPoint : public IFloopyObj
	{
	public:
		CPoint(CParameter *parameter) : IFloopyObj(parameter) { m_offset = 0; }
		virtual ~CPoint() {}

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZENS); }

		void Move(int dx, int WXUNUSED(dy));

		int m_offset;
	};

public:
	CParameter() {}
	CParameter(CRegion *region, IFloopySoundInput *obj, int index);
	virtual ~CParameter();

//	IFloopyObj *GetSelectedObj();
	IFloopyObj *GetChildAt(int x, int y);

	float GetValueAt(int x);

	//void DrawBG  (wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);
	//void DrawPreview(wxDC& dc, wxRect& rc);

	bool HitTest(int x, int y);

private:
	inline CRegion	*getRegion()	{ return (CRegion*)GetParent(); }
	inline CTrack	*getTrack()		{ return (CTrack*) getRegion()->GetParent(); }
	inline CTracks	*getTracks()	{ return (CTracks*)getTrack()->GetParent(); }

private:
	int		m_index;
	bool	m_bEdit;
	int		m_iSamplePos, m_iPrevSamplePos;
	CPoint	*m_pPoint;
	IFloopySoundInput *m_pObj;
//	OffsetArrays m_Offsets;
};


class CPlayThread : public wxThread  
{
public:
	CPlayThread(CTracks *pTracks);
	virtual ~CPlayThread();

	void *Entry();

	void Play(int sample);
	void Pause();
	void Stop();

	void OnExit();

	bool IsPlaying()	{ return m_bPlaying; }
	bool IsPaused()		{ return m_bPaused; }

	int GetPosition();

	void SetStartPos(int pos);

private:
	CTracks *m_pTracks;
	int m_iStartPos;
	bool m_bPlaying, m_bPaused;
	IFloopySoundOutput *m_pOutput;
	IFloopySoundInput *m_pInput;
	int m_iBufferLength; // In samples
	int m_iPosition; // In samples
};

/*
class CWaveDisplay : public IFloopyObj  
{
public:
	CWaveDisplay(CTrack *track);
	virtual ~CWaveDisplay();

	void DrawBG(wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);

	void LoadPeaks();

	void DrawRegion(CRegion *region, wxDC& dc, wxRect& rc);

private:
	void drawDBLines(wxDC& dc, wxRect& rc);
	void drawWaveform(wxDC& dc, wxRect& rc, int start);
	IFloopySoundInput *getInput();

private:
	float m_fdB;
//	PointList m_points;
	PeaksArray m_peaks;
	IFloopySoundInput *m_pInput;
};
*/

class CRegionDisplay// : public wxThread  
{
	/*class CLoadThread : public wxThread
	{
	public:
		CLoadThread(CRegionDisplay *display) { m_pDisplay = display; }
		//virtual ~CLoadThread();
	
		void *Entry() { m_pDisplay->loadPeaks(); return NULL; }

		CRegionDisplay *m_pDisplay;
	};*/

public:
	CRegionDisplay(CRegion *region);
	virtual ~CRegionDisplay();

	void DrawBG(wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);

	void LoadPeaks();

//	void *Entry();

private:
	void drawDBLines(wxDC& dc, wxRect& rc);
	void drawMiddleLine(wxDC& dc, wxRect& rc);
	void drawPeaks(wxDC& dc, wxRect& rc, int start);
	void loadPeaks();
	void loadPeaksChunked();
	int getLengthNotLooped();
//	IFloopySoundInput *getInput(CTrack *track);

private:
	float				m_fdB;
	IFloopySoundInput	*m_pInput;
	CRegion				*m_pRegion;
	bool				m_bLoaded;
	PeaksArray			m_peaks;
	bool				m_bDrawDBLines;

	CTrack				*m_pTrack;
	CTracks				*m_pTracks;

	bool				m_bDrawVertical;

	wxMemoryDC			m_tempDC;
	wxBitmap			m_tempBitmap;
	bool				m_bRepaint;

//	PointList			m_points;
//	PeaksArray			m_peaks;
//	wxMutex				*m_pMutex;
//	CLoadThread			*m_pLoadThread;
};


#endif // #ifndef Tracks_H
