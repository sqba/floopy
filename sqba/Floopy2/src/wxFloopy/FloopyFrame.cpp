// For compilers that supports precompilation , includes "wx/wx.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "FloopyFrame.h"



enum // menu items
{
	ID_FULL  = 109,
	ID_OPEN,
	ID_SAVE,
	ID_SAVEAS,
	ID_CLOSE,
	ID_EXIT,
	ID_ABOUT,
	ID_PLAY,
	ID_PAUSE,
	ID_STOP,
	ID_PARAMS,
	ID_PROPS
};

//IMPLEMENT_DYNAMIC_CLASS(CFloopyFrame, wxFrame)

BEGIN_EVENT_TABLE(CFloopyFrame, wxFrame)
	EVT_MENU(ID_FULL,		CFloopyFrame::OnFullScreen)
	EVT_MENU(ID_OPEN,		CFloopyFrame::OnFileOpen)
	EVT_MENU(ID_SAVE,		CFloopyFrame::OnFileSave)
	EVT_MENU(ID_SAVEAS,		CFloopyFrame::OnFileSaveAs)
	EVT_MENU(ID_CLOSE,		CFloopyFrame::OnFileClose)
	EVT_MENU(ID_EXIT,		CFloopyFrame::OnQuit)
	EVT_MENU(ID_ABOUT,		CFloopyFrame::OnAbout)
	EVT_MENU(ID_PLAY,		CFloopyFrame::OnPlay)
	EVT_MENU(ID_PAUSE,		CFloopyFrame::OnPause)
	EVT_MENU(ID_STOP,		CFloopyFrame::OnStop)
	EVT_MENU(ID_PARAMS,		CFloopyFrame::OnShowParams)
	EVT_MENU(ID_PROPS,		CFloopyFrame::OnShowProperties)
END_EVENT_TABLE()


CFloopyFrame::CFloopyFrame() : wxFrame((wxFrame*)NULL,
									   -1,
									   _T("Floopy"),
									   wxPoint(-1, -1),
									   wxSize(640, 480))
{
#ifdef __WXMSW__
	this->SetIcon(wxICON(Floopy));
#endif
#ifdef __X__
	this->SetIcon(wxIcon(_T("doc.xbm")));
#endif

	m_pTracks = new CTracks();
	m_pTracks->SetFrame( this );

	m_pView = new CTracksView(this, m_pTracks);

	m_pDropTarget = new CDropTarget( this );
	SetDropTarget( m_pDropTarget );

	initMenus();
	initToolbar();

	wxFrame::Centre();
}

CFloopyFrame::~CFloopyFrame()
{
	delete m_pView;
	delete m_pTracks;
}

void CFloopyFrame::OnFullScreen( wxCommandEvent &WXUNUSED(event) )
{
   wxFrame::ShowFullScreen( !IsFullScreen(), wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION );
}

void CFloopyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
	if( Close() )
		wxFrame::Close( true );
}

void CFloopyFrame::OnFileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this, "Open", "", "",
		"XML Files(*.xml)|*.xml|Wav files(*.wav)|*.wav|All files(*.*)|*.*",
		wxOPEN, wxDefaultPosition);
	if ( dlg->ShowModal() == wxID_OK )
	{
		SetStatusText(dlg->GetFilename(), 0);
		char *filename = (char*)dlg->GetPath().c_str();
		Open(filename);
	}
	dlg->Destroy();
}

