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
	CFloopyFrame *frame = new CFloopyFrame();

	frame->CreateStatusBar(4);//, wxST_SIZEGRIP);  // XP?
	frame->SetStatusText("Floopy");

	if(argc > 1)
	{
		for(int i=1; i<argc; i++)
			frame->Open(argv[i]);
	}

	frame->Show(true);

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
