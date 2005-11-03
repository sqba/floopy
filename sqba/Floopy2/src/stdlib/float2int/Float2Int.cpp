// Float2Int.cpp: implementation of the CFloat2Int class.
//
//////////////////////////////////////////////////////////////////////

#include "Float2Int.h"





typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;

#define SHORT_MIN	-32768
#define SHORT_MAX	32767





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFloat2Int::CFloat2Int()
{
	m_pBuffer	= NULL;
	m_nBuffSize	= 0;
	m_bDither	= false;
}

CFloat2Int::~CFloat2Int()
{
	if(NULL != m_pBuffer)
		delete m_pBuffer;
}

SOUNDFORMAT *CFloat2Int::GetFormat()
{
	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL != fmt)
	{
		memcpy(&m_format, fmt, sizeof(SOUNDFORMAT));
		m_format.sampleDataType = SAMPLE_DATA_TYPE_INTEGER;
		m_format.bitsPerSample = 16;
	}
	return &m_format;
}

int CFloat2Int::Read(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL == fmt)
		return 0;

	if(fmt->sampleDataType != SAMPLE_DATA_TYPE_FLOAT)
		return IFloopySoundFilter::Read(m_pBuffer, size);

	int bytes = size * 2;

	if(bytes > m_nBuffSize)
	{
		if(m_pBuffer)
			delete m_pBuffer;
		m_nBuffSize = bytes;
		m_pBuffer = new BYTE[m_nBuffSize];
	}

	GetEngine()->EmptyBuffer(m_pBuffer, m_nBuffSize);

	int len = IFloopySoundFilter::Read(m_pBuffer, bytes);

	if(len > 0)
	{
		int numsamples = len / 4;

		FLOAT_32BIT *in		= (FLOAT_32BIT*)m_pBuffer;
		SAMPLE_16BIT *out	= (SAMPLE_16BIT*)data;

		quantize(in, out, numsamples);
	}

	return size;
}

void CFloat2Int::MoveTo(int samples)
{
	IFloopySoundFilter::MoveTo(samples);
	//IFloopySoundFilter::MoveTo(samples*2);
}




void CFloat2Int::quantize(FLOAT_32BIT *in, SAMPLE_16BIT *out, int numsamples)
{
	while(numsamples--)
	{
		FLOAT_32BIT val = *in;
		double dither = m_bDither ? frand() : 0;
		double res = (double)val + dither;

		SAMPLE_16BIT sample = (SAMPLE_16BIT)(res * 32768.0);

		if (sample < SHORT_MIN)
			sample = SHORT_MIN;
		else if (sample > SHORT_MAX)
			sample = SHORT_MAX;

		*out = sample;

		in++;
		out++;
	}
}

// returns random value between 0 and 1
// i got the magic numbers from csound so they should be ok but 
// I haven't checked them myself
double CFloat2Int::frand()
{
	static long stat = 0x16BA2118;
	stat = (stat * 1103515245 + 12345) & 0x7fffffff;
	return (double)stat * (1.0 / 0x7fffffff);
}
/*
void CFloat2Int::quantize(FLOAT_32BIT *pin, SAMPLE_16BIT *piout, int c)
{
	double const d2i = (1.5 * (1 << 26) * (1 << 26));

	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	int channels = fmt->channels;

	do
	{
		double dither = m_bDither ? frand() : 0;
		double res = ((double)pin[1] + dither) + d2i;

		int r = *(int *)&res;

		if (r < SHORT_MIN)
			r = SHORT_MIN;
		else if (r > SHORT_MAX)
			r = SHORT_MAX;

		res = ((double)pin[0] + frand()) + d2i;
		int l = *(int *)&res;

		if (l < SHORT_MIN)
			l = SHORT_MIN;
		else if (l > SHORT_MAX)
			l = SHORT_MAX;

		*piout++ = (r << 16) | (word)l;
		pin += channels;
	} while(--c);
}
*/