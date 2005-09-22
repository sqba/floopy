// MixerDlg.cpp: implementation of the CControlDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "ControlDlg.h"


//#include <wx/arrimpl.cpp>
//WX_DEFINE_OBJARRAY(LabelArray);


BEGIN_EVENT_TABLE( CControlDlg, wxDialog )
	EVT_CLOSE( CControlDlg::OnClose )
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CControlDlg::CSliderCtrl, wxSlider)
	EVT_SCROLL(CControlDlg::CSliderCtrl::OnScroll)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


static bool property;
CTrack *pTrack;

//CControlDlg::CControlDlg(CTracks *pTracks)
CControlDlg::CControlDlg()
 : wxDialog(NULL, -1, "Mixer", wxDefaultPosition, wxSize(200, 250),
	wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxSTAY_ON_TOP)
{
//	m_pTracks = pTracks;

	m_count = 0;

	m_pSizer = new wxFlexGridSizer( 3 );

	m_pSizer->AddGrowableCol(1, 1);

	SetSizer( m_pSizer );
}

CControlDlg::~CControlDlg()
{

}

void CControlDlg::Clear()
{
	/*if(m_pSizer)
	{
		delete m_pSizer;

		m_pSizer = new wxFlexGridSizer( 3 );

		m_pSizer->AddGrowableCol(1, 1);

		SetSizer( m_pSizer );
	}*/

	int index = 0;
	for(index=0; index<m_count; index++)
	{
		m_pSizer->Remove(index);
	}
	m_count = 0;
}

void CControlDlg::InitParams(CTrack *track, IFloopy *obj)
{
	wxDialog::SetTitle(wxString(obj->GetDisplayName()));

	property = false;
	pTrack = track;

	/*IFloopySoundInput *engine = m_pTracks->GetInput();
	IFloopySoundMixer *mixer = (IFloopySoundMixer*)CTracks::GetComponent(engine, "mixer");

	if(NULL == mixer)
		return;

	int count = mixer->GetInputCount();
	
	for(int i=0; i<count; i++)
	{
		IFloopy *volume = CTracks::GetComponent(mixer->GetSource(i), "volume");
		if(NULL != volume)
		{
			wxSlider *slider = new wxSlider(this, -1, 0, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
			m_pSizer->Add( slider );
			//m_pSizer->Fit( slider );
			m_pSizer->Layout();
		}
	}*/


	Clear();

	float val = 0.f;

	for(int index=0; index<obj->GetParamCount(); index++)
	{
		wxString txt(obj->GetParamName(index));
		obj->GetParamVal(index, &val);
		float min = obj->GetParamMin(index);
		float max = obj->GetParamMax(index);

		wxStaticText	*name	= new wxStaticText(this, -1, txt);
		wxStaticText	*value	= new wxStaticText(this, -1, "value");
		wxSlider		*slider	= new CSliderCtrl(this, obj, index, min, max, val, value);

		m_pSizer->Add( name,	0, wxALIGN_RIGHT );
		m_pSizer->Add( slider,	1, wxEXPAND|wxALL );
		m_pSizer->Add( value,	0, wxALIGN_LEFT );

		m_pSizer->AddGrowableRow(index, 1);

		m_count += 3;
	}
}

void CControlDlg::InitProps(CTrack *track, IFloopy *obj)
{
	wxDialog::SetTitle(wxString(obj->GetDisplayName()));

	property = true;
	pTrack = track;

	Clear();

	float val = 0.f;

	for(int index=0; index<obj->GetPropertyCount(); index++)
	{
		wxString txt(obj->GetPropertyName(index));
		obj->GetParamVal(index, &val);
		float min = obj->GetPropertyMin(index);
		float max = obj->GetPropertyMax(index);

		wxStaticText	*name	= new wxStaticText(this, -1, txt);
		wxStaticText	*value	= new wxStaticText(this, -1, "value");
		wxSlider		*slider	= new CSliderCtrl(this, obj, index, min, max, val, value);

		m_pSizer->Add( name,	0, wxALIGN_RIGHT );
		m_pSizer->Add( slider,	1, wxEXPAND|wxALL );
		m_pSizer->Add( value,	0, wxALIGN_LEFT );

		m_pSizer->AddGrowableRow(index, 1);

		m_count += 3;
	}
}

bool CControlDlg::Show(const bool show)
{
	return wxDialog::Show( show );
}

void CControlDlg::OnClose(wxCloseEvent &evt)
{
	Show( false );
}





CControlDlg::CSliderCtrl::CSliderCtrl(CControlDlg* parent,
									  IFloopy *obj,
									  int index,
									  float min,
									  float max,
									  float val,
									  wxStaticText* label)
 : wxSlider(parent, -1, val, min, max, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL)
{
	m_index = index;
	m_pLabel = label;
	m_pObj = obj;

	wxString txt;
	txt.Printf("%.4f", val);
	m_pLabel->SetLabel( txt );
}

CControlDlg::CSliderCtrl::~CSliderCtrl()
{

}

void CControlDlg::CSliderCtrl::OnScroll(wxScrollEvent &evt)
{
	int pos = evt.GetPosition();
	if(property)
		m_pObj->SetPropertyVal(m_index, (float)pos);
	else
		m_pObj->SetParamVal(m_index, (float)pos);
	wxString txt;
	txt.Printf("%.4f", (float)pos);
	m_pLabel->SetLabel( txt );

	pTrack->Invalidate();
	pTrack->Refresh();
}
