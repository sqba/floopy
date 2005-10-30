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
	m_bDither	= true;
}

CFloat2Int::~CFloat2Int()
{

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

	if(size > m_nBuffSize)
	{
		if(m_pBuffer)
			delete m_pBuffer;
		m_nBuffSize = size;
		m_pBuffer = new BYTE[m_nBuffSize];
	}

	GetEngine()->EmptyBuffer(m_pBuffer, m_nBuffSize);

	int len = IFloopySoundFilter::Read(m_pBuffer, size);

	if(len > 0)
	{
		int numsamples = len / 4;

		FLOAT_32BIT *in		= (FLOAT_32BIT*)m_pBuffer;
		SAMPLE_16BIT *out	= (SAMPLE_16BIT*)data;
		//int *out	= (int*)data;

		if (m_bDither)
			quantizeWithDither(in, out, numsamples);
		else
			quantize(in, out, numsamples);

		//while(numsamples--)
		//	*(out++) = (SAMPLE_16BIT)(*(in++) * 32768.f);
	}

	return len;
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

void CFloat2Int::quantizeWithDither(FLOAT_32BIT *in, SAMPLE_16BIT *out, int numsamples)
{
	double const d2i = (1.5 * (1 << 26) * (1 << 26));

	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	int channels = fmt->channels;
	
	do
	{
		FLOAT_32BIT val = *(in++);
		double res = ((double)val + frand()) + d2i;

		int sample = *(int *)&res;

		if (sample < SHORT_MIN)
			sample = SHORT_MIN;
		else if (sample > SHORT_MAX)
			sample = SHORT_MAX;

		*(out++) = (SAMPLE_16BIT)sample;
	} while(--numsamples);

	/*do
	{
		double res = ((double)in[0] + frand()) + d2i;

		int l = *(int *)&res;

		if (l < SHORT_MIN)
			l = SHORT_MIN;
		else if (l > SHORT_MAX)
			l = SHORT_MAX;

		if(channels == 2)
		{
			res = ((double)in[1] + frand()) + d2i;

			int r = *(int *)&res;

			if (r < SHORT_MIN)
				r = SHORT_MIN;
			else if (r > SHORT_MAX)
				r = SHORT_MAX;

			*out++ = (r << 16) | (word)l;
		}
		else
			*out++ = (l << 16) | (word)l;

		in += channels;
	} while(--numsamples);*/
}

void CFloat2Int::quantize(FLOAT_32BIT *in, SAMPLE_16BIT *out, int numsamples)
{
	double const d2i = (1.5 * (1 << 26) * (1 << 26));

	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	int channels = fmt->channels;

	do
	{
		double res = ((double)in[0]) + d2i;

		int l = *(int *)&res;

		if (l < SHORT_MIN)
			l = SHORT_MIN;
		else if (l > SHORT_MAX)
			l = SHORT_MAX;

		if(channels == 2)
		{
			res = ((double)in[1]) + d2i;

			int r = *(int *)&res;

			if (r < SHORT_MIN)
				r = SHORT_MIN;
			else if (r > SHORT_MAX)
				r = SHORT_MAX;

			*out++ = (r << 16) | (word)l;
		}
		else
			*out++ = (l << 16) | (word)l;

		in += channels;
	} while(--numsamples);
}
