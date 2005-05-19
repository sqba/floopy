// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{

}

CInput::~CInput()
{

}

int CInput::Read(BYTE *data, int size)
{
	//return IFloopySoundInput::Read(data, size);

	BYTE *tmp = new BYTE[size/2];
	memset(tmp, 0, size/2);

	int len = IFloopySoundInput::Read(tmp, size/2);

	short int *mono = (short int*)tmp;
	short int *stereo = (short int*)data;
	int samples = size/2 / sizeof(short int);

	for(int i=0; i<samples; i++)
	{
		*(stereo++) = *mono;
		*(stereo++) = *(mono++);
	}

	delete[] tmp;

//	assert(len == size/2);

	return len * 2;
}

SOUNDFORMAT *CInput::GetFormat()
{
	memcpy(&m_format, IFloopySoundInput::GetFormat(), sizeof(SOUNDFORMAT));
	m_format.channels = 2;
	return &m_format;
}

/*
void CInput::MoveTo(int samples)
{
	IFloopySoundInput::MoveTo(samples/2);
}

int CInput::GetSize()
{
	return IFloopySoundInput::GetSize() * 2;
}
*/
/*
int CInput::Read(BYTE *data, int size)
{
	int len = IFloopySoundInput::Read(data, size);
	for(int i=0; i<len-1; i++)
	{
		*(++data) = *(++data);
	}
	return len;
}

int CInput::Read(BYTE *data, int size)
{
	BYTE *tmp = new BYTE[size*2];
	memset(tmp, 0, size*2);

	int len = IFloopySoundInput::Read(tmp, size*2);

	BYTE *pTmp = tmp;
	BYTE *pData = data;
	for(int i=0; i<len/2; i++)
	{
		*pData = *pTmp;
		*(++pData) = *pTmp;
		pData++;
		pTmp+=2;
	}

	delete[] tmp;

	return len / 2;
}
*/