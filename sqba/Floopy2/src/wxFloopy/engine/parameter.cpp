// parameter.cpp: implementation of the CParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"

IMPLEMENT_DYNAMIC_CLASS(CParameter, IFloopyObj)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParameter::CParameter(CRegion *region, IFloopySoundInput *obj, int index)
 : IFloopyObj(region)
{
	m_index  = index;
	m_color  = *wxCYAN;
	m_pPoint = new CPoint(this);

	m_pObj = obj;

	
	
	/*IFloopySoundInput *input = getTrack()->GetInput();
	int offset = 0;
	do {
		float value = 0.f;
		if(input->GetParamVal(m_index, &value))
		{
			m_Offsets.Add(offset);
		}
		offset = input->GetNextOffset(offset);
	} while (offset > 0);
	int c = m_Offsets.Count();*/
}

CParameter::~CParameter()
{
	delete m_pPoint;
//	WX_CLEAR_ARRAY(m_Offsets);
}

float CParameter::GetValueAt(int x)
{
	int samplesPerPixel = getTracks()->GetSamplesPerPixel();
	int offset = x * samplesPerPixel;
	float value = 0.f;
	IFloopySoundInput *input = getTrack()->GetInput();
	if( input->GetParamAt(offset, m_index, &value) )
		return value;
	else
	{
		//int prev = input->GetPrevOffset(offset);
	}
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
		int offset	= getRegion()->GetStartOffset();
		int end		= getRegion()->GetEndOffset();
		int bottom	= rc.GetTop() + rc.GetHeight();
		int min		= input->GetParamMin(m_index);
		int left	= rc.GetX();
		int right	= rc.GetX()+rc.GetWidth();
		float scale = (float)rc.GetHeight() / (float)(max - min);
		float value = 0.f;
		int prevX	= left;
		int prevY	= bottom;

		//wxBrush oldBrush = dc.GetBrush();
		wxPen oldpen = dc.GetPen();
		//dc.SetBrush( wxBrush(m_colour) );
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
				int x = bDrawCircle ? offset / hres : getRegion()->GetStartOffset() / hres;
				int y = (int)((float)bottom - (value * scale));
				
				if( bDrawCircle )
					dc.DrawCircle(prevX+1, y, 3);
					//dc.DrawRectangle(prevX-2, y-2, 5, 5);

				bDrawCircle = true;
				
				if(prevX > left)
					dc.DrawLine(prevX, prevY, prevX, y);
				
				dc.DrawLine(prevX, y, x, y);

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
void CParameter::DrawPreview(wxDC& dc, wxRect& rc)
{

}
*/

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

void CParameter::CPoint::Move(int dx, int WXUNUSED(dy))
{
}
