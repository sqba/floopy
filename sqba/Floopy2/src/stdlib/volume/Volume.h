// Volume.h: interface for the CVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_)
#define AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

//#include <string.h>


class CVolume : public IFloopySoundFilter
{
public:
	CVolume();
	virtual ~CVolume();

	int Read(BYTE*, int);

	char *GetName()					{ return "stdlib.volume"; }
	char *GetDescription()			{ return "Volume control"; }
	char *GetVersion()				{ return "0.1"; }
	char *GetAuthor()				{ return "sqba"; }

	int   GetParamCount()			{ return 3; }
	float GetParamMin(int)			{ return 0.f; }
	float GetParamMax(int)			{ return 1000.f; }
	char *GetParamUnit(int)			{ return "%"; }
	char *GetParamName(int);
	char *GetParamDesc(int);
	float GetParamStep(int)			{ return 0.01f; }
	void  SetParamVal(int, float);
	bool  GetParamVal(int, float*);

	bool ReadSourceIfDisabled()		{ return false; }

private:
	int getChannels();

private:
	int m_left, m_right; // In percents
};

#endif // !defined(AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_)
