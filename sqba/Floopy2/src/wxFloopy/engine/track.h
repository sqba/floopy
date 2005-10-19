#ifndef Track_H
#define Track_H


#include "../../../ifloopy.h"
#include "floopyobj.h"
#include "ObjTypes.h"


#include <wx/string.h>
#include <strstrea.h>


class CTracks;
class CRegion;
class CLabel;


WX_DECLARE_LIST(CRegion, RegionList);


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
//	CTrack() {}
	CTrack(CTracks *tracks, IFloopySoundInput *input, int level);
	virtual ~CTrack();

	int GetType()	{ return FLOOPY_TRACK; }

	void DrawBG   (wxDC& dc, wxRect& rc);
	void DrawFore (wxDC& dc, wxRect& rc);

	bool GetName(wxString& name)	{ name = m_name; return true; }
	int GetWidth();
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

	IFloopySoundInput *GetTrack()	{ return m_pTrack; }
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

	CLabel	*GetLabel() { return m_pLabel; }

	int   GetPropertyCount();
	bool  GetPropertyVal(int, float*);
	void  SetPropertyVal(int, float);
	char *GetPropertyName(int);
	char *GetPropertyDesc(int);
	float GetPropertyMax(int);
	float GetPropertyMin(int);
	char *GetPropertyUnit(int);
	float GetPropertyStep(int);

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
	IFloopySoundInput *m_pTrack;
	IFloopySoundInput *m_pInput;
	IFloopySoundInput *m_pSource;

	bool		m_bReset;

	CLabel	*m_pLabel;
};


#endif // #ifndef Track_H
