// Invert.cpp: implementation of the CInvert class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>

#include "invert.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInvert::CInvert()
{

}

CInvert::~CInvert()
{

}

int CInvert::Read(BYTE *data, int size)
{
	int len = IFloopySoundFilter::Read(data, size);

	SOUNDFORMAT *fmt = GetFormat();
	if(!fmt || !fmt->bitsPerSample || !fmt->channels)
		return len;

	if(EOF != len)
	{
		int numsamples = len / (fmt->bitsPerSample/8);

		short int *sample = (short int*)data;

		int pos = numsamples;
		
		while(pos--) {
			*(sample++) *= -1;
		};
	}
	return len;
}
