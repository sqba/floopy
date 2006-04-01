// MadxlibWrapper.h: interface for the CMadxlibWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MADXLIBWRAPPER_H__E70ADC37_6CC9_4D5C_AFD7_F4CA55ECBAE4__INCLUDED_)
#define AFX_MADXLIBWRAPPER_H__E70ADC37_6CC9_4D5C_AFD7_F4CA55ECBAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

#include "../ifloopy.h"
#include "madxlib/madxlib.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

class CMadxlibWrapper : public IFloopySoundFilter
{
public:
	CMadxlibWrapper();
	virtual ~CMadxlibWrapper();


	char *GetName()			{ return "madxlib.wrapper"; }
	char *GetDescription()	{ return "Madxlib wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	bool ReadSourceIfDisabled()	{ return false; }

	SOUNDFORMAT *GetFormat();

	bool Open(char *filename);
	int Read(BYTE *data, int size);
	void Reset();
	void MoveTo(int offset);
	void Close();

private:
	char m_szLastError[100];

	FILE	*in_file;

	// Output file and output buffer
	unsigned char	in_buffer[MADX_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
	unsigned char	out_buffer[MADX_OUTPUT_BUFFER_SIZE];
    
	size_t	a;
	size_t	in_size;

	// Structure necessary to use madxlib
	madx_house	mxhouse;
	// Status and control
	madx_stat	mxstat;
	// Catch signals from madx_read()
	madx_sig	mxsig;
};

#endif // !defined(AFX_MADXLIBWRAPPER_H__E70ADC37_6CC9_4D5C_AFD7_F4CA55ECBAE4__INCLUDED_)
