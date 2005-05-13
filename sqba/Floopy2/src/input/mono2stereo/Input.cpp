// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Input.h"

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
	BYTE *tmp = new BYTE[size/2];

	int len = IFloopySoundInput::Read(tmp, size/2);

	for(int i=0; i<size/2; i++)
	{
		*(data++) = *(data++) = *tmp;
	}

	delete[] tmp;

	return 0;
}

void CInput::MoveTo(int samples)
{
	IFloopySoundInput::MoveTo(samples/2);
}
