
#ifndef _WAV_HEADERS_
#define _WAV_HEADERS_

#ifndef _TCHAR
#define _TCHAR char
#endif

struct WAVEFORM
{
	WORD  wFormatTag;		//format of digital sound
	WORD  nChannels;		//Number of channels (1 for mono and 2 for stereo)
	DWORD nSamplesPerSec;	//Number of samples per second
	DWORD nAvgBytesPerSec;	//Average number bytes of data per second
	WORD  nBlockAlign;		//Minimal data size for playing
	WORD  wBitsPerSample;	//Bits per sample (8 or 16)
};



struct RIFF
{
	_TCHAR riffID[4];		//contains identifier "RIFF"
	DWORD  riffSIZE;		//File size minus 8 bytes
	_TCHAR riffFORMAT[4];	//contains identifier "WAVE"
};

struct FMT
{
	_TCHAR   fmtID[4];		//contains identifier: "fmt " (with space)
	DWORD    fmtSIZE;		//contains the size of this block (for WAVE PCM 16)
	WAVEFORM fmtFORMAT;		//structure WAVEFORMATEX but without cbSize field
};

struct DATA
{
	_TCHAR dataID[4];		//contains identifier: "data"
	DWORD  dataSIZE;		//data size
};

#endif // _WAV_HEADERS_
