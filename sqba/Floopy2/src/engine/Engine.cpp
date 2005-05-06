// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
	m_offset = m_stopAt = m_length = 0;
}

CEngine::~CEngine()
{
	if(m_source)
		delete m_source;
}

IFloopySoundInput  *CEngine::CreateInput(char *plugin)
{
	return new CInput(plugin);
}

IFloopySoundOutput *CEngine::CreateOutput(char *plugin, WAVFORMAT *fmt)
{
	return new COutput(plugin, fmt);
}

BOOL CEngine::Open(char *filename)
{
	IFloopySoundInput *master = testCreateMaster();
	master->Reset();
	SetSource( master );
	return TRUE;
}

void CEngine::MoveTo(UINT samples)
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	int x = (fmt->size / 8) * fmt->channels;

	m_offset = samples * x;

	if(m_length > 0)
		m_stopAt = m_offset + m_length * x;

	if(NULL != m_source)
		m_source->MoveTo(samples);
}

void CEngine::Reset()
{
	if(m_length > 0)
	{
		WAVFORMAT *fmt = GetFormat();
		assert((fmt->size > 0) && (fmt->channels > 0));
		int x = (fmt->size / 8) * fmt->channels;
		m_stopAt = m_offset + m_length * x;
	}
	m_offset = 0;
	if(NULL != m_source)
		m_source->Reset();
}

UINT CEngine::Read(BYTE *data, UINT size)
{
	if((m_stopAt > 0) && (m_offset + size > m_stopAt))
		size = m_stopAt - m_offset;
	if(size <= 0)
		return 0;
	//printf("offset: %d\n", m_offset);
	UINT len = (NULL != m_source ? m_source->Read(data, size) : 0);
	m_offset += len;
	return len;
}

void CEngine::SetSize(DWORD size)
{
	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	int x = (fmt->size / 8) * fmt->channels;
	m_length = size;
	m_stopAt = m_offset + m_length * x;
}
/*
DWORD CEngine::GetSize()
{
	if(m_stopAt > 0)
	{
		WAVFORMAT *fmt = GetFormat();
		int x = (fmt->size / 8) * fmt->channels;
		return (m_stopAt - m_offset) / x;
	}
	return IFloopyEngine::GetSize();
}
*/
/*
DWORD CEngine::GetSize()
{
	//UINT offset = GetSize();
	UINT size = 0;
	UINT tmp = 0;
	while((tmp=m_timeline.GetNextOffset(tmp)) > 0)
	{
		tParam *param = m_timeline.GetParam(tmp, TIMELINE_PARAM);
		if(param && (param->value == PARAM_DISABLE))
		{
			if(tmp > size)
				size = tmp;
		}
	}

	WAVFORMAT *fmt = GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	int x = ( (fmt->size / 8) * fmt->channels );
	size /= x;

	if(0 == size)
	{
		size = (m_length > 0 ? m_length : GetSource()->GetSize());
	}

	return size;
}
*/
////////////////////////////////////////////////////////////////////////////


IFloopySoundInput *CEngine::testCreateTrack1()
{
	IFloopySoundInput *wavfile	= CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
	IFloopySoundInput *loop	= CreateInput(TEXT("loop"));
	if( wavfile->Open(TEXT("Ischlju.wav")) )
	{
		volume->SetSource(wavfile);
//		WAVFORMAT *fmt = volume->GetFormat();
		loop->SetSource(wavfile);
		//mixer->AddSource(volume);

		loop->Reset();
		loop->SetParam(0, 3);
		//loop->MoveTo(44100*1);
		//loop->Enable(FALSE);
		volume->Reset();
		volume->SetSource(loop);
		volume->SetParam(0, 50);
		volume->MoveTo(44100*2);
		volume->SetParam(0, 100);
		volume->MoveTo(44100*3);
		volume->SetParam(0, 150);
		volume->Reset();
	}

	return volume;
}

IFloopySoundInput *CEngine::testCreateTrack2()
{
	IFloopySoundInput *wavfile	= CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
	if( wavfile->Open(TEXT("presence.wav")) )
	{
		volume->SetSource(wavfile);
		volume->SetParam(0, 10);
		volume->MoveTo(44100);
		volume->SetParam(0, 20);
		volume->MoveTo(44100 + 4410*1);
		volume->SetParam(0, 40);
		volume->MoveTo(44100 + 4410*2);
		volume->SetParam(0, 60);
		volume->MoveTo(44100 + 4410*3);
		volume->SetParam(0, 80);
		volume->MoveTo(44100 + 4410*4);
		volume->SetParam(0, 60);
		volume->MoveTo(44100 + 4410*5);
		volume->SetParam(0, 50);
		volume->MoveTo(44100 + 4410*6);
		volume->SetParam(0, 20);
		volume->MoveTo(44100 + 4410*7);
		volume->SetParam(0, 10);
		volume->MoveTo(44100 + 4410*8);
		volume->SetParam(0, 1);
		//mixer->AddSource(volume);
	}

	return volume;
}

