// floopy.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "engine.h"
#include <assert.h>



#define BUFFER_LENGTH	5120 //512


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
		
		space[i] = (char)0xb3;//0xc3;//0xc4;
		printf("%s\n", space);
		
		space[i] = (char)0xc0;//0xc3;//0xc4;
		
		space[i+1] = (char)0xc4;
		space[i+2] = (char)0xc4;
	}

	printf("%s%s\n", space, input->GetName());

	for(int i=0; i<input->GetInputCount(); i++)
	{
		printPath(input->GetSource(i), level+1);
	}
}

void process(IFloopySoundInput *input, IFloopySoundOutput *output)
{
	printf("Reading %d samples...\n\n", input->GetSize());

	clock_t start = clock();

	int offset = 0;
	BYTE buff[BUFFER_LENGTH];
	int len, size=sizeof(buff);

	memset(buff, 0, sizeof(buff));

	while((len=input->Read(buff, size)) > 0)
	{
		offset += len;
		output->Write(buff, len);
		memset(buff, 0, sizeof(buff));
	}
	DWORD speed = clock() - start;

	output->Close();

	WAVFORMAT *fmt = input->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));

	printf("Samples read:\t%d\n", (offset / ((fmt->bitsPerSample / 8) * fmt->channels)));
	printf("Output length:\t%.3f seconds\n", ((float)offset / (((float)fmt->bitsPerSample / 8.f) * (float)fmt->channels)) / (float)fmt->frequency);

	if(speed < 1000)
		printf("Total time:\t%d ms\n", speed);
	else
		printf("Total time:\t%.3f sec\n", (float)speed / 1000.f);
}

void main(int argc, char* argv[])
{
	CEngine *engine = new CEngine("engine");
	//engine->Open(TEXT("test.xml"));

	if(!engine->Open(TEXT("test.xml")))
		return;

	IFloopySoundInput *region = engine->CreateInput("playrgn");

	//COutput *output = NULL;
	IFloopySoundOutput *output = NULL;

	region->SetSource( engine );

	WAVFORMAT *fmt = engine->GetFormat();
	assert((fmt->frequency > 0) && (fmt->bitsPerSample > 0) && (fmt->channels > 0));

	if(argc >= 2)
	{
		float start = (float)atof(argv[1]);
		region->Reset();
		region->SetParam(0, start*fmt->frequency);
	}
	if(argc >= 3)
	{
		float end = (float)atof(argv[2]);
		region->Reset();
		region->SetParam(1, end*fmt->frequency);
	}

	WAVFORMAT format;
	memcpy(&format, fmt, sizeof(WAVFORMAT));

	int i = 0;

	switch(i)
	{
	case 0: // Render to A wav file
		output = engine->CreateOutput("wavfile", format);
		output->Open("floopy.wav");
		break;
	case 1:	// Output to speakers
		output = engine->CreateOutput("waveout", format);
		break;
	case 2:	// Output to svg
		output = engine->CreateOutput("svgfile", format);
		output->Open("floopy.svg");
		break;
	}

	// stdout?

	printf("%s\n", output->GetName());
	printPath(region, 1);
	printf("Press enter to continue...\n");
	getchar();

	process(region, output);

	printf("\nPress enter to exit...");
	getchar();

	delete output;
	delete engine;
}
