// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "Engine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
//	m_master = NULL;

	m_offset = 0;

	test();
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

void CEngine::MoveTo(UINT samples)
{
	m_offset = samples;

	if(NULL != m_source)
		m_source->MoveTo(samples);
}

void CEngine::Reset()
{
	m_offset = 0;
	if(NULL != m_source)
		m_source->Reset();
}

UINT CEngine::Read(BYTE *data, UINT size)
{
	//printf("offset: %d\n", m_offset);
	UINT len = (NULL != m_source ? m_source->Read(data, size) : 0);
	m_offset += len;
	return len;
}





void CEngine::testAddTrack1(IFloopySoundMixer *mixer)
{
	CInput *wavfile	= new CInput(TEXT("wavfile"));
	CInput *volume	= new CInput(TEXT("volume"));
	CInput *loop	= new CInput(TEXT("loop"));
	if( wavfile->Open(TEXT("Ischlju.wav")) )
	{
		loop->SetSource(wavfile);
		mixer->AddSource(volume);

		loop->Reset();
		loop->SetParam(0, 3);
		volume->Reset();
		volume->SetSource(loop);
		volume->SetParam(0, 50);
		volume->MoveTo(44100*2);
		volume->SetParam(0, 100);
		volume->MoveTo(44100*3);
		volume->SetParam(0, 150);
		volume->Reset();
	}
}

void CEngine::testAddTrack2(IFloopySoundMixer *mixer)
{
	CInput *wavfile	= new CInput(TEXT("wavfile"));
	CInput *volume	= new CInput(TEXT("volume"));
	if( wavfile->Open(TEXT("presence.wav")) )
	{
		volume->SetSource(wavfile);
		volume->SetParam(0, 40);
		volume->MoveTo(44100);
		volume->SetParam(0, 80);
		mixer->AddSource(volume);
	}
}

void CEngine::testAddTrack3(IFloopySoundMixer *mixer)
{
	CInput *wavfile	= new CInput(TEXT("wavfile"));
	CInput *volume	= new CInput(TEXT("volume"));
	CInput *echo	= new CInput(TEXT("echo"));
	if( wavfile->Open(TEXT("Gimme Sh_T.wav")) )
	{
		echo->SetSource(wavfile);
		volume->SetSource(echo);
		mixer->AddSource(volume);
	}
}

void CEngine::testAddTrack4(IFloopySoundMixer *mixer)
{
	CInput *tonegen	= new CInput(TEXT("tonegen"));
	CInput *volume	= new CInput(TEXT("volume"));
	CInput *echo	= new CInput(TEXT("echo"));

	tonegen->SetFormat( mixer->GetFormat() );

	volume->SetSource(tonegen);
	mixer->AddSource(volume);

	volume->SetParam(0, 80);

	tonegen->Reset();
	tonegen->SetActive(FALSE);
	tonegen->Reset();
	tonegen->MoveTo(44100*3);
	tonegen->SetParam(0, 1800.f);
	tonegen->SetActive(TRUE);
	tonegen->MoveTo(44100*4);
	tonegen->SetActive(FALSE);
	tonegen->Reset();
	tonegen->MoveTo(44100*8);
	volume->MoveTo(44100*8);
	volume->SetParam(0, 100);
	tonegen->SetParam(0, 2600.f);
	tonegen->SetActive(TRUE);
	tonegen->MoveTo(44100*9);
	tonegen->SetActive(FALSE);
	tonegen->MoveTo(44100*12);
	tonegen->SetParam(0, 4000.f);
	tonegen->SetActive(TRUE);
	volume->MoveTo(44100*13);
	volume->SetParam(0, 50);
	tonegen->MoveTo(44100*13);
	tonegen->SetParam(0, 800.f);
	tonegen->MoveTo(44100*14);
	tonegen->SetParam(0, 600.f);
	tonegen->MoveTo(44100*15);
	tonegen->SetActive(FALSE);
	tonegen->Reset();
}

