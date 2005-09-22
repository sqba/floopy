// FloopyDlg.h: interface for the CMixerDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXERDLG_H__D335C71B_F743_4617_BB22_2E0AB290D90E__INCLUDED_)
#define AFX_MIXERDLG_H__D335C71B_F743_4617_BB22_2E0AB290D90E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/slider.h>

#include "../engine/tracks.h"


//WX_DECLARE_OBJARRAY(wxStaticText, LabelArray);

class CControlDlg : public wxDialog  
{
private:
	class CSliderCtrl : public wxSlider  
	{
	public:
		CSliderCtrl(CControlDlg*, IFloopy*, int, bool, wxStaticText*);
		virtual ~CSliderCtrl();

		void OnScroll(wxScrollEvent &evt);
	
		void Update();

	private:
		void setValue();

	private:
		int				m_index;
		bool			m_bProperty;
		wxStaticText	*m_pLabel;
		IFloopy			*m_pInput;

	private:
		DECLARE_EVENT_TABLE()
	};

public:
	CControlDlg(CTracks *pTracks);
	virtual ~CControlDlg();

	void InitParams(IFloopyObj *obj);
	void InitProps(IFloopyObj *obj);

	bool Show(const bool show);

	void OnClose(wxCloseEvent &evt);

	void SetLabelValue(int index, float value);

	void Clear();
	void Update();

	IFloopyObj *GetObject()	{ return m_pObject; }
	CTracks *GetTracks();
	CTrack *GetTrack();

private:
	void init(IFloopyObj *obj, bool property);
	void load(IFloopy *obj);

private:
	wxFlexGridSizer	*m_pSizer;
	int				m_count;
	IFloopyObj		*m_pObject;
	bool			m_bProperty;
	CTracks			*m_pTracks;

private:
    DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_MIXERDLG_H__D335C71B_F743_4617_BB22_2E0AB290D90E__INCLUDED_)
