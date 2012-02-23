// WaveOut.h: interface for the CWaveOut class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32

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
#define BLOCK_SIZE		8192
#define BLOCK_COUNT		20

// module level variables
static CRITICAL_SECTION waveCriticalSection;
static WAVEHDR*         waveBlocks;
static int              waveCurrentBlock;

class CWaveOut : public IFloopySoundOutput
{
public:
	CWaveOut(SOUNDFORMAT fmt);
	virtual ~CWaveOut();

	int Write(BYTE *data, int size);
	int Write2(BYTE **data, int channels, int samples);

	const char *GetName()			{ return "std.waveout"; }
	const char *GetDescription()	{ return "Wave Out"; }
	const char *GetVersion()		{ return "0.1"; }
	const char *GetAuthor()			{ return "sqba"; }

	int GetPosition();
	void Reset();
	void Flush();
	void Pause();

	const char *GetLastErrorDesc() { return NULL; }

private:
	void resume();

private:
	static void CALLBACK	waveOutProc(HWAVEOUT, UINT, DWORD, DWORD, DWORD);
	static WAVEHDR*			allocateBlocks(int size, int count);
	static void				freeBlocks(WAVEHDR* blockArray);

private:
	int				waveFreeBlockCount;
	HWAVEOUT		m_hWaveOut;	// device handle
	WAVEFORMATEX	m_wfx;
	bool			m_bPaused;
};
#endif // !defined(AFX_WAVEOUT_H__E86DF3E1_4689_4FCA_9D36_21200D8A38EA__INCLUDED_)

#endif //WIN32

