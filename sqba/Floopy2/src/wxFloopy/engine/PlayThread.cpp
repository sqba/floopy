// PlayThread.cpp: implementation of the CPlayThread class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"
#include "track.h"
#include "playthread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayThread::CPlayThread(CTracks *pTracks)
{
	m_pTracks		= pTracks;
	m_iStartPos		= 0;
	m_bPlaying		= m_bPaused = false;
	m_pOutput		= NULL;
	m_pInput		= NULL;
	m_iBufferLength	= 128; // In samples
	m_iPosition		= 0;
}

CPlayThread::~CPlayThread()
{
	if( wxThread::IsRunning() )
		Stop();
}

void *CPlayThread::Entry()
{
	if(!m_pTracks || m_iStartPos<0 || !m_pEngine || !m_pInput || !m_pOutput)
		return NULL;

	int len	= 0;										// bytes
	m_iPosition = m_iStartPos;							// samples

	int bufflen = m_iBufferLength * m_iSamplesToBytes;	// bytes
	BYTE *buff = new BYTE[bufflen];

	m_pEngine->EmptyBuffer( buff, bufflen );			// Fill with silence
	m_pInput->Reset();									// Recalculate variables
	m_pInput->MoveTo( m_iStartPos );					// Move to cursor position

	m_pOutput->Reset();

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int totalLength = m_pTracks->GetLength() * fmt->frequency;

	//while((len=m_pInput->Read(buff, bufflen)) != EOF)	// Play till the end of the track
	while(m_iPosition < totalLength)					// Play till the end of the project
	{
		len = m_pInput->Read(buff, bufflen);			// Play till the end of the project

		// If the view has been resized horizontally the position is lost.
		if(m_pTracks->GetViewUpdatedWhilePlaying())
		{
			m_pTracks->SetViewUpdatedWhilePlaying(true);
			m_pInput->MoveTo( m_iPosition );
		}

		//if(len == 0)									// Play till the end of the track
		if(len == EOF)									// Play till the end of the project
			len = bufflen;

		m_iPosition += len / m_iSamplesToBytes;
		m_pOutput->Write( buff, len );
		m_pEngine->EmptyBuffer( buff, bufflen );

		if ( TestDestroy() )
			break;
	}

	// Wait for output to finish!!!
	if( !TestDestroy() )
	{
		int endPos = totalLength;
		int pos=0, prev=0;
		do {
			prev = pos;
			pos = GetPosition();
			if(prev == pos)
				break;
			wxThread::Sleep(10);
		} while(pos < endPos);
	}

	m_pOutput->Flush();

	m_bPlaying = false;

	delete buff;

	return NULL;
}

bool CPlayThread::Play(int sample)
{
	m_bPlaying = true;
	m_bPaused  = false;


	m_pEngine = (IFloopySoundEngine*)m_pTracks->GetInput();
	if(NULL != m_pEngine)
		m_pInput = m_pEngine;


	// Check if a track is selected
	CTrack *track = m_pTracks->GetSelectedTrack();
	if(track)
		m_pInput = track->GetInput();


	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	m_iSamplesToBytes = (fmt->bitsPerSample/8) * fmt->channels;
	m_pOutput = m_pEngine->CreateOutput("stdlib.waveout", *fmt);


	if( wxThread::IsPaused() )
		wxThread::Resume();
	else
	{
		m_iStartPos = sample;
		wxThread::Run();
	}

	return true;
}

bool CPlayThread::Pause()
{
	if( wxThread::IsPaused() )
	{
		wxThread::Resume();
		m_bPaused = false;
	}
	else if( wxThread::IsRunning() )
	{
		m_pOutput->Pause();
		wxThread::Pause();
		m_bPaused = true;
	}

	return true;
}

void CPlayThread::Stop()
{
	if(NULL != m_pOutput)
		m_pOutput->Reset();

	if( wxThread::IsRunning() )
		wxThread::Delete();
		//wxThread::Exit();
}

void CPlayThread::OnExit()
{
	m_pTracks->OnExitThread();
}

int CPlayThread::GetPosition()
{
	if(!m_pInput)
		return 0;

	if(!m_pOutput)
		return m_iStartPos;

	int samples = m_pOutput->GetPosition();

	return m_iStartPos + samples;
}

void CPlayThread::SetStartPos(int pos)
{
	m_iStartPos = pos;
	m_iPosition = m_iStartPos;
//	if(m_pInput)
//		m_pInput->MoveTo( m_iPosition );
}

bool CPlayThread::IsPlaying()
{
	return m_bPlaying;
}

bool CPlayThread::IsPaused()
{
	return m_bPaused;
}
