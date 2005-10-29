// ConvertToFloat.cpp: implementation of the CConvertToFloat class.
//
//////////////////////////////////////////////////////////////////////

#include "ConvertToFloat.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConvertToFloat::CConvertToFloat()
{

}

CConvertToFloat::~CConvertToFloat()
{

}

SOUNDFORMAT *CConvertToFloat::GetFormat()
{
	return IFloopySoundFilter::GetFormat();
}

int CConvertToFloat::Read(BYTE *data, int size)
{
	return IFloopySoundFilter::Read(data, size);
}
