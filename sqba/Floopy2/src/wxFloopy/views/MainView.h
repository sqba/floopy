// MainView.h: interface for the CMainView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINVIEW_H__A7C38C86_CD71_4FA6_A5F5_6F21967302A3__INCLUDED_)
#define AFX_MAINVIEW_H__A7C38C86_CD71_4FA6_A5F5_6F21967302A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include <wx/docview.h>
#include <wx/log.h>



class CMainView : public wxScrolledWindow  
{
public:
	CMainView(wxWindow* parent);
	virtual ~CMainView();

};

#endif // !defined(AFX_MAINVIEW_H__A7C38C86_CD71_4FA6_A5F5_6F21967302A3__INCLUDED_)
