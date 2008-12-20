#ifndef GridView_H
#define GridView_H

#include "mainview.h"
#include "../engine/tracks.h"

class CGridView : public CMainView
{
public:
	CGridView(CRulerView* parent, CTracks *tracks);
	virtual ~CGridView();

	void OnDraw(wxDC& dc);

protected:
	bool m_bDrawVGrid;
};

#endif // #ifndef GridView_H
