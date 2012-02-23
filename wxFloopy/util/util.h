
/*
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
*/

#include <wx/dc.h>

void FormatTime(float fSec, wxString &csTime);
void DrawAquaRect(wxDC& dc, wxRect& rc, int radius = 3);
void DrawRect3D(wxDC& dc, wxRect& rc);
