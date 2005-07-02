#ifndef Tracks_H
#define Tracks_H
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/log.h>
#include <wx/wxprec.h>
#include <wx/list.h>
#include <wx/listimpl.cpp>
#include <wx/docview.h>
#include <wx/dynlib.h>
#include <strstrea.h>
//#include <SDL.h>
//#include <wx/generic/dragimgg.h>
//#include <wx/dynarray.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif


#include "../../../ifloopy.h"

#include "floopyobj.h"


#define MIN_DISTANCE	96


class CTrack;
class CRegion;
class CRegionBorder;
class CParameter;
//class CDragObjects;

WX_DECLARE_LIST(CTrack, TracksList);
WX_DECLARE_LIST(CRegion, RegionList);
WX_DECLARE_LIST(CParameter, ParameterList);
//WX_DECLARE_LIST(IFloopyObj, GraphicObjList);
//WX_DEFINE_ARRAY(int, OffsetArrays);


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

		void Move(int dx, int WXUNUSED(dy), int WXUNUSED(pps))
		{
			getTracks()->SetWidth(getTracks()->GetWidth() + dx);
		}

		void OnMouseEvent(wxMouseEvent& event)
		{
			if(event.Dragging() && (0 != m_ptPrev.x)) {
				int dx = event.GetX() - m_ptPrev.x;
				int dy = event.GetY() - m_ptPrev.y;
				Move(dx, dy, getTracks()->GetPixelsPerSecond());
			}

			IFloopyObj::OnMouseEvent(event);
		}

	private:
		inline CTracks *getTracks()	{ return (CTracks*)GetParent(); }
	};

	class CSelectedEvents : public IFloopyObj
	{
	public:
		CSelectedEvents(CTracks *tracks) : IFloopyObj(tracks) {}
		virtual ~CSelectedEvents() {}

		void Move(int dx, int WXUNUSED(dy), int WXUNUSED(pps))
		{
			getTracks()->MoveSelectedRegions(dx);
		}

	private:
		inline CTracks *getTracks()	{ return (CTracks*)GetParent(); }
	};

public:
	CTracks();
	virtual ~CTracks();

	void init();

	void SetTracksView(wxScrolledWindow *panel);// { m_pTracksView = panel; }
	void SetLabelsView(wxScrolledWindow *panel) { m_pLabelsView = panel; }
	
	wxScrolledWindow *GetTracksView() { return m_pTracksView; };
	wxScrolledWindow *GetLabelsView() { return m_pLabelsView; };

	void SetCaret(wxCaret *caret)		{ m_pCaret = caret; }
	wxCaret *GetCaret()					{ return m_pCaret; }

	void SetPixelsPerSecond(int pps);
	int GetPixelsPerSecond();

	int GetPixelsPerBeat();
	//void SetPixelsPerBeat(int ppb);

//	void SetPixelsPerSample(int pps);
//	int GetPixelsPerSample();

	int GetSamplesPerPixel();

	float GetLength()					{ return m_length; }
	void SetLength(float len)			{ m_length = len; }

	int GetWidth()						{ return m_length * (float)m_pps;}
	void SetWidth(int width);
	int GetHeight();

	void DrawBG  (wxDC& dc);
	void DrawFore(wxDC& dc);
	void DrawLabels(wxDC& dc, wxSize size);
	void DrawPreview(wxDC& dc, wxSize size);

	//CTrack *AddTrack(wxString name, wxColour colour = wxColour(100, 100, 100));
	CTrack *AddTrack(IFloopySoundInput *input, int level);
	bool RemoveTrack(CTrack *track);

	void DeselectAllTracks();
	void DeselectAllRegions();

	void MoveSelectedRegions(int dx);
	void RemoveSelectedObjects();
	void UpdateSelectedRegions();

	CTrack *GetTrackAt(int y);
	IFloopyObj *GetChildAt(int x, int y);

	void Refresh();

	void Dump(ostream& stream);

	void Play();

//	TracksList &GetTracks() { return m_tracks; }

	BOOL createEngine(char *plugin);
	void loadTracks(IFloopySoundInput *input, int level);
	BOOL Open(char *filename);
	BOOL Save(char *filename);
	void Clear();

	IFloopySoundEngine *GetInput() { return m_pEngine; }

	void OnKeyDown(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);
	IFloopyObj *GetSelectedObj();
	void changeHeight(int dy);

