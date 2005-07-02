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
	m_bPlaying = FALSE;
	Create();
}

CPlayThread::~CPlayThread()
{
	if( IsRunning() )
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

	while(((len=input->Read(buff, size)) != EOF) && m_bPlaying)
	{
		offset += len;
		output->Write(buff, len);
		memset(buff, 0, sizeof(buff));
		percent = (int)((float)offset * 100.f / (float)max);
		//del = fprintf(stderr, "%d - %d%%", output->GetWrittenSamples(), percent);
	}

	m_bPlaying = FALSE;

	return NULL;
}

void CPlayThread::Play(int sample)
{
	m_bPlaying = TRUE;
	m_pTracks->GetEngine()->MoveTo(sample);
	Run();
}

void CPlayThread::Pause()
{
	if( IsPaused() )
		Resume();
	else
		Pause();
}

void CPlayThread::Stop()
{
	m_bPlaying = FALSE;
}
