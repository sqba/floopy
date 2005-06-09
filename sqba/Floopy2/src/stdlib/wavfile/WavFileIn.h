// WavFileIn.h: interface for the CWavFileIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVFILEIN_H__65A430F3_282F_497A_8035_BA212889D047__INCLUDED_)
#define AFX_WAVFILEIN_H__65A430F3_282F_497A_8035_BA212889D047__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* flags for wFormatTag field of WAVEFORMAT */
#define WAVE_FORMAT_PCM     1
#define MAX_PATH          260

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#include <windows.h>
//#include <mmsystem.h>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>

#include "../../ifloopy.h"
#include "headers.h"

class CWavFileIn : public IFloopySoundInput
{
public:
	CWavFileIn();
	virtual ~CWavFileIn();

	BOOL Open(char *filename);
	int GetSize();
	void MoveTo(int samples);
	void Reset();
	int Read(BYTE *data, int size);
	void Close();

	//char *GetName()			{ return "wavfile"; }
	char *GetName()			{ return (m_pFile ? m_filename : "wavfile"); }
	char *GetDescription()	{ return "WAV file reader"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

private:
	int m_nHeaderLength;
	int m_nSamplesToBytes;
	int m_size;	// Number of samples
	FILE *m_pFile;
	RIFF m_riff;
	FMT  m_fmt;
	DATA m_data;
	char m_filename[MAX_PATH];
};

#endif // !defined(AFX_WAVFILEIN_H__65A430F3_282F_497A_8035_BA212889D047__INCLUDED_)
