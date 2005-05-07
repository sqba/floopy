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

		m_wavformat.channels = (int)m_fmt.fmtFORMAT.nChannels;
		m_wavformat.freq = m_fmt.fmtFORMAT.nSamplesPerSec;
		m_wavformat.size = m_fmt.fmtFORMAT.wBitsPerSample;

		// Number of samples
		m_size = m_data.dataSIZE / ((m_wavformat.size/8) * m_wavformat.channels);

		memset(m_filename, 0, sizeof(m_filename));
		strncpy(m_filename, filename, MAX_PATH);

		return TRUE;
	}
	return FALSE;
}

UINT CInput::GetSize()
{
	return m_size; // In samples!
	//return (NULL != m_pFile ? 0 : 0);
}

void CInput::MoveTo(UINT samples)
{
	UINT n = samples * ((m_wavformat.size/8) * m_wavformat.channels);
	fseek(m_pFile, n, SEEK_SET);
}

void CInput::Reset()
{
	long hdrlen = (sizeof(RIFF)+sizeof(FMT)+sizeof(DATA));
	fseek(m_pFile, hdrlen, SEEK_SET);
}

UINT CInput::Read(BYTE *data, UINT size)
{
	if(NULL != m_pFile)
	{
		long pos = ftell(m_pFile);
		long hdrlen = (sizeof(RIFF)+sizeof(FMT)+sizeof(DATA));
		if(pos+size > m_data.dataSIZE+hdrlen)
		{
			size = m_data.dataSIZE + hdrlen - pos;
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
	}
}
