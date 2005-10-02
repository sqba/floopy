#ifndef _FLOOPYOBJ_H_
#define _FLOOPYOBJ_H_

#include <wx/menu.h>
#include <wx/list.h>

#include "../../../ifloopy.h"


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


#define FLOOPY_OBJ	0

/*********************************************************************
 *! \class IFloopyObj
 *  \brief Interface implemented by all GUI objects.
 *  \author Filip Pavlovic
 *  \version 0.0
 *********************************************************************/
class IFloopyObj : public IFloopy
{
public:
	IFloopyObj(IFloopyObj *parent = NULL)
	{
		m_bSelected	= false;
		m_pParent	= parent;
		m_pMenu		= NULL;
		m_ptPrev.x	= m_ptPrev.y = 0;
		m_bChanged	= false;
		m_iWidth	= m_iHeight = 0;
	}

	virtual ~IFloopyObj()
	{
		if(m_pMenu)
			delete m_pMenu;
	}

	virtual int GetType()							{ return FLOOPY_OBJ; }

	virtual bool GetName(wxString &str)				{ return false; }
	virtual bool GetDescription(wxString &str)		{ return false; }

	virtual void Select(bool select=true)			{ m_bSelected = select; }
	virtual bool IsSelected()						{ return m_bSelected; }

	virtual wxCursor GetCursor()					{ return *wxSTANDARD_CURSOR; }
	virtual wxMenu *GetMenu()						{ return m_pMenu; }

	wxColor GetColor()								{ return m_color; }
	void SetColor(wxColor color)					{ m_color = color; }

	virtual int GetCaretPos()						{ return 0; }

	virtual IFloopyObj *GetParent()					{ return m_pParent; }
	virtual IFloopyObj *GetChildAt(int x, int y)	{ return NULL; }
	virtual int GetChildCount()						{ return 0; }
	virtual IFloopyObj *GetChild(int index)			{ return NULL; }

	virtual IFloopySoundInput *GetInput()			{ return NULL; }

	virtual void DrawBG  (wxDC &dc, wxRect &rc)		{};
	virtual void DrawFore(wxDC &dc, wxRect &rc)		{};

	virtual void Refresh()							{}
	virtual void Invalidate()						{}

	virtual void Move(int dx, int dy)				{}

	virtual IFloopyObj *GetSelectedObj()			{ return NULL; }

	virtual bool OnKeyDown(wxKeyEvent& event)		{ return false; }
	virtual void OnMouseEvent(wxMouseEvent& event)
	{
		m_ptPrev.x = event.GetX();
		m_ptPrev.y = event.GetY();
	}

	virtual void SetChanged(bool bChanged)			{ m_bChanged = bChanged; }
	virtual bool IsChanged()						{ return m_bChanged; }

	virtual int GetWidth()							{ return m_iWidth; }
	virtual void SetWidth(int width)				{ m_iWidth = width; }
	virtual int GetHeight()							{ return m_iHeight; }
	virtual void SetHeight(int height)				{ m_iHeight = height; }

protected:
	IFloopyObj	*m_pParent;
	CMenu		*m_pMenu;
	bool		m_bSelected;
	wxColor		m_color;
	wxPoint		m_ptPrev;
	bool		m_bChanged;
	int			m_iWidth;
	int			m_iHeight;
};


#endif // _FLOOPYOBJ_H_