//	void SetPanel(wxScrolledWindow *panel) { m_pPanel = panel; }
//	wxScrolledWindow *GetPanel() { return m_pPanel; }

	void RefreshTracks(CTrack *track);

	IFloopySoundEngine *GetEngine() { return m_pEngine; }

	int GetClosestGridPos(int pos);

	int CalcStep(int mindist);

private:
	int					m_pps, m_bpm;
//	int					m_hres;		//! Samples per pixel
	bool				m_bInit;
	float				m_length;	// In seconds
	wxCaret				*m_pCaret;
	TracksList			m_tracks;
	wxScrolledWindow	*m_pTracksView, *m_pLabelsView;
	CBorder				*m_pBorder;

	IFloopySoundEngine *m_pEngine;
	wxDynamicLibrary	m_libEngine;

//	wxScrolledWindow	*m_pPanel;

	BOOL m_bSnapTo;
};

class CTrack : public IFloopyObj  
{
    DECLARE_DYNAMIC_CLASS(CTrack)

	class CBorder : public IFloopyObj
	{
	public:
		CBorder(CTrack *track) : IFloopyObj(track) {}
		virtual ~CBorder() {}

		wxCursor GetCursor() { return wxCursor(wxCURSOR_SIZENS); }

		void Move(int WXUNUSED(dx), int dy, int WXUNUSED(pps))
		{
			int height = getTrack()->GetHeight();
			getTrack()->SetHeight(height + dy);
		}

	private:
		CTrack *getTrack() { return (CTrack*)GetParent(); }
	};

public:
	CTrack() {}
	CTrack(CTracks *tracks, IFloopySoundInput *input, int level, wxColour colour);
	virtual ~CTrack();

	//void Draw(wxDC& dc, wxRect& rc);
	void DrawLabel(wxDC& dc, wxRect& rc);
	void DrawBG   (wxDC& dc, wxRect& rc, int WXUNUSED(pps));
	void DrawFore (wxDC& dc, wxRect& rc, int pps);
	void DrawPreview(wxDC& dc, wxRect& rc, int pps);

	bool GetName(wxString& name) { name = _T("Track"); return TRUE; }
	int GetWidth()				{ return GetTracks()->GetWidth(); }
	int GetHeight()				{ return m_height; }
	int GetTop()				{ return m_top; }
	void SetHeight(int height);
	wxColour GetColour()		{ return m_colour; }
	wxString GetName()			{ return m_name; }
	wxBitmap *GetBitmap()		{ return m_pBitmap; }
	wxCursor GetCursor() { return wxCursor(wxCURSOR_PENCIL); }

	//CRegion *AddRegion(float start, float end);
	CRegion *AddRegion(int start, int end);
	CRegion *AddNewRegionAt(int left);
	bool RemoveRegion(CRegion *event);

	CTracks *GetTracks()	{ return (CTracks*)GetParent(); }

	bool HitTest(int y);

	IFloopyObj *GetChildAt(int x, int y);
	IFloopyObj *GetSelectedObj();
	IFloopyObj *GetDisplay() { return m_pDisplay; }

	void DeselectAllRegions();

	void Refresh();

	void Move(int dx, int dy, int pps);

	void MoveSelectedRegions(int dx);
	void RemoveSelectedObjects();
	void UpdateSelectedRegions();

	void Dump(ostream& stream);

