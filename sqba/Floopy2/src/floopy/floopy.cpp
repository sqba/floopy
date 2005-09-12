// floopy.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "engine.h"
#include "args.h"



#define BUFFER_LENGTH	5120 //512


int length=0;
/*
void printTree(FILE *fp, IFloopySoundInput *input, int level, BOOL bTree, BOOL bLast)
{
	int len=0;

	if(!input)
		return;

	char *name = input->GetName();
	
	char *tmp = strrchr(name, '\\');
	if(tmp)
		name = tmp + 1;

	SOUNDFORMAT *fmt = input->GetFormat();
	assert(fmt->frequency > 0);
	float size = (float)input->GetSize() / (float)fmt->frequency;

	if(bTree)
	{
		//char space[300] = {0}; //////////
		char *space = new char[length+4]; //////////
		memset(space, 0, length+4);
		//strnset(space, '\0', length+4);
		for(int i=0; i<length-2; i++)
			space[i] = ' ';
		
		space[i] = (char)0xb3;
		len = fprintf(fp, "\n%s", space);
		
		space[i] = (char)(bLast ? 0xc0 : 0xc3);

		space[i+1] = (char)0xc4;
		space[i+2] = (char)0xc4;
		//space[i+3] = '\0';

		//len += fprintf(fp, "\n%s< %s(%.3f)", space, name, size);
		len += fprintf(fp, "\n%s< %s", space, name);

		delete space;
	}
	else
		//len = fprintf(fp, "%s%s(%.3f)", (level>0?" < ":""), name, size);
		len = fprintf(fp, "%s%s", (level>0?" < ":""), name);

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
*/

void printTree(FILE *fp, IFloopySoundInput *input, int level, BOOL bTree, BOOL bLast)
{
	if(!input)
		return;

	char *name = input->GetDisplayName();
	if(!name || strlen(name) == 0)
		name = input->GetName();
	
	char *tmp = strrchr(name, '\\');
	if(tmp)
		name = tmp + 1;

	SOUNDFORMAT *fmt = input->GetFormat();
	if(fmt->frequency > 0)
	{
		int samples = input->GetSize();
		float size = samples != SIZE_INFINITE ? (float)samples / (float)fmt->frequency : (float)samples;

		char *space = new char[level*2+1];
		memset(space, ' ', level*2);
		//memset(space, 0xc3, level);
		//space[level-1] = 0xc0;
		space[level*2] = 0;

		fprintf(fp, "\n%s%s (%.3f)", space, name, size);
		//fprintf(fp, "\n%s%s", space, name);
		/*float srcsize = (float)input->GetSourceSize();
		if(srcsize > 0)
			srcsize = (float)input->GetSourceSize() / (float)fmt->frequency;
		fprintf(fp, "\n%s%s (%.3f) (%.3f)", space, name, size, srcsize);*/

		delete space;
	}

	if(input->GetType() == TYPE_FLOOPY_SOUND_MIXER)
	{
		IFloopySoundMixer *mixer = (IFloopySoundMixer*)input;

		int count = mixer->GetInputCount();
		
		for(int i=0; i<count; i++)
		{
			printTree(fp, mixer->GetSource(i), level+1, (count>1), (i==count-1));
		}
	}
	else
	{
		if( input->IsFilter() )
			printTree(fp, ((IFloopySoundFilter*)input)->GetSource(), level+1, FALSE, TRUE);
	}
}

void process(IFloopySoundInput *input, IFloopySoundOutput *output)
{
	int samples = input->GetSize();
	fprintf(stderr, "Reading %d samples...\n\n", samples);

	SOUNDFORMAT *fmt = input->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int stb = (fmt->bitsPerSample / 8) * fmt->channels;

	clock_t start = clock();

	int offset = 0;
	int bufflen = fmt->frequency * stb / 10;
	//int buffsize = fmt->frequency/10; // In samples
	//int bufflen = buffsize * stb;
	BYTE *buff = new BYTE[bufflen];
	int len;
	memset(buff, 0, bufflen);

	int max = samples * stb;
	int percent = 0;

	int del = 0;
	fprintf(stderr, "Reading: ");//  0%%");
	while((len=input->Read(buff, bufflen)) != EOF)
	{
		offset += len;
		output->Write(buff, len);
		memset(buff, 0, bufflen);
		//assert(offset < max);
		percent = (int)((float)offset * 100.f / (float)max);
		for(int i=0; i<del; i++)
			fprintf(stderr, "\b");
		del = fprintf(stderr, "%d - %d%%", output->GetWrittenSamples(), percent);
	}

	DWORD speed = clock() - start;

	if(speed < 1000)
		fprintf(stderr, "\nFinished in %d ms\n\n", speed);
	else
		fprintf(stderr, "\nFinished in %.3f sec\n\n", (float)speed / 1000.f);

	fprintf(stderr, "Samples: %d\n", (offset / stb));
	fprintf(stderr, "Seconds: %.3f\n", (float)offset / (float)stb / (float)fmt->frequency);

	delete buff;
}