void CFloopyFrame::OnFileClose(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void CFloopyFrame::OnFileSave(wxCommandEvent& WXUNUSED(event))
{
	Save();
}

void CFloopyFrame::OnFileSaveAs(wxCommandEvent& WXUNUSED(event))
{
	SaveAs();
}

void CFloopyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    // called when help - about is picked from the menu or toolbar
    //wxString msg;
    //msg.Printf(_("About Floopy...\n"));
    //wxMessageBox(msg, _("About Floopy"), wxOK | wxICON_INFORMATION, this);
    (void)wxMessageBox(_T("Floopy\nAuthor: Filip Pavlovic"), _T("About Floopy"));
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

void CFloopyFrame::OnShowParams( wxCommandEvent &WXUNUSED(event) )
{
	m_pView->ShowParamsDlg();
}

void CFloopyFrame::OnShowProperties( wxCommandEvent &WXUNUSED(event) )
{
	m_pView->ShowPropertiesDlg();
}

void CFloopyFrame::initMenus()
{
	//// Make a menubar
	wxMenu *file_menu = new wxMenu;

	file_menu->Append(ID_CLOSE,		_T("&New..."),			_T("New project"));
	file_menu->Append(ID_OPEN,		_T("&Open..."),			_T("Open project file"));
	file_menu->Append(ID_CLOSE,		_T("&Close"),			_T("Close current project"));
	file_menu->Append(ID_SAVE,		_T("&Save"),			_T("Save current project"));
	file_menu->Append(ID_SAVEAS,	_T("Save &As..."),		_T("Save current project with a different name"));
	file_menu->AppendSeparator();
	file_menu->Append(ID_EXIT,		_T("E&xit"),			_T("Exit Floopy!"));

	wxMenu *edit_menu = new wxMenu;
	edit_menu->Append(wxID_UNDO,	_T("&Undo"),			_T("Undo last action"));
	edit_menu->Append(wxID_REDO,	_T("&Redo"),			_T("Redo last action"));
	edit_menu->AppendSeparator();

	wxMenu *view_menu = new wxMenu;
	view_menu->Append(ID_FULL,		_T("&Full screen"),		_T("Full screen on/off"));
	view_menu->Append(ID_PARAMS,	_T("Show &parameters..."),	_T("Show parameters dialog"));
	view_menu->Append(ID_PROPS,		_T("Show pr&operties..."),	_T("Show properties dialog"));

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

void CFloopyFrame::initToolbar()
{
	wxToolBar *toolbar = CreateToolBar();

	//wxBitmap aboutImage("res/help.bmp", wxBITMAP_TYPE_BMP);

	wxBitmap aboutImage("CSQUERY",	wxBITMAP_TYPE_RESOURCE);
	wxBitmap playImage( "PLAY",		wxBITMAP_TYPE_RESOURCE);
	wxBitmap pauseImage("PAUSE",	wxBITMAP_TYPE_RESOURCE);
	wxBitmap stopImage( "STOP",		wxBITMAP_TYPE_RESOURCE);

	toolbar->AddTool(ID_ABOUT,	_("About"),	aboutImage,	_("About Floopy"));
	toolbar->AddTool(ID_PLAY,	_("Play"),	playImage,	_("Play"));
	toolbar->AddTool(ID_PAUSE,	_("Pause"),	pauseImage,	_("Pause"));
	toolbar->AddTool(ID_STOP,	_("Stop"),	stopImage,	_("Stop"));

	toolbar->Realize();
}

void CFloopyFrame::Open(char *filename)
{
	m_pView->Open(filename);
}

bool CFloopyFrame::Save()
{
	return m_pView->Save();
}

void CFloopyFrame::SaveAs()
{
	wxFileDialog *dlg = new wxFileDialog(this, "Save", "", "",
		"XML Files(*.xml)|*.xml|Wav files(*.wav)|*.wav|All files(*.*)|*.*",
		wxSAVE, wxDefaultPosition);
	if ( dlg->ShowModal() == wxID_OK )
	{
		SetStatusText(dlg->GetFilename(), 0);
		char *filename = (char*)dlg->GetPath().c_str();
		m_pView->Open(filename);
	}
	dlg->Destroy();
	//delete dlg;
}

bool CFloopyFrame::Close()
{
	bool bResult = true;
	if(m_pTracks->IsChanged())
	{
		int answer = wxMessageBox(_T("File changed, save now?"), _T("Floopy!"),
			wxICON_QUESTION|wxYES_NO|wxCANCEL);

		switch(answer)
		{
		case wxYES:
			if(!Save())
				SaveAs();
		case wxNO:
			m_pTracks->Clear();
			break;
		case wxCANCEL:
			bResult =  false;
			break;
		}
	}
	else
		m_pTracks->Clear();
	return bResult;
}
