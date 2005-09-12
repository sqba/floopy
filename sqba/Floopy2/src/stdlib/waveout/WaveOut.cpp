// WaveOut.cpp: implementation of the CWaveOut class.
//
//////////////////////////////////////////////////////////////////////

#include "waveout.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaveOut::CWaveOut(SOUNDFORMAT fmt) : IFloopySoundOutput(fmt)
{
	// initialise the module variables
	waveBlocks         = allocateBlocks(BLOCK_SIZE, BLOCK_COUNT);
	waveFreeBlockCount = BLOCK_COUNT;
	waveCurrentBlock   = 0;
	
	InitializeCriticalSection(&waveCriticalSection);

	// set up the WAVEFORMATEX structure.
	m_wfx.nSamplesPerSec  = fmt.frequency;						// sample rate
	m_wfx.wBitsPerSample  = (unsigned short)fmt.bitsPerSample;	// sample size
	m_wfx.nChannels       = fmt.channels;						// channels
	
	m_wfx.cbSize          = 0;									// size of _extra_ info
	m_wfx.wFormatTag      = WAVE_FORMAT_PCM;
	m_wfx.nBlockAlign     = (m_wfx.wBitsPerSample * m_wfx.nChannels) >> 3;
	m_wfx.nAvgBytesPerSec = m_wfx.nBlockAlign * m_wfx.nSamplesPerSec;
	
	// try to open the default wave device. WAVE_MAPPER is
	// a constant defined in mmsystem.h, it always points to the
	// default wave device on the system (some people have 2 or
	// more sound cards).
	if(MMSYSERR_NOERROR != waveOutOpen(
								&m_hWaveOut, 
								WAVE_MAPPER, 
								&m_wfx, 
								(LONG)waveOutProc, 
								(LONG)this, 
								CALLBACK_FUNCTION)) {
		fprintf(stderr, "Unable to open wave mapper device\n");
		ExitProcess(1);
	}
}

CWaveOut::~CWaveOut()
{
	// wait for all blocks to complete
	while(waveFreeBlockCount < BLOCK_COUNT)
		Sleep(10);
	
	// unprepare any blocks that are still prepared
	for(int i = 0; i < waveFreeBlockCount; i++) 
	{
		if(waveBlocks[i].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hWaveOut, &waveBlocks[i], sizeof(WAVEHDR));
	}
		
	DeleteCriticalSection(&waveCriticalSection);
	freeBlocks(waveBlocks);
	waveOutClose(m_hWaveOut);
}

int CWaveOut::Write(BYTE *data, int size)
{
	WAVEHDR* current;
	int remain;
	
	current = &waveBlocks[waveCurrentBlock];
	
	while(size > 0)
	{
		// first make sure the header we're going to use is unprepared
		if(current->dwFlags & WHDR_PREPARED) 
			waveOutUnprepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));
		
		if(size < (int)(BLOCK_SIZE - current->dwUser)) {
			memcpy(current->lpData + current->dwUser, data, size);
			current->dwUser += size;
			break;
		}
		
		remain = BLOCK_SIZE - current->dwUser;
		memcpy(current->lpData + current->dwUser, data, remain);
		size -= remain;
		data += remain;
		current->dwBufferLength = BLOCK_SIZE;
		
		waveOutPrepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));
		waveOutWrite(m_hWaveOut, current, sizeof(WAVEHDR));
		
		EnterCriticalSection(&waveCriticalSection);
		waveFreeBlockCount--;
		LeaveCriticalSection(&waveCriticalSection);
		
		// wait for a block to become free
		while(!waveFreeBlockCount)
			Sleep(10);
		
		// point to the next block
		waveCurrentBlock++;
		waveCurrentBlock %= BLOCK_COUNT;
		
		current = &waveBlocks[waveCurrentBlock];
		current->dwUser = 0;
	}


	return size;
}




void CALLBACK CWaveOut::waveOutProc(
    HWAVEOUT m_hWaveOut, 
    UINT uMsg, 
    DWORD dwInstance,  
    DWORD dwParam1,    
    DWORD dwParam2)
{
	CWaveOut *pWaveOut = (CWaveOut*)dwInstance;

	// pointer to free block counter
	int* freeBlockCounter = &pWaveOut->waveFreeBlockCount;
	
	// ignore calls that occur due to openining and closing the device.
	if(uMsg != WOM_DONE)
		return;
	
	EnterCriticalSection(&waveCriticalSection);
	(*freeBlockCounter)++;
	LeaveCriticalSection(&waveCriticalSection);
}

WAVEHDR* CWaveOut::allocateBlocks(int size, int count)
{
	unsigned char* buffer;
	int i;
	WAVEHDR* blocks;
	DWORD totalBufferSize = (size + sizeof(WAVEHDR)) * count;
	
	// allocate memory for the entire set in one go
	if((buffer = (unsigned char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, totalBufferSize)) == NULL)
	{
		fprintf(stderr, "Memory allocation error\n");
		ExitProcess(1);
	}
	
	// and set up the pointers to each bit
	blocks = (WAVEHDR*)buffer;
	buffer += sizeof(WAVEHDR) * count;
	for(i = 0; i < count; i++) {
		blocks[i].dwBufferLength = size;
		blocks[i].lpData = (char*)buffer;
		buffer += size;
	}
	
	return blocks;
}

void CWaveOut::freeBlocks(WAVEHDR* blockArray)
{
	// and this is why allocateBlocks works the way it does
	HeapFree(GetProcessHeap(), 0, blockArray);
}

int CWaveOut::GetWrittenSamples()
{
	int samples = 0;
	MMTIME mmt;
	mmt.wType = TIME_SAMPLES;
	MMRESULT result = waveOutGetPosition(m_hWaveOut, &mmt, sizeof(mmt));
	if(MMSYSERR_NOERROR == result)
		samples = mmt.u.sample;

	return samples;
}

void CWaveOut::Reset()
{
	waveOutReset( m_hWaveOut );

	// wait for all buffers to be returned by the reset
	//while(waveFreeBlockCount < BLOCK_COUNT)
	//	Sleep(10);

	// Why doesn't the buffer get emptied when the playback restarts?
}

void CWaveOut::Flush()
{
    WAVEHDR* current;
    //assert(ivwave_device);

    current = &waveBlocks[waveCurrentBlock];
    
    /*
     * if the current buffer has some data in it then write it
     * anyway.
     */
    if(current->dwUser) {
        current->dwBufferLength = current->dwUser;
        current->dwUser = 0;
        
        waveOutPrepareHeader(m_hWaveOut, current, sizeof(WAVEHDR));
        waveOutWrite(m_hWaveOut, current, sizeof(WAVEHDR));
        
        EnterCriticalSection(&waveCriticalSection);
        waveFreeBlockCount--;
        LeaveCriticalSection(&waveCriticalSection);
    }

    /*
     * reset the device - take note that the microsoft documentation
     * states that calling other waveOut functions from the callback
     * will cause a deadlock - well calling waveOutUnprepare header
     * doesn't until you call waveOutReset, at which point the app
     * will freeze (hence the lame arse implementation here).
     */
    waveOutReset(m_hWaveOut);
    
    /*
     * wait for all buffers to be returned by the reset
     */ 
    while(waveFreeBlockCount != BLOCK_COUNT)
        Sleep(10);

    /*
     * point to the next block
     */
    waveCurrentBlock++;
    waveCurrentBlock %= BLOCK_COUNT;

    /*
     * reset positioning/statistics variables
     */
    //ivwave_output = 0;
    //ivwave_written = 0;
    //ivwave_pos = time;

    /*
     * playback can now continue
     */
}
