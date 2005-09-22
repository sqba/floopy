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
		CSliderCtrl(CControlDlg*, IFloopy*, int, float, float, float, wxStaticText*);
		virtual ~CSliderCtrl();

		void OnScroll(wxScrollEvent &evt);

	private:
		int m_index;
		wxStaticText *m_pLabel;
		IFloopy *m_pObj;

	private:
		DECLARE_EVENT_TABLE()
	};

public:
	CControlDlg();
	virtual ~CControlDlg();

	void InitParams(CTrack *track, IFloopy *obj);
	void InitProps(CTrack *track, IFloopy *obj);

	bool Show(const bool show);

	void OnClose(wxCloseEvent &evt);

	void SetLabelValue(int index, float value);

	void Clear();

private:
	wxFlexGridSizer	*m_pSizer;
	int m_count;

private:
    DECLARE_EVENT_TABLE()
};

#endif // !defined(AFX_MIXERDLG_H__D335C71B_F743_4617_BB22_2E0AB290D90E__INCLUDED_)
