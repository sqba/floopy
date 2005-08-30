// PlayThread.cpp: implementation of the CPlayThread class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayThread::CPlayThread(CTracks *pTracks)
{
	m_pTracks	= pTracks;
	m_iStartPos	= 0;
	m_bPlaying	= m_bPaused = FALSE;
	m_pOutput	= NULL;
	m_pInput	= NULL;
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
	IFloopyObj *tmpobj = m_pTracks->GetSelectedObj();
	if(tmpobj && tmpobj->IsKindOf(CLASSINFO(CTrack)))
		m_pInput = ((CTrack*)tmpobj)->GetInput();

	if(!m_pInput)
		return NULL;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));

	m_pOutput = engine->CreateOutput("stdlib.waveout", *fmt);
	if(!m_pOutput)
		return NULL;
	int stb	= (fmt->bitsPerSample/8) * fmt->channels; // samples to bytes
	int pos	= m_iStartPos;	// samples
	int len	= 0;			// bytes

	//int bufflen = 5120; // Zujanje (shto je bafer manji veca je frekvenca)
	//int bufflen = fmt->frequency * stb; // Problemi na pochetku regiona
	//int bufflen = fmt->frequency * stb / 10;
	int bufflen = 64 * stb;
	BYTE *buff = new BYTE[bufflen];

	engine->EmptyBuffer( buff, bufflen );
	engine->MoveTo( m_iStartPos ); // Move to cursor position

	m_pOutput->Reset();

//	int delaySamples = 40000; // Odprilike!!! (44100?)

	while((len=m_pInput->Read(buff, bufflen)) != EOF)
	{
		pos += len / stb;
		m_pOutput->Write( buff, len );
		engine->EmptyBuffer( buff, bufflen );
		
		// Kako znati kada je stvarno pocheo da svira?
		// GetWrittenSamples == pos
//		int samples = m_pOutput->GetWrittenSamples();
//		if(samples > delaySamples)
//		{
//			m_pTracks->SetCursorPosition( m_iStartPos + samples - delaySamples );
//		}

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
		} while(pos < m_pOutput->GetWrittenSamples());
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

int CPlayThread::GetWrittenSamples()
{
	if(!m_pInput)
		return NULL;

	SOUNDFORMAT *fmt = m_pInput->GetFormat();

	int delaySamples = fmt->channels == 2 ? 40000 : 80000; // Odprilike!!! (44100?)

	if(!m_pOutput)
		return m_iStartPos;

	int samples = m_pOutput->GetWrittenSamples();
	if(samples > delaySamples)
		return m_iStartPos + samples - delaySamples;
	else
		return m_iStartPos;
}
