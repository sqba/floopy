
#include "parameter.h"
#include "tracks.h"
#include "track.h"
#include "region.h"


#include <wx/wx.h>

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ParameterList);


CParameters::CParameters(IFloopyObj *parent) : IFloopyObj(parent)
{

}

CParameters::~CParameters()
{
	WX_CLEAR_LIST(ParameterList, m_Parameters);
}

void CParameters::LoadInput(IFloopySoundInput *obj)
{
	if(NULL == obj)
		return;

	// Check if object exists
	if( paramsLoaded(obj) )
		return;

	bool bAfterTrack = isAfterTrack(obj);

//	WX_CLEAR_LIST(ParameterList, m_Parameters);

	for(int index=0; index<obj->GetParamCount(); index++)
	{
		m_Parameters.Append( new CParameter(this, obj, index, bAfterTrack) );
	}
}

void CParameters::RemoveInput(IFloopySoundInput *obj)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->GetInput()==obj)
		{
			ParameterList::Node *tmp = node->GetNext();
			if(m_Parameters.DeleteNode(node))
			{
				delete param;
				node = tmp;
			}
			else
				node = node->GetNext();
		}
		else
			node = node->GetNext();
	}
}

bool CParameters::paramsLoaded(IFloopySoundInput *obj)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->GetInput()==obj)
			return true;
		node = node->GetNext();
	}
	return false;
}

IFloopyObj *CParameters::GetChildAt(int x, int y)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		//if(param->HitTest(x, y))
		//	return param->GetChildAt(x, y);
		IFloopyObj *obj = param->GetChildAt(x, y);
		if(obj)
			return obj;
		node = node->GetNext();
	}
	return NULL;
}

void CParameters::DrawFore(wxDC& dc, wxRect& rc)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		param->DrawFore(dc, rc);
		node = node->GetNext();
	}
}

void CParameters::DeselectAll()
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param)
			param->Select(false);
		node = node->GetNext();
	}
}

bool CParameters::isAfterTrack(IFloopySoundInput *obj)
{
	bool bAfterTrack = false; // Is obj source of track?
	CRegion *region = (CRegion*)GetParent();
	CTrack *track = (CTrack*)region->GetParent();
	IFloopySoundInput *input = track->GetTrack();
	IFloopySoundInput *tmp = input;

	while(tmp)
	{
		if(obj == tmp)
			return true;

		int type = tmp->GetType();
		if(type == (TYPE_FLOOPY_SOUND_FILTER | type))
			tmp = ((IFloopySoundFilter*)tmp)->GetSource();
		else
			tmp = NULL;
	}

	return false;
}

IFloopyObj *CParameters::GetSelectedObj()
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->IsSelected())
			return param;
		node = node->GetNext();
	}
	return NULL;
}

void CParameters::DeselectAll(CParameter *caller)
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param!=caller)
			param->Select(false);
		node = node->GetNext();
	}
}

wxCursor CParameters::GetCursor()
{
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->IsSelected())
			param->Select(false);
		node = node->GetNext();
	}

	return wxCursor(wxCURSOR_HAND);
}

bool CParameters::OnMouseEvent(wxMouseEvent& event)
{
	bool bResult = false;
	ParameterList::Node *node = m_Parameters.GetFirst();
	while (node)
	{
		CParameter *param = (CParameter*)node->GetData();
		if(param && param->IsSelected())
		{
			param->OnMouseEvent(event);
			bResult = true;
			break;
		}
		node = node->GetNext();
	}
	IFloopyObj::OnMouseEvent(event);
	return bResult;
}
