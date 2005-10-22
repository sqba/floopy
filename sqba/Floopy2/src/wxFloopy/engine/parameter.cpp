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

CParameter::CParameter(CRegion *region, IFloopySoundInput *obj, int index,
					   bool bAfterTrack, wxColor color)
 : IFloopyObj(region)
{
	m_pRegion		= region;
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
	case WXK_UP:
	case WXK_NUMPAD_UP:
		break;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
		break;
	case WXK_LEFT:
	case WXK_NUMPAD_LEFT:
		break;
	case '-':
		break;
	case WXK_RIGHT:
	case WXK_NUMPAD_RIGHT:
		break;
	case '+':
		break;
	case WXK_DELETE:
		break;
	case 'p':
	case 'P':
		insertParam(event.GetX(), event.GetY());
		return true;
	case WXK_F5:
	case WXK_SPACE:
		break;
	case WXK_END:
	case WXK_RETURN:
		break;
	default:
		break;
	}
		
	return false;
}

void CParameter::insertParam(int x, int y)
{
	int start = m_pRegion->GetStartPos();
	int end = m_pRegion->GetEndPos();
	int offset = x * m_iSamplesPerPixel;
	if(offset<start || offset>end)
		return;

	if(m_bAfterTrack)
		offset -= start;

	int top = m_pRegion->GetTop();
	int bottom = top + m_pRegion->GetHeight();
	if(y<top || y>bottom)
		return;

	int ypos = bottom - y;
	float value = (float)ypos / m_fScale;

	m_pInput->SetParamAt(offset, m_index, value);

	m_pRegion->Invalidate();
	Refresh();
}

bool CParameter::HitTest(int x, int y)
{
	return NULL != GetChildAt(x, y);
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
}
