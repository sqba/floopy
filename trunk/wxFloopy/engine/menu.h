// Menu.h: interface for the CMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENU_H__9AF6C19A_CB65_49E0_BE72_1511B7C0CAEF__INCLUDED_)
#define AFX_MENU_H__9AF6C19A_CB65_49E0_BE72_1511B7C0CAEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/menu.h>
#include <wx/list.h>

//typedef void (*funcType)(void);

class CMenuItem
{
public:
	CMenuItem(int id, const wxString &item, void (*func)(void))
	{
		this->id = id;
		this->item = item;
		this->func = func;
	}

	int id;
	wxString item;
	void (*func)(void);
};

WX_DECLARE_LIST(CMenuItem, MenuItems);

class CMenu : public wxMenu  
{
public:
	CMenu();
	virtual ~CMenu();

	void Append(const wxString& item, void (*func)(void));
    void OnMenuItem(wxCommandEvent &event);

private:
	MenuItems m_items;
    DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_MENU_H__9AF6C19A_CB65_49E0_BE72_1511B7C0CAEF__INCLUDED_)
