// WavFileIn.cpp: implementation of the CWavFileIn class.
//
//////////////////////////////////////////////////////////////////////

#include "wavfilein.h"


struct chunk_hdr	// CHUNK 8-byte header
{
	char	id[4];	// identifier, e.g. "fmt" or "data"
	DWORD	len;	// remaining chunk length after header
};

struct WAVEFORM
{
	WORD  wFormatTag;		//format of digital sound
	WORD  nChannels;		//Number of channels (1 for mono and 2 for stereo)
	DWORD nSamplesPerSec;	//Number of samples per second
	DWORD nAvgBytesPerSec;	//Average number bytes of data per second
	WORD  nBlockAlign;		//Minimal data size for playing
	WORD  wBitsPerSample;	//Bits per sample (8 or 16)
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWavFileIn::CWavFileIn()
{
	m_pFile				= NULL;
	m_size				= 0;
	m_nSamplesToBytes	= 0;
	m_nHeaderLength		= 0;
	m_iDataSize			= 0;
	memset(m_filename, 0, sizeof(m_filename));
}

CWavFileIn::~CWavFileIn()
{
	if(NULL != m_pFile)
		fclose(m_pFile);
}

BOOL CWavFileIn::Open(char *filename)
{
	m_pFile = fopen(filename, "rb");
	if(NULL == m_pFile)
		return FALSE;

	chunk_hdr hdr;

	while( !feof(m_pFile) )
	{
		memset(&hdr, 0, sizeof(chunk_hdr));
		
		if(fread( &hdr, sizeof(chunk_hdr), 1, m_pFile ) <= 0)
			break;
		
		if(0 == strncmp("RIFF", hdr.id, 4))
		{
			char wave_id[4] = {0};
			int numread = fread( wave_id, sizeof(char), 4, m_pFile );

			if(numread != 4)
				return FALSE;

			if(0 != strncmp("WAVE", wave_id, 4))
				return FALSE;
		}
		else if(0 == strncmp("fmt", hdr.id, 3))
		{
			WAVEFORM format;
			memset(&format, 0, sizeof(WAVEFORM));
			int numread = fread( &format, sizeof(WAVEFORM), 1, m_pFile );

			if(numread != 1)
				return FALSE;

			m_format.format			= format.wFormatTag;
			m_format.channels		= (int)format.nChannels;
			m_format.frequency		= format.nSamplesPerSec;
			m_format.bitsPerSample	= format.wBitsPerSample;

			if( !m_format.format || !m_format.channels ||
				!m_format.frequency || !m_format.bitsPerSample )
				return FALSE;

			m_nSamplesToBytes = ((m_format.bitsPerSample/8) * m_format.channels);
		}
		else if(0 == strncmp("data", hdr.id, 4))
		{
			if(0 == m_nSamplesToBytes)
				return FALSE;
			m_nHeaderLength = ftell(m_pFile);
			m_iDataSize = hdr.len;
			m_size = hdr.len / m_nSamplesToBytes; // Number of samples
			break;
		}
		else
		{
			// Unknown header, skip it.
			fseek(m_pFile, hdr.len, SEEK_CUR);
		}
	}

	if(0==m_nHeaderLength || m_iDataSize==0)
		return FALSE;

	memset(m_filename, 0, sizeof(m_filename));
	strcpy(m_filename, filename);

	return TRUE;
}

int CWavFileIn::GetSize()
{
	return m_size; // In samples!
}

void CWavFileIn::MoveTo(int samples)
{
	int n = samples * m_nSamplesToBytes;
	fseek(m_pFile, m_nHeaderLength+n, SEEK_SET);
}

void CWavFileIn::Reset()
{
	fseek(m_pFile, m_nHeaderLength, SEEK_SET);
}

int CWavFileIn::Read(BYTE *data, int size)
{
	if(NULL != m_pFile)
	{
		long pos = ftell(m_pFile);
		if(pos+(long)size > (long)(m_iDataSize+m_nHeaderLength))
		{
			size = m_iDataSize + m_nHeaderLength - pos;
		}

		return fread( data, 1, size, m_pFile );
	}
	return 0;
}

void CWavFileIn::Close()
{
	if(NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}
