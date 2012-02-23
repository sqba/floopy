// Mono2Stereo.h: interface for the CMono2Stereo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONO2STEREO_H__FA02682D_F5C0_4929_97AE_6F64D57BF411__INCLUDED_)
#define AFX_MONO2STEREO_H__FA02682D_F5C0_4929_97AE_6F64D57BF411__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

class CMono2Stereo : public IFloopySoundFilter
{
public:
	CMono2Stereo();
	virtual ~CMono2Stereo();

	char *GetName()			{ return "stdlib.mono2stereo"; }
	char *GetDescription()	{ return "Mono to stereo converter"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int Read(BYTE *data, int size);

	bool ReadSourceIfDisabled()	{ return false; }

	SOUNDFORMAT *GetFormat();

private:
	BYTE *m_pBuffer;
	int m_nBuffSize;
};

#endif // !defined(AFX_MONO2STEREO_H__FA02682D_F5C0_4929_97AE_6F64D57BF411__INCLUDED_)
