// parameter.cpp: implementation of the CParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"
#include "track.h"
#include "region.h"
#include "parameter.h"
#include "actionhistory.h"

//IMPLEMENT_DYNAMIC_CLASS(CParameter, IFloopyObj)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


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
	m_pPoint->m_pActionHistory = m_pTracks->GetActionHistory();
	m_bAfterTrack	= bAfterTrack;
	m_pInput		= obj;
	m_fMin			= m_pInput->GetParamMin(m_index);
	m_fMax			= m_pInput->GetParamMax(m_index);
	m_iSamplesPerPixel = 0;
	m_pSelectedPoint = NULL;
}

CParameter::~CParameter()
{
	delete m_pPoint;
	if(m_pSelectedPoint)
		delete m_pSelectedPoint;
}

bool CParameter::GetName(wxString &str)
{
	const char *name = m_pInput->GetParamName(m_index);
	if(NULL != name)
	{
		str.Printf(_T("%s"), name);
		return true;
	}
	return false;
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
					x += start - offset;

				x /= m_iSamplesPerPixel;

				if(x <= left)
					prevX = left;
				else if(x >= right)
					y = prevY;
				else
				{
					dc.DrawLine(prevX, prevY, x, prevY);	// Horizontal line
					dc.DrawLine(x, prevY, x, y);			// Vertical line

					if( bDrawCircle )
					{
						if(m_pSelectedPoint &&
							m_pSelectedPoint->m_offset==pos &&
							m_pSelectedPoint->m_value==value)
						{
							dc.DrawRectangle(x-3, y-3, 6, 6);	// Selected parameter
						}
						else
							dc.DrawCircle(x, y, 3);				// Parameter
					}

					prevX = x;
				}

				bDrawCircle = true;

				prevY = y;
			}
		}
		pos = m_pInput->GetNextOffset(pos);
	} while ( pos>0 && pos<end && prevX<right );

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
			if( m_pInput->GetParamAt(prevOffset, m_index, &valuePrev) )
			{
				int prevY = (int)((float)bottom - (valuePrev * m_fScale));

				if(prevY > tmpY)
				{
					int tmp	= tmpY;
					tmpY	= prevY;
					prevY	= tmp;
				}

				if(y>prevY && y<tmpY)
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
	case WXK_UP:
	case WXK_NUMPAD_UP:
	case '+':
		if(NULL != m_pSelectedPoint)
			m_pSelectedPoint->Move(0, -1);
		return true;
	case WXK_DOWN:
	case WXK_NUMPAD_DOWN:
	case '-':
		if(NULL != m_pSelectedPoint)
			m_pSelectedPoint->Move(0, 1);
		return true;
	case WXK_PRIOR:
//	case WXK_PAGEUP:
	case WXK_NUMPAD_PAGEUP:
		if(NULL != m_pSelectedPoint)
			m_pSelectedPoint->Move(-1, 0);
		return true;
	case WXK_NEXT:
//	case WXK_PAGEDOWN:
	case WXK_NUMPAD_PAGEDOWN:
		if(NULL != m_pSelectedPoint)
			m_pSelectedPoint->Move(1, 0);
		return true;
	case WXK_DELETE:
		if(NULL != m_pSelectedPoint)
		{
			if( m_pInput->ResetParamAt( m_pSelectedPoint->m_offset,
										m_index, m_pSelectedPoint->m_value) )
				Refresh();
		}
		break;
	default:
		break;
	}

	return false;
}

bool CParameter::OnMouseEvent(wxMouseEvent& event)
{
	bool bResult = false;
	if( event.LeftDown() )
	{
		if( !IsSelected() )
			Select();
//		else
//			insertParam(event.GetX(), event.GetY());
		bResult = true;
	}
	else if( event.MiddleDown() )
	{
		insertParam(event.GetX(), event.GetY());
		bResult = true;
	}
	IFloopyObj::OnMouseEvent(event);
	return bResult;
}

