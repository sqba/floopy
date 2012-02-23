
/*
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
*/

#include <wx/dc.h>

#ifndef _UTIL_H_
#define _UTIL_H_

void FormatTime(float fSec, wxString &csTime);
void DrawAquaRect(wxDC& dc, wxRect& rc, int radius = 3);
void DrawRect3D(wxDC& dc, wxRect& rc);

#endif // _UTIL_H_
