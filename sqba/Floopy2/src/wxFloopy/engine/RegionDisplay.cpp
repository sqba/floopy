// RegionDisplay.cpp: implementation of the CRegionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "../engine/track.h"
#include "../engine/region.h"
#include "../engine/tracks.h"
#include "../engine/regiondisplay.h"


#include <math.h>
#include <wx/arrimpl.cpp>
//#include <wx/dcbuffer.h>


#define SAMPLE	short int


WX_DEFINE_OBJARRAY(PeaksArray);


CRegionDisplay::CRegionDisplay(CRegion *region) : IFloopyObj(region)
{
	m_pRegion		= region;
	m_pTrack		= (CTrack*)m_pRegion->GetParent();
	m_pTracks		= (CTracks*)m_pTrack->GetParent();
	//m_pInput		= m_pTrack->GetInput();
	m_pInput		= m_pTrack->GetSource();
//	m_pTrackInput	= m_pTrack->GetTrack();
	m_fdB			= -6.f;
	m_bDrawDBLines	= true;
	m_bLoaded		= false;
	m_bDrawVertical = false;
	m_bRepaint		= false;
	m_bDrawContour	= false;

	LoadPeaks();
}

CRegionDisplay::~CRegionDisplay()
{
	//WX_CLEAR_ARRAY(m_peaks);
}

void CRegionDisplay::DrawBG(wxDC& dc, wxRect& rc)
{
	if(!m_bDrawDBLines)
		return;

	SOUNDFORMAT *fmt	= m_pInput->GetFormat();
	int channels		= fmt->channels;
	int width			= rc.GetWidth();
	int height			= rc.GetHeight() / channels;

	wxRect rcChannel(rc);
	rcChannel.SetHeight(height);
	for(int i=0; i<channels; i++)
	{
		rcChannel.Offset(0, height*i);
		drawDBLines(dc, rcChannel);
		drawCenterLine(dc, rcChannel);
	}
}

void CRegionDisplay::DrawFore(wxDC& dc, wxRect& rc)
{
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();

	if(m_bRepaint || height!=m_tempBitmap.GetHeight())
	{
		SOUNDFORMAT *fmt = m_pInput->GetFormat();
		int channels = fmt->channels;
		int channelHeight = height / channels;

		m_tempDC.SetPen(dc.GetPen());
		m_tempBitmap.Create(width, height, -1);
		m_tempDC.SelectObject(m_tempBitmap);
		m_tempDC.Clear();

		wxRect rcTmp(0, 0, width, channelHeight);
		for(int i=0; i<channels; i++)
		{
			rcTmp.Offset(0, channelHeight*i);
			drawPeaks(m_tempDC, rcTmp, i);
		}

		m_bRepaint = false;
	}

	//dc.Blit(rc.GetX(), rc.GetY(), width, height, &m_tempDC, 0, 0, wxAND);
	
	int logicalFunction = m_pRegion->IsSelected() ? wxAND_INVERT : wxAND;
	dc.Blit(rc.GetX(), rc.GetY(), width, height, &m_tempDC, 0, 0, logicalFunction);



	/*wxBufferedDC bdc(&dc, wxSize(rc.GetWidth(), rc.GetHeight()));
	wxRect rcTmp = rc;
	for(int i=0; i<channels; i++)
	{
		rcTmp.SetY(rc.GetY() + height*i);
		rcTmp.SetHeight(height);
		drawPeaks(bdc, rcTmp, i);
	}*/


	// Drawing directly on the canvas
	/*wxRect rcTmp = rc;
	for(int i=0; i<channels; i++)
	{
		rcTmp.SetY(rc.GetY() + height/2*i);
		rcTmp.SetHeight(height/2);
		drawPeaks(dc, rcTmp, i);
	}*/
}

