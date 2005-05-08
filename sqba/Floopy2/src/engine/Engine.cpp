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
	//m_offset = m_stopAt = m_length = 0;
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

IFloopySoundOutput *CEngine::CreateOutput(char *plugin, WAVFORMAT fmt)
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


////////////////////////////////////////////////////////////////////////////


IFloopySoundInput *CEngine::testCreateTrack1()
{
	IFloopySoundInput *wavfile	= CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
	IFloopySoundInput *loop	= CreateInput(TEXT("loop"));
	IFloopySoundInput *cache	= CreateInput(TEXT("cache"));
	if( wavfile->Open(TEXT("Ischlju.wav")) )
	{
		cache->SetSource(wavfile);

//		loop->SetSource(wavfile);
		loop->SetSource(cache);

		volume->Reset();
		volume->SetSource(loop);

		loop->Reset();

		if(1)
		{
			volume->SetParam(0, 50);
			volume->MoveTo(44100*2);
			volume->SetParam(0, 100);
			volume->MoveTo(44100*3);
			volume->SetParam(0, 150);
			volume->MoveTo(44100*23);
			volume->SetParam(0, 50);
			volume->Reset();

			loop->SetParam(0, 3);

			loop->MoveTo(44100 * 23);
			loop->SetParam(0, 2);
		}
		else
		{
			// Infinite
			loop->MoveTo(44100 * 22);
			loop->Enable(FALSE);
		}
	}

	return volume;
}

IFloopySoundInput *CEngine::testCreateTrack2()
{
	IFloopySoundInput *wavfile	= CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= CreateInput(TEXT("volume"));
//	IFloopySoundInput *cache	= CreateInput(TEXT("cache"));
	if( wavfile->Open(TEXT("presence.wav")) )
	{
//		cache->SetSource(wavfile);
//		volume->SetSource(cache);
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
	IFloopySoundInput *cache	= CreateInput(TEXT("cache"));
	if( wavfile->Open(TEXT("Gimme Sh_T.wav")) )
	{
		if(1)
		{
			cache->SetSource(wavfile);
			echo->SetSource(cache);
		}
		else
			echo->SetSource(wavfile);

		volume->SetSource(echo);
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

	volume->SetSource(tonegen);
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
//	IFloopySoundInput *playrgn	= CreateInput(TEXT("playrgn"));
//	IFloopySoundInput *cache	= CreateInput(TEXT("cache"));

//	IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->GetSource();

	mixer->AddSource( testCreateTrack1() );
	mixer->AddSource( testCreateTrack2() );
	mixer->AddSource( testCreateTrack3() );
	WAVFORMAT *fmt = mixer->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	mixer->AddSource( testCreateTrack4(fmt) );

//	int size = mixer->GetSize();

	volume->SetSource(mixer);
//	echo->SetSource(volume);

	volume->Reset(); // Reset all
	volume->SetParam(0, 150);

//	if(0)
		return volume;
/*	else
	{
		if(0)
		{
			cache->SetSource(volume);
		}
		else
		{
			playrgn->SetSource(volume);
			playrgn->SetParam(0, 44100*20);
			playrgn->SetParam(1, 44100*50);
			cache->SetSource(playrgn);
		}

		if(1)
		{
			loop->Reset();
			loop->SetParam(0, 2);
			loop->SetSource(cache);
			return loop;
		}
		else
			return cache;
	}*/
}

