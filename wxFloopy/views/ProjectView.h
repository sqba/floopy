// ProjectView.h: interface for the CProjectView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTVIEW_H__F526EC88_56B8_4A16_AF62_3EF834814E32__INCLUDED_)
#define AFX_PROJECTVIEW_H__F526EC88_56B8_4A16_AF62_3EF834814E32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __GNUG__
// #pragma implementation
#endif

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

#include <wx/log.h>

#include "RulerView.h"

class CProjectView : public CView  
{
public:
	CProjectView(CRulerView* parent);
	virtual ~CProjectView();

};

#endif // !defined(AFX_PROJECTVIEW_H__F526EC88_56B8_4A16_AF62_3EF834814E32__INCLUDED_)
