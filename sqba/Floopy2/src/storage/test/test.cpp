// test.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "../../ifloopy.h"


/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
*/


BOOL loadXML(IFloopyEngine *engine, char *filename);
BOOL saveXML(IFloopyEngine *engine, char *filename);
IFloopySoundInput *testCreateMaster(IFloopyEngine *engine);
IFloopySoundInput *testCreateTrack4(IFloopyEngine *engine, WAVFORMAT *fmt);
IFloopySoundInput *testCreateTrack3(IFloopyEngine *engine);
IFloopySoundInput *testCreateTrack2(IFloopyEngine *engine);
IFloopySoundInput *testCreateTrack1(IFloopyEngine *engine);
void printPath(IFloopySoundInput *input, int level);


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) BOOL Load(IFloopyEngine *engine, char *filename)
{
	return loadXML(engine, filename);
}

__declspec( dllexport ) BOOL Save(IFloopyEngine *engine, char *filename)
{
	return saveXML(engine, filename);
}

#ifdef __cplusplus
}
#endif


BOOL loadXML(IFloopyEngine *engine, char *filename)
{
	IFloopySoundInput *master = testCreateMaster(engine);
	master->Reset();
	engine->SetSource( master );

	return TRUE;
}

BOOL saveXML(IFloopyEngine *engine, char *filename)
{
//	printPath(engine, 1);

	FILE *fp = fopen("test.txt", "w");
	if(NULL == fp)
		return FALSE;

	engine->Reset();

	IFloopySoundInput *tmp = engine;
	while(tmp)
	{
		fprintf(fp, "%s\n", tmp->GetName());
		int offset = 0;
		do {
			fprintf(fp, "%d\t", offset);
			tmp->MoveTo(offset);
			for(int i=0; i<tmp->GetParamCount(); i++)
			{
				fprintf(fp, "%d %s %f\t", i, tmp->GetParamName(i), tmp->GetParam(i));
			}
			fprintf(fp, "\n");
		} while ((offset=tmp->GetNextOffset(offset)) > 0);
		tmp = tmp->GetSource();
	}

	engine->Reset();

	fclose(fp);

	return FALSE;
}




void printPath(IFloopySoundInput *input, int level)
{
	if(!input)
		return;

	char space[100] = {0};
	if(level > 0)
	{
		int indent = level*2;
		for(int i=0; i<indent-2; i++)
			space[i] = ' ';
		
		space[i] = (char)0xb3;
		printf("%s\n", space);
		
		space[i] = (char)0xc0;
		
		space[i+1] = (char)0xc4;
		space[i+2] = (char)0xc4;
	}

	printf("%s%s\n", space, input->GetName());

	for(int i=0; i<input->GetInputCount(); i++)
	{
		printPath(input->GetSource(i), level+1);
	}
}



















