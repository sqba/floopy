// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__0A78814E_64D6_426F_8A9F_4A95D2928905__INCLUDED_)
#define AFX_INPUT_H__0A78814E_64D6_426F_8A9F_4A95D2928905__INCLUDED_

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

	char *GetName()			{ return "echo"; }
	char *GetDescription()	{ return "Echo effect"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

private:
	short echo_buf[65536], echo_buf2[65536];
};

#endif // !defined(AFX_INPUT_H__0A78814E_64D6_426F_8A9F_4A95D2928905__INCLUDED_)
