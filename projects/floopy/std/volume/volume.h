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
	void Reset();

	const char *GetName()					{ return "libstd.volume"; }
	const char *GetDescription()			{ return "Volume control"; }
	const char *GetVersion()				{ return "0.1"; }
	const char *GetAuthor()				{ return "sqba"; }

	int   GetParamCount();//			{ return 3; }
	float GetParamMin(int)			{ return 0.f; }
	float GetParamMax(int)			{ return 1000.f; }
	const char *GetParamUnit(int)			{ return "%"; }
	const char *GetParamName(int);
	const char *GetParamDesc(int);
	float GetParamStep(int)			{ return 0.01f; }
	void  SetParamVal(int, float);
	bool  GetParamVal(int, float*);

	int   GetPropertyCount();
	//bool  GetPropertyIndex(char *name, int *index);
	bool  GetPropertyVal(int index, float *value);
	void  SetPropertyVal(int index, float value);
	float GetPropertyMin(int index)	{ return 0.0f; }
	float GetPropertyMax(int index)	{ return 1000.0f; }
	//float GetPropertyStep(int index);
	const char *GetPropertyName(int index);
	const char *GetPropertyDesc(int index);
	const char *GetPropertyUnit(int index){ return "%"; }

	bool CanReadSourceIfDisabled()		{ return true; }

private:
	int getChannels();

private:
	int m_master[2];	/// Property (not on the timeline)
	int m_volume[2];	/// Parameter
};

#endif // !defined(AFX_VOLUME_H__05C04C12_471D_4FD8_BCD1_350030686D0F__INCLUDED_)
