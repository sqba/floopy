// RegionDisplay.cpp: implementation of the CRegionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "../engine/Tracks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(PeaksArray);

CRegionDisplay::CRegionDisplay(CRegion *region)
{
	m_pRegion		= region;
	m_pTrack		= (CTrack*)m_pRegion->GetParent();
	m_pTracks		= (CTracks*)m_pTrack->GetParent();
	m_pInput		= m_pTrack->GetInput();
	m_fdB			= -6.0;
	m_bDrawDBLines	= TRUE;
	m_bLoaded		= FALSE;
	m_bDrawVertical = FALSE;

	LoadPeaks();

//	m_pMutex = new wxMutex();
//	Create();
//	m_pLoadThread = NULL;
}

CRegionDisplay::~CRegionDisplay()
{
//	WX_CLEAR_LIST(PointList, m_points);
	//WX_CLEAR_ARRAY(m_peaks);
}

void CRegionDisplay::DrawBG(wxDC& dc, wxRect& rc)
{
	if(!m_bDrawDBLines)
		return;

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

	if(NULL == m_pRegion)
		return;

	if(NULL == m_pTrack)
		return;

	if(NULL == m_pTracks)
		return;

	if(NULL == m_pInput)
		return;

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
//	m_pMutex->Lock();

//	int maxSample		= (int)pow(2, fmt->bitsPerSample) / 2;


	m_bLoaded = FALSE;

	m_peaks.Empty();

	int interval = m_pTracks->GetSamplesPerPixel();
	if(interval < 0)
		return;

	m_bDrawVertical = (interval > 10);

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	if(NULL == fmt)
		return;
	int channels = fmt->channels;
	if(0 == channels)
		return;

	int start	= m_pRegion->GetStartOffset();
	int end		= m_pRegion->GetEndOffset();
	if(end<=0 || start<0)
		return;

	int samples	= (end - start) * channels;

	// Mozda bi bilo bolje kada bi se uchitavalo u chankovima

	short int *buffer = new short int[samples];
	int bytes = samples * sizeof(short int);
	memset(buffer, 0, bytes);
	
	int bytesRead = 0;

	try
	{
		m_pInput->MoveTo(start);
		bytesRead = m_pInput->Read((BYTE*)buffer, bytes);
	}
	catch(...)
	{
		delete buffer;
		return;
	}
	
	m_pTracks->SetViewUpdatedWhilePlaying(TRUE);
	
	if(EOF != bytesRead)
	{
		samples = bytesRead / (fmt->bitsPerSample / 8);

		int counter=0;
		int ch=0;
		
		int srcLen = getLengthNotLooped();

		short int min[2]={0}, max[2]={0};
		int peakcount=0;
		counter = interval;

		for(int pos=0; pos<samples; pos+=channels)
		{
			short int sample = buffer[pos+ch];

			for(ch=0; ch<channels; ch++)
			{
				if(sample > max[ch])
					max[ch] = sample;
				else if(sample < min[ch])
					min[ch] = sample;
			}

			if(counter >= interval || (srcLen && pos%srcLen==0))
			{
				for(ch=0; ch<channels; ch++)
				{
					if(!m_bDrawVertical)
					{
						if(max[ch] == 0 && min[ch] != 0)
							max[ch] = min[ch];
						else if(min[ch] == 0 && max[ch] != 0)
							min[ch] = max[ch];

						Peak peak;
						peak.value = ( (peakcount % 2) == 0 ? max[ch] : min[ch] );
						peak.pos = pos/channels;
						m_peaks.Add( peak );
					}
					else
					{
						Peak peakMax;
						peakMax.value = max[ch];
						peakMax.pos = pos/channels;
						m_peaks.Add( peakMax );

						Peak peakMin;
						peakMin.value = min[ch];
						peakMin.pos = pos/channels;
						m_peaks.Add( peakMin );
					}

					max[ch] = min[ch] = sample;
				}
				counter = 0;
				peakcount++;
			} else
				counter++;
		}

		m_bLoaded = TRUE;
	}

	delete buffer;

//	m_pMutex->Unlock();

}
/*
void CRegionDisplay::loadPeaks()
{
//	m_pMutex->Lock();

//	int maxSample = (int)pow(2, fmt->bitsPerSample) / 2;


	m_bLoaded = FALSE;

	m_peaks.Empty();

	int interval = m_pTracks->GetSamplesPerPixel();
	if(interval < 0)
		return;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	if(NULL == fmt)
		return;
	int channels = fmt->channels;
	if(0 == channels)
		return;

	m_bDrawVertical = (interval > 10);

	int start = m_pRegion->GetStartOffset();
	int end   = m_pRegion->GetEndOffset();

	if(end<=0 || start<0)
		return;

	int totalSamples = (end - start) * channels;

	int buffSize = interval*2;// totalSamples>fmt->frequency ? totalSamples/fmt->frequency : totalSamples;

	// Mozda bi bilo brze kada bi se uchitavalo u chankovima
	short int *buffer = new short int[buffSize];
	int bytes = buffSize * sizeof(short int);
	memset(buffer, 0, bytes);

	m_pInput->MoveTo(start);
	int bytesRead = m_pInput->Read((BYTE*)buffer, bytes);
	int samples = bytesRead / (fmt->bitsPerSample / 8);
	
	m_pTracks->SetViewUpdatedWhilePlaying(TRUE);
	
	if(EOF != bytesRead)
	{
		int counter=0;
		int ch=0;
		
		int srcLen = getLengthNotLooped();

		short int min[2]={0}, max[2]={0};
		int peakcount=0;
		counter = interval;

		int buffPos = 0;

		for(int pos=0; pos<=totalSamples; pos+=channels)
		{
			if(buffPos >= buffSize)
			{
				if(buffPos+buffSize > totalSamples)
				{
					buffSize = totalSamples - pos;
					bytes = buffSize * sizeof(short int);
				}
				// Ovo je jako interesantno!
				//m_pInput->MoveTo(pos);
				memset(buffer, 0, bytes);
				bytesRead = m_pInput->Read((BYTE*)buffer, bytes);
				if(EOF == bytesRead)
					break;
				samples = bytesRead / (fmt->bitsPerSample / 8);
				buffPos = 0;
			}

			short int sample = buffer[buffPos++];

			for(ch=0; ch<channels; ch++)
			{
				if(sample > max[ch])
					max[ch] = sample;
				else if(sample < min[ch])
					min[ch] = sample;
			}

			if(counter >= interval || (srcLen && (pos/channels)%srcLen==0))
			{
				for(ch=0; ch<channels; ch++)
				{
					if(!m_bDrawVertical)
					{
						if(max[ch] == 0 && min[ch] != 0)
							max[ch] = min[ch];
						else if(min[ch] == 0 && max[ch] != 0)
							min[ch] = max[ch];

						Peak peak;
						peak.value = ( (peakcount % 2) == 0 ? max[ch] : min[ch] );
						peak.pos = pos/channels;
						m_peaks.Add( peak );
					}
					else
					{
						Peak peakMax;
						peakMax.value = max[ch];
						peakMax.pos = pos/channels;
						m_peaks.Add( peakMax );

						Peak peakMin;
						peakMin.value = min[ch];
						peakMin.pos = pos/channels;
						m_peaks.Add( peakMin );
					}

					max[ch] = min[ch] = sample;
				}
				counter = 0;
				peakcount++;

				if(peakcount >= 280)
				{ int d=1; }
			} else
				counter++;
		}

		m_bLoaded = TRUE;
	}

	delete buffer;

//	m_pMutex->Unlock();

}
*/
/**
 * Draws dB line(s) for a single channel.
 */
