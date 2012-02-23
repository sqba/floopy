// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__579B912B_2195_41EE_95BE_0B229EB45826__INCLUDED_)
#define AFX_INPUT_H__579B912B_2195_41EE_95BE_0B229EB45826__INCLUDED_

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

class CInput : public IFloopySoundInput
{
public:
	CInput();
	virtual ~CInput();

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

#endif // !defined(AFX_INPUT_H__579B912B_2195_41EE_95BE_0B229EB45826__INCLUDED_)