void CRegionDisplay::LoadPeaks()
{
	if(NULL == m_pRegion)
		return;

	if(NULL == m_pTrack)
		return;

	if(NULL == m_pTracks)
		return;

	if(NULL == m_pInput)
		return;

//	if(NULL == m_pTrackInput)
//		return;

	//loadPeaks();
	loadPeaksChunked();
}


/**
 * Loads peaks in one big chunk.
 */
void CRegionDisplay::loadPeaks()
{
	m_bLoaded = false;

	m_peaks.Empty();

	int interval = m_pTracks->GetSamplesPerPixel() - 1;
	if(interval < 0)
		return;

	m_bDrawVertical = (interval > 10);

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	if(NULL == fmt)
		return;
	int channels = fmt->channels;
	if(0 == channels)
		return;

	int start	= m_pRegion->GetStartPos();
	int end		= m_pRegion->GetEndPos();
	if(end<=0 || start<0)
		return;

	int samples	= (end - start) * channels;

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
	
	m_pTracks->SetViewUpdatedWhilePlaying(true);
	
	if(EOF != bytesRead)
	{
		samples = bytesRead / (fmt->bitsPerSample / 8);

		int srcLen = getLengthNotLooped();
		int counter=interval;
		int peakcount=0;
		int ch=0;

		short int min[2]={0}, max[2]={0};
		short int prev[2] = {0};

		for(int pos=channels; pos<samples; pos+=channels)
		{
			for(ch=0; ch<channels; ch++)
			{
				short int sample = buffer[pos-channels+ch];

				if(sample > max[ch])
					max[ch] = sample;
				else if(sample < min[ch])
					min[ch] = sample;
			}

			//if(counter >= interval || (srcLen && pos%srcLen==0))
			if(counter >= interval || (srcLen && (pos&(srcLen-1))==0))
			{
				for(ch=0; ch<channels; ch++)
				{
					short int sample = buffer[pos-channels+ch];

					Peak peak;

					if(!m_bDrawVertical)
					{
						if(max[ch]==0 && min[ch]!=0)
							max[ch] = min[ch];
						else if(min[ch]==0 && max[ch]!=0)
							min[ch] = max[ch];

						peak.prev = prev[ch];
						//peak.value = ( (peakcount%2) == 0 ? max[ch] : min[ch] );
						peak.value = ( (peakcount&1) == 0 ? max[ch] : min[ch] );

						prev[ch] = peak.value;
					}
					else
					{
						peak.prev = max[ch];
						peak.value = min[ch];
					}

					peak.pos = pos/channels;
					m_peaks.Add( peak );

					max[ch] = min[ch] = sample;
				}
				counter = 0;
				peakcount++;
			} else
				counter++;
		}

		m_bLoaded = true;
	}

	delete buffer;

	m_bRepaint = true;
}

/**
 * Loads peaks in small chunks.
 */