void CEngine::test()
{
	CInput *volume	= new CInput(TEXT("volume"));
	CInput *mixer = new CInput(TEXT("mixer"));
	IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->GetSource();

	volume->SetSource(mxr);

	testAddTrack1(mxr);
	testAddTrack2(mxr);
	testAddTrack3(mxr);
	testAddTrack4(mxr);

	volume->Reset(); // Reset all
	volume->SetParam(0, 150);

	SetSource( volume );
}








/*
void CEngine::test()
{
//	m_timeline.Set(44100, 0, 0.5f);
//	int i = m_timeline.GetCount();
//	m_timeline.Set(44100*2, 0, 2.5f);
//	m_timeline.Set(44101, 0, 3.5f);
//	i = m_timeline.GetCount();
//	UINT s = m_timeline.GetNextOffset(44100);

	CInput *wavfilein1	= new CInput(TEXT("wavfile"));
	CInput *wavfilein2	= new CInput(TEXT("wavfile"));
	CInput *wavfilein3	= new CInput(TEXT("wavfile"));
	CInput *echo1		= new CInput(TEXT("echo"));
	CInput *echo2		= new CInput(TEXT("echo"));
	CInput *echo3		= new CInput(TEXT("echo"));
	CInput *mixer		= new CInput(TEXT("mixer"));
	CInput *loop		= new CInput(TEXT("loop"));
	CInput *tonegen		= new CInput(TEXT("tonegen"));
	CInput *volume1		= new CInput(TEXT("volume"));
	CInput *volume2		= new CInput(TEXT("volume"));
	CInput *volume3		= new CInput(TEXT("volume"));
	CInput *volume4		= new CInput(TEXT("volume"));
//	CInput *playrgn1	= new CInput(TEXT("playrgn"));
//	CInput *playrgn2	= new CInput(TEXT("playrgn"));
//	CInput *playrgn3	= new CInput(TEXT("playrgn"));

	CInput *master = mixer;

	if( wavfilein1->Open(TEXT("Ischlju.wav")) && 
		wavfilein2->Open(TEXT("presence.wav")) && 
		wavfilein3->Open(TEXT("Gimme Sh_T.wav")) )
	{
		IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->GetSource();

		WAVFORMAT *fmt = wavfilein1->GetFormat();

		loop->SetParam(0, 3);
		loop->SetSource(wavfilein1);
//		playrgn3->SetSource(loop);
//		playrgn3->SetParam(0, 44100.f);	// Start
		volume3->SetSource(loop);
		volume3->SetParam(0, 50);
		volume3->MoveTo(44100*2);
		volume3->SetParam(0, 100);
		volume3->MoveTo(44100*3);
		volume3->SetParam(0, 150);
		volume3->Reset();
		mxr->AddSource(volume3);

//		playrgn2->SetSource(wavfilein2);
//		playrgn2->SetParam(0, 44100.f*5.f);	// Start
		mxr->AddSource(wavfilein2);

		echo2->SetSource(wavfilein3);
		mxr->AddSource(echo2);

		mxr->AddSource(tonegen);

		tonegen->Reset();
		tonegen->SetActive(FALSE);

		tonegen->SetFormat( fmt );
		tonegen->Reset();
		master->MoveTo(44100*3);
		tonegen->SetParam(0, 1800.f);
		tonegen->SetActive(TRUE);
		tonegen->MoveTo(44100*4);
		tonegen->SetActive(FALSE);
//		playrgn1->SetSource(tonegen);
//		playrgn1->SetParam(0, 44100.f*6.f);	// Start
//		playrgn1->SetParam(1, 44100.f*7.f);	// Stop
		tonegen->Reset();
		tonegen->MoveTo(44100*8);
		tonegen->SetParam(0, 2600.f);
		tonegen->SetActive(TRUE);
		tonegen->MoveTo(44100*9);
		tonegen->SetActive(FALSE);
//		playrgn1->MoveTo(44100*9);
//		playrgn1->SetParam(0, 44100.f*9.f);	// Start
//		playrgn1->SetParam(1, 44100.f*10.f);	// Stop
//		playrgn1->Reset();
//		playrgn1->MoveTo(44100*12);
//		tonegen->Reset();
//		tonegen->MoveTo(44100*13);
//		tonegen->SetParam(0, 2600.f);
//		playrgn1->SetParam(0, 44100.f*12.f);	// Start
//		playrgn1->SetParam(1, 44100.f*15.f);	// Stop
//		playrgn1->Reset();
		//tonegen->Reset();
//		tonegen->MoveTo(44100*13);
//		tonegen->SetActive(FALSE);
//		tonegen->Reset();
		tonegen->MoveTo(44100*12);
		tonegen->SetParam(0, 4000.f);
		tonegen->SetActive(TRUE);
		tonegen->MoveTo(44100*13);
		tonegen->SetParam(0, 800.f);
		tonegen->MoveTo(44100*14);
		tonegen->SetParam(0, 600.f);
		tonegen->MoveTo(44100*15);
		tonegen->SetActive(FALSE);
		tonegen->Reset();

		loop->Reset();
		loop->MoveTo(44100*15);
		loop->SetParam(0, 2);
		volume3->MoveTo(44100*15);
		volume3->SetParam(0, 200);
		loop->SetActive(TRUE);

		loop->Reset();
		loop->MoveTo(44100*17);
		loop->SetParam(0, 2);
		volume3->MoveTo(44100*17);
		volume3->SetParam(0, 100);
		loop->SetActive(TRUE);

		echo1->SetSource(mixer);

//		loop->Reset();
//		echo1->Reset();
//		echo2->Reset();
//		volume3->Reset();
//		tonegen->Reset();
//		mixer->Reset();

		// Master
		master->Reset();
		volume4->SetSource(master);
		volume4->SetParam(0, 200);
		SetSource( volume4 );

//		volume2->SetSource(echo1);
//		volume2->MoveTo(44100);
//		volume2->SetParam(0, 150);
//		volume2->Reset();
//		SetSource( volume2 );

		//int n = playrgn1->GetParam(0);



//		mxr->AddSource(wavfilein1);
//		mxr->AddSource(wavfilein2);
//		mxr->AddSource(wavfilein3);

//		WAVFORMAT *fmt = wavfilein1->GetFormat();
//		tonegen->SetFormat(wavfilein1->GetFormat());
//		fmt = tonegen->GetFormat();
//		tonegen->SetParam(0, 1800.f);
//		playrgn1->SetSource(tonegen);
//		fmt = playrgn1->GetFormat();
//		playrgn1->SetParam(0, 44100.f*2.f);	// Start
//		playrgn1->SetParam(1, 44100.f*3.f);	// Stop
//		mxr->AddSource(playrgn1);

//		SetSource( mxr );


		// Total time: 25056 ms WaveOut
		// Total time: 2984 ms
		// Total time: 3104 ms
		// Total time: 3054 ms
		// Total time: 2473 ms posle prve optimizacije
		// Total time: 2333 ms
		// Total time: 2204 ms posle druge optimizacije
		// Total time: 1812 ms
		// Total time: 2393 ms
		// Total time: 1792 ms

	}
}
*/

