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

protected:
	bool m_bDrawVGrid;
};

#endif // #ifndef GridView_H
