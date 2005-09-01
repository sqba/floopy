// WavFileIn.cpp: implementation of the CWavFileIn class.
//
//////////////////////////////////////////////////////////////////////

#include "wavfilein.h"


/*typedef struct {	// CHUNK 8-byte header
	char	id[4];	// identifier, e.g. "fmt" or "data"
	DWORD	len;	// remaining chunk length after header
} chunk_hdr;*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWavFileIn::CWavFileIn()
{
	m_pFile				= NULL;
	m_size				= 0;
	m_nSamplesToBytes	= 0;
	m_nHeaderLength		= 0;
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
	if(NULL != m_pFile)
	{
		/*chunk_hdr hdr;
		while(true)
		{
			memset(&hdr, 0, sizeof(chunk_hdr));
			
			if(fread( &hdr, 1, sizeof(RIFF), m_pFile ) <= 0)
				break;
			
			if(0 == strncmp("RIFF", hdr.id, 4))
			{
				char wave_id[4] = {0};
				fread( &wave_id, sizeof(wave_id), 1, m_pFile );
			}
			fseek(m_pFile, hdr.len, SEEK_CUR);
		}*/

		fread( &m_riff,	sizeof(RIFF),	1,	m_pFile );
		fread( &m_fmt,	sizeof(FMT),	1,	m_pFile );
		fread( &m_data,	sizeof(DATA),	1,	m_pFile );

		while(!feof(m_pFile) && 0 != strncmp("data", m_data.dataID, 4))
		{
			fseek(m_pFile, m_data.dataSIZE, SEEK_CUR);
			fread( &m_data,	sizeof(DATA),	1,	m_pFile );
		}

		m_format.format			= m_fmt.fmtFORMAT.wFormatTag;
		m_format.channels		= (int)m_fmt.fmtFORMAT.nChannels;
		m_format.frequency		= m_fmt.fmtFORMAT.nSamplesPerSec;
		m_format.bitsPerSample	= m_fmt.fmtFORMAT.wBitsPerSample;

		m_nSamplesToBytes = ((m_format.bitsPerSample/8) * m_format.channels);

		// Number of samples
		m_size = m_data.dataSIZE / m_nSamplesToBytes;

		memset(m_filename, 0, sizeof(m_filename));
		strncpy(m_filename, filename, MAX_PATH);
	
		m_nHeaderLength = (sizeof(RIFF) + sizeof(FMT) + sizeof(DATA));

		return TRUE;
	}
	return FALSE;
}

int CWavFileIn::GetSize()
{
	return m_size; // In samples!
	//return (NULL != m_pFile ? 0 : 0);
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
		if(pos+(long)size > (long)(m_data.dataSIZE+m_nHeaderLength))
		{
			size = m_data.dataSIZE + m_nHeaderLength - pos;
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
