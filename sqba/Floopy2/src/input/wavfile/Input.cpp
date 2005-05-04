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

		return TRUE;
	}
	return FALSE;
}

DWORD CInput::GetSize()
{
	return m_size; // In samples!
	//return (NULL != m_pFile ? 0 : 0);
}

UINT CInput::Read(BYTE *data, UINT size, UINT offset)
{
	if(NULL != m_pFile)
	{
		// Moze li se offset potpuno izbaciti?
		if(-1 == offset)
		{
			long hdrlen = (sizeof(RIFF)+sizeof(FMT)+sizeof(DATA));
			fseek(m_pFile, hdrlen, SEEK_SET);
		}
		else if(0 != offset)
		{
			UINT n = offset * ((m_wavformat.size/8) * m_wavformat.channels);
			fseek(m_pFile, n, SEEK_SET);
		}

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

void CInput::Reset()
{
	long hdrlen = (sizeof(RIFF)+sizeof(FMT)+sizeof(DATA));
	fseek(m_pFile, hdrlen, SEEK_SET);
}
