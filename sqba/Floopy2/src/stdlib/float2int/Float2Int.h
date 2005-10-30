// Float2Int.h: interface for the CFloat2Int class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOAT2INT_H__67F54047_EF9A_45E2_96B5_EC9ED30595B4__INCLUDED_)
#define AFX_FLOAT2INT_H__67F54047_EF9A_45E2_96B5_EC9ED30595B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

#define SAMPLE_16BIT	short int
#define SAMPLE_8BIT		BYTE
#define FLOAT_32BIT		float //?

class CFloat2Int : public IFloopySoundFilter
{
public:
	CFloat2Int();
	virtual ~CFloat2Int();

	char *GetName()			{ return "stdlib.float2int"; }
	char *GetDescription()	{ return "Converts float samples to integer"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	SOUNDFORMAT *GetFormat();

	int Read(BYTE *data, int size);

	bool ReadSourceIfDisabled()	{ return false; }

private:
	inline double frand();
	void quantizeWithDither(FLOAT_32BIT *pin, SAMPLE_16BIT *piout, int numsamples);
	void quantize(FLOAT_32BIT *pin, SAMPLE_16BIT *piout, int numsamples);

private:
	BYTE *m_pBuffer;
	int m_nBuffSize;
	bool m_bDither;
};

#endif // !defined(AFX_FLOAT2INT_H__67F54047_EF9A_45E2_96B5_EC9ED30595B4__INCLUDED_)
