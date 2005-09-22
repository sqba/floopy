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


CControlDlg::CControlDlg(CTracks *pTracks)
 : wxDialog(NULL, -1, "Mixer", wxDefaultPosition, wxSize(200, 250),
	wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxSTAY_ON_TOP)
{
	m_count		= 0;
	m_pObject	= NULL;
	m_bProperty	= true;
	m_pTracks	= pTracks;

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
		assert( m_pSizer->Remove(index) );
	}
	m_pSizer->Layout();
	m_count = 0;
	//m_pSizer->RecalcSizes();
	//wxDialog::Refresh();
}

void CControlDlg::InitParams(IFloopyObj *obj)
{
	init(obj, false);
}

void CControlDlg::InitProps(IFloopyObj *obj)
{
	init(obj, true);
}

void CControlDlg::init(IFloopyObj *obj, bool property)
{
	//wxDialog::SetTitle(wxString(obj->GetDisplayName()));
	//wxDialog::SetTitle(wxString(obj->GetDescription()));
	wxDialog::SetTitle(property ? "Properties" : "Parameters");

#ifdef __WXMSW__
	wxDialog::SetIcon(wxICON(Floopy));
#endif
#ifdef __X__
	wxDialog::SetIcon(wxIcon(_T("doc.xbm")));
#endif

	if(NULL == obj)
		return;

	m_pObject	= obj;
	m_bProperty	= property;

	IFloopy *input = obj->GetInput();
	if(NULL == input)
		return;

	Clear();

	load( input );
}

void CControlDlg::load(IFloopy *obj)
{
	int count = m_bProperty ? obj->GetPropertyCount() : obj->GetParamCount();

	for(int index=0; index<count; index++)
	{
		wxString txt(m_bProperty ? obj->GetPropertyName(index) : obj->GetParamName(index));

		wxStaticText	*name	= new wxStaticText(this, -1, txt);
		wxStaticText	*value	= new wxStaticText(this, -1, "value");
		wxSlider		*slider	= new CSliderCtrl(this, obj, index, m_bProperty, value);

		m_pSizer->Add( name,	0, wxALIGN_RIGHT );
		m_pSizer->Add( slider,	1, wxEXPAND|wxALL );
		m_pSizer->Add( value,	0, wxALIGN_LEFT );

		m_pSizer->AddGrowableRow(index, 1);

		//m_count += 3;
		m_count++;
	}

	int type = obj->GetType();
	if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
	{
		IFloopy *src = ((IFloopySoundFilter*)obj)->GetSource();
		if(NULL != src)
			load(src);
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

CTracks *CControlDlg::GetTracks()
{
	return m_pTracks;

	/*IFloopyObj *obj = m_pObject;
	while(obj)
	{
		if( obj->IsKindOf(CLASSINFO(CTracks)) )
			return (CTracks*)obj;
		obj = obj->GetParent();
	}
	return NULL;*/
}

CTrack *CControlDlg::GetTrack()
{
	IFloopyObj *obj = m_pObject;
	while(obj)
	{
		if( obj->IsKindOf(CLASSINFO(CTrack)) )
			return (CTrack*)obj;
		obj = obj->GetParent();
	}
	return NULL;
}

void CControlDlg::Update()
{
	if(NULL!=m_pObject)
		init(m_pObject, m_bProperty);
}





CControlDlg::CSliderCtrl::CSliderCtrl(CControlDlg* parent,
									  IFloopy *input,
									  int index,
									  bool property,
									  wxStaticText* label)
 : wxSlider(parent, -1, 0, 0, 0, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL)
{
	m_index		= index;
	m_pLabel	= label;
	m_pInput	= input;
	m_bProperty	= property;

	m_Unit = property ? input->GetPropertyUnit(index) : input->GetParamUnit(index);

	init();
}

CControlDlg::CSliderCtrl::~CSliderCtrl()
{

}

void CControlDlg::CSliderCtrl::OnScroll(wxScrollEvent &evt)
{
	float value = (float)evt.GetPosition();

	CTracks *tracks = ((CControlDlg*)GetParent())->GetTracks();
	IFloopyObj *obj = ((CControlDlg*)GetParent())->GetObject();

	if(m_bProperty)
	{
		CTrack *track = ((CControlDlg*)GetParent())->GetTrack();
		m_pInput->SetPropertyVal(m_index, value);
		if(NULL!=tracks && NULL!=track)
			tracks->RefreshTracks(track);
	}
	else
	{
		int sample = 0;
		//if(NULL != tracks)
		//	sample = tracks->GetCaretPos();
		sample = obj->GetCaretPos();
		if( obj->IsKindOf(CLASSINFO(CRegion)) )
		{
			CRegion *region = (CRegion*)obj;
			float val = 0.f;
			int end = region->GetEndOffset();
			//IFloopy *input = region->GetInput();
			m_pInput->GetParamAt(end, m_index, &val);
			m_pInput->SetParamAt(sample, m_index, value);
			m_pInput->SetParamAt(end, m_index, val);
		}
		else
			m_pInput->SetParamAt(sample, m_index, value);
	}

	wxString txt;
	txt.Printf("%.4f %s", value, m_Unit);
	m_pLabel->SetLabel( txt );

	obj->Invalidate();
	obj->Refresh();
}

void CControlDlg::CSliderCtrl::init()
{
	float min = 0.f;
	float max = 0.f;
	float val = 0.f;

	if(m_bProperty)
	{
		m_pInput->GetPropertyVal(m_index, &val);
		min = m_pInput->GetPropertyMin(m_index);
		max = m_pInput->GetPropertyMax(m_index);
	}
	else
	{
		//CTracks *tracks = ((CControlDlg*)GetParent())->GetTracks();
		int sample = 0;
		//if(NULL != tracks)
		//	sample = tracks->GetCaretPos();
		IFloopyObj *obj = ((CControlDlg*)GetParent())->GetObject();
		sample = obj->GetCaretPos();
		m_pInput->GetParamAt(sample, m_index, &val);
		min = m_pInput->GetParamMin(m_index);
		max = m_pInput->GetParamMax(m_index);
	}

	wxSlider::SetRange(min, max);
	wxSlider::SetValue(val);

	wxString txt;
	txt.Printf("%.4f %s", val, m_Unit);
	m_pLabel->SetLabel( txt );
}

void CControlDlg::CSliderCtrl::Update()
{
	init();
}