void CRegionDisplay::drawDBLines(wxDC& dc, wxRect& rc)
{
	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int width	= rc.GetWidth();
	int height	= rc.GetHeight();
	int max		= (int)pow(2, fmt->bitsPerSample) / 2;
	int scale	= max / height;
	int dby		= (pow(2, (m_fdB/20.0)) * height) / 4.0;
	int middle	= rc.GetY() + (height / 2);
	int x1		= rc.GetX();
	int x2		= rc.GetX()+width;
	int y1		= middle - dby;
	int y2		= middle + dby;
	//int amp	= max * pow(2, (m_fdB/20.0));

	dc.DrawLine(x1, y1, x2, y1);
	dc.DrawLine(x1, y2, x2, y2);
}

/**
 * Draws a waveform of a single channel.
 */
void CRegionDisplay::drawWaveform(wxDC& dc, wxRect& rc, int start)
{
	if(!m_bLoaded)
		return;

//	wxMutexLocker lock(*m_pMutex);

	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int channels = fmt->channels;
	int left	= rc.GetX();
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();
	int mid		= rc.GetY() + height/2;
	int max		= (int)pow(2, fmt->bitsPerSample);
	int yscale	= max / height;
	int count	= m_peaks.GetCount();
	int i		= start * (m_bDrawVertical ? 2 : 1);
	int y		= mid;

	wxPoint ptPrev(start, mid);


	// Mozda je bolje da se krajevi lupova crtajy u CRegion-u?

	int top		= rc.GetTop();
	int bottom	= rc.GetBottom();
	int origLen	= getLengthNotLooped();

	// Pen for drawing those little lines at the end of the loop
	wxPen oldpen = dc.GetPen();
	wxPen pen( wxSystemSettings::GetColour(wxSYS_COLOUR_MENU), 2, wxSOLID );

	// Length of those little lines at the end of the loop
	int t = height / 8;


	int pos = 0;

	for(int x=0; x<width && i<count; x++)
	{
		if(!m_bDrawVertical)
		{
			Peak peak = m_peaks.Item(i);
			y = (int)((float)mid - peak.value/yscale);
			dc.DrawLine(left+ptPrev.x, ptPrev.y, left+x, y);

			pos = peak.pos;

			ptPrev.x = x;
			ptPrev.y = y;

			i += channels;
		}
		else
		{
			Peak peakMax = m_peaks.Item(i);
			Peak peakMin = m_peaks.Item(i+1);
			int y1 = mid - peakMax.value/yscale;
			int y2 = mid - peakMin.value/yscale;
			if(y1 != y2)
				dc.DrawLine(left+x, y1, left+x, y2);
			else
				dc.DrawPoint(left+x, mid);
			
			pos = peakMax.pos;

			i += channels*2;
		}

		// Draw the end of the loop
		if((origLen>0) && (x>1) && ((pos%origLen)==0))
		{
			dc.SetPen(pen);
			dc.DrawLine(left+x, top, left+x, top+t);
			dc.DrawLine(left+x, bottom-t, left+x, bottom);
			dc.SetPen(oldpen);
		}
	}
}

int CRegionDisplay::getLengthNotLooped()
{
	int len = 0;
	if( m_pTrack->IsLooped() )
	{
		IFloopySoundFilter *loop = (IFloopySoundFilter*)m_pTrack->GetComponent("loop");
		if(loop)
		{
			IFloopySoundInput *src = loop->GetSource();
			if(src)
				len = src->GetSize();
		}
	}
	return len;
}
