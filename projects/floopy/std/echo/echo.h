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

	const char *GetName()			{ return "libstd.echo"; }
	const char *GetDescription()	{ return "Echo effect"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int Read(BYTE *data, int size);
	void Reset();
//	void Enable(bool bEnabled);

    const char *GetLastErrorDesc() { return NULL; }

private:
	int read(BYTE *data, int size);

private:
	short echo_buf[65536], echo_buf2[65536];
};

#endif // !defined(AFX_ECHO_H__0B70C3FC_B76D_46BC_B7FA_F124DC04949A__INCLUDED_)
