#ifndef _FLOOPYFRAME_H
#define _FLOOPYFRAME_H


#include <wx/dnd.h>
#include <wx/timer.h>

#include "views/tracksview.h"


class CFloopyFrame : public wxFrame
{
//	DECLARE_DYNAMIC_CLASS(CFloopyFrame)

private:
	class CTimer : public wxTimer
	{
	public:
		CTimer() : wxTimer() {}

		void Start(CFloopyFrame *frame);
		void Notify();

	private:
		CFloopyFrame *m_pFrame;
	};

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
			    wxString str( filenames.Item(i).GetData() );
				m_pFrame->Open( str );
			}
			return true;
		}

	private:
		CFloopyFrame *m_pFrame;
	};

public:
	CFloopyFrame();
	~CFloopyFrame();

	bool Open(wxString &filename);
	bool Save();
	bool SaveAs();
	bool Close();
	void ShowFreeMemory();

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
	void OnShowPreview( wxCommandEvent &WXUNUSED(event) );

	void initMenus();
	void initToolbar();

private:
	CDropTarget	*m_pDropTarget;
	CTracksView	*m_pView;
	CTracks		*m_pTracks;
	CTimer		m_Timer;

private:
    DECLARE_EVENT_TABLE()
};

#endif // _FLOOPYFRAME_H
