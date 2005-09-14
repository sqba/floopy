// Reverse.h: interface for the CReverse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REVERSE_H__13C844C0_C3A6_4054_95A6_4D087F651E88__INCLUDED_)
#define AFX_REVERSE_H__13C844C0_C3A6_4054_95A6_4D087F651E88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

class CReverse : public IFloopySoundFilter  
{
public:
	CReverse();
	virtual ~CReverse();

	char *GetName()				{ return "stdlib.reverse"; }
	char *GetDescription()		{ return "Reverse"; }
	char *GetVersion()			{ return "0.1"; }
	char *GetAuthor()			{ return "sqba"; }

	bool ReadSourceIfDisabled()	{ return false; }

	int Read(BYTE *data, int size);
	void MoveTo(int samples);
	void Reset();
	int GetPosition();
	bool SetSource(IFloopySoundInput *src);
//	void Enable(bool bEnabled);

private:
	void reverse(BYTE *data, int size);

private:
	int m_nPosition;
	BYTE *m_pBuffer;
	int m_nBuffSize;
	int m_nSamplesToBytes;
};

#endif // !defined(AFX_REVERSE_H__13C844C0_C3A6_4054_95A6_4D087F651E88__INCLUDED_)
