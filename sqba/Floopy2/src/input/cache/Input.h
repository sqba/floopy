// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__7F7E81C5_BFE1_483B_A8C6_7AC5C56F08AE__INCLUDED_)
#define AFX_INPUT_H__7F7E81C5_BFE1_483B_A8C6_7AC5C56F08AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../../ifloopy.h"

class CInput : public IFloopySoundInput
{
public:
	CInput();
	virtual ~CInput();

	char *GetName()			{ return "cache"; }
	char *GetDescription()	{ return "Cache Component"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	UINT Read(BYTE *data, UINT size);
	void SetSource(IFloopySoundInput *src);
	void MoveTo(UINT samples);
	UINT GetPos();
	void Reset();
//	UINT GetSize();

private:
	int samplesToBytes();
	BOOL createBuffer();

private:
	BYTE *m_pBuffer;
	UINT m_nSize;
	UINT m_nPosition;
};

#endif // !defined(AFX_INPUT_H__7F7E81C5_BFE1_483B_A8C6_7AC5C56F08AE__INCLUDED_)