IFloopySoundInput *testCreateTrack1(IFloopyEngine *engine)
{
	IFloopySoundInput *wavfile	= engine->CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= engine->CreateInput(TEXT("volume"));
	IFloopySoundInput *loop		= engine->CreateInput(TEXT("loop"));
	IFloopySoundInput *cache	= engine->CreateInput(TEXT("cache"));
	if( wavfile->Open(TEXT("Ischlju.wav")) )
	{
		if(0)
		{
			loop->SetSource(wavfile);
		}
		else
		{
			cache->SetSource(wavfile);
			loop->SetSource(cache);
		}

		volume->SetSource(loop);

		wavfile->Reset();
		volume->Reset();
		loop->Reset();
			
		loop->SetParam(-333, 1);
		wavfile->SetParam(-333, 2);
		volume->SetParam(-333, 3);

//		if(1)
//		{

		int prb1=1, prb2=0, prb3=0;
		if(prb1)
		{
			wavfile->Enable(TRUE);
			wavfile->MoveTo(44100 * 3);
			wavfile->Enable(FALSE);
			wavfile->MoveTo(44100 * 23);
			wavfile->Enable(TRUE);
			wavfile->MoveTo(44100 * 24);
			wavfile->Enable(FALSE);
		}
		if(prb1)
		{
			loop->Enable(TRUE);
			loop->MoveTo(44100 * 3);
			loop->Enable(FALSE);
			loop->MoveTo(44100 * 23);
			loop->Enable(TRUE);
			loop->MoveTo(44100 * 24);
			loop->Enable(FALSE);
		}
		if(prb1)
		{
			volume->MoveTo(44100 * 3);
			volume->Enable(FALSE);

			volume->MoveTo(44100 * 23);
			volume->Enable(TRUE);

			volume->MoveTo(44100 * 24);
			volume->Enable(FALSE);
		}

			volume->SetParam(0, 50);
			volume->MoveTo(44100*2);
			volume->SetParam(0, 100);
			volume->MoveTo(44100*3);
			volume->SetParam(0, 150);
			volume->MoveTo(44100*23);
			volume->SetParam(0, 50);

//			loop->SetParam(0, 3);

			volume->Reset();

		/*}
		else
		{
			// Infinite
			loop->MoveTo(44100 * 22);
			loop->Enable(FALSE);
		}*/
	}

	return volume;
}

IFloopySoundInput *testCreateTrack2(IFloopyEngine *engine)
{
	IFloopySoundInput *wavfile	= engine->CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= engine->CreateInput(TEXT("volume"));
	IFloopySoundInput *cache	= engine->CreateInput(TEXT("cache"));
	if( wavfile->Open(TEXT("presence.wav")) )
	{
		if(0)
		{
			volume->SetSource(wavfile);
		}
		else
		{
			cache->SetSource(wavfile);
			volume->SetSource(cache);
		}
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

IFloopySoundInput *testCreateTrack3(IFloopyEngine *engine)
{
	IFloopySoundInput *wavfile	= engine->CreateInput(TEXT("wavfile"));
	IFloopySoundInput *volume	= engine->CreateInput(TEXT("volume"));
	IFloopySoundInput *echo		= engine->CreateInput(TEXT("echo"));
	IFloopySoundInput *cache	= engine->CreateInput(TEXT("cache"));
	if( wavfile->Open(TEXT("Gimme Sh_T.wav")) )
	{
		if(0)
		{
			echo->SetSource(wavfile);
		}
		else
		{
			cache->SetSource(wavfile);
			echo->SetSource(cache);
		}

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

IFloopySoundInput *testCreateTrack4(IFloopyEngine *engine, WAVFORMAT *fmt)
{
	IFloopySoundInput *tonegen	= engine->CreateInput(TEXT("tonegen"));
	IFloopySoundInput *volume	= engine->CreateInput(TEXT("volume"));
	//IFloopySoundInput *echo		= CreateInput(TEXT("echo"));

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

IFloopySoundInput *testCreateMaster(IFloopyEngine *engine)
{
	IFloopySoundInput *volume	= engine->CreateInput(TEXT("volume"));
	IFloopySoundInput *mixer	= engine->CreateInput(TEXT("mixer"));
//	IFloopySoundInput *echo	= CreateInput(TEXT("echo"));
//	IFloopySoundInput *loop	= CreateInput(TEXT("loop"));
//	IFloopySoundInput *startat	= CreateInput(TEXT("startat"));
//	IFloopySoundInput *playrgn	= CreateInput(TEXT("playrgn"));
//	IFloopySoundInput *cache	= CreateInput(TEXT("cache"));

//	IFloopySoundMixer *mxr = (IFloopySoundMixer*)mixer->GetSource();

	mixer->AddSource( testCreateTrack1(engine) );
	mixer->AddSource( testCreateTrack2(engine) );
	mixer->AddSource( testCreateTrack3(engine) );
	WAVFORMAT *fmt = mixer->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	mixer->AddSource( testCreateTrack4(engine, fmt) );

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

