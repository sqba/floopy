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

CParameter::CParameter(CRegion *region, IFloopySoundInput *obj, int index, bool bAfterTrack, wxColor color)
 : IFloopyObj(region)
{
	m_index  = index;
	m_color  = color;
	m_pPoint = new CPoint(this);

	m_bAfterTrack = bAfterTrack;

	m_pObj = obj;
}

CParameter::~CParameter()
{
	delete m_pPoint;
}

float CParameter::GetValueAt(int x)
{
	int samplesPerPixel = getTracks()->GetSamplesPerPixel();
	int offset = x * samplesPerPixel;
	float value = 0.f;
	IFloopySoundInput *input = getTrack()->GetInput();
	if( input->GetParamAt(offset, m_index, &value) )
		return value;
	return 0.f;
}
/*
void CParameter::DrawBG(wxDC& dc, wxRect& rc)
{

}
*/
void CParameter::DrawFore(wxDC& dc, wxRect& rc)
{
	IFloopySoundInput *input = m_pObj;
	int max = input->GetParamMax(m_index);

	if(max > 0)
	{
		int hres	= getTracks()->GetSamplesPerPixel();
		int start	= getRegion()->GetStartPos();
		int end		= getRegion()->GetEndPos();
		int startOffset = getRegion()->GetStartOffset();
		int top		= rc.GetTop();
		int bottom	= rc.GetTop() + rc.GetHeight();
		int min		= input->GetParamMin(m_index);
		int left	= rc.GetX();
		int right	= rc.GetX()+rc.GetWidth();
		m_fScale	= (float)rc.GetHeight() / (float)(max - min);
		float value = 0.f;
		int prevX	= left;
		int prevY	= bottom;
		int offset	= m_bAfterTrack ? 0 : start;

		wxPen oldpen = dc.GetPen();
		dc.SetPen( wxPen(m_color) );

		bool bDrawCircle = true;

		if(!input->GetParamAt(offset, m_index, &value))
		{
			offset = input->GetPrevOffset(offset);
			bDrawCircle = false;
		}

		do {
			if(input->GetParamAt(offset, m_index, &value))
			{
				int x = offset;
				int y = (int)((float)bottom - (value * m_fScale));

				if(y>top && y<bottom)
				{
					if(m_bAfterTrack)
						x += start-startOffset;
					x /= hres;

					if(x > left)
					{
						dc.DrawLine(prevX, prevY, x, prevY);	// Horizontal line
						dc.DrawLine(x, prevY, x, y);			// Vertical line
						
						if( bDrawCircle )
							dc.DrawCircle(x+1, y, 3);			// Parameter
						
						prevX = x;
					}
					else
					{
						dc.DrawLine(prevX, prevY, left, prevY);	// Horizontal line
						prevX = left;
					}

					bDrawCircle = true;

					prevY = y;
				}
			}
			offset = input->GetNextOffset(offset);
		} while ( (offset > 0) && (offset < end) );

		if(bDrawCircle) // there have been parameters!
			dc.DrawLine(prevX, prevY, right, prevY);

		dc.SetPen(oldpen);
	}
}
/*
IFloopyObj *CParameter::GetSelectedObj()
{
	return IsSelected() ? this : NULL;
}
*/
IFloopyObj *CParameter::GetChildAt(int x, int y)
{
	int max = m_pObj->GetParamMax(m_index);
	if(max != 0)
	{
		int samplesPerPixel = getTracks()->GetSamplesPerPixel();

		//int startOffset = getRegion()->GetStartOffset();
		int start = getRegion()->GetStartPos();
		int startOffset = getRegion()->GetStartOffset();
		if(m_bAfterTrack)
			x -= (start-startOffset)/samplesPerPixel;
		
		int offset = x * samplesPerPixel;

		float value = 0.f;

		if( !m_pObj->GetParamAt(offset, m_index, &value) )
		{
			int prev = m_pObj->GetPrevOffset(offset, m_index);
			if(prev>0 && samplesPerPixel>=offset-prev)
				offset = prev;
			else
			{
				int next = m_pObj->GetNextOffset(offset, m_index);
				if(next>0 && samplesPerPixel>=next-offset)
					offset = next;
				else if( m_pObj->GetParamAt(prev, m_index, &value) )
				{
					if((prev==0 || offset>prev) && (next==0 || offset<=next))
					{
						//int min		= m_pObj->GetParamMin(m_index);
						//int height	= getRegion()->GetHeight();
						int bottom	= getRegion()->GetTop() + getRegion()->GetHeight();
						//float scale	= (float)height / (float)(max - min);
						
						int tmpY = (int)((float)bottom - (value * m_fScale));
						if(tmpY >= y-1 && tmpY <= y+1)
						{
							m_pPoint->m_offset = prev;
							m_pPoint->m_samplesPerPixel = samplesPerPixel;
							m_pPoint->m_fScale = m_fScale;
							m_pPoint->m_pObj = m_pObj;
							m_pPoint->m_index = m_index;
							m_pPoint->m_value = value;
							m_pPoint->m_bValueOnly = true;
							return m_pPoint;
						}
						//else
						//	return NULL;
					}
				}
				else
					return NULL;
			}
		}

		if( m_pObj->GetParamAt(offset, m_index, &value) )
		{
			//int min		= m_pObj->GetParamMin(m_index);
			//int height	= getRegion()->GetHeight();
			int bottom	= getRegion()->GetTop() + getRegion()->GetHeight();
			//float scale	= (float)height / (float)(max - min);
			
			int tmpY = (int)((float)bottom - (value * m_fScale));
			if(tmpY >= y-1 && tmpY <= y+1)
			{
				m_pPoint->m_offset = offset;
				m_pPoint->m_samplesPerPixel = samplesPerPixel;
				m_pPoint->m_fScale = m_fScale;
				m_pPoint->m_pObj = m_pObj;
				m_pPoint->m_index = m_index;
				m_pPoint->m_value = value;
				m_pPoint->m_bValueOnly = false;
				return m_pPoint;
			}
		}
	}

	return NULL;
}

