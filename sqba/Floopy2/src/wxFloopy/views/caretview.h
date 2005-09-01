#ifndef CaretView_H
#define CaretView_H

#include <wx/log.h>
#include <wx/dc.h>
#include <wx/docview.h>
#include <wx/caret.h>

#include "gridview.h"
#include "../engine/tracks.h"

class CCaretView : public CGridView  
{
public:
	CCaretView(wxWindow* parent, CTracks *tracks);
	virtual ~CCaretView();

//	void OnDraw(wxDC& dc);
	void ScrollWindow( int dx, int dy, const wxRect *rect );

	void OnMouseEvent(wxMouseEvent& event);
	void OnSize( wxSizeEvent &event );

//	int GetCaretPos(); // In samples!
//	void SetCaretPos(int samples);

private:
	//wxRect m_rcSelection;
	int m_iSelStart, m_iSelEnd;
	bool m_bSelection;
	wxPoint m_ptSelStart, m_ptSelEnd;
    DECLARE_EVENT_TABLE()
};

#endif // #ifndef CaretView_H