IFloopySoundInput *CEngine::testCreateTrack3()
{
	IFloopySoundInput *wavfile	= CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
	IFloopySoundInput *echo		= CreateInput(TEXT("echo"));
	if( wavfile->Open(TEXT("Gimme Sh_T.wav")) )
	{
		echo->SetSource(wavfile);
		volume->SetSource(echo);
		//mixer->AddSource(volume);

		volume->Reset();
		volume->SetParam(0, 80);
		volume->MoveTo(44100*5);
		volume->SetParam(0, 100);
		volume->MoveTo(44100*13);
		volume->SetParam(0, 50);
		volume->MoveTo(44100*17);
		volume->SetParam(0, 200);
		volume->MoveTo(44100*18);
		volume->Enable(FALSE);
		volume->MoveTo(44100*19);
		volume->Enable(TRUE);
		volume->MoveTo(44100*20);
		volume->Enable(FALSE);

		echo->Reset();
		echo->Enable(FALSE);
		echo->MoveTo(44100*10);
		echo->Enable(TRUE);
		echo->MoveTo(44100*11);
		echo->Enable(FALSE);

		echo->MoveTo(44100*21);
		echo->Enable(TRUE);
		echo->MoveTo(44100*22);
		echo->Enable(FALSE);
	}

	return echo;
}

IFloopySoundInput *CEngine::testCreateTrack4(WAVFORMAT *fmt)
{
	IFloopySoundInput *tonegen	= CreateInput(TEXT("tonegen"));
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
	IFloopySoundInput *echo		= CreateInput(TEXT("echo"));

	tonegen->SetFormat( fmt );

	volume->SetSource(tonegen);
	//mixer->AddSource(volume);

	tonegen->Reset();
	tonegen->Enable(FALSE);
	tonegen->Reset();
	tonegen->MoveTo(44100*3);
	tonegen->SetParam(0, 1800.f);
	tonegen->Enable(TRUE);
	tonegen->MoveTo(44100*4);
	tonegen->Enable(FALSE);
	tonegen->Reset();
	tonegen->MoveTo(44100*8);
	tonegen->SetParam(0, 2600.f);
	tonegen->Enable(TRUE);
	tonegen->MoveTo(44100*9);
	tonegen->Enable(FALSE);

	tonegen->MoveTo(44100*12);
	tonegen->SetParam(0, 4000.f);
	tonegen->Enable(TRUE);
	tonegen->MoveTo(44100*13);
	tonegen->SetParam(0, 800.f);
	tonegen->MoveTo(44100*14);
	tonegen->SetParam(0, 600.f);
	tonegen->MoveTo(44100*15);
	tonegen->Enable(FALSE);

	tonegen->MoveTo(44100*18);
	tonegen->SetParam(0, 1000.f);
	tonegen->Enable(TRUE);
	tonegen->MoveTo(44100*19);
	tonegen->Enable(FALSE);

	tonegen->MoveTo(44100*20);
	tonegen->SetParam(0, 2000.f);
	tonegen->Enable(TRUE);
	tonegen->MoveTo(44100*21);
	tonegen->Enable(FALSE);

	tonegen->Reset();

	volume->Reset();
	volume->SetParam(0, 80);
	volume->MoveTo(44100*8);
	volume->SetParam(0, 100);
	volume->MoveTo(44100*13);
	volume->SetParam(0, 50);

	return volume;
}

IFloopySoundInput *CEngine::testCreateMaster()
{
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
	IFloopySoundInput *mixer	= CreateInput(TEXT("mixer"));
//	IFloopySoundInput *echo	= CreateInput(TEXT("echo"));
	IFloopySoundInput *loop	= CreateInput(TEXT("loop"));
//	IFloopySoundInput *startat	= CreateInput(TEXT("startat"));

//	IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->GetSource();

	mixer->AddSource( testCreateTrack1() );
	mixer->AddSource( testCreateTrack2() );
	mixer->AddSource( testCreateTrack3() );
	WAVFORMAT *fmt = mixer->GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	mixer->AddSource( testCreateTrack4(fmt) );

	UINT size = mixer->GetSize();

	volume->SetSource(mixer);
//	echo->SetSource(volume);
	size = volume->GetSize();

	volume->Reset(); // Reset all
	volume->SetParam(0, 150);

if(0)
	return volume;
else{
	/*mixer->Reset();
	mixer->MoveTo(44100*10);
	mixer->Enable(FALSE);
	mixer->Reset();*/
	size = mixer->GetSize(); // 1142016

	loop->Reset();
	loop->SetParam(0, 2);
	loop->SetSource(volume);
	size = loop->GetSize();
	return loop;
//	startat->SetSource(loop);
//	startat->SetParam(0, 44100*2);
//	return startat;
	}
}

