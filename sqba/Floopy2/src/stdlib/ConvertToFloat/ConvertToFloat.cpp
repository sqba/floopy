// ConvertToFloat.cpp: implementation of the CConvertToFloat class.
//
//////////////////////////////////////////////////////////////////////

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
	if(m_pBuffer)
		delete m_pBuffer;
}

SOUNDFORMAT *CConvertToFloat::GetFormat()
{
	memcpy(&m_format, IFloopySoundFilter::GetFormat(), sizeof(SOUNDFORMAT));
	m_format.sampleDataType = SAMPLE_DATA_TYPE_FLOAT;
	return &m_format;
}

int CConvertToFloat::Read(BYTE *data, int size)
{
	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL == fmt)
		return 0;

	if(fmt->sampleDataType != SAMPLE_DATA_TYPE_INTEGER)
		return IFloopySoundFilter::Read(m_pBuffer, size);

	switch(fmt->bitsPerSample)
	{
	case 8:
		size /= 4;
		break;
	case 16:
		size /= 2;
		break;
	}

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
		switch(fmt->bitsPerSample)
		{
		case 8:
			return convert8bitToFloat((SAMPLE_8BIT*)m_pBuffer, (FLOAT_32BIT*)data, size);
		case 16:
			return convert16bitToFloat((SAMPLE_16BIT*)m_pBuffer, (FLOAT_32BIT*)data, size);
		}
	}

	return 0;
}

int CConvertToFloat::convert8bitToFloat(SAMPLE_8BIT *in, FLOAT_32BIT *out, int size)
{
	if(size != EOF)
	{
		int numsamples = size;

		while(numsamples--)
			*(out++) = (FLOAT_32BIT)((SAMPLE_16BIT)(*(in++)) * 256 - 32768) / 32768.f;
	}

	return size;
}

int CConvertToFloat::convert16bitToFloat(SAMPLE_16BIT *in, FLOAT_32BIT *out, int size)
{
	if(size != EOF)
	{
		int numsamples = size / 2;

		while(numsamples--)
			*(out++) = (FLOAT_32BIT)(*(in++)) / 32768.f;
	}
	return size / 2;
}
