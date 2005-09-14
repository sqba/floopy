// WavFileIn.h: interface for the CWavFileIn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVFILEIN_H__65A430F3_282F_497A_8035_BA212889D047__INCLUDED_)
#define AFX_WAVFILEIN_H__65A430F3_282F_497A_8035_BA212889D047__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define MAX_PATH          260

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <tchar.h>

#include "../../ifloopy.h"

class CWavFileIn : public IFloopySoundInput
{
public:
	CWavFileIn();
	virtual ~CWavFileIn();

	bool Open(char *filename);
	int GetSize();
	void MoveTo(int samples);
	void Reset();
	int Read(BYTE *data, int size);
	void Close();
	int GetPosition();

	char *GetName()			{ return (m_pFile ? m_filename : "stdlib.wavfile"); }
	char *GetDescription()	{ return "WAV file reader"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

private:
	int		m_nHeaderLength;
	int		m_nSamplesToBytes;
	int		m_size;	// Number of samples
	FILE	*m_pFile;
	int		m_iDataSize;
	char	m_filename[_MAX_PATH];
};

#endif // !defined(AFX_WAVFILEIN_H__65A430F3_282F_497A_8035_BA212889D047__INCLUDED_)
