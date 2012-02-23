// Output.h: interface for the COutput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUT_H__B3C4861E_AE40_401B_B735_F829B51B2E15__INCLUDED_)
#define AFX_OUTPUT_H__B3C4861E_AE40_401B_B735_F829B51B2E15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
static volatile int     waveFreeBlockCount;
static int              waveCurrentBlock;

class COutput : public IFloopySoundOutput
{
public:
	COutput(SOUNDFORMAT fmt);
	virtual ~COutput();

	int Write(BYTE *data, int size);

	char *GetName()			{ return "waveout"; }
	char *GetDescription()	{ return "Wave Out"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

private:
	// function prototypes
	static void CALLBACK	waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
	static WAVEHDR*			allocateBlocks(int size, int count);
	static void				freeBlocks(WAVEHDR* blockArray);
	static void				writeAudio(HWAVEOUT hWaveOut, LPSTR data, int size);

	HWAVEOUT hWaveOut;	// device handle
	WAVEFORMATEX wfx;	// look this up in your documentation
};

#endif // !defined(AFX_OUTPUT_H__B3C4861E_AE40_401B_B735_F829B51B2E15__INCLUDED_)
