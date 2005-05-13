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

	int Read(BYTE *data, int size);

	char *GetName()			{ return "volume"; }
	char *GetDescription()	{ return "Volume control"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int   GetParamCount()			{ return 1; }
	void  SetParam(int index, float value) { m_volume = (int)value; }
	float GetParam(int index)		{ return (float)m_volume; }
	char *GetParamName(int index)	{ return "volume"; }
	char *GetParamDesc(int index)	{ return "Volume"; }
	int   GetParamIndex(char *name)	{ return (0==strcmpi(name, "volume") ? 0 : -1); }

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

private:
	int m_volume; // In percents
};

#endif // !defined(AFX_INPUT_H__92AD1778_7D98_4217_9A76_ECD63DA4B8FA__INCLUDED_)
