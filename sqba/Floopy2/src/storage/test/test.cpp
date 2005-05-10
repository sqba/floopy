// test.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <stdio.h>
#include <string.h>
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


BOOL load(IFloopyEngine *engine, char *filename);
BOOL save(IFloopyEngine *engine, char *filename);

void printTree(FILE *fp, IFloopySoundInput *input, int level, BOOL bTree, BOOL bLast);
void printTimeline(FILE *fp, IFloopySoundInput *input, int freq, BOOL recursive);

void saveXML(FILE *fp, IFloopySoundInput *input, BOOL recursive);

IFloopySoundInput *testCreateMaster(IFloopyEngine *engine);
IFloopySoundInput *testCreateTrack4(IFloopyEngine *engine, WAVFORMAT *fmt);
IFloopySoundInput *testCreateTrack3(IFloopyEngine *engine);
IFloopySoundInput *testCreateTrack2(IFloopyEngine *engine);
IFloopySoundInput *testCreateTrack1(IFloopyEngine *engine);


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) BOOL Load(IFloopyEngine *engine, char *filename)
{
	return load(engine, filename);
}

__declspec( dllexport ) BOOL Save(IFloopyEngine *engine, char *filename)
{
	return save(engine, filename);
}

#ifdef __cplusplus
}
#endif


BOOL load(IFloopyEngine *engine, char *filename)
{
	IFloopySoundInput *master = testCreateMaster(engine);
	master->Reset();
	engine->SetSource( master );

	return TRUE;
}

BOOL save(IFloopyEngine *engine, char *filename)
{

	FILE *fp = fopen(filename, "w");
//	FILE *fp = stdout;
	if(NULL == fp)
		return FALSE;

	WAVFORMAT *fmt = engine->GetFormat();

	fprintf(stderr, "\nEngine diagram:\n\n");
	printTree(stderr, engine, 0, FALSE, FALSE);
	fprintf(stderr, "\n\n");
	fprintf(stderr, "Press enter to continue...");
	getchar();

//	printTimeline(fp, engine, fmt->frequency, TRUE);
	saveXML(fp, engine, TRUE);

	engine->Reset();

	if(fp)
		fclose(fp);

	return FALSE;
}


int length=0;
void printTree(FILE *fp, IFloopySoundInput *input, int level, BOOL bTree, BOOL bLast)
{
	int len=0;

	if(!input)
		return;

	input = input->GetComponent();

	char *name = input->GetName();
	int size = input->GetSize();

	if(bTree)
	{
		char space[100] = {0};
		//if(level > 0)
		//{
			//int indent = level*2;
			//for(int i=0; i<indent-2; i++)

			for(int i=0; i<length-2; i++)
				space[i] = ' ';
			
			space[i] = (char)0xb3;
			len = fprintf(fp, "\n%s", space);
			
			space[i] = (char)(bLast ? 0xc0 : 0xc3);

			space[i+1] = (char)0xc4;
			space[i+2] = (char)0xc4;
		//}

		//len += fprintf(fp, "\n%s< %s(%d)", space, name, size);
		len += fprintf(fp, "\n%s< %s", space, name, size);
	}
	else
		len = fprintf(fp, "%s%s", (level>0?" < ":""), name, size);
		//len = fprintf(fp, "%s%s(%d)", (level>0?" < ":""), name, size);

	//len /= sizeof(char);

	length += len;

	int count = input->GetInputCount();

	if(count > 1)
		length -= strlen(name) / 2 - 1;
	
	for(int i=0; i<count; i++)
	{
		printTree(fp, input->GetSource(i), level+1, (count>1), (i==count-1));
	}

	length -= len;
}


