#ifndef _FLOOPY_WAVFILE_IN_H_
#define _FLOOPY_WAVFILE_IN_H_

#include <stdio.h>	// FILE
#include <stdlib.h>	// _MAX_PATH

#include "../../ifloopy.h"
#include "../../platform.h"

class CWavFileIn : public IFloopySoundInput
{
public:
	CWavFileIn();
	virtual ~CWavFileIn();

	const char *GetName()			{ return (m_pFile ? m_filename : "std.wavfile"); }
	const char *GetDescription()	{ return "WAV file reader"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	bool Open(const char*);
	int  GetSize();
	void MoveTo(int);
	void Reset();
	int  Read(BYTE*, int);
	int  Read2(BYTE**, int, int);
	void Close();
	int  GetPosition();

	bool IsEOF();

	const char *GetLastErrorDesc() { return NULL; }

private:
	int		m_nHeaderLength;
	int		m_nSamplesToBytes;
	int		m_size;	// Number of samples
	FILE	*m_pFile;
	int		m_iDataSize;
	char	m_filename[MAX_PATH];
};

#endif // _FLOOPY_WAVFILE_IN_H_