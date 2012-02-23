
#include "../globals.h"
#include "tracks.h"
#include "track.h"
#include "region.h"
#include "../util/util.h"


CRegion::COffsetBar::COffsetBar(CRegion *region) : IFloopyObj(region)
{
	m_bDrawAquaBG = DRAW_AQUA_RULERS;
}

int CRegion::COffsetBar::GetHeight()
{
	return 15;
}

void CRegion::COffsetBar::Move(int dx, int WXUNUSED(dy))
{
	// Move start offset
	CRegion *pRegion	= getRegion();
	CTrack  *pTrack		= (CTrack*)pRegion->GetParent();
	CTracks *pTracks	= (CTracks*)pTrack->GetParent();

//	SOUNDFORMAT *fmt = pTracks->GetInput()->GetFormat();
//	int channels = fmt->channels;

	//int start = pRegion->GetStartOffset();
	int spp = pTracks->GetSamplesPerPixel();
	int newOffset = m_iStart - dx*spp;

	if(newOffset >= 0)
		pRegion->SetStartOffset( newOffset );
}

void CRegion::COffsetBar::DrawBG(wxDC &dc, wxRect &rc)
{
	int left	= rc.GetLeft();
	int top		= rc.GetTop();
	int width	= rc.GetWidth();
	int height	= this->GetHeight();
//	int right	= left + width;
//	int bottom	= rc.GetBottom();

	if(m_bDrawAquaBG)
	{
	    wxRect rc(left, top, width, height);
		DrawAquaRect(dc, rc, 2);
	}
	else
	{
	    wxRect rc(left, top, width, height);
		DrawRect3D(dc, rc);
	}
}

void CRegion::COffsetBar::DrawFore(wxDC &dc, wxRect &rc)
{
	CRegion *pRegion = getRegion();
	CTracks *pTracks = (CTracks*)pRegion->GetParent()->GetParent();

	int pix		= pTracks->GetPixelsPerSecond();
	int iStep	= pTracks->CalcStep( MIN_DISTANCE );

	int left	= rc.GetLeft() + 1;
	int top		= rc.GetTop();
	int right	= left + rc.GetWidth() - 1;
	int height	= this->GetHeight() - 1;
//	int bottom	= top + height;
	int iMiddle	= top + height/2;

	int iLineTop1 = iMiddle + 1;
	int iLineTop2 = iMiddle + (height/3);

	int iLineTop = top;
	int iLineBottom = top + height;

	wxFont oldFont = dc.GetFont();
	//dc.SetFont( *wxSWISS_FONT );
	wxFont font(7, wxSWISS, wxNORMAL, wxLIGHT);
	dc.SetFont( font );

	int w=0, h=0;
	wxString csLabel(_T("00:00:000"));
	dc.GetTextExtent(csLabel, &w, &h);
	int iTextTop = iLineTop1 - (h - h/3);

	int start = pRegion->GetStartOffset();
	int spp = pTracks->GetSamplesPerPixel();

	if(start >= 0)
	{
		m_iStart = start;
		start /= spp;
	}
	else
	{
		///////////////////////////////////////////////////////
		// Ovde treba uzeti kraj prethodnog regiona + razmak!!!
		///////////////////////////////////////////////////////
		CRegion *prev = getPrevRegion();
		if(NULL != prev)
		{
			CRegion	*region	= getRegion();
			int prevEndOffset = prev->GetEndOffset();
			int dist = region->GetStartPos() - prev->GetEndPos();
			start = prevEndOffset + dist;
		}
		else
			start = pRegion->GetStartPos();
		m_iStart = start;
		start /= spp;
	}

	int pos = start;

	for(int x=left; x<right; x+=iStep)
	{
		//if((x/iStep)%4 == 0)
		//Optimization: x%a == x&(a-1) for binary numbers
		if((((x-left)/iStep)&3) == 0)
		{
			iLineTop = iLineTop1;

			if(x+4+w < right)
			{
				float fSeconds = (float)pos / (float)pix;
				FormatTime(fSeconds, csLabel);
				dc.DrawText(csLabel, x+4, iTextTop);
			}
		}
		else
		{
			iLineTop = iLineTop2;
		}

		pos += iStep;


		dc.SetPen(*wxBLACK_PEN);
		dc.DrawLine(x, iLineTop, x, iLineBottom);

		dc.SetPen(*wxWHITE_PEN);
		dc.DrawLine(x+1, iLineTop, x+1, iLineBottom);
	}

	dc.SetFont(oldFont);
}

CRegion *CRegion::COffsetBar::getPrevRegion()
{
	CRegion	*region	= getRegion();
	CTrack	*track	= (CTrack*)region->GetParent();
	CRegion	*prev	= NULL;
	int start = region->GetStartPos();
	int max = 0;

	for(int i=0; i<track->GetRegionCount(); i++)
	{
		CRegion *tmp = track->GetRegion(i);
		int end = tmp->GetEndPos();
		if(end<start && end>=max)
		{
			prev = tmp;
			max = end;
		}
	}

	return prev;
}

