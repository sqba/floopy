#include "track.h"

CTrack::CTrack(CObject *pEngine) : CObject(pEngine)
{
	float fHeight = pEngine->Height() / 6;
	float fGap = 2.f;

	m_fLeft		= pEngine->Left() + fGap;
	m_fTop		= pEngine->Top() - fGap;
	m_fRight	= pEngine->Right() - fGap;
	m_fBottom	= pEngine->Top() - fHeight;

	// yellow
	m_Color.fRed	= 0.9;
	m_Color.fGreen	= 1.0;
	m_Color.fBlue	= 0.0;
}

CTrack::~CTrack()
{
}

void CTrack::DrawFrame()
{
	std::vector<CRegion>::iterator it;
	for ( it=m_Regions.begin() ; it != m_Regions.end(); it++ )
	{
		CRegion *pRegion = &(*it);
		pRegion->DrawFrame();
	}

	CObject::DrawFrame();
}

CRegion *CTrack::AddRegion()
{
	CRegion region(this);
	m_Regions.push_back( region );
	return &m_Regions[m_Regions.size()-1];
}
