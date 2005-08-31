#ifndef GridView_H
#define GridView_H

#include "MainView.h"
#include "../engine/tracks.h"

class CGridView : public CMainView
{
public:
	CGridView(wxWindow* parent, CTracks *tracks);
	virtual ~CGridView();

	void OnDraw(wxDC& dc);

//	int GetClosestGridPos(int pos);

protected:
	bool m_bDrawVGrid;
	CTracks *m_pTracks;

private:
//	BOOL m_bSnapTo;
};

#endif // #ifndef GridView_H
