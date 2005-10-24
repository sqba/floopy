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
	int x = rc.GetX();
	int y = rc.GetY();
	int w = rc.GetWidth();
	int h = rc.GetHeight();
	
	const int dist = 2;

	wxPoint points[6];

	points[0] = wxPoint(x,				y);
	points[1] = wxPoint(x+w-dist,		y);
	points[2] = wxPoint(x+w+w/3-dist,	y+h/2);
	points[3] = wxPoint(x+w-dist,		y+h);
	points[4] = wxPoint(x,				y+h);
	points[5] = wxPoint(x+w/3,			y+h/2);

	dc.DrawPolygon(6, points);
}

void CPathItem::DrawFore(wxDC &dc, wxRect &rc)
{
	wxString csName;
	char *name = m_pinput->GetName();
	char *dname = m_pinput->GetDisplayName();
	csName = strlen(name) > strlen(dname) ? dname : name;

	int width = rc.GetWidth() - rc.GetWidth()/3;

	int w=0, h=0;
	dc.GetTextExtent(csName, &w, &h);
	if(w<width && h<rc.GetHeight())
	{
		int x = rc.GetX() + rc.GetWidth()/3 + width/2 - w/2;
		int y = rc.GetY() + rc.GetHeight()/2 - h/2;
		dc.DrawText( csName, x, y );
	}
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

	ItemList::Node *prev = NULL;
	IFloopySoundInput *tmp = input;
	while(tmp)
	{
		CPathItem *item = new CPathItem(tmp);
//		if(prev)
//			prev = m_PathList.Insert(prev, item);
//		else
//			prev = m_PathList.Append(item);

		m_PathList.Insert((int)0, item);
		
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
	int count = m_PathList.GetCount();
	if(count > 0)
	{
		wxRect rcItem = rc;
		int width = rc.GetWidth() / count;
		width = (rc.GetWidth()-width/3) / count;
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
	int count = m_PathList.GetCount();
	if(count > 0)
	{
		wxRect rcItem = rc;
		int width = rc.GetWidth() / count;
		width = (rc.GetWidth()-width/3) / count;
		rcItem.SetWidth(width);

		ItemList::Node *node = m_PathList.GetFirst();
		while (node)
		{
			CPathItem *item = (CPathItem*)node->GetData();
			item->DrawFore(dc, rcItem);
			rcItem.Offset(width, 0);
			node = node->GetNext();
		}
	}
}
