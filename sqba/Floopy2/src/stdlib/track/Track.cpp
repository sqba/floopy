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
//	m_bReset = TRUE;
//	m_iOffset = 0;
	Enable( FALSE );
}

CTrack::~CTrack()
{

}
/*
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
//	if(bEnabled && m_bReset)
//		IFloopySoundFilter::Reset();

	IFloopySoundFilter::Enable(bEnabled);
}
*/
/*
void CTrack::MoveTo(int samples)
{
	m_iOffset = samples;
	IFloopySoundFilter::MoveTo(samples);
}

void CTrack::Reset()
{
	m_iOffset = 0;
	IFloopySoundFilter::Reset();
}

int CTrack::Read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);

	SOUNDFORMAT *fmt = m_source->GetFormat();
	int stb = (fmt->bitsPerSample / 8) * fmt->channels;
	m_iOffset += len / stb;

	return len;
}
*/