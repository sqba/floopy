// PlayThread.cpp: implementation of the CPlayThread class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"

#define BUFFER_LENGTH	5120 //512

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayThread::CPlayThread(CTracks *pTracks)
{
	m_pTracks	= pTracks;
	m_iStartPos	= 0;
	m_bPlaying	= m_bPaused = FALSE;
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
	IFloopySoundInput	*input = engine;

	if(!engine)
		return NULL;

	// Check if a track is selected
	IFloopyObj *tmpobj = m_pTracks->GetSelectedObj();
	if(tmpobj && tmpobj->IsKindOf(CLASSINFO(CTrack)))
		input = ((CTrack*)tmpobj)->GetInput();

	if(!input)
		return NULL;

	SOUNDFORMAT *fmt = input->GetFormat();

	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));

	IFloopySoundOutput *output = engine->CreateOutput("stdlib.waveout", *fmt);
	if(!output)
		return NULL;

	BYTE buff[BUFFER_LENGTH] = {0};
	int stb	= (fmt->bitsPerSample/8) * fmt->channels; // samples to bytes
	int pos	= m_iStartPos;	// samples
	int len	= 0;			// bytes

	engine->EmptyBuffer( buff, BUFFER_LENGTH );
	engine->MoveTo( m_iStartPos ); // Move to cursor position

	while((len=input->Read(buff, BUFFER_LENGTH)) != EOF)
	{
		pos += len / stb;
		output->Write( buff, len );
		engine->EmptyBuffer( buff, BUFFER_LENGTH );
		int samples = output->GetWrittenSamples();
		m_pTracks->SetCursorPosition( m_iStartPos + samples );

		// If the view has been resized horizontally the position is lost.
		engine->MoveTo( pos );

		if ( TestDestroy() )
			break;
	}

	// Wait for output to finish!!!
	if( !TestDestroy() )
	{
		do {
			wxThread::Sleep(1000);
		} while(pos < output->GetWrittenSamples()/fmt->channels);
	}

	m_bPlaying = FALSE;

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
