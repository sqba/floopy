// MainView.cpp: implementation of the CMainView class.
//
//////////////////////////////////////////////////////////////////////

#include "MainView.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainView::CMainView(CRulerView* parent, CTracks *tracks) : CView(parent)
{
	m_pTracks = tracks;

	//SetWindowStyle(wxSIMPLE_BORDER);
	//SetWindowStyle(wxNO_BORDER);
	//SetWindowStyle(wxSUNKEN_BORDER);

	//SetBackgroundColour(*wxWHITE);
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
	//SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND));
	//SetBackgroundColour(*wxLIGHT_GREY);
	//SetBackgroundColour(*wxBLACK);
}

CMainView::~CMainView()
{

}
