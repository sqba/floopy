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
		//int top		= rc.GetTop();
		int bottom	= rc.GetTop() + rc.GetHeight();
		int min		= input->GetParamMin(m_index);
		if(min == 0)
			min = 1;
		int left	= rc.GetX();
		int right	= rc.GetX()+rc.GetWidth();
		float scale = (float)(min * rc.GetHeight()) / (float)max;
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
				int y = (int)((float)bottom - (value * scale));

				if(m_bAfterTrack)
					x += start;
				x /= hres;
				
				if(prevX > left)
					dc.DrawLine(prevX, prevY, prevX, y);	// Vertical line
				
				dc.DrawLine(prevX, y, x, y);				// Horizontal line

				if( bDrawCircle )
					dc.DrawCircle(prevX+1, y, 3);			// Parameter

				bDrawCircle = true;

				prevX = x;
				prevY = y;
			}
			offset = input->GetNextOffset(offset);
		} while ( (offset > 0) && (offset < end) );

		dc.DrawLine(prevX, prevY, right, prevY);

		//dc.SetBrush( oldBrush );
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
	IFloopySoundInput *input = getTrack()->GetInput();

	int max = input->GetParamMax(m_index);
	if(max != 0)
	{
		int samplesPerPixel = getTracks()->GetSamplesPerPixel();
		int offset = x * samplesPerPixel;
		
		float value = 0.f;

		if( input->GetParamAt(offset, m_index, &value) )
		{
			int min = input->GetParamMin(m_index);
			int height = getRegion()->GetHeight();
			int bottom	= getRegion()->GetTop() + getRegion()->GetHeight();
			float scale = (float)height / (float)(max - min);
			
			int tmpY = bottom - (value * scale);
			if(tmpY >= y-20 && tmpY <= y+20)
			{
				m_pPoint->m_offset = offset;
				return m_pPoint;
			}
		}

	
		/*int c = m_Offsets.Count();
		for(int i=0; i<m_Offsets.Count(); i++)
		{
			int tmp = m_Offsets.Item(i);
			if(tmp >= offset-samplesPerPixel && tmp <= y+samplesPerPixel)
			{
				int min = input->GetParamMin(m_index);
				int height = getRegion()->GetHeight();
				int bottom	= getRegion()->GetTop() + getRegion()->GetHeight();
				float scale = (float)height / (float)(max - min);

				float value = 0.f;
				
				if( input->GetParamAt(tmp, m_index, &value) )
				{
					int tmpY = bottom - (value * scale);
					if(tmpY >= y-20 && tmpY <= y+20)
					{
						m_pPoint->m_offset = offset;
						return m_pPoint;
					}
				}
			}
		}*/
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

void CParameter::CPoint::Move(int dx, int WXUNUSED(dy))
{
}
