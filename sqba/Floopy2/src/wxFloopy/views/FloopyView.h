// FloopyView.h: interface for the CFloopyView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOOPYVIEW_H__7E793800_358E_41AA_B7DB_B3F81FBB2B02__INCLUDED_)
#define AFX_FLOOPYVIEW_H__7E793800_358E_41AA_B7DB_B3F81FBB2B02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/splitter.h>

#include "labelsview.h"
#include "tracksview.h"
#include "rulerview.h"

class CFloopyView : public wxSplitterWindow  
{
    DECLARE_EVENT_TABLE()

public:
	CFloopyView(wxWindow* parent, CTracks *tracks);
	virtual ~CFloopyView();

//	void OnKeyDown(wxKeyEvent& event);

	bool Open(char *filename);

private:
	CTracks		*m_pTracks;
	CRulerView	*m_pLabelsView;
	CRulerView	*m_pTracksView;
};

#endif // !defined(AFX_FLOOPYVIEW_H__7E793800_358E_41AA_B7DB_B3F81FBB2B02__INCLUDED_)
