// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	m_volume = 2;
}

CInput::~CInput()
{

}

UINT CInput::Read(BYTE *data, UINT size)
{
	UINT len = IFloopySoundInput::Read(data, size);

	short int *sample = (short int*)data;

	WAVFORMAT *fmt = GetFormat();
	//UINT numsamples = size / ((fmt->size/8) * fmt->channels);
	UINT numsamples = len / (fmt->size/8);

	int max = (int)pow(2, fmt->size) / 2;

	short int coef = m_volume / 100;

	int i = numsamples;
	while(i--)
	{
		*(sample++) += (*sample * coef);
	}
	return len;
}