void main(int argc, char* argv[])
{
	if(argc == 1)
	{
		fprintf(stderr, "Usage: %s -i [input] -s [start] -e [end] -o [output] -v [filename]\n", argv[0]);
		fprintf(stderr, "\t-i - Input file\n");
		fprintf(stderr, "\t-s - Start (in seconds)\n");
		fprintf(stderr, "\t-e - End (in seconds)\n");
		fprintf(stderr, "\t-o - Output (file name or waveout)\n");
		fprintf(stderr, "\t-v - Document file (save to)\n");
		return;
	}

	float start = 0.f;
	float end = 0.f;
	int i = 0;
//	char *outfile = NULL;

	CEngine *engine = new CEngine("engine");
	IFloopySoundOutput *output = NULL;
	IFloopySoundInput *input = engine;

	char *filename = GetArg(argc, argv, "i", "test.test");

	fprintf(stderr, "Opening %s", filename);
	if(!engine->Open(filename))
	{
		fprintf(stderr, "%s: File not found!\n", filename);
		return;
	}

	fprintf(stderr, "\n\n");

	SOUNDFORMAT *fmt = engine->GetFormat();
	if((fmt->frequency == 0) || (fmt->bitsPerSample == 0) || (fmt->channels == 0))
	{
		fprintf(stderr, "Engine not initialized properly!\n");
		goto ERR_EXIT;
	}

	start = GetArg(argc, argv, "s", 0.f);
	end   = GetArg(argc, argv, "e", 0.f);
	if(start > 0.f && end > 0.f)
	{
		input = engine->CreateInput("stdlib.playrgn");
		if(!input)
		{
			fprintf(stderr, "stdlib.playrgn not found!\n");
			return;
		}

		((IFloopySoundFilter*)input)->SetSource( engine );

		if(input->GetParamIndex("startat", &i))
			input->SetParamVal(i, start*fmt->frequency);
		if(input->GetParamIndex("stopat", &i))
			input->SetParamVal(i, end*fmt->frequency);
	}


	//SOUNDFORMAT format;
	//memcpy(&format, fmt, sizeof(SOUNDFORMAT));

	filename = GetArg(argc, argv, "o", "stdlib.waveout");
	output   = engine->CreateOutput(filename, *fmt);
	/*if(!output)
	{
		fprintf(stderr, "Failed to create output: %s!\n", outfile);
		return;
	}*/

	
	//length = fprintf(stdout, "%s < ", output->GetName());
	if(output)
		fprintf(stdout, "%s", output->GetName());
	//printTree(stdout, engine->GetSource(), 1, FALSE, FALSE);
	printTree(stdout, engine, 1, FALSE, FALSE);
	fprintf(stdout, "\n");


	fprintf(stderr, "\nPress enter to start...");
	getchar();

	fprintf(stderr, "Start: %.3f seconds\n", start);
	fprintf(stderr, "End:   %.3f seconds\n", 
		(end > 0.f ? end : (float)engine->GetSize()/(float)fmt->frequency));

	if(output)
	{
		process(input, output);

		engine->Reset();
	}

	filename = GetArg(argc, argv, "v", "");
	if(strlen(filename) > 0)
	{
		fprintf(stderr, "\nSaving to %s\n", filename);
		engine->Save(filename);
	}

	fprintf(stderr, "\n");

	//region->Close();
	if(output)
		output->Close();
	if(engine)
		engine->Close();

ERR_EXIT:
	delete output;
	delete engine;

	fprintf(stderr, "\nPress enter to exit...");
	getchar();
}

/*
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
*/
