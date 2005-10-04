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


#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#define MIN_DISTANCE	96


#define FLOOPY_TRACKS				FLOOPY_OBJ+1
#define FLOOPY_TRACK				FLOOPY_OBJ+2
#define FLOOPY_REGION				FLOOPY_OBJ+3
#define FLOOPY_PARAMETER			FLOOPY_OBJ+4
#define FLOOPY_REGION_DISP			FLOOPY_OBJ+5
#define FLOOPY_REGION_BORDER		FLOOPY_OBJ+6
#define  FLOOPY_REGION_OFFSET_BAR	FLOOPY_OBJ+7
#define FLOOPY_TRACKS_BORDER		FLOOPY_OBJ+8
#define FLOOPY_TRACK_BORDER			FLOOPY_OBJ+9


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
	void RemoveSelectedObjects();
	void UpdateSelectedRegions();


	void SetRegionsView(CRulerView *panel);
	void SetLabelsView(CRulerView *panel) { m_pLabelsView = panel; }
	
	CRulerView *GetRegionsView()			{ return m_pRegionsView; };
	CRulerView *GetLabelsView()			{ return m_pLabelsView; };

	void SetCaretPos(int samples);
	int  GetCaretPos();

	int  GetSamplesPerPixel();
	void SetSamplesPerPixel(int);

	float GetLength()							{ return m_length; }
	void  SetLength(float len)					{ m_length = len; }

	int GetTrackCount()								{ return m_tracks.GetCount(); }

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
	IFloopySoundEngine *GetEngine()				{ return m_pEngine; }

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

private:
	CTrack *addTrack(IFloopySoundInput*, IFloopySoundInput*, int);
	IFloopySoundMixer *getMixer();
	bool createEngine(char *plugin);
	void loadTracks(IFloopySoundInput*, IFloopySoundInput*, int);
	void changeHeight(int dy);
	void init();

private:
	TracksList			m_tracks;
	CRulerView			*m_pRegionsView, *m_pLabelsView;
	CBorder				*m_pBorder;
	IFloopySoundEngine	*m_pEngine;
	IFloopySoundMixer	*m_pMixer;
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

class CTrack : public IFloopyObj  
{
//	DECLARE_DYNAMIC_CLASS(CTrack)

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

		int GetType()	{ return FLOOPY_TRACK_BORDER; }

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZENS); }
		void Move(int WXUNUSED(dx), int dy);
		void DrawBG(wxDC&, wxRect&);
		void DrawFore(wxDC&, wxRect&);
		int GetHeight();

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

public:
	class CTrackLabel : public IFloopyObj
	{
	public:
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
		CTrackLabel(CTrack *track);
		virtual ~CTrackLabel();

		void DrawBG   (wxDC& dc, wxRect& rc);
		void DrawFore (wxDC& dc, wxRect& rc);

		bool HitTest(int y);

	private:
		inline CTrack  *getTrack()	{ return (CTrack*)GetParent(); }
		void drawLoopSign(wxDC& dc, wxRect& rc);
		void drawCacheSign(wxDC& dc, wxRect& rc);

	private:
		wxRect m_rcLabel;
		CLoopButton		*m_pButtonLoop;
		CCacheButton	*m_pButtonCache;
	};

public:
	CTrack() {}
	CTrack(CTracks *tracks, IFloopySoundInput *input, IFloopySoundInput *parent, int level);
	virtual ~CTrack();

	int GetType()	{ return FLOOPY_TRACK; }

	void DrawBG   (wxDC& dc, wxRect& rc);
	void DrawFore (wxDC& dc, wxRect& rc);

	bool GetName(wxString& name)	{ name = m_name; return true; }
	int GetWidth()					{ return GetTracks()->GetWidth(); }
	int GetHeight();//					{ return m_height + m_pBorder->GetHeight(); }
	int GetTop()					{ return m_top; }
	void SetHeight(int height);
//	wxString GetName()				{ return m_name; }
	wxBitmap *GetBitmap()			{ return m_pBitmap; }
	wxCursor GetCursor()			{ return wxCursor(wxCURSOR_PENCIL); }

	CRegion *AddRegion(int start, int end);
	CRegion *AddNewRegionAt(int left);
	bool RemoveRegion(CRegion *region);

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

	CRegion *GetRegion(int index);

	int GetChildCount()						{ return GetRegionCount(); }
	IFloopyObj *GetChild(int index)			{ return (IFloopyObj*)GetRegion(index); }

	void CheckIntersections(CRegion *pRegion1, int &left, int &right, bool bResize);

	bool IsLooped();
	void SetLooped(bool bLooped);

	bool IsReverse();
	void SetReverse(bool bReverse);

	bool GetReset();
	void SetReset(bool bReset);

	void InvalidateRegions(CRegion *start);

	wxColor GetBGColor();
	wxColor GetForeColor();

	IFloopySoundInput *FindComponentByName(char *name);

	void Select(bool selected=true);

	wxColor GetColor();
	void SetColor(wxColor color);

	int GetCaretPos();//	{ return GetTracks()->GetCaretPos(); }

	CTrackLabel	*GetLabel() { return m_pLabel; }

private:
	bool LoadDisplay(wxString strType);
	void loadRegions();

private:
	int			m_nLevel;
	RegionList	m_regions;
	int			m_top;
//	int			m_height;
	wxString	m_name;
	wxBitmap	*m_pBitmap;
	CBorder		*m_pBorder;
	IFloopySoundInput *m_pInput;

	bool		m_bReset;

	IFloopySoundInput *m_pSource;
	CTrackLabel	*m_pLabel;
};