bool CParameter::HitTest(int x, int y)
{
//	int max = input->GetParamMax(m_index);
//	if(max > 0)
	{
//		int min = input->GetParamMin(m_index);
//		float f = (float)rc.GetHeight() / (float)(max - min);

		int samplesPerPixel = getTracks()->GetSamplesPerPixel();
		int offset = x * samplesPerPixel;
		float value = 0.f;
		IFloopySoundInput *input = getTrack()->GetInput();
		if( input->GetParamAt(offset, m_index, &value) )
		{
			return true;
		}
	}

	return false;
}

CTrack *CParameter::getTrack()
{
	return (CTrack*)getRegion()->GetParent();
}

CTracks *CParameter::getTracks()
{
	return (CTracks*)getTrack()->GetParent();
}

void CParameter::CPoint::Move(int dx, int dy)
{
	bool bRefresh = false;
	CParameter *param = (CParameter*)GetParent();

	//int samplesPerPixel = param->getTracks()->GetSamplesPerPixel();
	//int offset = x * samplesPerPixel;
	int offset = m_offset;

	if(dx!=0 && !m_bValueOnly)
	{
		offset += dx*m_samplesPerPixel;
		if( m_pObj->MoveParam(m_offset, m_index, m_value, offset) )
		{
			m_offset = offset;
			bRefresh = true;
		}
		else
			offset = m_offset;
	}

	if(dy != 0)
	{
		m_value -= (float)dy / m_fScale;
		m_pObj->SetParamAt(offset, m_index, m_value);
		bRefresh = true;
	}

	//m_pObj->ResetParamAt(m_offset, m_index, m_value);

	if( bRefresh )
		param->GetParent()->Refresh();
}
