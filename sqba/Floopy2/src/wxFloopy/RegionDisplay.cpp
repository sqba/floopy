// RegionDisplay.cpp: implementation of the CRegionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "engine/Tracks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegionDisplay::CRegionDisplay(CRegion *region)
{
	m_pRegion = region;
//	m_pMutex = new wxMutex();
	m_pInput = NULL;
	m_fdB = -6.0;
//	Create();
	LoadPeaks();
}

CRegionDisplay::~CRegionDisplay()
{
//	WX_CLEAR_LIST(PointList, m_points);
	//WX_CLEAR_ARRAY(m_peaks);
}

void CRegionDisplay::DrawBG(wxDC& dc, wxRect& rc)
{
	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int channels = fmt->channels;

	int width	= rc.GetWidth();
	int height	= rc.GetHeight() / channels;

	wxRect rcTmp = rc;
	for(int i=0; i<channels; i++)
	{
		rcTmp.SetY(rc.GetY() + height*i);
		rcTmp.SetHeight(height);
		drawDBLines(dc, rcTmp);
	}
}

void CRegionDisplay::DrawFore(wxDC& dc, wxRect& rc)
{
	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int channels = fmt->channels;

	int width	= rc.GetWidth();
	int height	= rc.GetHeight() / channels;

	wxRect rcTmp = rc;
	for(int i=0; i<channels; i++)
	{
		rcTmp.SetY(rc.GetY() + height*i);
		rcTmp.SetHeight(height);
		drawWaveform(dc, rcTmp, i);
	}
}

void CRegionDisplay::LoadPeaks()
{
//	Create();
//	Run();
	loadPeaks();
}

// Do this in another thread!!!
void CRegionDisplay::loadPeaks()
{
//	m_pMutex->Lock();


	m_peaks.Empty();

	CRegion *region = m_pRegion;//(CRegion*)GetParent();
	CTrack  *track  = (CTrack*)region->GetParent();
	CTracks *tracks = (CTracks*)track->GetParent();

//	m_pInput = getInput(track);
	m_pInput = track->GetInput();
	if(NULL == m_pInput)
		return;

	int start	= region->GetStartOffset();
	int end		= region->GetEndOffset();
	int samples	= end - start;

	SOUNDFORMAT *fmt	= m_pInput->GetFormat();
	int samplesPerPixel = tracks->GetSamplesPerPixel();
	int channels		= fmt->channels;
//	int maxSample		= (int)pow(2, fmt->bitsPerSample) / 2;

	int bytes = samples * channels * sizeof(short int);
	short int *buffer = new short int[bytes];
	memset(buffer, 0, bytes);

	m_pInput->MoveTo(start);
	samples = m_pInput->Read((BYTE*)buffer, bytes);

//	int interval = samples / samplesPerPixel;
	int interval = samplesPerPixel;

	int counter=0;;
	int step=1;

	int avg[2] = {0};
	int n=0;

	for(int i=0; i<samples; i++)
	{
		for(n=0; n<channels; n++)
			avg[n] += buffer[i+n];

		if(counter >= interval)
		{
			for(n=0; n<channels; n++)
			{
				m_peaks.Add(avg[n]/interval);
				avg[n] = 0;
			}

			counter = 0;
		} else
			counter++;
	}

	delete buffer;

//	m_pMutex->Unlock();
}


/**
 * Draws dB line(s) for a single channel.
 */
void CRegionDisplay::drawDBLines(wxDC& dc, wxRect& rc)
{
	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int channels = fmt->channels;

	int width	= rc.GetWidth();
	int height	= rc.GetHeight();

	int maxSample = (int)pow(2, fmt->bitsPerSample) / 2;
	int scale = maxSample / height;
	
	int amp = maxSample * pow(2, (m_fdB/20.0));
	int dby = (pow(2, (m_fdB/20.0)) * height) / 4.0;
	int middle = rc.GetY() + (height / 2);
	int y1 = middle - dby, y2 = middle + dby;

	dc.DrawLine(rc.GetX(),  y1, rc.GetX()+width, y1);

	dc.DrawLine(rc.GetX(),  y2, rc.GetX()+width, y2);
}

/**
 * Draws a waveform of a single channel.
 */
void CRegionDisplay::drawWaveform(wxDC& dc, wxRect& rc, int start)
{
//	wxMutexLocker lock(*m_pMutex);

	int left	= rc.GetX();
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();

	float mid = rc.GetY() + height/2.f;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	int maxSample = (int)pow(2, fmt->bitsPerSample);// / 2;
	int yscale = maxSample / height;
	int channels = fmt->channels;

	int count = m_peaks.GetCount() / channels;

	wxPoint ptPrev(start, mid);

	int step = channels;//count > width ? count / width : 1;
	int i=start;
	int y=mid;
	for(int x=0; x<width&&i<count; x++)
	{
		y = (int)((float)mid + m_peaks.Item(i)/yscale);
		dc.DrawLine(left+ptPrev.x, ptPrev.y, left+x, y);

		ptPrev.x = x;
		ptPrev.y = y,
		
		i+=step;
	}
}

IFloopySoundInput *CRegionDisplay::getInput(CTrack *track)
{
	CTracks *tracks = (CTracks*)track->GetParent();

//	IFloopySoundInput *input = ((IFloopySoundFilter*)track->GetInput())->GetSource();
	IFloopySoundInput *input = track->GetInput();
/*
	int samples	= input->GetSize();
	if(SIZE_INFINITE == samples)
		input = track->GetInput();

	samples	= input->GetSize();
	if(SIZE_INFINITE == samples)
		input = NULL;
*/
	return input;
}

void *CRegionDisplay::Entry()
{
	loadPeaks();
	return NULL;
}
