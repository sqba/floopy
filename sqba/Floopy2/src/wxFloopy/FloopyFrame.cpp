// For compilers that supports precompilation , includes "wx/wx.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "FloopyFrame.h"



enum
{
	// menu items
//	Minimal_Quit = 1,
//	Minimal_About,
	ID_FULL  = 109,
	ID_OPEN,
	ID_SAVEAS,
	ID_CLOSE,
	ID_EXIT,
	ID_ABOUT,
	ID_PLAY,
	ID_PAUSE,
	ID_STOP
};

//IMPLEMENT_DYNAMIC_CLASS(CFloopyFrame, wxFrame)

BEGIN_EVENT_TABLE(CFloopyFrame, wxFrame)
	EVT_MOUSE_EVENTS(CFloopyFrame::OnMouseEvent)

//	EVT_MENU(Minimal_Quit,  CFloopyFrame::OnQuit)
//	EVT_MENU(Minimal_About, CFloopyFrame::OnAbout)
	EVT_MENU(ID_FULL,		CFloopyFrame::OnFullScreen)
	EVT_MENU(ID_OPEN,		CFloopyFrame::OnFileOpen)
	EVT_MENU(ID_SAVEAS,		CFloopyFrame::OnFileSave)
	EVT_MENU(ID_CLOSE,		CFloopyFrame::OnFileClose)
	EVT_MENU(ID_EXIT,		CFloopyFrame::OnQuit)
	EVT_MENU(ID_ABOUT,		CFloopyFrame::OnAbout)
	EVT_MENU(ID_PLAY,		CFloopyFrame::OnPlay)
	EVT_MENU(ID_PAUSE,		CFloopyFrame::OnPause)
	EVT_MENU(ID_STOP,		CFloopyFrame::OnStop)
END_EVENT_TABLE()


CFloopyFrame::CFloopyFrame(const wxChar *title, int xpos, int ypos, int width, int height)
	: wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height))
{
#ifdef __WXMSW__
	//this->SetIcon(wxString(isCanvas ? _T("chart") : _T("notepad")));
	this->SetIcon(wxICON(Floopy));
#endif
#ifdef __X__
	this->SetIcon(wxIcon(_T("doc.xbm")));
#endif

	m_pTracks = new CTracks();

	m_pDropTarget = new CDropTarget( this );
	SetDropTarget( m_pDropTarget );

	initMenus();
	initViews();

	// create the toolbar and add our 1 tool to it
	wxToolBar *toolbar = CreateToolBar();
	//wxBitmap aboutImage("res/help.bmp", wxBITMAP_TYPE_BMP);
	wxBitmap aboutImage("CSQUERY", wxBITMAP_TYPE_RESOURCE);
	wxBitmap playImage("PLAY", wxBITMAP_TYPE_RESOURCE);
	wxBitmap pauseImage("PAUSE", wxBITMAP_TYPE_RESOURCE);
	wxBitmap stopImage("STOP", wxBITMAP_TYPE_RESOURCE);
	toolbar->AddTool(ID_ABOUT, _("About"), aboutImage, _("About Floopy"));
	toolbar->AddTool(ID_PLAY, _("Play"), playImage, _("Play"));
	toolbar->AddTool(ID_PAUSE, _("Pause"), pauseImage, _("Pause"));
	toolbar->AddTool(ID_STOP, _("Stop"), stopImage, _("Stop"));
	toolbar->Realize();
}

CFloopyFrame::~CFloopyFrame()
{
	delete m_pSplitter;
	delete m_pTracks;
}

void CFloopyFrame::OnMouseEvent(wxMouseEvent& event)
{
	if( event.ButtonDown(wxMOUSE_BTN_LEFT) ) {
	}
}

void CFloopyFrame::OnFullScreen( wxCommandEvent &WXUNUSED(event) )
{
   ShowFullScreen( !IsFullScreen(), wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION );
}

void CFloopyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void CFloopyFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this, "Open",
		"", "", "XML Files(*.xml)|*.xml|All files(*.*)|*.*",
		wxOPEN, wxDefaultPosition);
	if ( dlg->ShowModal() == wxID_OK )
	{
		//m_pTextCtrl->LoadFile(dlg->GetFilename());
		SetStatusText(dlg->GetFilename(), 0);
		char *filename = (char*)dlg->GetPath().c_str();
		//m_pTracks->Open(filename);
		Open(filename);
	}
	dlg->Destroy();
	//delete dlg;
}

void CFloopyFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
	m_pTracks->Clear();
}

void CFloopyFrame::Open(char *filename)
{
	if( m_pTracks->Open(filename) )
	{
		wxString str;
		str.Printf("Floopy! - %s", filename);
		SetTitle( str );
//		m_pTracksView->RefreshRulers();
		m_pTracksView->SetFocus();
	}
}

void CFloopyFrame::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this, "Save",
		"", "", "XML Files(*.xml)|*.xml|All files(*.*)|*.*",
		wxSAVE, wxDefaultPosition);
	if ( dlg->ShowModal() == wxID_OK )
	{
		//m_pTextCtrl->LoadFile(dlg->GetFilename());
		SetStatusText(dlg->GetFilename(), 0);
		char *filename = (char*)dlg->GetPath().c_str();
		m_pTracks->Save(filename);
	}
	dlg->Destroy();
	//delete dlg;
}

void CFloopyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    // called when help - about is picked from the menu or toolbar
    //wxString msg;
    //msg.Printf(_("About Floopy...\n"));
    //wxMessageBox(msg, _("About Floopy"), wxOK | wxICON_INFORMATION, this);
    (void)wxMessageBox(_T("Floopy\nAuthor: Filip Pavlovic"), _T("About Floopy"));
}

void CFloopyFrame::initMenus()
{
	//// Make a menubar
	wxMenu *file_menu = new wxMenu;

	file_menu->Append(wxID_NEW,		_T("&New..."),			_T("New project"));
	file_menu->Append(ID_OPEN,		_T("&Open..."),			_T("Open project file"));
	file_menu->Append(ID_CLOSE,		_T("&Close"),			_T("Close current project"));
	file_menu->Append(wxID_SAVE,	_T("&Save"),			_T("Save current project"));
	file_menu->Append(ID_SAVEAS,	_T("Save &As..."),		_T("Save current project with a different name"));
	file_menu->AppendSeparator();
	file_menu->Append(ID_EXIT,		_T("E&xit"),			_T("Exit Floopy!"));

	wxMenu *edit_menu = new wxMenu;
	edit_menu->Append(wxID_UNDO,	_T("&Undo"),			_T("Undo last action"));
	edit_menu->Append(wxID_REDO,	_T("&Redo"),			_T("Redo last action"));
	edit_menu->AppendSeparator();

	wxMenu *view_menu = new wxMenu;
	view_menu->Append(ID_FULL,		_T("&Full screen"),		_T("Full screen on/off"));

	wxMenu *help_menu = new wxMenu;
	help_menu->Append(ID_ABOUT,		_("&About...\tCtrl-A"),	_("Show about dialog"));

	wxMenuBar *menu_bar = new wxMenuBar;
	menu_bar->Append(file_menu,	_T("&File"));
	menu_bar->Append(edit_menu,	_T("&Edit"));
	menu_bar->Append(view_menu,	_T("&View"));
	menu_bar->Append(help_menu,	_T("&Help"));

	//// Associate the menu bar with the frame
	this->SetMenuBar(menu_bar);
}

void CFloopyFrame::initViews()
{
	m_pSplitter = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(600, 400), wxSP_NOBORDER | wxSP_LIVE_UPDATE, _T("TrackView"));
	//m_pSplitter = new wxSplitterWindow(this, -1, wxPoint(0, 0), wxSize(600, 400), wxSP_3DSASH | wxSP_LIVE_UPDATE, _T("TrackView"));
	//m_pSplitter->SetBackgroundColour( *wxWHITE );
	//m_pSplitter->SetForegroundColour( *wxWHITE );

	m_pLabelsView = new CLabelsView(m_pSplitter, m_pTracks);
	m_pTracksView = new CTracksView(m_pSplitter, m_pLabelsView, m_pTracks);

    m_pSplitter->SplitVertically(m_pLabelsView, m_pTracksView, 100);

	m_pLabelsView->SetTracksView(m_pTracksView);

	m_pTracks->SetTracksView( m_pTracksView );
	m_pTracks->SetLabelsView( m_pLabelsView );

	m_pTracks->SetFrame( this );
}

void CFloopyFrame::OnPlay( wxCommandEvent &WXUNUSED(event) )
{
	m_pTracks->Play();
}

void CFloopyFrame::OnPause( wxCommandEvent &WXUNUSED(event) )
{
	m_pTracks->Pause();
}

void CFloopyFrame::OnStop( wxCommandEvent &WXUNUSED(event) )
{
	m_pTracks->Stop();
}
