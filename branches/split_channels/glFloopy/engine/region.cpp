#include "region.h"

CRegion::CRegion(CObject *pTrack) : CObject(pTrack)
{
	float fWidth = pTrack->Width() / 3;
	float fHeight = pTrack->Height() / 6;
	float fGap = 2.f;

	m_fLeft		= pTrack->Left() + fGap;
	m_fTop		= pTrack->Top() - fGap;
	m_fRight	= m_fLeft + fWidth;
	m_fBottom	= pTrack->Bottom() - fGap;

	// red
	m_Color.fRed	= 1.0;
	m_Color.fGreen	= 0.0;
	m_Color.fBlue	= 0.0;
}

CRegion::~CRegion()
{
}

void CRegion::DrawFrame()
{
	CObject::DrawFrame();
}
