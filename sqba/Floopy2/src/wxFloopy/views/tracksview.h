// TracksView.h: interface for the CTracksView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOOPYVIEW_H__7E793800_358E_41AA_B7DB_B3F81FBB2B02__INCLUDED_)
#define AFX_FLOOPYVIEW_H__7E793800_358E_41AA_B7DB_B3F81FBB2B02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/splitter.h>

#include "labelsview.h"
#include "timelineview.h"
#include "rulerview.h"

class CTracksView : public wxSplitterWindow  
{
    DECLARE_EVENT_TABLE()

public:
	CTracksView(wxWindow* parent, CTracks *tracks);
	virtual ~CTracksView();

//	void OnKeyDown(wxKeyEvent& event);

	bool Open(char *filename);
	bool Save();

	void ShowParamsDlg();
	void ShowPropertiesDlg();

private:
	CTracks		*m_pTracks;
	CRulerView	*m_pLabelsView;
	CRulerView	*m_pTimelineView;
};

#endif // !defined(AFX_FLOOPYVIEW_H__7E793800_358E_41AA_B7DB_B3F81FBB2B02__INCLUDED_)
