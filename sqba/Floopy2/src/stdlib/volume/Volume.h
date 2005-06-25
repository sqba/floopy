// Volume.h: interface for the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_)
#define AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

#include <string.h>

class CVolume : public IFloopySoundInput
{
public:
	CVolume();
	virtual ~CVolume();

	int Read(BYTE *data, int size);

	char *GetName()			{ return "stdlib.volume"; }
	char *GetDescription()	{ return "Volume control"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParamVal(int index, float value) { m_volume = (int)value; }
	BOOL  GetParamVal(int index, float *value);
	char *GetParamName(int index)	{ return (index == 0 ? "volume" : NULL); }
	char *GetParamDesc(int index)	{ return (index == 0 ? "Volume" : NULL); }
	char *GetParamUnit(int index)	{ return (index == 0 ? "%" : NULL); }
	float GetParamMin(int index)	{ return 0.f; }
	float GetParamMax(int index)	{ return 1000.f; }
	float GetParamStep(int index)	{ return 0.01f; }

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

private:
	int m_volume; // In percents
};

#endif // !defined(AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_)
