// ConvertToFloat.cpp: implementation of the CConvertToFloat class.
//
//////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "ConvertToFloat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvertToFloat::CConvertToFloat()
{
	m_pBuffer	= NULL;
	m_nBuffSize	= 0;
}

CConvertToFloat::~CConvertToFloat()
{
	if(NULL != m_pBuffer)
		delete m_pBuffer;
}

SOUNDFORMAT *CConvertToFloat::GetFormat()
{
	memcpy(&m_format, IFloopySoundFilter::GetFormat(), sizeof(SOUNDFORMAT));
	m_format.sampleDataType = SAMPLE_DATA_TYPE_FLOAT;
	m_format.bitsPerSample = 32;
	return &m_format;
}

int CConvertToFloat::Read(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL == fmt)
		return 0;

	if(fmt->sampleDataType != SAMPLE_DATA_TYPE_INTEGER)
		return IFloopySoundFilter::Read(m_pBuffer, size);

	int bytes = size;

	/*switch(fmt->bitsPerSample)
	{
	case 8:
		bytes /= 4;
		break;
	case 16:
		bytes /= 2;
		break;
	}*/

	bytes /= (32 / fmt->bitsPerSample);

	if(bytes > m_nBuffSize)
	{
		if(m_pBuffer)
			delete m_pBuffer;
		m_nBuffSize = bytes;
		m_pBuffer = new BYTE[m_nBuffSize];
	}

	GetEngine()->EmptyBuffer(m_pBuffer, m_nBuffSize);

	int len = IFloopySoundFilter::Read(m_pBuffer, bytes);

	int result = 0;

	if(len > 0)
	{
		switch(fmt->bitsPerSample)
		{
		case 8:
			result = convert8bitToFloat(m_pBuffer, data, len);
			break;
		case 16:
			result = convert16bitToFloat(m_pBuffer, data, len);
			break;
		}
	}

//	assert(result == size);

	return result;
}

int CConvertToFloat::convert8bitToFloat(BYTE *inBuff, BYTE *outBuff, int inBytes)
{
	if(inBytes > 0)
	{
		SAMPLE_8BIT  *in = (SAMPLE_8BIT*)inBuff;
		FLOAT_32BIT *out = (FLOAT_32BIT*)outBuff;

		int numsamples = inBytes / 4;

		while(numsamples--)
			*(out++) = (FLOAT_32BIT)(*(in++) * 256 - 32768) / 32768.f;

		return inBytes / 4;
	}
	return 0;
}

int CConvertToFloat::convert16bitToFloat(BYTE *inBuff, BYTE *outBuff, int inBytes)
{
	if(inBytes > 0)
	{
		SAMPLE_16BIT *in = (SAMPLE_16BIT*)inBuff;
		FLOAT_32BIT *out = (FLOAT_32BIT*)outBuff;

		int numsamples = inBytes / 2;

		int n=0;

		while(numsamples--)
		{
			//*(out++) = (FLOAT_32BIT)(*(in++)) / 32768.f;
			FLOAT_32BIT sample = (FLOAT_32BIT)(*in) / 32768.f;
			*out = sample;
			in++;
			out++;
			n++;
		}
		//return inBytes / 2;
		return n * 4;
	}
	return 0;
}

void CConvertToFloat::MoveTo(int samples)
{
	/*SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL != fmt)
	{
		switch(fmt->bitsPerSample)
		{
		case 8:
			samples /= 4;
			break;
		case 16:
			samples /= 2;
			break;
		}
	}*/
	IFloopySoundFilter::MoveTo(samples);
}
