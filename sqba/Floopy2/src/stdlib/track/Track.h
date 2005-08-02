// Track.h: interface for the CTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRACK_H__966CC29E_48E0_487B_B8B3_A7DDDF36967B__INCLUDED_)
#define AFX_TRACK_H__966CC29E_48E0_487B_B8B3_A7DDDF36967B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

class CTrack : public IFloopySoundFilter  
{
public:
	CTrack();
	virtual ~CTrack();

	enumClassType GetType() { return TYPE_FLOOPY_SOUND_TRACK; }

	char *GetName()				{ return "stdlib.track"; }
	char *GetDescription()		{ return "Audio track"; }
	char *GetVersion()			{ return "0.1"; }
	char *GetAuthor()			{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParamVal(int index, float value) { m_bReset = (value!=0); }
	BOOL  GetParamVal(int index, float *value);
	char *GetParamName(int index)	{ return (index == 0 ? "reset" : NULL); }
	char *GetParamDesc(int index)	{ return (index == 0 ? "Reset" : NULL); }

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

	void  Enable(BOOL bEnabled);

private:
	BOOL m_bReset;
};

#endif // !defined(AFX_TRACK_H__966CC29E_48E0_487B_B8B3_A7DDDF36967B__INCLUDED_)
