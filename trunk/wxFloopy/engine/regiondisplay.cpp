// RegionDisplay.cpp: implementation of the CRegionDisplay class.
//
//////////////////////////////////////////////////////////////////////

#include "../engine/track.h"
#include "../engine/region.h"
#include "../engine/tracks.h"
#include "../engine/regiondisplay.h"
#include "../globals.h"


#include <math.h>


CRegionDisplay::CRegionDisplay(CRegion *region) : IFloopyObj(region)
{
	m_pRegion		= region;
	m_pTrack		= (CTrack*)m_pRegion->GetParent();
	m_pTracks		= (CTracks*)m_pTrack->GetParent();
	m_pInput		= m_pTrack->GetSource();
	m_bLoaded		= false;
	m_bDrawVertical = false;
	m_bRepaint		= false;

	m_pFirstPeak	= NULL;
	m_pLastPeak		= NULL;
	m_iPeakCount	= 0;

	m_fdB			= DB_LINES;
	m_bDrawDBLines	= DRAW_DB_LINES;
	m_bDrawContour	= DRAW_WAVE_CONTOUR;
	m_iBufferLength	= BUFFER_LENGTH;
}

CRegionDisplay::~CRegionDisplay()
{
	clear();
}

void CRegionDisplay::DrawBG(wxDC& dc, wxRect& rc)
{
	if(!m_bDrawDBLines)
		return;

	SOUNDFORMAT *fmt	= m_pInput->GetFormat();
	int channels		= fmt->channels;
//	int width			= rc.GetWidth();
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
	m_bLoaded = false;

	clear();

	if(!m_pRegion || !m_pTrack || !m_pTracks || !m_pInput)
		return;

	int interval = m_pTracks->GetSamplesPerPixel() - 1;
	if(interval < 0)
		return;

	m_bDrawVertical = (interval > 1);

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

	bool bMax		= true;
	int counter		= interval;					// Load first sample
	int peakcount	= 0;						// Number of peaks loaded
	int ch			= 0;						// Channel counter
	int buffPos		= 0;						// Buffer position
	int totalSamples = (end - start) * channels;
	int buffLen		= m_iBufferLength>0 ? m_iBufferLength : totalSamples;	// Samples
	int size		= buffLen;					// Remaining buffer size

	SAMPLE min[2]={0}, max[2]={0};				// Current peaks
	SAMPLE prev[2] = {0};						// Previous peak (when !m_bDrawVertical)

	SAMPLE *buffer = new SAMPLE[buffLen];		// Create the buffer

	m_pInput->MoveTo(start);					// Move to the beginning of the region

	m_pTracks->SetViewUpdatedWhilePlaying(true);

	for(int pos=channels; pos<=totalSamples; pos+=channels)
	{
		// Load the chunk
		if(buffPos==0 || buffPos>=size)
		{
			if( buffPos+size > totalSamples )
				size = totalSamples - pos;		// Fewer samples left than the buffer accepts
			if(size <= 0)
				break;
			int bytes = size * sizeof(SAMPLE);	// Buffer size in bytes
			memset(buffer, 0, bytes);			// Fill with silence instead!
			int bytesRead = m_pInput->Read((BYTE*)buffer, bytes);	// Load the chunk
			if(EOF == bytesRead)				// No data inside, exit the loop
				break;
			size = bytesRead / (fmt->bitsPerSample / 8);
			buffPos = 0;						// Reset buffer position
		}

		// Find peaks
		for(ch=0; ch<channels; ch++)
		{
			SAMPLE sample = buffer[buffPos+ch];

			if(sample > max[ch])
				max[ch] = sample;
			else if(sample < min[ch])
				min[ch] = sample;
		}

		// Time to add the peak to the array
		if(counter >= interval)
		{
			for(ch=0; ch<channels; ch++)
			{
				SAMPLE sample = buffer[buffPos+ch];
				Peak *peak = new Peak;
				if(!m_bDrawVertical)
				{
					if(max[ch]==0 && min[ch]!=0)
						max[ch] = min[ch];
					else if(min[ch]==0 && max[ch]!=0)
						min[ch] = max[ch];

					// Trick: x % a = x & (a - 1) for binary numbers
					//SAMPLE val = (peakcount%2) == 0 ? max[ch] : min[ch];
					//SAMPLE val = (peakcount&1) == 0 ? max[ch] : min[ch];
					SAMPLE val = bMax ? max[ch] : min[ch];

					bMax = !bMax;

					peak->value1 = prev[ch];
					peak->value2 = prev[ch] = val;
				}
				else							// We're drawing vertical lines
				{
					peak->value1 = max[ch];		// Top point
					peak->value2 = min[ch];		// Bottom point
				}
				peak->pos = pos/channels;		// Sample offset
				add( peak );
				max[ch] = min[ch] = sample;		// New peak values
			}
			counter = 0;						// Reset the counter
			peakcount++;						// Increase peak counter
		} else
			counter++;							// Increase the counter

		buffPos += channels;					// Increase buffer position
	}

	delete[] buffer;

	m_bLoaded = m_bRepaint = true;
}