class CRegion : public IFloopyObj
{
//	DECLARE_DYNAMIC_CLASS(CRegion)

public:
	/*! \class CBorder
	 *  \brief Region's left or right border
	 *  \author sqba
	 *  \version 0.0
	 *  \date 2005
	 *
	 *  Class represents event's left or right border...
	 */
	class CBorder : public IFloopyObj
	{
	public:
		CBorder(CRegion *region, bool left) : IFloopyObj(region) { m_bLeft = left;}
		virtual ~CBorder() {}

		int GetType()	{ return FLOOPY_REGION_BORDER; }

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZEWE); }

		void Move(int dx, int WXUNUSED(dy));

	private:
		inline CRegion  *getRegion()	{ return (CRegion*)GetParent(); }
		bool m_bLeft; // Left or right, start or end of region
	};

	class COffsetBar : public IFloopyObj
	{
	public:
		COffsetBar(CRegion *region) : IFloopyObj(region) {}
		virtual ~COffsetBar() {}

		int GetType()	{ return FLOOPY_REGION_OFFSET_BAR; }

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZEWE); }

		void Move(int dx, int WXUNUSED(dy));
		void DrawBG  (wxDC &dc, wxRect &rc);
		void DrawFore(wxDC &dc, wxRect &rc);

		int GetHeight();// { return 20; }

	private:
		inline CRegion  *getRegion()	{ return (CRegion*)GetParent(); }
	};

public:
	CRegion() {}
	CRegion(CTrack *track, UINT startSample, UINT endSample);
	virtual ~CRegion();

	int GetType()	{ return FLOOPY_REGION; }

	int GetChildCount()						{ return 1; }
	IFloopyObj *GetChild(int index)			{ return (IFloopyObj*)m_pDisplay; }

	void DrawBG  (wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);

	int GetLeft();
	int GetRight();
	int GetWidth();

	void GetRect(wxRect& rc);

	wxCursor GetCursor() { return wxCursor(wxCURSOR_HAND); }
	wxColor GetColor()	 { return getTrack()->GetColor(); }

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

	int GetStartPos()	{ return m_iStartSample; }
	int GetEndPos()		{ return m_iEndSample; }

	int GetHeight()	{ return getTrack()->GetHeight() - 2; }
	int GetTop()	{ return getTrack()->GetTop() + 1; }

	bool GetReset();
	void SetReset(bool bReset);

	void SetStartOffset(int sample);

	wxColor GetBGColor();
	wxColor GetForeColor();

	void Select(bool selected=true);

	IFloopySoundInput *GetInput()				{ return getTrack()->GetInput(); }
	int GetCaretPos();

	int GetStartOffset() { return m_iStartOffset; }

private:
	static void remove(IFloopyObj *region);
	void createMenu();
	void startEdit();
	void calcPos(int *left, int *right);
	inline CTrack  *getTrack()	{ return (CTrack*)GetParent(); }
	inline CTracks *getTracks()	{ return (CTracks*)getTrack()->GetParent(); }
	void loadParameters(IFloopySoundInput *obj);
	void drawParametersFore(wxDC& dc, wxRect& rc);
	bool getReset(int sample);

	int getStartOffset();
	void drawFrame(wxDC& dc, wxRect& rc);
	int getLengthNotLooped();

private:
	bool			m_bEdit;
	int				m_iStartSample, m_iEndSample, m_iPrevStart, m_iPrevEnd;
	wxRect			m_rcRegion;
	CBorder			*m_pLeftBorder, *m_pRightBorder;
	ParameterList	m_Parameters;
	CRegionDisplay	*m_pDisplay;
	bool			m_bDrawPreview;
	int				m_iStartOffset;
	int				m_iLengthNotLooped;
	COffsetBar		*m_pOffsetBar;
	bool			m_bShowOffsetBar;
};


class CParameter : public IFloopyObj
{
//	DECLARE_DYNAMIC_CLASS(CParameter)

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

	int GetType()	{ return FLOOPY_PARAMETER; }

	IFloopyObj *GetChildAt(int x, int y);
	float GetValueAt(int x);
	void DrawFore(wxDC& dc, wxRect& rc);
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


class CRegionDisplay : public IFloopyObj
{
public:
	CRegionDisplay(CRegion *region);
	virtual ~CRegionDisplay();

	int GetType()	{ return FLOOPY_REGION_DISP; }

	void DrawBG(wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);

	void LoadPeaks();

	int   GetPropertyCount();
	bool  GetPropertyVal(int index, float *value);
	void  SetPropertyVal(int index, float value);
	char *GetPropertyName(int index);
	char *GetPropertyDesc(int index);
	float GetPropertyMax(int index);
	float GetPropertyMin(int index);
	char *GetPropertyUnit(int index);
	float GetPropertyStep(int index);

private:
	void drawDBLines(wxDC& dc, wxRect& rc);
	void drawCenterLine(wxDC& dc, wxRect& rc);
	void drawPeaks(wxDC& dc, wxRect& rc, int start);
	void loadPeaks();
	void loadPeaksChunked();
	int getLengthNotLooped();
	int getMaxSampleValue();

private:
	IFloopySoundInput	*m_pInput;
	CRegion				*m_pRegion;
	bool				m_bLoaded;
	PeaksArray			m_peaks;

	CTrack				*m_pTrack;
	CTracks				*m_pTracks;

	bool				m_bDrawVertical;

	wxMemoryDC			m_tempDC;
	wxBitmap			m_tempBitmap;
	bool				m_bRepaint;

	// Properties
	float				m_fdB;
	bool				m_bDrawDBLines;
	bool				m_bDrawContour;
};


#endif // #ifndef Tracks_H
