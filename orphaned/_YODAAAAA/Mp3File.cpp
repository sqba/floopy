// Mp3File.cpp: implementation of the CMp3File class.
//
//////////////////////////////////////////////////////////////////////

#include "mp3file.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMp3File::CMp3File()
{
	m_hInput = NULL;
	m_hinstLib = NULL;
	m_pChunk = NULL;
	m_nChunkPos = 0;
	m_lChunkSize = 0;
	
	memset(m_filename, 0, sizeof(m_filename));

//	m_hWnd = hWnd;
	m_hDllInstance = GetModuleHandle(NULL);

	//char tmp[MAX_PATH] = {0};
	//GetModuleFileName(m_hDllInstance, tmp, MAX_PATH);

	m_hinstLib = LoadLibrary("mp3.flt");
	if (m_hinstLib != NULL) 
	{ 
		m_OpenFilterInput	= (OpenFilterInput)GetProcAddress(m_hinstLib, "OpenFilterInput"); 
		m_CloseFilterInput	= (CloseFilterInput)GetProcAddress(m_hinstLib, "CloseFilterInput"); 
		m_ReadFilterInput	= (ReadFilterInput)GetProcAddress(m_hinstLib, "ReadFilterInput"); 

		m_FilterGetOptions	= (FilterGetOptions)GetProcAddress(m_hinstLib, "FilterGetOptions");
		m_FilterGetFileSize	= (FilterGetFileSize)GetProcAddress(m_hinstLib, "FilterGetFileSize");

		m_FilterSetOptions2	= (FilterSetOptions2)GetProcAddress(m_hinstLib, "FilterSetOptions2");
	}
}

CMp3File::~CMp3File()
{
	if(m_hinstLib!=NULL)
	{
		FreeLibrary(m_hinstLib);
		m_hinstLib = NULL;
	}
	if(m_pChunk)
		delete m_pChunk;
}

int CMp3File::Read(BYTE *data, int size)
{
	int read = 0;
	if(m_lChunkSize == 0)
	{
		m_lChunkSize = m_ReadFilterInput(m_hInput, m_pChunk, m_lMaxChunkSize);
		if(m_lChunkSize == 0)
			return 0;
		m_nChunkPos = 0;
	}

	if(m_nChunkPos + size < m_lChunkSize)
	{
		memcpy(data, m_pChunk+m_nChunkPos, size);
		read = size;
		m_nChunkPos += read;
	}
	else
	{
		do {
			int s = m_lChunkSize - m_nChunkPos;
			if(s > size-read)
			{
				s = size-read;
				memcpy(data+read, m_pChunk+m_nChunkPos, s);
				read += s;
				m_nChunkPos = s;
			}
			else
			{
				memcpy(data+read, m_pChunk+m_nChunkPos, s);
				read += s;
				m_lChunkSize = m_ReadFilterInput(m_hInput, m_pChunk, m_lMaxChunkSize);
				m_nChunkPos = 0;
			}
		} while(read < size && m_lChunkSize > 0);
	}
	return read;
}

void CMp3File::Reset()
{
	/*if(m_hInput)
	{
		Close();
		Open(m_filename);
	}*/
}

int CMp3File::GetSize()
{
	int size = m_FilterGetFileSize(m_hInput);
	return size / samplesToBytes();
}

BOOL CMp3File::Open(char *filename)
{
	if (m_hinstLib == NULL) 
		return FALSE;

	long lSamprate = 0;
	WORD wBitsPerSample = 0;
	WORD wChannels = 0;

	m_hInput = m_OpenFilterInput(filename,
								&lSamprate,
								&wBitsPerSample,
								&wChannels,
								NULL,
								&m_lMaxChunkSize);


	if(m_hInput != NULL)
	{
		m_pChunk = new BYTE[m_lMaxChunkSize];
		m_nChunkPos = 0;

		//m_wfx.wFormatTag = WAVE_FORMAT_PCM;
		m_format.channels = wChannels;
		m_format.frequency = lSamprate;
		m_format.bitsPerSample = wBitsPerSample;
		//m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
		//m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;

		m_FilterSetOptions2(m_hInput, 0, &lSamprate, &wChannels, &wBitsPerSample);

		memset(m_filename, 0, sizeof(m_filename));
		strncpy(m_filename, filename, MAX_PATH);

		return TRUE;
	}

	return FALSE;
}

void CMp3File::Close()
{
	if(m_hInput!=NULL)
	{
		CloseFilterInput(m_hInput);
		m_hInput = NULL;
	}
	if(m_pChunk)
	{
		delete[] m_pChunk;
		m_pChunk = NULL;
	}
	m_nChunkPos = 0;
	m_lChunkSize = 0;
}

int CMp3File::samplesToBytes()
{
	SOUNDFORMAT *fmt = GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	return (fmt->bitsPerSample / 8) * fmt->channels;
}
