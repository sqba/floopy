#ifndef _FLOOPYFRAME_H
#define _FLOOPYFRAME_H


#include <wx/dnd.h>
#include <wx/splitter.h>

#include "views/labelsview.h"
#include "views/tracksview.h"

#include "dialogs/controldlg.h"

class CFloopyFrame : public wxFrame
{
//	DECLARE_DYNAMIC_CLASS(CFloopyFrame)

private:
	class CDropTarget : public wxFileDropTarget
	{
	public:
		CDropTarget(CFloopyFrame *frm) { m_pFrame = frm; }
		virtual ~CDropTarget() {}

		bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
		{
			for(int i=0; i<(int)filenames.Count(); i++)
			{
				m_pFrame->Open( (char*)filenames.Item(i).GetData() );
			}
			return true;
		}

	private:
		CFloopyFrame *m_pFrame;
	};

public:
	CFloopyFrame();
	~CFloopyFrame();

	void Open(char *filename);
	bool Save();
	void SaveAs();
	bool Close();

private:
	void OnFullScreen( wxCommandEvent &WXUNUSED(event) );
    void OnQuit(wxCommandEvent &WXUNUSED(event));
    void OnAbout(wxCommandEvent &WXUNUSED(event));
    void OnFileOpen(wxCommandEvent &WXUNUSED(event));
    void OnFileSave(wxCommandEvent &WXUNUSED(event));
    void OnFileSaveAs(wxCommandEvent &WXUNUSED(event));
    void OnFileClose(wxCommandEvent &WXUNUSED(event));
	void OnPlay( wxCommandEvent &WXUNUSED(event) );
	void OnPause( wxCommandEvent &WXUNUSED(event) );
	void OnStop( wxCommandEvent &WXUNUSED(event) );
	void OnShowParams( wxCommandEvent &WXUNUSED(event) );
	void OnShowProperties( wxCommandEvent &WXUNUSED(event) );

	void initMenus();
	void initViews();
	void initToolbar();

private:
	CDropTarget			*m_pDropTarget;
	wxSplitterWindow	*m_pSplitter;
	CLabelsView			*m_pLabelsView;
	CTracksView			*m_pTracksView;
	CTracks				*m_pTracks;
	CControlDlg			*m_pParamsDialog;
	CControlDlg			*m_pPropsDialog;

private:
    DECLARE_EVENT_TABLE()
};

#endif _FLOOPYFRAME_H
