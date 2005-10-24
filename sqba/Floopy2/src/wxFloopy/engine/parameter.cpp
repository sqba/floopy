// parameter.cpp: implementation of the CParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"
#include "track.h"
#include "region.h"
#include "parameter.h"

//IMPLEMENT_DYNAMIC_CLASS(CParameter, IFloopyObj)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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





CParameter::CParameter(CParameters *parent, IFloopySoundInput *obj, int index,
					   bool bAfterTrack, wxColor color)
 : IFloopyObj(parent)
{
	m_pRegion		= (CRegion*)parent->GetParent();
	m_pTrack		= (CTrack*)m_pRegion->GetParent();
	m_pTracks		= (CTracks*)m_pTrack->GetParent();
	m_index			= index;
	m_color			= color;
	m_pPoint		= new CPoint(this);
	m_bAfterTrack	= bAfterTrack;
	m_pInput		= obj;
	m_fMin			= m_pInput->GetParamMin(m_index);
	m_fMax			= m_pInput->GetParamMax(m_index);
	m_iSamplesPerPixel = 0;
}

CParameter::~CParameter()
{
	delete m_pPoint;
}

void CParameter::DrawFore(wxDC& dc, wxRect& rc)
{
	m_iSamplesPerPixel = m_pTracks->GetSamplesPerPixel();

	if(m_fMax == 0.f)
		return;

	m_fScale	= (float)rc.GetHeight() / (float)(m_fMax - m_fMin);
	int start	= m_pRegion->GetStartPos();
	int end		= m_pRegion->GetEndPos();
	int offset	= m_pRegion->GetStartOffset();
	int top		= rc.GetTop();
	int bottom	= rc.GetTop() + rc.GetHeight();
	int left	= rc.GetX() + 1;
	int right	= left + rc.GetWidth() - 1;
	float value = 0.f;
	int prevX	= left;
	int prevY	= bottom;
	int pos	= m_bAfterTrack ? 0 : start;
	bool bLoaded = false;

	bool bDrawCircle = true;

	if(!m_pInput->GetParamAt(pos, m_index, &value))
	{
		pos = m_pInput->GetPrevOffset(pos);
		bDrawCircle = false;
	}
	else
		bLoaded = true;

	wxPen oldpen = dc.GetPen();
	wxColor color = m_color;
	if( IsSelected() )
		color.Set(255-color.Red(), 255-color.Green(), 255-color.Blue());
	dc.SetPen( wxPen(color) );

	do {
		if(m_pInput->GetParamAt(pos, m_index, &value))
		{
			bLoaded = true;

			int x = pos;
			int y = (int)((float)bottom - (value * m_fScale));

			if(y>top && y<bottom)
			{
				if(m_bAfterTrack && offset>=0)
					x += start-offset;
				
				x /= m_iSamplesPerPixel;

				if(x > left)
				{
					dc.DrawLine(prevX, prevY, x, prevY);	// Horizontal line
					dc.DrawLine(x, prevY, x, y);			// Vertical line
					
					if( bDrawCircle )
						dc.DrawCircle(x, y, 3);				// Parameter
					
					prevX = x;
				}
				else
				{
					prevX = left;
				}

				bDrawCircle = true;

				prevY = y;
			}
		}
		pos = m_pInput->GetNextOffset(pos);
	} while ( pos>0 && pos<end );

	if(bLoaded && prevX<right) // there have been parameters!
		dc.DrawLine(prevX, prevY, right, prevY);

	dc.SetPen(oldpen);
}

IFloopyObj *CParameter::GetChildAt(int x, int y)
{
	if(m_fMax == 0.f)
		return NULL;

	int start = m_pRegion->GetStartPos();
	int offset = m_pRegion->GetStartOffset();
	if(m_bAfterTrack && offset>=0)
		x -= (start-offset)/m_iSamplesPerPixel;
	
	int pos = x * m_iSamplesPerPixel;
	int prevOffset	= 0;
	int nextOffset	= 0;
	float value		= 0.f;
	float valuePrev	= 0.f;
	float valueNext = 0.f;

	int bottom	= m_pRegion->GetTop() + m_pRegion->GetHeight();

	if( !m_pInput->GetParamAt(pos, m_index, &value) )
	{
		prevOffset = m_pInput->GetPrevOffset(pos, m_index);
		if(prevOffset>0 && m_iSamplesPerPixel>=pos-prevOffset)
			pos = prevOffset;
		else
		{
			nextOffset = m_pInput->GetNextOffset(pos, m_index);
			if(nextOffset>0 && m_iSamplesPerPixel>=nextOffset-pos)
				pos = nextOffset;
			else if( m_pInput->GetParamAt(prevOffset, m_index, &value) )
			{
				if( (prevOffset==0 || pos>prevOffset) && 
					(nextOffset==0 || pos<=nextOffset) )
				{
					int tmpY = (int)((float)bottom - (value * m_fScale));
					if(tmpY >= y-1 && tmpY <= y+1)
					{
						m_pPoint->m_samplesPerPixel = m_iSamplesPerPixel;
						m_pPoint->m_sizing	= SIZE_VALUE;
						m_pPoint->m_offset	= prevOffset;
						m_pPoint->m_fScale	= m_fScale;
						m_pPoint->m_pInput	= m_pInput;
						m_pPoint->m_index	= m_index;
						m_pPoint->m_value	= value;
						return m_pPoint;
					}
				}
			}
			else
				return NULL;
		}
	}

	if( m_pInput->GetParamAt(pos, m_index, &value) )
	{
		int tmpY = (int)((float)bottom - (value * m_fScale));
		if(tmpY >= y-1 && tmpY <= y+1)
		{
			m_pPoint->m_samplesPerPixel = m_iSamplesPerPixel;
			m_pPoint->m_sizing	= SIZE_ALL;
			m_pPoint->m_offset	= pos;
			m_pPoint->m_fScale	= m_fScale;
			m_pPoint->m_pInput	= m_pInput;
			m_pPoint->m_index	= m_index;
			m_pPoint->m_value	= value;
			return m_pPoint;
		}
		else
		{
			if( m_pInput->GetParamAt(prevOffset, m_index, &valuePrev) &&
				m_pInput->GetParamAt(nextOffset, m_index, &valueNext))
			{
				int tmpY1 = (int)((float)bottom - (valuePrev * m_fScale));
				int tmpY2 = (int)((float)bottom - (valueNext * m_fScale));
				if(tmpY1 > tmpY2)
				{
					int tmp = tmpY2;
					tmpY2 = tmpY1;
					tmpY1 = tmp;
				}

				if(y>tmpY1 && y<tmpY2)
				{
					m_pPoint->m_samplesPerPixel = m_iSamplesPerPixel;
					m_pPoint->m_sizing	= SIZE_OFFSET;
					m_pPoint->m_offset	= pos;
					m_pPoint->m_fScale	= m_fScale;
					m_pPoint->m_pInput	= m_pInput;
					m_pPoint->m_index	= m_index;
					m_pPoint->m_value	= value;
					return m_pPoint;
				}
			}
		}
	}

	return NULL;
}

