#include "engine.h"

CEngine::CEngine()
{
/*
	m_fWidth = 100.f;
	m_fHeight = 60.f;
	m_fLeft = -m_fWidth;
	m_fTop = m_fHeight;
*/
}

CEngine::~CEngine()
{
}

void CEngine::DrawFrame()
{
	std::vector<CTrack>::iterator it;
	for ( it=m_Tracks.begin() ; it != m_Tracks.end(); it++ )
	{
		CTrack *pTrack = &(*it);
		pTrack->DrawFrame();
	}

	CObject::DrawFrame();
}

CTrack *CEngine::AddTrack()
{
	CTrack track(this);
	m_Tracks.push_back( track );
	return &m_Tracks[m_Tracks.size()-1];
}
