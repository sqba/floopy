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

	BOOL ReadSourceIfDisabled()	{ return FALSE; }
};

#endif // !defined(AFX_TRACK_H__966CC29E_48E0_487B_B8B3_A7DDDF36967B__INCLUDED_)