	IFloopySoundInput *GetInput() { return m_pInput; }

//	EventList &GetEvents() { return m_events; }
	void OnKeyDown(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	int GetRegionCount() { return m_regions.GetCount(); }

	void CheckIntersections(CRegion *pRegion1, int &left, int &right, bool bResize);

	bool IsHidden() { return m_bHide; } // ->Collapse/expand
	void Hide(bool bHide) { m_bHide = bHide; }

private:
	bool LoadDisplay(wxString strType);
	void loadRegions();
	int	 m_nLevel;

private:
	bool		m_bHide;
	RegionList	m_regions;
	wxRect		m_rcLabel;
	wxColour	m_colour;
	int			m_top;
	int			m_height;
	wxString	m_name;
	wxBitmap	*m_pBitmap;
	CBorder		*m_pBorder;
	IFloopyObj	*m_pDisplay;
	wxDynamicLibrary	m_libDisplay;
	IFloopySoundInput *m_pInput;
};

class CRegion : public IFloopyObj
{
    DECLARE_DYNAMIC_CLASS(CRegion)

public:
	/*! \class CRegionBorder
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

		void Move(int dx, int WXUNUSED(dy), int pps)
		{
			if(m_bLeft) {
				getRegion()->Resize(dx, 0, pps);
			} else {
				getRegion()->Resize(0, dx, pps);
			}
		}

	private:
		inline CRegion  *getRegion()	{ return (CRegion*)GetParent(); }
		bool m_bLeft; // Left or right, start or end
		CRegion *m_pRegion;
	};

public:
	CRegion() {}
	CRegion(CTrack *track, UINT startSample, UINT endSample);
	virtual ~CRegion();

	void DrawBG  (wxDC& dc, wxRect& rc, int pps);
	void DrawFore(wxDC& dc, wxRect& rc, int pps);
	void DrawPreview(wxDC& dc, wxRect& rc, int pps);

	int GetLeft();
	int GetRight();
	int GetWidth();

	void GetRect(wxRect& rc);

	wxCursor GetCursor() { return wxCursor(wxCURSOR_HAND); }
	wxColour GetColour() { return getTrack()->GetColour(); }

	IFloopyObj *GetBorder(bool left) { return(left ? m_pLeftBorder : m_pRightBorder); }
	IFloopyObj *GetSelectedObj();// { return IsSelected() ? this : NULL; }
	IFloopyObj *GetChildAt(int x, int y);

	void Move(int dx, int WXUNUSED(dy), int WXUNUSED(pps));
	bool HitTest(int x, int y);
	void Refresh();
	void Resize(int dl, int dr, int pps);
	void Update();
	void CancelUpdate();
	void Remove();

	void OnKeyDown(wxKeyEvent& event);
	void OnMouseEvent(wxMouseEvent& event);

	int GetStartOffset()	{ return m_iStartSample; }
	int GetEndOffset()		{ return m_iEndSample; }

	int GetHeight()	{ return getTrack()->GetHeight(); }
	int GetTop()	{ return getTrack()->GetTop(); }

private:
	static void remove(IFloopyObj *event);
	void createMenu();
	void startEdit();
	void calcPos(int *left, int *right);
	inline CTrack  *getTrack()	{ return (CTrack*)GetParent(); }
	//inline CTracks *getTracks()	{ return getTrack()->GetTracks(); }
	inline CTracks *getTracks()	{ return (CTracks*)getTrack()->GetParent(); }
	void loadParameters(IFloopySoundInput *obj);
	//void drawParametersBG(wxDC& dc, wxRect& rc, int pps);
	void drawParametersFore(wxDC& dc, wxRect& rc, int pps);

private:
	BOOL			m_bEdit;
	int				m_iStartSample, m_iEndSample, m_iPrevStart, m_iPrevEnd;
	wxRect			m_rcRegion;
	CBorder			*m_pLeftBorder, *m_pRightBorder;
	ParameterList	m_Parameters;
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

		void Move(int dx, int WXUNUSED(dy), int pps);

		int m_offset;
	};

public:
	CParameter() {}
	CParameter(CRegion *region, int index);
	virtual ~CParameter();

//	IFloopyObj *GetSelectedObj();
	IFloopyObj *GetChildAt(int x, int y);

	float GetValueAt(int x);

	//void DrawBG  (wxDC& dc, wxRect& rc, int pps);
	void DrawFore(wxDC& dc, wxRect& rc, int pps);
	//void DrawPreview(wxDC& dc, wxRect& rc, int pps);

	bool HitTest(int x, int y);

private:
	inline CRegion	*getRegion()	{ return (CRegion*)GetParent(); }
	inline CTrack	*getTrack()		{ return (CTrack*) getRegion()->GetParent(); }
	inline CTracks	*getTracks()	{ return (CTracks*)getTrack()->GetParent(); }

private:
	int		m_index;
	BOOL	m_bEdit;
	int		m_iSamplePos, m_iPrevSamplePos;
	CPoint	*m_pPoint;
//	OffsetArrays m_Offsets;
};


#endif // #ifndef Tracks_H