/*

void CEngine::test()
{
//	m_timeline.Set(44100, 0, 0.5f);
//	int i = m_timeline.GetCount();
//	m_timeline.Set(44100*2, 0, 2.5f);
//	m_timeline.Set(44101, 0, 3.5f);
//	i = m_timeline.GetCount();
//	UINT s = m_timeline.GetNextOffset(44100);

	CInput *wavfilein1	= new CInput(TEXT("wavfile"));
	CInput *wavfilein2	= new CInput(TEXT("wavfile"));
	CInput *wavfilein3	= new CInput(TEXT("wavfile"));
	CInput *echo1		= new CInput(TEXT("echo"));
	CInput *echo2		= new CInput(TEXT("echo"));
	CInput *echo3		= new CInput(TEXT("echo"));
	CInput *mixer		= new CInput(TEXT("mixer"));
	CInput *loop		= new CInput(TEXT("loop"));
	CInput *tonegen		= new CInput(TEXT("tonegen"));
	CInput *volume1		= new CInput(TEXT("volume"));
	CInput *volume2		= new CInput(TEXT("volume"));
	CInput *volume3		= new CInput(TEXT("volume"));
	CInput *playrgn1	= new CInput(TEXT("playrgn"));
	CInput *playrgn2	= new CInput(TEXT("playrgn"));
	CInput *playrgn3	= new CInput(TEXT("playrgn"));

	CInput *master = echo1;

	if( wavfilein1->Open(TEXT("Ischlju.wav")) && 
		wavfilein2->Open(TEXT("presence.wav")) && 
		wavfilein3->Open(TEXT("Gimme Sh_T.wav")) )
	{
		IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->GetSource();

		WAVFORMAT *fmt = wavfilein1->GetFormat();

//		loop->SetParam(0, 2);
//		loop->SetSource(wavfilein1);
//		playrgn3->SetSource(loop);
//		playrgn3->SetParam(0, 44100.f);	// Start
//		mxr->AddSource(playrgn3);

		loop->SetParam(0, 3);
		loop->SetSource(wavfilein1);
		playrgn3->SetSource(loop);
		playrgn3->SetParam(0, 44100.f);	// Start
		volume3->SetSource(playrgn3);
		volume3->MoveTo(44100*2);
		volume3->SetParam(0, 100);
		volume3->MoveTo(44100*3);
		volume3->SetParam(0, 1);
		master->Reset();
		mxr->AddSource(volume3);

		playrgn2->SetSource(wavfilein2);
		playrgn2->SetParam(0, 44100.f*5.f);	// Start
		mxr->AddSource(playrgn2);

		echo2->SetSource(wavfilein3);
		mxr->AddSource(echo2);

//		//echo2->SetSource(wavfilein3);
//		volume3->SetSource(wavfilein3);
//		volume3->MoveTo(44100*12);
//		volume3->SetParam(0, 150);
//		volume3->Reset();
//		mxr->AddSource(volume3);
		
		//echo2->SetSource(wavfilein3);
		//volume1->SetSource(echo2);
		
		//volume1->SetSource(wavfilein3);
		//volume1->SetParam(0, 150);
		//mxr->AddSource(volume1);

		tonegen->SetFormat( fmt );
		tonegen->SetParam(0, 1800.f);
		playrgn1->SetSource(tonegen);
		playrgn1->SetParam(0, 44100.f*6.f);	// Start
		playrgn1->SetParam(1, 44100.f*7.f);	// Stop
		tonegen->MoveTo(44100*8);
		tonegen->SetParam(0, 2600.f);
		playrgn1->MoveTo(44100*9);
		playrgn1->SetParam(0, 44100.f*9.f);	// Start
		playrgn1->SetParam(1, 44100.f*10.f);	// Stop
		playrgn1->Reset();
		playrgn1->MoveTo(44100*12);
		tonegen->Reset();
		tonegen->MoveTo(44100*13);
		tonegen->SetParam(0, 2600.f);
		playrgn1->SetParam(0, 44100.f*12.f);	// Start
		playrgn1->SetParam(1, 44100.f*15.f);	// Stop
		playrgn1->Reset();
		//tonegen->Reset();
		mxr->AddSource(playrgn1);

		echo1->SetSource(mixer);

		// Master
		master->Reset();
		SetSource( master );

//		volume2->SetSource(echo1);
//		volume2->MoveTo(44100);
//		volume2->SetParam(0, 150);
//		volume2->Reset();
//		SetSource( volume2 );

		//int n = playrgn1->GetParam(0);



//		mxr->AddSource(wavfilein1);
//		mxr->AddSource(wavfilein2);
//		mxr->AddSource(wavfilein3);

//		WAVFORMAT *fmt = wavfilein1->GetFormat();
//		tonegen->SetFormat(wavfilein1->GetFormat());
//		fmt = tonegen->GetFormat();
//		tonegen->SetParam(0, 1800.f);
//		playrgn1->SetSource(tonegen);
//		fmt = playrgn1->GetFormat();
//		playrgn1->SetParam(0, 44100.f*2.f);	// Start
//		playrgn1->SetParam(1, 44100.f*3.f);	// Stop
//		mxr->AddSource(playrgn1);

//		SetSource( mxr );


		// Total time: 25056 ms WaveOut
		// Total time: 2984 ms
		// Total time: 3104 ms
		// Total time: 3054 ms
		// Total time: 2473 ms posle prve optimizacije
		// Total time: 2333 ms
		// Total time: 2204 ms posle druge optimizacije
		// Total time: 1812 ms
		// Total time: 2393 ms
		// Total time: 1792 ms

	}
}
*/





