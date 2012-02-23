#ifndef _FLOOPYAPP_H
#define _FLOOPYAPP_H

#include <wx/wx.h>

class CFloopyApp : public wxApp
{
public:
	bool OnInit();
    int OnExit(void);
};

DECLARE_APP(CFloopyApp)

#endif _FLOOPYAPP_H
