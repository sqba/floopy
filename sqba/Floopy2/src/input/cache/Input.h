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

	char *GetName()				{ return "cache"; }
	char *GetDescription()		{ return "Stores entire source input in memory"; }
	char *GetVersion()			{ return "0.1"; }
	char *GetAuthor()			{ return "sqba"; }

	int Read(BYTE *data, int size);
	BOOL SetSource(IFloopySoundInput *src);

	void MoveTo(int samples)	{ m_nPosition = samples * samplesToBytes(); }
	int GetPos()				{ return m_nPosition / samplesToBytes(); }
	void Reset()				{ m_nPosition = 0; }
	void Close();

private:
	int samplesToBytes();
	BOOL createBuffer();
	inline BOOL passedTheEnd()	{ return (m_nPosition > m_nSize); }
	inline BOOL bufferIsEmpty()	{ return (m_nSize == 0); }
	void clearBuffer();

private:
	BYTE *m_pBuffer;
	int m_nSize;
	int m_nPosition;
};

#endif // !defined(AFX_INPUT_H__7F7E81C5_BFE1_483B_A8C6_7AC5C56F08AE__INCLUDED_)
