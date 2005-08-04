// For compilers that supports precompilation , includes "wx/wx.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "FloopyApp.h"
#include "FloopyFrame.h"


IMPLEMENT_APP(CFloopyApp)


bool CFloopyApp::OnInit()
{
	CFloopyFrame *frame = new CFloopyFrame("Floopy", 100, 100, 640, 480);

	frame->CreateStatusBar(3, wxST_SIZEGRIP);
	frame->SetStatusText("Floopy");

	if(argc > 1)
		frame->Open(argv[1]);

	frame->Show(TRUE);

	SetTopWindow(frame);

	//// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
	//pFrame->SetIcon(wxICON(Floopy));
	frame->SetIcon(wxICON(Floopy));
#endif
#ifdef __X__
	frame->SetIcon(wxIcon(_T("doc.xbm")));
#endif

	return true;
}

int CFloopyApp::OnExit(void)
{
//	delete m_docManager;
	return 0;
}