/*
void printPath(IFloopySoundInput *input)
{
	IFloopySoundInput *tmp = input;
	while(NULL != tmp)
	{
		printf("<< %s ", tmp->GetName());
		tmp = tmp->GetSource();
	}
	printf("\n");
}

void CEngine::process(CInput *input, COutput *output)
{
	printPath(input, output);

	clock_t start = clock();

	BYTE buff[128];
	UINT len, size=sizeof(buff);
	memset(buff, 0, sizeof(buff));
//	while(((len=input->Read(buff, size, 0)) > 0) && (offset<1000000))
	while((len=input->Read(buff, size, 0)) > 0)
	{
//		offset += len;
		output->Write(buff, len);
		memset(buff, 0, sizeof(buff));
	}
	output->Close();

	DWORD speed = clock() - start;
	printf("Total time: %d ms\n", speed);
}

void CEngine::test()
{
	CInput *wavfilein1 = NULL;
	CInput *wavfilein2 = NULL;
	CInput *wavfilein3 = NULL;
	CInput *input = NULL;
//	COutput *output = NULL;
	CInput *echo1 = NULL;
	CInput *echo2 = NULL;
	CInput *echo3 = NULL;
	CInput *mixer = NULL;
	CInput *loop = NULL;
	CInput *tonegen = NULL;
	CInput *volume = NULL;
//	CInput *length = NULL;
//	CInput *startat1 = NULL;
//	CInput *startat2 = NULL;
	CInput *playrgn1 = NULL;
	CInput *playrgn2 = NULL;
//	CInput *view = NULL;

//	CStorage *storage = NULL;
//	storage = new CStorage(TEXT("storage"));

	//wavfilein1 = new CInput(argv[1]);
	wavfilein1 = new CInput(TEXT("wavfile"));
	wavfilein2 = new CInput(TEXT("wavfile"));
	wavfilein3 = new CInput(TEXT("wavfile"));

	echo1 = new CInput(TEXT("echo"));
	echo2 = new CInput(TEXT("echo"));
	echo3 = new CInput(TEXT("echo"));

	mixer = new CInput(TEXT("mixer"));

	loop = new CInput(TEXT("loop"));

	tonegen = new CInput(TEXT("tonegen"));

	volume = new CInput(TEXT("volume"));

//	length = new CInput(TEXT("length"));

//	startat1 = new CInput(TEXT("startat"));

//	startat2 = new CInput(TEXT("startat"));

	playrgn1 = new CInput(TEXT("playrgn"));
	playrgn2 = new CInput(TEXT("playrgn"));

//	view = new CInput(TEXT("buffview"));

	if( wavfilein1->Open(TEXT("Ischlju.wav")) && 
		wavfilein2->Open(TEXT("presence.wav")) && 
		wavfilein3->Open(TEXT("Gimme Sh_T.wav")) )
	{
		IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->getPlugin();

		loop->getPlugin()->SetParam(0, 4);
		loop->getPlugin()->SetSource(wavfilein1->getPlugin());

		//volume->getPlugin()->SetSource(loop->getPlugin());
		//mxr->AddSource(volume->getPlugin());
		mxr->AddSource(loop->getPlugin());

//		startat1->getPlugin()->SetSource(wavfilein2->getPlugin());
//		startat1->getPlugin()->SetParam(0, 44100.f * 3.f);
//		mxr->AddSource(startat1->getPlugin());
		playrgn2->getPlugin()->SetSource(wavfilein2->getPlugin());
		playrgn2->getPlugin()->SetParam(0, 44100.f*5.f);	// Start
		//playrgn2->getPlugin()->SetParam(1, 44100.f*3.f);	// Stop
		mxr->AddSource(playrgn2->getPlugin());

		echo2->getPlugin()->SetSource(wavfilein3->getPlugin());
		mxr->AddSource(echo2->getPlugin());

		tonegen->getPlugin()->SetFormat(wavfilein1->getPlugin()->GetFormat());
		//mxr->AddSource(echo3->getPlugin());
		tonegen->getPlugin()->SetParam(0, 1800.f);

//		//length->getPlugin()->SetParam(0, 300000.f);
//		length->getPlugin()->SetSource(tonegen->getPlugin());
//		length->getPlugin()->SetParam(0, 44100.f);
//		startat2->getPlugin()->SetSource(length->getPlugin());
//		startat2->getPlugin()->SetParam(0, 44100.f*2.f);
//		mxr->AddSource(startat2->getPlugin());

		playrgn1->getPlugin()->SetSource(tonegen->getPlugin());
		playrgn1->getPlugin()->SetParam(0, 44100.f*2.f);	// Start
		playrgn1->getPlugin()->SetParam(1, 44100.f*3.f);	// Stop
		//playrgn2->getPlugin()->SetSource(playrgn1->getPlugin());
		//playrgn1->getPlugin()->SetParam(0, 44100.f*4.f);	// Start
		//playrgn1->getPlugin()->SetParam(1, 44100.f*5.f);	// Stop
		//mxr->AddSource(playrgn2->getPlugin());
		mxr->AddSource(playrgn1->getPlugin());


//		output = new COutput(TEXT("wavfile"), mxr);
//		output->Open(TEXT("out.wav"));
		//output = new COutput(TEXT("waveout"), mxr);


		echo1->getPlugin()->SetSource(mixer->getPlugin());
		//echo3->getPlugin()->SetSource(echo2->getPlugin());

		//input = wavfile;
//		m_master = echo1;

//		printPath(echo1);

		// Master
		SetSource( echo1->getPlugin() );

//		printPath(GetSource());
		//printPath( echo1 );

		//input = mixer;
		//input = wavfilein1;

		//printf("Press enter to start");
		//getchar();

//		view->getPlugin()->SetSource(input->getPlugin());

//		process(input, output);

//		mxr->Close();
	
		//printf("Press enter to continue");
		//getchar();

		// Total time: 25056 ms WaveOut
		// Total time: 2984 ms
		// Total time: 3104 ms
		// Total time: 3054 ms
		// Total time: 2473 ms posle prve optimizacije
		// Total time: 2333 ms
		// Total time: 2204 ms posle druge optimizacije
		// Total time: 1812 ms
		// Total time: 2393 ms
		// Total time: 1792 ms

	}
}

UINT CEngine::Read(BYTE *data, UINT size, UINT offset)
{
	return IFloopySoundInput::Read(data, size, offset);
}
*/