void CParameter::insertParam(int x, int y)
{
	int offset	= m_pRegion->GetStartOffset();
	int start	= m_pRegion->GetStartPos();
	int end		= m_pRegion->GetEndPos();
	int pos		= x * m_iSamplesPerPixel;

	if(pos<start || pos>end)
		return;

	if(offset > 0)
		pos += offset;

	if(m_bAfterTrack)
		pos -= start;

	int top = m_pRegion->GetTop();
	int bottom = top + m_pRegion->GetHeight();
	if(y<top || y>bottom)
		return;

	int ypos = bottom - y;
	float value = (float)ypos / m_fScale;

	//m_pInput->SetParamAt(pos, m_index, value);
	m_pTracks->GetActionHistory()->SetParamAt(this, m_pInput, pos, m_index, value);


	if(NULL == m_pSelectedPoint)
	{
		m_pSelectedPoint = new CPoint(this);
		m_pSelectedPoint->m_pActionHistory = m_pTracks->GetActionHistory();
	}
	m_pSelectedPoint->m_samplesPerPixel = m_iSamplesPerPixel;
	m_pSelectedPoint->m_sizing	= SIZE_ALL;
	m_pSelectedPoint->m_offset	= pos;
	m_pSelectedPoint->m_fScale	= m_fScale;
	m_pSelectedPoint->m_pInput	= m_pInput;
	m_pSelectedPoint->m_index	= m_index;
	m_pSelectedPoint->m_value	= value;


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
	Refresh();
}

void CParameter::SelectPoint(CPoint *pt)
{
	if(m_pSelectedPoint)
	{
		delete m_pSelectedPoint;
		m_pSelectedPoint = NULL;
	}
	if(pt)
	{
		m_pSelectedPoint = new CPoint(pt);
		m_pSelectedPoint->m_pActionHistory = m_pTracks->GetActionHistory();
	}
}

IFloopyObj *CParameter::GetSelectedObj()
{
	if(m_pSelectedPoint)
		return m_pSelectedPoint;
	else
		return this;
}

void CParameter::Smooth(int start, int end, int res)
{
	/*int offset = start;
	int prevOffset = start;
	float value = 0.f;
	float prevValue = 0.f;
	if( m_pInput->GetParamAt(offset, m_index, &value) )
		prevValue = value;
	while(offset=m_pInput->GetNextOffset(offset, m_index) < end)
	{
		int points = (offset - prevOffset) / m_iSamplesPerPixel;
		prevOffset = offset;
		prevValue = value;
	}*/
}









CParameter::CPoint::CPoint(CParameter *parameter) : IFloopyObj(parameter)
{
	m_pParameter = parameter;
	m_offset = 0;
}


CParameter::CPoint::CPoint(CPoint *pt)
{
	IFloopyObj( pt->GetParent() );
	m_pParameter = (CParameter*)pt->GetParent();

	m_samplesPerPixel = pt->m_samplesPerPixel;
	m_sizing	= SIZE_ALL;
	m_offset	= pt->m_offset;
	m_fScale	= pt->m_fScale;
	m_pInput	= pt->m_pInput;
	m_index		= pt->m_index;
	m_value		= pt->m_value;
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
		//if( m_pInput->MoveParam(m_offset, m_index, m_value, offset) )
		if(m_pActionHistory->MoveParam(this, m_pInput, m_offset, m_index, m_value, offset))
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
		//m_pInput->SetParamAt(offset, m_index, m_value);
		m_pActionHistory->SetParamAt(this, m_pInput, m_offset, m_index, m_value);
		bRefresh = true;
	}

	if( bRefresh )
		m_pParameter->Refresh();
}

void CParameter::CPoint::Select(bool selected)
{
	m_pParameter->Select(selected);
	m_pParameter->SelectPoint(selected ? this : NULL);
	m_pParameter->Refresh();
}

bool CParameter::CPoint::OnMouseEvent(wxMouseEvent& event)
{
	return m_pParameter->OnMouseEvent(event);
}

void CParameter::CPoint::Refresh()
{
	m_pParameter->Refresh();
}

bool CParameter::CPoint::GetName(wxString &str)
{
	return m_pParameter->GetName(str);
}
