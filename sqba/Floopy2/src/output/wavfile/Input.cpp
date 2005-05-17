// Input.cpp: implementation of the CInput class.
//
//////////////////////////////////////////////////////////////////////

#include "Input.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInput::CInput()
{
	m_pFile = NULL;
	m_size = 0;
	memset(m_filename, 0, sizeof(m_filename));
	m_nSamplesToBytes = 0;
	m_nHeaderLength = 0;
}

CInput::~CInput()
{
	if(NULL != m_pFile)
		fclose(m_pFile);
}

BOOL CInput::Open(char *filename)
{
	m_pFile = fopen(filename, "rb");
	if(NULL != m_pFile)
	{
		fread( &m_riff, 1, sizeof(RIFF), m_pFile );
		fread( &m_fmt,  1, sizeof(FMT),  m_pFile );
		fread( &m_data, 1, sizeof(DATA), m_pFile );

		m_format.channels = (int)m_fmt.fmtFORMAT.nChannels;
		m_format.frequency = m_fmt.fmtFORMAT.nSamplesPerSec;
		m_format.bitsPerSample = m_fmt.fmtFORMAT.wBitsPerSample;

		// Number of samples
		m_size = m_data.dataSIZE / ((m_format.bitsPerSample/8) * m_format.channels);

		memset(m_filename, 0, sizeof(m_filename));
		strncpy(m_filename, filename, MAX_PATH);
	
		m_nSamplesToBytes = ((m_format.bitsPerSample/8) * m_format.channels);
		m_nHeaderLength = (sizeof(RIFF) + sizeof(FMT) + sizeof(DATA));

		return TRUE;
	}
	return FALSE;
}

int CInput::GetSize()
{
	return m_size; // In samples!
	//return (NULL != m_pFile ? 0 : 0);
}

void CInput::MoveTo(int samples)
{
	int n = samples * m_nSamplesToBytes;
	fseek(m_pFile, m_nHeaderLength+n, SEEK_SET);
}

void CInput::Reset()
{
	fseek(m_pFile, m_nHeaderLength, SEEK_SET);
}

int CInput::Read(BYTE *data, int size)
{
	if(NULL != m_pFile)
	{
		long pos = ftell(m_pFile);
		if(pos+size > m_data.dataSIZE+m_nHeaderLength)
		{
			size = m_data.dataSIZE + m_nHeaderLength - pos;
		}

		return fread( data, 1, size, m_pFile );
	}
	return 0;
}

void CInput::Close()
{
	if(NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}
