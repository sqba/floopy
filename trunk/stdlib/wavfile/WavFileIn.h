#ifndef _FLOOPY_WAVFILE_IN_H_
#define _FLOOPY_WAVFILE_IN_H_

#include <stdio.h>	// FILE
#include <stdlib.h>	// _MAX_PATH

#include "../../ifloopy.h"

class CWavFileIn : public IFloopySoundInput
{
public:
	CWavFileIn();
	virtual ~CWavFileIn();

	char *GetName()			{ return (m_pFile ? m_filename : "stdlib.wavfile"); }
	char *GetDescription()	{ return "WAV file reader"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	bool Open(char*);
	int  GetSize();
	void MoveTo(int);
	void Reset();
	int  Read(BYTE*, int);
	void Close();
	int  GetPosition();

	bool IsEOF();

private:
	int		m_nHeaderLength;
	int		m_nSamplesToBytes;
	int		m_size;	// Number of samples
	FILE	*m_pFile;
	int		m_iDataSize;
	char	m_filename[_MAX_PATH];
};

#endif // _FLOOPY_WAVFILE_IN_H_