void CRegionDisplay::loadPeaksChunked()
{
	m_bLoaded = false;

	m_peaks.Empty();

	int interval = m_pTracks->GetSamplesPerPixel() - 1;
	if(interval < 0)
		return;

	m_bDrawVertical = (interval > 85);

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	if(NULL == fmt)
		return;
	int channels = fmt->channels;
	if(0 == channels)
		return;

	int start = m_pRegion->GetStartPos();
	int end   = m_pRegion->GetEndPos();
	if(end<=0 || start<0)
		return;
	
	m_pTracks->SetViewUpdatedWhilePlaying(true);

	int srcLen		= getLengthNotLooped();	// Length of the not looped source
	int counter		= interval;				// Load first sample
	int peakcount	= 0;					// Number of peaks loaded
	int ch			= 0;					// Channel counter
	int totalSamples = (end - start) * channels;
	int buffSize	= 128;
	int buffPos		= 0;					// Buffer position

	SAMPLE min[2]={0}, max[2]={0};			// Largest and smallest sample values
	SAMPLE prev[2] = {0};					// Previous peak value

	SAMPLE *buffer = new SAMPLE[buffSize];	// Create the buffer

	m_pInput->MoveTo(start);				// Move to the beginning of the region

	for(int pos=channels; pos<=totalSamples; pos+=channels)
	{
		// Load the chunk because we're at the
		// beginning or the end of the chunk
		if((buffPos == 0) || (buffPos >= buffSize))
		{
			// Fewer samples left than the buffer accepts
			if( buffPos + buffSize > totalSamples )
				buffSize = totalSamples - pos;

			if(buffSize <= 0)
				break;

			//m_pInput->MoveTo(pos); // <-- Ovde se neshto jako interesantno deshava!

			int bytes = buffSize * sizeof(SAMPLE);	// Buffer size in bytes

			memset(buffer, 0, bytes);		// Fill with silence instead!

			int bytesRead = m_pInput->Read((BYTE*)buffer, bytes);	// Load the chunk

			if(EOF == bytesRead)			// No data inside, exit the loop
				break;

			buffSize = bytesRead / (fmt->bitsPerSample / 8);

			buffPos = 0;					// Reset buffer position
		}

		// Find largest and smallest sample values
		for(ch=0; ch<channels; ch++)
		{
			SAMPLE sample = buffer[buffPos+ch];

			if(sample > max[ch])
				max[ch] = sample;
			else if(sample < min[ch])
				min[ch] = sample;
		}

		// Time to add peak to the array
		if(counter >= interval)// || (srcLen && pos%srcLen==0))
		{
			for(ch=0; ch<channels; ch++)
			{
				SAMPLE sample = buffer[buffPos+ch];

				Peak peak;

				if(!m_bDrawVertical)
				{
					if(max[ch] == 0 && min[ch] != 0)
						max[ch] = min[ch];
					else if(min[ch] == 0 && max[ch] != 0)
						min[ch] = max[ch];

					peak.prev	= prev[ch];
					// Trick: x % a = x & (a - 1) for binary numbers
					//SAMPLE val	= (peakcount%2) == 0 ? max[ch] : min[ch];
					SAMPLE val	= (peakcount&1) == 0 ? max[ch] : min[ch];
					peak.value	= val;

					prev[ch]	= val;
				}
				else						// We're drawing vertical lines
				{
					peak.prev	= max[ch];	// Top point
					peak.value	= min[ch];	// Bottom point
				}

				peak.pos = pos/channels;	// Sample offset
				
				m_peaks.Add( peak );
				
				max[ch] = min[ch] = sample;	// New value
			}
			counter = 0;					// Reset the counter
			peakcount++;					// Increase peak counter
		} else
			counter++;						// Increase the counter

		buffPos += channels;				// Increase buffer position
	}

	delete buffer;

	m_bRepaint = true;
	m_bLoaded = true;
}

/**
 * Draws dB lines for a single channel.
 */
void CRegionDisplay::drawDBLines(wxDC& dc, wxRect& rc)
{
	if(0.f == m_fdB)
		return;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int top		= rc.GetTop();
	int left	= rc.GetX();
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();
	int middle	= rc.GetY() + height/2;
	int max		= getMaxSampleValue();
	int amp		= pow(10, (m_fdB/20.f + log10(max)));	/// Amplitude at m_fdB Db
	int yscale	= max / height * 2;
	int x1		= left;
	int x2		= left + width;
	int y1		= middle - amp / yscale;
	int y2		= middle + amp / yscale;

	dc.DrawLine(x1, y1, x2, y1);
	dc.DrawLine(x1, y2, x2, y2);
}

/**
 * Draws a center line.
 */
void CRegionDisplay::drawCenterLine(wxDC& dc, wxRect& rc)
{
	int x1	= rc.GetX();
	int x2	= x1 + rc.GetWidth();
	int y	= rc.GetY() + (rc.GetHeight() / 2);
	dc.DrawLine(x1, y, x2, y);
}

/**
 * Draws a waveform of a single channel.
 */
