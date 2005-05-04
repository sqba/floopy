// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__BCB6BC65_B81F_423F_989D_E7064591475A__INCLUDED_)
#define AFX_OUTPUT_H__BCB6BC65_B81F_423F_989D_E7064591475A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>

#include "../../ifloopy.h"
#include "headers.h"

class COutput : public IFloopySoundOutput
{
public:
	COutput(int nSamplesPerSec, int wBitsPerSample, int nChannels);
	virtual ~COutput();

	BOOL Open(char *filename);
	UINT Write(BYTE *data, UINT size);
	void Close();

	char *GetName()			{ return "wavfile"; }
	char *GetDescription()	{ return "WAV file writer"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

private:
	FILE *m_pFile;
	RIFF m_riff;
	FMT  m_fmt;
	DATA m_data;
};

#endif // !defined(AFX_OUTPUT_H__BCB6BC65_B81F_423F_989D_E7064591475A__INCLUDED_)
