// PathCtrl.cpp: implementation of the CPathCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "PathCtrl.h"

#include <wx/wx.h>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ItemList);




CPathItem::CPathItem(IFloopySoundInput *input)
{
	m_pinput = input;
}

CPathItem::~CPathItem()
{

}

void CPathItem::DrawBG(wxDC &dc, wxRect &rc)
{
	dc.DrawRectangle(rc.GetX(), rc.GetY(), rc.GetWidth(), rc.GetHeight());
}

void CPathItem::DrawFore(wxDC &dc, wxRect &rc)
{

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPathCtrl::CPathCtrl(IFloopySoundInput *input)
{
	SetPath(input);
}

CPathCtrl::~CPathCtrl()
{
	Clear();
}

void CPathCtrl::SetPath(IFloopySoundInput *input)
{
	Clear();

	IFloopySoundInput *tmp = input;
	while(tmp)
	{
		CPathItem *item = new CPathItem(tmp);
		m_PathList.Append(item);
		
		int type = tmp->GetType();
		if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
			tmp = ((IFloopySoundFilter*)tmp)->GetSource();
		else
			tmp = NULL;
	}
}

void CPathCtrl::Clear()
{
	/*ItemList::Node *node = m_PathList.GetFirst();
	while (node)
	{
		CPathItem *item = (CPathItem*)node->GetData();
		node = node->GetNext();
		delete item;
	}*/
	WX_CLEAR_LIST(ItemList, m_PathList);
}

void CPathCtrl::DrawBG(wxDC &dc, wxRect &rc)
{
	wxRect rcItem = rc;
	int count = m_PathList.GetCount();
	if(count > 0)
	{
		int width = rc.GetWidth() / count;
		rcItem.SetWidth(width);

		ItemList::Node *node = m_PathList.GetFirst();
		while (node)
		{
			CPathItem *item = (CPathItem*)node->GetData();
			item->DrawBG(dc, rcItem);
			rcItem.Offset(width, 0);
			node = node->GetNext();
		}
	}
}

void CPathCtrl::DrawFore(wxDC &dc, wxRect &rc)
{

}
