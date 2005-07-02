// WaveOut.h: interface for the CWaveOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WAVEOUT_H__E86DF3E1_4689_4FCA_9D36_21200D8A38EA__INCLUDED_)
#define AFX_WAVEOUT_H__E86DF3E1_4689_4FCA_9D36_21200D8A38EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma comment(lib, "winmm")

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <mmsystem.h>

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "../../ifloopy.h"

// some good values for block size and count
#define BLOCK_SIZE  8192
#define BLOCK_COUNT 20

// module level variables
static CRITICAL_SECTION waveCriticalSection;
static WAVEHDR*         waveBlocks;
//static volatile int     waveFreeBlockCount;
static int              waveCurrentBlock;

class CWaveOut : public IFloopySoundOutput
{
public:
	CWaveOut(SOUNDFORMAT fmt);
	virtual ~CWaveOut();

	int Write(BYTE *data, int size);

	char *GetName()			{ return "stdlib.waveout"; }
	char *GetDescription()	{ return "Wave Out"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	int GetWrittenSamples();
	void Reset();

	int waveFreeBlockCount;
	int samplesToBytes;
	int bytesRead;

private:
	// function prototypes
	static void CALLBACK	waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
	static WAVEHDR*			allocateBlocks(int size, int count);
	static void				freeBlocks(WAVEHDR* blockArray);
//	static void				writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);
	void writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);

	HWAVEOUT hWaveOut;	// device handle
	WAVEFORMATEX wfx;	// look this up in your documentation
};
#endif // !defined(AFX_WAVEOUT_H__E86DF3E1_4689_4FCA_9D36_21200D8A38EA__INCLUDED_)
