// Track.cpp: implementation of the CTrack class.
//
//////////////////////////////////////////////////////////////////////

#include "Track.h"

#define MIN_HEIGHT	15

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrack::CTrack()
{
	Enable( FALSE );

	m_nHeight = MIN_HEIGHT * 3;
	m_nBGRed = m_nBGGreen = m_nBGBlue = 256;
	m_nForeRed = m_nForeGreen = m_nForeBlue = 256;
}

CTrack::~CTrack()
{

}


int CTrack::GetPropertyCount()
{
	return 7;
}

BOOL CTrack::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = (float)m_nBGRed;
		return TRUE;
	case 1:
		*value = (float)m_nBGGreen;
		return TRUE;
	case 2:
		*value = (float)m_nBGBlue;
		return TRUE;
	case 3:
		*value = (float)m_nForeRed;
		return TRUE;
	case 4:
		*value = (float)m_nForeGreen;
		return TRUE;
	case 5:
		*value = (float)m_nForeBlue;
		return TRUE;
	case 6:
		*value = (float)m_nHeight;
		return TRUE;
	}
	return FALSE;
}

void CTrack::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_nBGRed = (int)value;
		break;
	case 1:
		m_nBGGreen = (int)value;
		break;
	case 2:
		m_nBGBlue = (int)value;
		break;
	case 3:
		m_nForeRed = (int)value;
		break;
	case 4:
		m_nForeGreen = (int)value;
		break;
	case 5:
		m_nForeBlue = (int)value;
		break;
	case 6:
		m_nHeight = (int)value;
		break;
	}
}

char *CTrack::GetPropertyName(int index)
{
	switch(index)
	{
	case 0:
		return "BGColorRed";
	case 1:
		return "BGColorGreen";
	case 2:
		return "BGColorBlue";
	case 3:
		return "ForeColorRed";
	case 4:
		return "ForeColorGreen";
	case 5:
		return "ForeColorBlue";
	case 6:
		return "Height";
	}
	return NULL;
}

char *CTrack::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0:
		return "Red component of the background coror";
	case 1:
		return "Green component of the background coror";
	case 2:
		return "Blue component of the background coror";
	case 3:
		return "Red component of the foreground coror";
	case 4:
		return "Green component of the foreground coror";
	case 5:
		return "Blue component of the foreground coror";
	case 6:
		return "Track height";
	}
	return NULL;
}

float CTrack::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		return 0.f;
	case 6:
		return (float)MIN_HEIGHT;
	}
	return 0.f;
}

float CTrack::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		return 255.f;
	case 6:
		return 300.f;
	}
	return 0.f;
}

float CTrack::GetPropertyStep(int index)
{
	return 1.f;
}




/*
BOOL CTrack::GetPropertyIndex(char *name, int *index)
{
	return FALSE;
}
*/