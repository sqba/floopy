// Menu.cpp: implementation of the CMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"
#include <wx/listimpl.cpp>


#define MENU_ITEM_ID_START	33

WX_DEFINE_LIST(MenuItems);

BEGIN_EVENT_TABLE(CMenu, wxMenu)
	//EVT_MENU(MENU_ITEM_ID,  CMenu::OnMenuItem)
	//EVT_COMMAND_RANGE(100, MENU_ITEM_ID, EVT_MENU, CMenu::OnMenuItem)
	EVT_COMMAND_RANGE(MENU_ITEM_ID_START, MENU_ITEM_ID_START+100, wxEVT_COMMAND_MENU_SELECTED, CMenu::OnMenuItem)
	//EVT_COMMAND(MENU_ITEM_ID, wxEVT_COMMAND_MENU_SELECTED, CMenu::OnMenuItem)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenu::CMenu(IFloopyObj *parent)
{
	m_pParent = parent;
}

CMenu::~CMenu()
{
	WX_CLEAR_LIST(MenuItems, m_items);
}

void CMenu::Append(const wxString &item, void (*func)(IFloopyObj*))
{
	int id = MENU_ITEM_ID_START + m_items.GetCount();
	CMenuItem *menuitem = new CMenuItem(id, item, func);
	try {
		m_items.Append( menuitem );
		wxMenu::Append(id, item);
		//wxMenu::SetLabel(id, item);
	} catch(...) {
		//wxLogTrace(_T("CTrack"), _T("Append exception"));
	}
}

void CMenu::OnMenuItem( wxCommandEvent &event )
{
	int id = event.GetId();

	MenuItems::Node *node = m_items.GetFirst();
	while (node)
	{
		CMenuItem *menuitem = (CMenuItem*)node->GetData();
		if(id == menuitem->id) {
			if( menuitem->func )
				menuitem->func( m_pParent );
		}
		node = node->GetNext();
	}
}