void CParameter::Refresh()
{
//	m_pRegion->Invalidate();
	m_pRegion->Refresh();
}

/*
float CParameter::GetValueAt(int x)
{
	int pos = x * m_iSamplesPerPixel;
	float value = 0.f;
	IFloopySoundInput *m_pInput = GetTrack()->GetInput();
	if( m_pInput->GetParamAt(pos, m_index, &value) )
		return value;
	return 0.f;
}
*/

/*
IFloopyObj *CParameter::GetSelectedObj()
{
	return IsSelected() ? this : NULL;
}
*/

bool CParameter::OnKeyDown(wxKeyEvent& event)
{
	switch( event.GetKeyCode() )
	{
	case 'p':
	case 'P':
		insertParam(event.GetX(), event.GetY());
		return true;
	/*case WXK_UP:
	case WXK_NUMPAD_UP:
		if(IsSelected())
		{
			IFloopyObj *obj = GetChildAt(event.GetX(), event.GetY());
			if(NULL != obj)
			{
				obj->Move(0, -1);
			}
		}
		return true;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		if(IsSelected())
		{
			IFloopyObj *obj = GetChildAt(event.GetX(), event.GetY());
			if(NULL != obj)
			{
				obj->Move(0, 1);
			}
		}
		return true;*/
	default:
		break;
	}
		
	return false;
}

void CParameter::insertParam(int x, int y)
{
	int offset = m_pRegion->GetStartOffset();
	int start = m_pRegion->GetStartPos();
	int end = m_pRegion->GetEndPos();

	int pos = x * m_iSamplesPerPixel;
	if(pos<start || pos>end)
		return;

	pos += offset;

	if(m_bAfterTrack)
		pos -= start;

	int top = m_pRegion->GetTop();
	int bottom = top + m_pRegion->GetHeight();
	if(y<top || y>bottom)
		return;

	int ypos = bottom - y;
	float value = (float)ypos / m_fScale;

	m_pInput->SetParamAt(pos, m_index, value);

	m_pRegion->Invalidate();
	Refresh();
}

bool CParameter::HitTest(int x, int y)
{
	return NULL != GetChildAt(x, y);
}

void CParameter::Select(bool selected)
{
	IFloopyObj::Select(selected);
	// Deselect other parameters
	if(selected)
	{
		CParameters *parent = (CParameters*)GetParent();
		parent->DeselectAll(this);
	}
}




CParameter::CPoint::CPoint(CParameter *parameter) : IFloopyObj(parameter)
{
	m_pParameter = parameter;
	m_offset = 0;
}

wxCursor CParameter::CPoint::GetCursor()
{
	switch(m_sizing)
	{
	case SIZE_VALUE:	return wxCursor(wxCURSOR_SIZENS);
	case SIZE_OFFSET:	return wxCursor(wxCURSOR_SIZEWE);
	case SIZE_ALL:		return wxCursor(wxCURSOR_SIZING);
	default:			return wxCursor(wxCURSOR_ARROW);
	}
}

void CParameter::CPoint::Move(int dx, int dy)
{
	bool bRefresh = false;

	int offset = m_offset;

	if(dx!=0 && m_sizing!=SIZE_VALUE)
	{
		offset += dx*m_samplesPerPixel;
		if( m_pInput->MoveParam(m_offset, m_index, m_value, offset) )
		{
			m_offset = offset;
			bRefresh = true;
		}
		else
			offset = m_offset;
	}

	if(dy!=0 && m_sizing!=SIZE_OFFSET)
	{
		m_value -= (float)dy / m_fScale;
		m_pInput->SetParamAt(offset, m_index, m_value);
		bRefresh = true;
	}

	if( bRefresh )
		m_pParameter->Refresh();
}

void CParameter::CPoint::Select(bool selected)
{
	m_pParameter->Select(selected);
	m_pParameter->Refresh();
}
