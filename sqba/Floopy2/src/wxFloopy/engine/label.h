#ifndef Label_H
#define Label_H


#include "../../../ifloopy.h"
#include "../FloopyControl.h"
#include "floopyobj.h"
#include "ObjTypes.h"
#include "PathCtrl.h"


class CTrack;


class CLabel : public IFloopyObj
{
public:
	CLabel(CTrack *track);
	virtual ~CLabel();

	void DrawBG   (wxDC& dc, wxRect& rc);
	void DrawFore (wxDC& dc, wxRect& rc);

	bool HitTest(int y);

	IFloopyObj *GetChildAt(int x, int y);
	int GetChildCount();
	IFloopyObj *GetChild(int index);
	void Move(int WXUNUSED(dx), int dy);
	IFloopyObj *GetSelectedObj();
	void Select(bool selected);
	wxCursor GetCursor()					{ return wxCursor(wxCURSOR_HAND); }

private:
	inline CTrack  *getTrack()				{ return (CTrack*)GetParent(); }

	wxRect getPathRect(wxRect& rc);

private:
	wxRect		m_rcLabel;
	CPathCtrl	*m_pPathCtrl;
};


#endif // #ifndef Label_H
