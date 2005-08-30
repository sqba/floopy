// RegionDisplay.cpp: implementation of the CRegionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "engine/Tracks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(PeaksArray);

CRegionDisplay::CRegionDisplay(CRegion *region)
{
	m_pRegion = region;
//	m_pMutex = new wxMutex();
	m_pInput = NULL;
	m_fdB = -6.0;
//	Create();
	m_bLoaded = FALSE;
//	m_pLoadThread = NULL;
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
/*
	if(m_pLoadThread)
		delete m_pLoadThread;

	m_pLoadThread = new CLoadThread(this);
	m_pLoadThread->Create();
	m_pLoadThread->Run();
*/
}

// Do this in another thread!!!
void CRegionDisplay::loadPeaks()
{
	m_bLoaded = FALSE;

//	m_pMutex->Lock();


	m_peaks.Empty();

	CRegion *region = m_pRegion;//(CRegion*)GetParent();
	CTrack  *track  = (CTrack*)region->GetParent();
	CTracks *tracks = (CTracks*)track->GetParent();

//	m_pInput = getInput(track);
	m_pInput = track->GetInput();
	if(NULL == m_pInput)
		return;

	SOUNDFORMAT *fmt	= m_pInput->GetFormat();
	int samplesPerPixel = tracks->GetSamplesPerPixel();
	int channels		= fmt->channels;
//	int maxSample		= (int)pow(2, fmt->bitsPerSample) / 2;

	int start	= region->GetStartOffset();
	int end		= region->GetEndOffset();
	if(end<=0 || start<0)
		return;
	int samples	= (end - start) * channels;

	short int *buffer = new short int[samples];
	int bytes = samples * sizeof(short int);
	//int bytes = samples * (fmt->bitsPerSample / 8);
	memset(buffer, 0, bytes);

	m_pInput->MoveTo(start);
	int bytesRead = m_pInput->Read((BYTE*)buffer, bytes);
	if(EOF == bytesRead)
	{
		delete buffer;
		return;
	}
	samples = bytesRead / (fmt->bitsPerSample / 8);

//	int interval = samples / samplesPerPixel;
	int interval = samplesPerPixel;// / channels;
	int counter=0;
	int ch=0;

	if(interval < 0)
	{
		delete buffer;
		return;
	}

/*
	short int peak[2] = {0};
	int peakcount=0;

	for(int pos=0; pos<samples; pos+=channels)
	{
		for(ch=0; ch<channels; ch++)
		{
			short int sample = buffer[pos+ch];

			if( (peakcount % 2) == 0 )
			{
				if(sample > peak[ch])
					peak[ch] = sample;
			}
			else
			{
				if(sample < peak[ch])
					peak[ch] = sample;
			}
		}

		if(counter >= interval)
		{
			for(ch=0; ch<channels; ch++)
			{
				m_peaks.Add( peak[ch] );
				peak[ch] = 0;
			}
			counter = 0;
			peakcount++;
		} else
			counter++;
	}
*/


	
	int srcLen = getSourceLength(); // If looped




	short int min[2]={0}, max[2]={0};
	int peakcount=0;
	counter = interval;

	for(int pos=0; pos<samples; pos+=channels)
	{
		for(ch=0; ch<channels; ch++)
		{
			short int sample = buffer[pos+ch];

			if(sample > max[ch])
				max[ch] = sample;
			else if(sample < min[ch])
				min[ch] = sample;
		}

		if(counter >= interval || (srcLen && pos%srcLen==0))
		{
			for(ch=0; ch<channels; ch++)
			{
				if(max[ch] == 0 && min[ch] != 0)
					max[ch] = min[ch];
				else if(min[ch] == 0 && max[ch] != 0)
					min[ch] = max[ch];

				short int value = ( (peakcount % 2) == 0 ? max[ch] : min[ch] );
				
//				m_peaks.Add( value );

				Peak peak;
				peak.value = value;
				peak.pos = pos/channels;
				m_peaks.Add( peak );

				/*CPeak *p = new CPeak(peak, pos/channels);
				//p->peak = peak;
				//p->pos = pos/channels;
				m_peaks2.Add( p );*/

				max[ch] = min[ch] = 0;
			}
			counter = 0;
			peakcount++;
		} else
			counter++;
	}

/*
	short int min[2]={0}, max[2]={0};
	int peakcount=0;
	counter = interval;

	for(int pos=0; pos<samples; pos+=channels)
	{
		for(ch=0; ch<channels; ch++)
		{
			short int sample = buffer[pos+ch];

			if(sample > max[ch])
				max[ch] = sample;
			else if(sample < min[ch])
				min[ch] = sample;
		}

		if(counter >= interval)
		{
			for(ch=0; ch<channels; ch++)
			{
				short int peak = ( abs(max[ch]) > abs(min[ch]) ? max[ch] : min[ch] );
				m_peaks.Add( peak );
				max[ch] = min[ch] = 0;
			}
			counter = 0;
			peakcount++;
		} else
			counter++;
	}
*/

	delete buffer;

//	m_pMutex->Unlock();

	m_bLoaded = TRUE;
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
	if(!m_bLoaded)
		return;

//	wxMutexLocker lock(*m_pMutex);

	int left	= rc.GetX();
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();

	float mid = rc.GetY() + height/2.f;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	int maxSample = (int)pow(2, fmt->bitsPerSample);// / 2;
	int yscale = maxSample / height;
	int channels = fmt->channels;

	int count = m_peaks.GetCount();// / channels;

	wxPoint ptPrev(start, mid);


	//CTrack *pTrack = (CTrack*)m_pRegion->GetParent();
	//CTracks *pTracks = (CTracks*)pTrack->GetParent();
	//int samplesPerPixel = pTracks->GetSamplesPerPixel();
	int origLen = getSourceLength();// / samplesPerPixel; // Not looped
	int top = rc.GetTop();
	int bottom = rc.GetBottom();
	wxPen oldpen = dc.GetPen();
	//wxPen pen( *wxCYAN, 2, wxSOLID );
	wxPen pen( m_pRegion->GetForeColour(), 2, wxSOLID );
	//wxBrush brush(pTrack->GetBGColour(), wxSOLID);

//	if(getSourceLength() % samplesPerPixel != 0)
//		origLen = 0; // Don't draw



	// Napraviti strukturu Peak koja ima vrednosti peak i pos.
	// Proveravati pos%origLen



//	assert(count == width/channels);

	int step = channels;//count > width ? count / width : 1;
	int i=start;
	int y=mid;
	for(int x=0; x<width&&i<count; x++)
	{
		Peak peak = m_peaks.Item(i);
		y = (int)((float)mid - peak.value/yscale);
		dc.DrawLine(left+ptPrev.x, ptPrev.y, left+x, y);

		if((origLen>0) && (x>1) && ((peak.pos%origLen)==0))
		{
			dc.SetPen(pen);
			dc.DrawLine(left+x, top, left+x, bottom);
			dc.SetPen(oldpen);
		}

		ptPrev.x = x;
		ptPrev.y = y,
		
		i+=step;
	}

	// Check if the input is looped. In that case put
	// some marker at the end of each loop.
}

int CRegionDisplay::getSourceLength()
{
	int len = 0;
	CTrack *pTrack = (CTrack*)m_pRegion->GetParent();
	if( pTrack->IsLooped() )
	{
		IFloopySoundFilter *loop = (IFloopySoundFilter*)pTrack->GetComponent("loop");
		if(loop)
		{
			IFloopySoundInput *src = loop->GetSource();
			if(src)
				len = src->GetSize();
		}
	}
	return len;
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
/*
void *CRegionDisplay::Entry()
{
	loadPeaks();
	return NULL;
}
*/