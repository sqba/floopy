#ifndef _FLOOPYOBJ_H_
#define _FLOOPYOBJ_H_

#include <wx/menu.h>
#include <wx/list.h>


class IFloopyObj;


class CMenuItem
{
public:
	CMenuItem(int id, const wxString &item, void (*func)(IFloopyObj*))
	{
		this->id = id;
		this->item = item;
		this->func = func;
	}

	int id;
	wxString item;
	void (*func)(IFloopyObj*);
};

WX_DECLARE_LIST(CMenuItem, MenuItems);

class CMenu : public wxMenu  
{
public:
	CMenu(IFloopyObj *parent);
	virtual ~CMenu();

	void Append(const wxString& item, void (*func)(IFloopyObj*));
    void OnMenuItem(wxCommandEvent &event);

private:
	IFloopyObj *m_pParent;
	MenuItems m_items;
    DECLARE_EVENT_TABLE()
};




//////////////////////////////////////////////////////////////////////
// IFloopyObj interface
//////////////////////////////////////////////////////////////////////
class IFloopyObj : public wxObject
{
public:
	IFloopyObj(IFloopyObj *parent = NULL)
	{
		m_bSelected = false;
		m_pParent = parent;
		m_pMenu = NULL;
		m_ptPrev.x = m_ptPrev.y = 0;
	}

	virtual ~IFloopyObj()
	{
		if(m_pMenu)
			delete m_pMenu;
	}

	virtual bool GetName(wxString& WXUNUSED(name))			{ return false; };
	virtual bool GetDescription(wxString& WXUNUSED(desc))	{ return false; };

	//////////////////////////////////////////////////////////////////
	// IFloopyObj::Update
	// Called when some of the parameters have been changed
	//////////////////////////////////////////////////////////////////
	//virtual void Update()								{}

	virtual void Select(bool selected=true)	{ m_bSelected = selected; }
	virtual bool IsSelected()				{ return m_bSelected; }

	virtual IFloopyObj *GetParent()			{ return m_pParent; }


	virtual void DrawBG  (wxDC& WXUNUSED(dc), wxRect& WXUNUSED(rc)) {};
	virtual void DrawFore(wxDC& WXUNUSED(dc), wxRect& WXUNUSED(rc)) {};

	virtual void Refresh() {}
	virtual void Invalidate() {}

	virtual IFloopyObj *GetChildAt(int WXUNUSED(x), int WXUNUSED(y)) { return NULL; }

	virtual wxCursor GetCursor()	{ return *wxSTANDARD_CURSOR; }
	virtual wxMenu *GetMenu()		{ return m_pMenu; }

	wxColour GetColor()				{ return m_color; }
	void SetColor(wxColour color)	{ m_color = color; }

	virtual void Move(int WXUNUSED(dx), int WXUNUSED(dy)) {}

	virtual IFloopyObj *GetSelectedObj() { return NULL; }

	virtual bool OnKeyDown(wxKeyEvent& event) { return false; }
	virtual void OnMouseEvent(wxMouseEvent& event)
	{
		m_ptPrev.x = event.GetX();
		m_ptPrev.y = event.GetY();
	}

protected:
	IFloopyObj	*m_pParent;
	CMenu		*m_pMenu;
	bool		m_bSelected;
	wxColour	m_color;
	wxPoint		m_ptPrev;
};


#endif // _FLOOPYOBJ_H_
