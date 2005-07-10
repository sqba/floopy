// FloopyControl.h: interface for the CFloopyControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOOPYCONTROL_H__21635A3D_E95A_4DE3_82C6_2B124905E5C3__INCLUDED_)
#define AFX_FLOOPYCONTROL_H__21635A3D_E95A_4DE3_82C6_2B124905E5C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/wx.h>

#include "engine/FloopyObj.h"

class CFloopyControl : public IFloopyObj  
{
public:
	CFloopyControl(IFloopyObj *parent);
	virtual ~CFloopyControl();

	wxCursor GetCursor() { return wxCursor(wxCURSOR_HAND); }

	void DrawBG(wxDC& dc, wxRect& rc);
};

#endif // !defined(AFX_FLOOPYCONTROL_H__21635A3D_E95A_4DE3_82C6_2B124905E5C3__INCLUDED_)
