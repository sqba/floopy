// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__92AD1778_7D98_4217_9A76_ECD63DA4B8FA__INCLUDED_)
#define AFX_INPUT_H__92AD1778_7D98_4217_9A76_ECD63DA4B8FA__INCLUDED_

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

	UINT Read(BYTE *data, UINT size);

	char *GetName()			{ return "volume"; }
	char *GetDescription()	{ return "Volume control"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParam(int index, float value) { m_volume = (int)value; }
	float GetParam(int index)		{ return (float)m_volume; }
	char *GetParamName(int index)	{ return "volume"; }
	char *GetParamDesc(int index)	{ return "Volume"; }

private:
	int m_volume;
};

#endif // !defined(AFX_INPUT_H__92AD1778_7D98_4217_9A76_ECD63DA4B8FA__INCLUDED_)
