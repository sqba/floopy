#ifndef Region_H
#define Region_H


#include "floopyobj.h"
#include "ObjTypes.h"


//class CParameter;
class CParameters;
class CRegionDisplay;


//WX_DECLARE_LIST(CParameter, ParameterList);


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

		int GetType()						{ return FLOOPY_REGION_OFFSET_BAR; }

		wxCursor GetCursor()				{ return wxCursor(wxCURSOR_SIZEWE); }

		void Move(int dx, int WXUNUSED(dy));
		void DrawBG  (wxDC &dc, wxRect &rc);
		void DrawFore(wxDC &dc, wxRect &rc);

		int GetHeight();// { return 20; }

	private:
		inline CRegion  *getRegion()		{ return (CRegion*)GetParent(); }
		CRegion *getPrevRegion();

	private:
		int m_iStart;
	};

public:
//	CRegion() {}
	CRegion(CTrack *track, UINT startSample, UINT endSample);
	virtual ~CRegion();

	int GetType()							{ return FLOOPY_REGION; }

	int GetChildCount()						{ return 1; }
	IFloopyObj *GetChild(int index)			{ return (IFloopyObj*)m_pDisplay; }

	void DrawBG  (wxDC& dc, wxRect& rc);
	void DrawFore(wxDC& dc, wxRect& rc);

	int GetLeft();
	int GetRight();
	int GetWidth();

	void GetRect(wxRect& rc);

	wxCursor GetCursor()					{ return wxCursor(wxCURSOR_HAND); }
	wxColor GetColor();

	IFloopyObj *GetBorder(bool left)		{ return(left ? m_pLeftBorder : m_pRightBorder); }
	IFloopyObj *GetSelectedObj();			// { return IsSelected() ? this : NULL; }
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

	int GetStartPos()							{ return m_iStartSample; }
	int GetEndPos()								{ return m_iEndSample; }

	int GetHeight();
	int GetTop();

	bool GetReset();
	void SetReset(bool bReset);
	bool HasStartOffset();

	void SetStartOffset(int sample);

	wxColor GetBGColor();
	wxColor GetForeColor();

	void Select(bool selected=true);

	IFloopySoundInput *GetInput();
	int GetCaretPos();

	int GetStartOffset()						{ return m_iStartOffset; }
	int GetEndOffset();

	int   GetPropertyCount();
	bool  GetPropertyVal(int, float*);
	void  SetPropertyVal(int, float);
	char *GetPropertyName(int);
	char *GetPropertyDesc(int);
	float GetPropertyMax(int);
	float GetPropertyMin(int);
	char *GetPropertyUnit(int);
	float GetPropertyStep(int);

	void ShowObjectParameters(IFloopySoundInput *obj, bool show=true);

//	void DeselectAllParameters(CParameter *caller);

private:
	static void remove(IFloopyObj *region);
	void createMenu();
	void startEdit();
	void calcPos(int *left, int *right);
	inline CTrack  *getTrack()					{ return (CTrack*)GetParent(); }
	inline CTracks *getTracks();
//	void loadParameters(IFloopySoundInput *obj);
//	void removeParameters(IFloopySoundInput *obj);
//	void drawParametersFore(wxDC& dc, wxRect& rc);
	bool getReset(int sample);
//	bool isAfterTrack(IFloopySoundInput *obj);
	bool paramsLoaded(IFloopySoundInput *obj);

	int getStartOffset();
	void drawFrame(wxDC& dc, wxRect& rc);
	int getLengthNotLooped();
	CRegion *getPrevRegion();

private:
	bool			m_bEdit;
	int				m_iStartSample, m_iEndSample, m_iPrevStart, m_iPrevEnd;
	wxRect			m_rcRegion;
	CBorder			*m_pLeftBorder, *m_pRightBorder;
//	ParameterList	m_Parameters;
	CRegionDisplay	*m_pDisplay;
	int				m_iStartOffset;
	int				m_iLengthNotLooped;
	COffsetBar		*m_pOffsetBar;
	bool			m_bShowOffsetBar;
	CParameters		*m_pParameters;
};


#endif // #ifndef Region_H
