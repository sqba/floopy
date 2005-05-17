// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__D56EF626_75C4_45E7_8E1D_2E065D0538AA__INCLUDED_)
#define AFX_INPUT_H__D56EF626_75C4_45E7_8E1D_2E065D0538AA__INCLUDED_

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

	char *GetName()			{ return "mono2stereo"; }
	char *GetDescription()	{ return "Mono to stereo converter"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int Read(BYTE *data, int size);
	//void MoveTo(int samples);
	//int GetSize();

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

	SOUNDFORMAT *GetFormat();

private:
};

#endif // !defined(AFX_INPUT_H__D56EF626_75C4_45E7_8E1D_2E065D0538AA__INCLUDED_)