void printTimeline(FILE *fp, IFloopySoundInput *input, int freq, BOOL recursive)
{
	if(NULL == input)
		return;

	input->Reset();

	IFloopySoundInput *comp = input->GetComponent();

	/*if(0 == strcmpi(comp->GetName(), "ischlju.wav"))
	//if(0 == strcmpi(comp->GetName(), "mixer"))
	{
		int debug=1;
	}*/

	char line[80] = {0};
	memset(line, '-', 79);
	fprintf(fp, "%s\n%s\n%s\n", line, comp->GetName(), line);

	int offset=0;
	do {
		if(input->GetParamCount() > 0)
		{
			/*int z = 44100 * 12;// + 4410*1;
			if(z == offset)
			{
				int debug=1;
			}*/
			fprintf(fp, "%d*%d", freq, offset/freq);
			input->MoveTo(offset);
			for(int i=0; i<input->GetParamCount(); i++)
			{
				char *paramName = input->GetParamName(i);
				float paramVal = input->GetParam(i);
				fprintf(fp, "\t%s %s %f\t", 
					(input->IsEnabled() ? "ON" : "OFF"), paramName, paramVal);
			}
			fprintf(fp, "\n");
		}
		else
		{
			fprintf(fp, "%d*%d", freq, offset/freq);
			input->MoveTo(offset);
			fprintf(fp, "\t%s\n", (input->IsEnabled() ? "ON" : "OFF"));
		}
		offset = input->GetNextOffset(offset);
	} while (offset > 0);

	if(recursive)
	{
		if(input->GetInputCount() > 1)
		{
			for(int i=0; i<input->GetInputCount(); i++)
			{
				printTimeline(fp, input->GetSource(i), freq, TRUE);
			}
		}
		else
			printTimeline(fp, input->GetSource(), freq, TRUE);
	}
}




void saveXML(FILE *fp, IFloopySoundInput *input, BOOL recursive)
{
	if(NULL == input)
		return;

	input->Reset();

	IFloopySoundInput *comp = input->GetComponent();

	fprintf(fp, "<%s source='%s'>\n", input->GetName(), comp->GetName());

	int offset=0;
	do {
		if(input->GetParamCount() > 0)
		{
			fprintf(fp, "%d", offset);
			input->MoveTo(offset);
			for(int i=0; i<input->GetParamCount(); i++)
			{
				char *paramName = input->GetParamName(i);
				float paramVal = input->GetParam(i);
				fprintf(fp, ":%d:%s:%f-", i, (input->IsEnabled() ? "ON" : "OFF"));
			}
			fprintf(fp, "\n");
		}
		else
		{
			fprintf(fp, "%d", offset);
			input->MoveTo(offset);
			fprintf(fp, ":%d:%s:%f-", -1, (input->IsEnabled() ? "ON" : "OFF"));
		}
		offset = input->GetNextOffset(offset);
	} while (offset > 0);

	if(recursive)
	{
		if(input->GetInputCount() > 1)
		{
			for(int i=0; i<input->GetInputCount(); i++)
			{
				saveXML(fp, input->GetSource(i), TRUE);
			}
		}
		else
			saveXML(fp, input->GetSource(), TRUE);
	}

	fprintf(fp, "</%s>\n", input->GetName());
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

		wavfile->SetParam(-333, 1);
		cache->SetParam(-333, 2);
		loop->SetParam(-333, 3);
		volume->SetParam(-333, 4);

//		if(1)
//		{

		//int prb1=0, prb2=1, prb3=0, prb4=0;
		//int prb1=0, prb2=1, prb3=0, prb4=0;
		//int prb1=0, prb2=1, prb3=1, prb4=0;
//		int prb1=0, prb2=0, prb3=1, prb4=0;
		//int prb1=1, prb2=0, prb3=1, prb4=0; // Jedino ovako radi kako treba!
		/*if(prb1)
		{
			wavfile->Reset();
			wavfile->Enable(TRUE);
			wavfile->MoveTo(44100 * 3);
			wavfile->Enable(FALSE);
			wavfile->MoveTo(44100 * 23);
			wavfile->Enable(TRUE);
			wavfile->MoveTo(44100 * 24);
			wavfile->Enable(FALSE);
		}
		if(prb2)
		{
			cache->Reset();
			cache->Enable(TRUE);
			cache->MoveTo(44100 * 3);
			cache->Enable(FALSE);
			cache->MoveTo(44100 * 23);
			cache->Enable(TRUE);
			cache->MoveTo(44100 * 24);
			cache->Enable(FALSE);
		}
		if(prb3)
		{*/
			loop->Reset();
			loop->Enable(TRUE);
			loop->MoveTo(44100 * 3);
			loop->Enable(FALSE);
			loop->MoveTo(44100 * 23);
			loop->Enable(TRUE);
			loop->MoveTo(44100 * 24);
			loop->Enable(FALSE);
//		}
		/*if(prb4)
		{
			volume->Reset();
			volume->MoveTo(44100 * 3);
			volume->Enable(FALSE);

			volume->MoveTo(44100 * 23);
			volume->Enable(TRUE);

			volume->MoveTo(44100 * 24);
			volume->Enable(FALSE);
		}*/

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
		
	tonegen->SetParam(-333, 5);

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

