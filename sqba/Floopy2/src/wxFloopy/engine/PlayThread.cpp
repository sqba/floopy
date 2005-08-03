// PlayThread.cpp: implementation of the CPlayThread class.
//
//////////////////////////////////////////////////////////////////////

#include "tracks.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayThread::CPlayThread(CTracks *pTracks)
{
	m_pTracks = pTracks;
	/*if(wxTHREAD_NO_ERROR != wxThread::Create())
	{
	}*/
}

CPlayThread::~CPlayThread()
{
	if( wxThread::IsRunning() )
		Stop();
}

#define BUFFER_LENGTH	5120 //512

void *CPlayThread::Entry()
{
	IFloopySoundEngine *input = m_pTracks->GetEngine();
	if(!input)
		return NULL;

	int samples = input->GetSize();

	SOUNDFORMAT *fmt = input->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int stb = (fmt->bitsPerSample / 8) * fmt->channels;

	IFloopySoundOutput *output = input->CreateOutput("stdlib.waveout", *fmt);
	if(!output)
		return NULL;

	int offset = 0;
	BYTE buff[BUFFER_LENGTH];
	int len, size=sizeof(buff);
	memset(buff, 0, sizeof(buff));

	int max = samples * stb;
	int percent = 0;

	while((len=input->Read(buff, size)) != EOF)
	{
		if ( TestDestroy() )
			break;

		offset += len;
		output->Write(buff, len);
		memset(buff, 0, sizeof(buff));
		percent = (int)((float)offset * 100.f / (float)max);
		//del = fprintf(stderr, "%d - %d%%", output->GetWrittenSamples(), percent);
		int samples = output->GetWrittenSamples();
		m_pTracks->SetCursorPosition( samples );
//		m_pTracks->SetCaretPos( samples );
	}

	return NULL;
}

void CPlayThread::Play(int sample)
{
	if( wxThread::IsPaused() )
		wxThread::Resume();
	else
	{
		//if(wxTHREAD_NO_ERROR != wxThread::Create())
		//{
			m_pTracks->GetEngine()->MoveTo(sample);
			wxThread::Run();
		//}
	}
}

void CPlayThread::Pause()
{
	if( wxThread::IsPaused() )
		wxThread::Resume();
	else if( wxThread::IsRunning() )
		wxThread::Pause();
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
