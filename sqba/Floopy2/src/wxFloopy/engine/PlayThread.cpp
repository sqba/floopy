// PlayThread.cpp: implementation of the CPlayThread class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayThread::CPlayThread(CTracks *pTracks)
{
	m_pTracks		= pTracks;
	m_iStartPos		= 0;
	m_bPlaying		= m_bPaused = FALSE;
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
	if(!m_pTracks || m_iStartPos<0)
		return NULL;

	IFloopySoundEngine	*engine = m_pTracks->GetEngine();
	m_pInput = engine;

	if(!engine)
		return NULL;

	// Check if a track is selected
	CTrack *track = m_pTracks->GetSelectedTrack();
	if(track)
		m_pInput = track->GetInput();

	if(!m_pInput)
		return NULL;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));

	m_pOutput = engine->CreateOutput("stdlib.waveout", *fmt);
	if(!m_pOutput)
		return NULL;

	int stb	= (fmt->bitsPerSample/8) * fmt->channels; // samples to bytes
	//int pos	= m_iStartPos;	// samples
	int len	= 0;			// bytes
	m_iPosition = m_iStartPos;

	//int bufflen = fmt->frequency * stb; // !!Problemi na pochetku regiona
	//int bufflen = fmt->frequency * stb / 10;
	int bufflen = m_iBufferLength * stb;
	BYTE *buff = new BYTE[bufflen];

	engine->EmptyBuffer( buff, bufflen );
	m_pInput->MoveTo( m_iStartPos ); // Move to cursor position

	m_pOutput->Reset();

	int totalLength = m_pTracks->GetLength() * fmt->frequency;

	/*while((len=m_pInput->Read(buff, bufflen)) != EOF)
	{
		// If the view has been resized horizontally the position is lost.
		if(m_pTracks->GetViewUpdatedWhilePlaying())
		{
			m_pTracks->SetViewUpdatedWhilePlaying(TRUE);
			engine->MoveTo( pos );
		}

		pos += len / stb;
		m_pOutput->Write( buff, len );
		engine->EmptyBuffer( buff, bufflen );

		if ( TestDestroy() )
			break;
	}*/

	while(m_iPosition<totalLength)
	{
		try
		{
			len = m_pInput->Read(buff, bufflen);
		}
		catch(...)
		{
			break;
		}

		// If the view has been resized horizontally the position is lost.
		if(m_pTracks->GetViewUpdatedWhilePlaying())
		{
			m_pTracks->SetViewUpdatedWhilePlaying(TRUE);
			m_pInput->MoveTo( m_iPosition );
		}

		if(len == EOF)
			len = bufflen;

		m_iPosition += len / stb;
		m_pOutput->Write( buff, len );
		engine->EmptyBuffer( buff, bufflen );

		if ( TestDestroy() )
			break;
	}

	// Wait for output to finish!!!
	if( !TestDestroy() )
	{
		do {
			wxThread::Sleep(1000);
		} while(m_iPosition < m_pOutput->GetWrittenSamples());
	}

	m_bPlaying = FALSE;

	delete buff;

	return NULL;
}

void CPlayThread::Play(int sample)
{
	m_bPlaying = TRUE;
	m_bPaused  = FALSE;

	if( wxThread::IsPaused() )
		wxThread::Resume();
	else
	{
		m_iStartPos = sample;
		wxThread::Run();
	}
}

void CPlayThread::Pause()
{
	if( wxThread::IsPaused() )
		wxThread::Resume();
	else if( wxThread::IsRunning() )
	{
		m_bPaused = TRUE;
		wxThread::Pause();
	}
}

void CPlayThread::Stop()
{
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

	int samples = m_pOutput->GetWrittenSamples();

	return m_iStartPos + samples;
}

void CPlayThread::SetStartPos(int pos)
{
	m_iStartPos = pos;
	m_iPosition = m_iStartPos;
//	if(m_pInput)
//		m_pInput->MoveTo( m_iPosition );
}
