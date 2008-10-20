// Float2Int.h: interface for the CFloat2Int class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOAT2INT_H__67F54047_EF9A_45E2_96B5_EC9ED30595B4__INCLUDED_)
#define AFX_FLOAT2INT_H__67F54047_EF9A_45E2_96B5_EC9ED30595B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../engine/include/ifloopy.h"

#define SAMPLE_16BIT	short int
#define SAMPLE_8BIT		BYTE
#define FLOAT_32BIT		float //?

class CFloat2Int : public IFloopySoundFilter
{
public:
	CFloat2Int();
	virtual ~CFloat2Int();

	const char *GetName()			{ return "libstd.float2int"; }
	const char *GetDescription()	{ return "Converts float samples to integer"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	SOUNDFORMAT *GetFormat();

	int Read(BYTE *data, int size);
	void MoveTo(int samples);

	bool CanReadSourceIfDisabled()	{ return false; }

private:
	inline double frand();
	void quantize(FLOAT_32BIT*, SAMPLE_16BIT*, int);

private:
	BYTE *m_pBuffer;
	int m_nBuffSize;
	bool m_bDither;
};

#endif // !defined(AFX_FLOAT2INT_H__67F54047_EF9A_45E2_96B5_EC9ED30595B4__INCLUDED_)
