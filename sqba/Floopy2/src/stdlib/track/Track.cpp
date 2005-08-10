// Track.cpp: implementation of the CTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "Track.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrack::CTrack()
{
//	m_bEnabled = FALSE;
	m_bReset = TRUE;
	Enable( FALSE );
}

CTrack::~CTrack()
{

}

BOOL CTrack::GetParamVal(int index, float *value)
{
	if(index==0)
	{
		*value = (float)m_bReset;
		return TRUE;
	}
	return FALSE;
}

void CTrack::Enable(BOOL bEnabled)
{
	if(bEnabled && m_bReset)
		IFloopySoundFilter::Reset();

	IFloopySoundFilter::Enable(bEnabled);
}
