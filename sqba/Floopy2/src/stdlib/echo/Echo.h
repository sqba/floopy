// Echo.h: interface for the CEcho class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ECHO_H__0B70C3FC_B76D_46BC_B7FA_F124DC04949A__INCLUDED_)
#define AFX_ECHO_H__0B70C3FC_B76D_46BC_B7FA_F124DC04949A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../ifloopy.h"

class CEcho : public IFloopySoundFilter
{
public:
	CEcho();
	virtual ~CEcho();

	char *GetName()			{ return "stdlib.echo"; }
	char *GetDescription()	{ return "Echo effect"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int Read(BYTE *data, int size);
	void Reset();

private:
	short echo_buf[65536], echo_buf2[65536];
};

#endif // !defined(AFX_ECHO_H__0B70C3FC_B76D_46BC_B7FA_F124DC04949A__INCLUDED_)
