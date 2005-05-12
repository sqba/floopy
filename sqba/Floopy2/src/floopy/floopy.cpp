// floopy.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "engine.h"
#include <assert.h>
#include "args.h"



#define BUFFER_LENGTH	5120 //512


int length=0;
void printTree(FILE *fp, IFloopySoundInput *input, int level, BOOL bTree, BOOL bLast)
{
	int len=0;

	if(!input)
		return;

	char *name = input->GetName();

	if(bTree)
	{
		char space[100] = {0};
		for(int i=0; i<length-2; i++)
			space[i] = ' ';
		
		space[i] = (char)0xb3;
		len = fprintf(fp, "\n%s", space);
		
		space[i] = (char)(bLast ? 0xc0 : 0xc3);

		space[i+1] = (char)0xc4;
		space[i+2] = (char)0xc4;

		len += fprintf(fp, "\n%s< %s", space, name);
	}
	else
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

void process(IFloopySoundInput *input, IFloopySoundOutput *output)
{
	int samples = input->GetSize();
	fprintf(stderr, "Reading %d samples...\n\n", samples);

	WAVFORMAT *fmt = input->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int x = (fmt->bitsPerSample / 8) * fmt->channels;

	clock_t start = clock();

	int offset = 0;
	BYTE buff[BUFFER_LENGTH];
	int len, size=sizeof(buff);
	memset(buff, 0, sizeof(buff));

	int max = samples * x;
	int percent = 0;

	fprintf(stderr, "Reading:   0%");
	while((len=input->Read(buff, size)) > 0)
	{
		offset += len;
		output->Write(buff, len);
		memset(buff, 0, sizeof(buff));
		percent = offset * 100 / max;
		fprintf(stderr, "\b\b\b%2d%%", percent);
	}

	DWORD speed = clock() - start;

	if(speed < 1000)
		fprintf(stderr, "\nFinished in %d ms\n\n", speed);
	else
		fprintf(stderr, "\nFinished in %.3f sec\n\n", (float)speed / 1000.f);

	fprintf(stderr, "Samples: %d\n", (offset / x));
	fprintf(stderr, "Seconds: %.3f\n", (float)offset / (float)x / (float)fmt->frequency);
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

	CEngine *engine = new CEngine("engine");
	IFloopySoundInput *region = engine->CreateInput("playrgn");
	IFloopySoundOutput *output = NULL;

	region->SetSource( engine );

	char *filename = GetArg(argc, argv, "i", "test.test");

	fprintf(stderr, "Opening %s", filename);

	if(!engine->Open(filename))
	{
		fprintf(stderr, ": File not found!\n", filename);
		return;
	}

	fprintf(stderr, "\n\n");

	WAVFORMAT *fmt = engine->GetFormat();
	assert((fmt->frequency > 0) && (fmt->bitsPerSample > 0) && (fmt->channels > 0));

	float start = GetArg(argc, argv, "s", 0.f);
	float end = GetArg(argc, argv, "e", 0.f);
	int i = region->GetParamIndex("startat");
	region->SetParam(i, start*fmt->frequency);
	i = region->GetParamIndex("stopat");
	region->SetParam(i, end*fmt->frequency);


	WAVFORMAT format;
	memcpy(&format, fmt, sizeof(WAVFORMAT));

	char *outfile = GetArg(argc, argv, "o", "floopy.wav");
	output = engine->CreateOutput(outfile, format);
	if(!output)
	{
		fprintf(stderr, "Failed to create output: %s!\n", outfile);
		return;
	}

	
	length = fprintf(stderr, "%s < ", output->GetName());
	printTree(stdout, engine->GetSource(), 0, FALSE, FALSE);
	fprintf(stderr, "\n");


	fprintf(stderr, "\nPress enter to start...");
	getchar();

	fprintf(stderr, "Start: %.3f seconds\n", start);
	fprintf(stderr, "End:   %.3f seconds\n", 
		(end > 0.f ? end : (float)engine->GetSize()/(float)fmt->frequency));

	process(region, output);

	engine->Reset();

	filename = GetArg(argc, argv, "v", "");
	if(strlen(filename) > 0)
	{
		fprintf(stderr, "\nSaving to %s\n", filename);
		engine->Save(filename);
	}

	fprintf(stderr, "\n");

	region->Close();
	output->Close();

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
