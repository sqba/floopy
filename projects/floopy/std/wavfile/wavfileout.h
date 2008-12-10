// WavFileOut.h: interface for the CWavFileOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVFILEOUT_H__73B41633_D564_4F1B_8602_A1A0CF7941FC__INCLUDED_)
#define AFX_WAVFILEOUT_H__73B41633_D564_4F1B_8602_A1A0CF7941FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* flags for wFormatTag field of WAVEFORMAT */
//#define MAX_PATH          260

#include <stdio.h>
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#include <tchar.h>
#endif

#include "../../ifloopy.h"
#include "../../platform.h"
#include "headers.h"

class CWavFileOut : public IFloopySoundOutput
{
public:
	CWavFileOut(int nSamplesPerSec, int wBitsPerSample, int nChannels);
	virtual ~CWavFileOut();

	bool Open(const char *filename);
	int Write(BYTE *data, int size);
	void Close();

	//char *GetName()			{ return "wavfile"; }
	const char *GetName()			{ return (m_pFile ? m_filename : "libstd.wavfile"); }
	const char *GetDescription()	{ return "WAV file writer"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	const char *GetLastErrorDesc() { return NULL; }

private:
	FILE *m_pFile;
	RIFF m_riff;
	FMT  m_fmt;
	DATA m_data;
	char m_filename[MAX_PATH];
};

#endif // !defined(AFX_WAVFILEOUT_H__73B41633_D564_4F1B_8602_A1A0CF7941FC__INCLUDED_)
