// PathCtrl.cpp: implementation of the CPathCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "PathCtrl.h"
#include "track.h"
#include "label.h"

#include <wx/wx.h>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ItemList);


#define GAP_LENGTH		2
#define CORNER_LENGTH	8


CPathItem::CPathItem(CPathCtrl *parent, IFloopySoundInput *input, bool first)
 : IFloopyObj(parent)
{
	m_pInput = input;
	m_bFirst = first;

	unsigned int r=255, g=255, b=255;
	input->GetColor(&r, &g, &b);
	m_color.Set(r, g, b);

	m_pRegion = NULL;
}

CPathItem::~CPathItem()
{
	if(m_pRegion)
		delete m_pRegion;
}

void CPathItem::DrawBG(wxDC &dc, wxRect &rc)
{
	wxPen oldpen = dc.GetPen();
	wxPen pen( *wxMEDIUM_GREY_PEN );
	pen.SetWidth(IsSelected() ? 2 : 1);
	dc.SetPen( pen );

	wxBrush oldBrush = dc.GetBrush();
	wxBrush brush(m_color, wxSOLID);
	dc.SetBrush(brush);

	int x = rc.GetX();
	int y = rc.GetY();
	int w = rc.GetWidth();
	int h = rc.GetHeight();

	wxPoint points[6];

	points[0] = wxPoint(x,								y);
	points[1] = wxPoint(x+w-GAP_LENGTH,					y);
	points[2] = wxPoint(x+w+CORNER_LENGTH-GAP_LENGTH,	y+h/2);
	points[3] = wxPoint(x+w-GAP_LENGTH,					y+h);
	points[4] = wxPoint(x,								y+h);
	points[5] = wxPoint(x+CORNER_LENGTH,				y+h/2);

	int n = m_bFirst ? 5 : 6;

	if(m_pRegion)
		delete m_pRegion;
	m_pRegion = new wxRegion(n, points);

	dc.DrawPolygon(n, points);

	dc.SetBrush( oldBrush );
	dc.SetPen(oldpen);
}

void CPathItem::DrawFore(wxDC &dc, wxRect &rc)
{
	dc.SetTextForeground(*wxBLACK);

	wxString csName;
	char *nname = m_pInput->GetName();
	char *dname = m_pInput->GetDisplayName();
	char *disp = strlen(nname) > strlen(dname) ? dname : nname;
	csName = disp;

	int width = rc.GetWidth();
	if(!m_bFirst)
		width -= CORNER_LENGTH;

	int w=0, h=0;
	dc.GetTextExtent(csName, &w, &h);
	if(h<rc.GetHeight())
	{
		if(w>width)
		{
			csName.Printf("%c\0", disp[0]);
			dc.GetTextExtent(csName, &w, &h);
		}
		int x = rc.GetX() + width/2 - w/2 + (m_bFirst ? 0 : CORNER_LENGTH);
		int y = rc.GetY() + rc.GetHeight()/2 - h/2;
		dc.DrawText( csName, x, y );
	}
}

void CPathItem::Select(bool selected)
{
	IFloopyObj::Select( selected );

	CPathCtrl *ctrl = (CPathCtrl*)GetParent();
	if(NULL != ctrl)
	{
		CLabel *label = (CLabel*)ctrl->GetParent();
		if(NULL != label)
		{
			CTrack *track = (CTrack*)label->GetParent();
			if(NULL != track)
			{
				track->ShowObjectParameters(m_pInput, selected);
				track->Refresh();
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPathCtrl::CPathCtrl(IFloopyObj parent, IFloopySoundInput *input)
 : IFloopyObj(parent)
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
		IFloopySoundInput *tmp2 = tmp;
		
		int type = tmp->GetType();
		if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
			tmp = ((IFloopySoundFilter*)tmp)->GetSource();
		else
			tmp = NULL;

		CPathItem *item = new CPathItem(this, tmp2, tmp==NULL);
		//m_PathList.Append(item);
		m_PathList.Insert((int)0, item);
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
	m_rc = rc;

	rc.Deflate(1, 2);
	rc.SetWidth(rc.GetWidth() - CORNER_LENGTH);

	int count = m_PathList.GetCount();
	if(count > 0)
	{
		wxRect rcItem = rc;
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
	rc.Deflate(0, 2);
	rc.SetWidth(rc.GetWidth() - CORNER_LENGTH);

	int count = m_PathList.GetCount();
	if(count > 0)
	{
		wxRect rcItem = rc;
		int width = rc.GetWidth() / count;
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

IFloopyObj *CPathCtrl::GetChildAt(int x, int y)
{
	ItemList::Node *node = m_PathList.GetFirst();
	while (node)
	{
		CPathItem *item = (CPathItem*)node->GetData();
		if(item->m_pRegion->Contains(x, y))
			return item;
		node = node->GetNext();
	}

	if(m_rc.Inside(x, y))
		return this;

	return NULL;
}

int CPathCtrl::GetChildCount()
{
	return m_PathList.GetCount();
}

IFloopyObj *CPathCtrl::GetChild(int index)
{
	ItemList::Node *node = m_PathList.Item(index);
	if(node)
		return (CPathItem*)node->GetData();
	return NULL;
}

void CPathCtrl::Move(int dx, int dy)
{
	IFloopyObj *obj = GetSelectedObj();
	if(obj)
		obj->Move(dx, dy);
}

IFloopyObj *CPathCtrl::GetSelectedObj()
{
	ItemList::Node *node = m_PathList.GetFirst();
	while (node)
	{
		CPathItem *item = (CPathItem*)node->GetData();
		if(item->IsSelected())
			return item;
		node = node->GetNext();
	}
	return NULL;
}