/**
 * Draws dB lines for a single channel.
 */
void CRegionDisplay::drawDBLines(wxDC& dc, wxRect& rc)
{
	if(0.f == m_fdB)
		return;

//	SOUNDFORMAT *fmt = m_pInput->GetFormat();

//	int top		= rc.GetTop();
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
//	int count	= m_iPeakCount;
	int right	= left + width;

	Peak *peak	= m_pFirstPeak;

	for(int i=0; i<start; i++)
		peak = peak->next;	// Move to the start channel

	int prevMin	= middle, prevMax = middle;

	for(int x=left; x<right && NULL!=peak; x++)
	{
		int y1 = middle - peak->value1 / yscale;
		int y2 = middle - peak->value2 / yscale;

		if( m_bDrawVertical )
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
		else
		{
			dc.DrawLine(x-1, y1, x, y2);
		}

		for(int c=0; c<channels; c++)
		{
			// Move to the next sample from the same channel
			if(peak)
				peak = peak->next;
		}
	}
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

void CRegionDisplay::add(CRegionDisplay::Peak *peak)
{
	if(NULL == m_pLastPeak)
		m_pFirstPeak = m_pLastPeak = peak;
	else
		m_pLastPeak = m_pLastPeak->next = peak;
	m_iPeakCount++;
	peak->next = NULL;
}

void CRegionDisplay::clear()
{
	Peak *peak = m_pFirstPeak;
	while(peak)
	{
		Peak *tmp = peak;
		peak = peak->next;
		delete tmp;
	}
	m_pFirstPeak = m_pLastPeak = NULL;
	m_iPeakCount = 0;
}


int CRegionDisplay::GetPropertyCount()
{
	return 4;
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
	case 3:
		*value = (float)m_iBufferLength;
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
	case 3:
		m_iBufferLength = (int)value;
		return;
	}
}

const char *CRegionDisplay::GetPropertyName(int index)
{
	switch(index)
	{
	case 0: return "DbLines";
	case 1: return "DrawDbLines";
	case 2: return "DrawContour";
	case 3: return "BufferLength";
	}
	return NULL;
}

const char *CRegionDisplay::GetPropertyDesc(int index)
{
	switch(index)
	{
	case 0: return "Decibell Lines";
	case 1: return "Draw Decibell Lines";
	case 2: return "Draw Contour";
	case 3: return "Buffer Length";
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
	case 3: return 0.f;
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
	case 3: return 2048;
	}
	return 0.f;
}

const char *CRegionDisplay::GetPropertyUnit(int index)
{
	switch(index)
	{
	case 0: return "Db";
	case 1: return "On/Off";
	case 2: return "On/Off";
	case 3: return "Samples";
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
	case 3: return 32.f;
	}
	return 0.f;
}