void CRegionDisplay::drawPeaks(wxDC& dc, wxRect& rc, int start)
{
	if(!m_bLoaded)
		return;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int channels = fmt->channels;
	int left	= rc.GetX();
	int width	= rc.GetWidth();
	int height	= rc.GetHeight();
	int middle	= rc.GetY() + height/2;
	int max		= getMaxSampleValue();
	int yscale	= max / height * 2;
	int count	= m_peaks.GetCount();
	int i		= start;
	int right	= left + width;

	int prevMin	= middle, prevMax = middle;

	for(int x=left; x<right && i<count; x++)
	{
		Peak peak = m_peaks.Item(i);

		int y1 = middle - peak.prev  / yscale;
		int y2 = middle - peak.value / yscale;

		if(!m_bDrawVertical)
		{
			dc.DrawLine(x-1, y1, x, y2);
		}
		else
		{
			if(y1 != y2)
			{
				if( m_bDrawContour )
				{
					dc.DrawLine(x-1, prevMax, x, y1);
					dc.DrawLine(x-1, prevMin, x, y2);

					prevMax = y1;
					prevMin = y2;
				}
				else
					dc.DrawLine(x, y1, x, y2);
			}
			else
				dc.DrawPoint(x, y1);
		}

		i += channels;
	}
}

/**
 * Returns the original source length, before the looping component.
 */
int CRegionDisplay::getLengthNotLooped()
{
	int len = 0;
	if( m_pTrack->IsLooped() )
	{
		IFloopySoundFilter *loop = (IFloopySoundFilter*)m_pTrack->FindComponentByName("loop");
		if(loop)
		{
			IFloopySoundInput *src = loop->GetSource();
			if(src)
				len = src->GetSize();
		}
	}
	return len;
}

/**
 * Returns maximum sample value.
 */
int CRegionDisplay::getMaxSampleValue()
{
	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	if(!fmt || fmt->bitsPerSample<=0)
		return 0;

	int max = (int)pow(2, fmt->bitsPerSample);
	if(fmt->bitsPerSample == 16)
	{
		max /= 2;
		max -= 1;
	}

	return max;
}



int CRegionDisplay::GetPropertyCount()
{
	return 3;
}

bool CRegionDisplay::GetPropertyVal(int index, float *value)
{
	switch(index)
	{
	case 0:
		*value = m_fdB;
		return true;
	case 1:
		*value = (float)m_bDrawDBLines;
		return true;
	case 2:
		*value = (float)m_bDrawContour;
		return true;
	}
	return false;
}

void CRegionDisplay::SetPropertyVal(int index, float value)
{
	switch(index)
	{
	case 0:
		m_fdB = value;
		return;
	case 1:
		m_bDrawDBLines = (value==0.f ? false : true);
		return;
	case 2:
		m_bDrawContour = (value==0.f ? false : true);
		return;
	}
}

char *CRegionDisplay::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "DbLines";
	case 1: return "DrawDbLines";
	case 2: return "DrawContour";
	}
	return NULL;
}

char *CRegionDisplay::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Decibell Lines";
	case 1: return "Draw Decibell Lines";
	case 2: return "Draw Contour";
	}
	return NULL;
}

float CRegionDisplay::GetPropertyMin(int index)
{
	switch(index)
	{
	case 0: return -24.f;
	case 1: return 0.f;
	case 2: return 0.f;
	}
	return 0.f;
}

float CRegionDisplay::GetPropertyMax(int index)
{
	switch(index)
	{
	case 0: return 0.f;
	case 1: return 1.f;
	case 2: return 1.f;
	}
	return 0.f;
}

char *CRegionDisplay::GetPropertyUnit(int index)
{
	switch(index)
	{
	case 0: return "Db";
	case 1: return "On/Off";
	case 2: return "On/Off";
	}
	return NULL;
}

float CRegionDisplay::GetPropertyStep(int index)
{
	switch(index)
	{
	case 0: return 0.1f;
	case 1: return 1.f;
	case 2: return 1.f;
	}
	return 0.f;
}
