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
		
		space[i] = 0xb3;//0xc3;//0xc4;
		printf("%s\n", space);
		
		space[i] = 0xc0;//0xc3;//0xc4;
		
		space[i+1] = 0xc4;
		space[i+2] = 0xc4;
	}

	printf("%s%s\n", space, input->GetName());

//	if(level!=1 && input->GetInputCount() > 0)
//	{
//	try {
		for(int i=0; i<input->GetInputCount(); i++)
		{
			printPath(input->GetSource(i), level+1);
		}
//	}
//	catch(...)
//	{
//		printf("This happens only in debug mode!!!\n");
//	}
	// Zasto se ovo dogadja prilikom debagovanja i
	// zasto se sve stampa po dva puta???
/*	}
	else
	{
		printPath(input->GetSource(), level+1);
	}*/
}
/*
void printPath(IFloopySoundInput *input, IFloopySoundOutput *output)
{
	printf("%s\n", output->GetName());
	printPath(input, 1);
	getchar();
}
*/
void process(IFloopySoundInput *input, IFloopySoundOutput *output)
{
	clock_t start = clock();

	UINT offset = 0;
	BYTE buff[BUFFER_LENGTH];
	UINT len, size=sizeof(buff);
	memset(buff, 0, sizeof(buff));
//	while(((len=input->Read(buff, size, 0)) > 0) && (offset<1000000))
	while((len=input->Read(buff, size)) > 0)
	{
		offset += len;
		output->Write(buff, len);
		memset(buff, 0, sizeof(buff));
	}
	output->Close();

	DWORD speed = clock() - start;
	printf("Processing time: %d ms\n", speed);
	WAVFORMAT *fmt = input->GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));
	//int x = ((fmt->size / 8) * fmt->channels);
	printf("Read:   %d samples\n", (offset / ((fmt->size / 8) * fmt->channels)));
	printf("Length: %f seconds\n", ((float)offset / (((float)fmt->size / 8.f) * (float)fmt->channels)) / (float)fmt->freq);
}

void main(int argc, char* argv[])
{
	CEngine *engine = new CEngine("engine");
	//engine->Open(TEXT("test.xml"));

	if(!engine->Open(TEXT("test.xml")))
		return;

	//COutput *output = NULL;
	IFloopySoundOutput *output = NULL;

	if(argc >= 2)
	{
		// Convert seconds to samples
		WAVFORMAT *fmt = engine->GetFormat();
		assert((fmt->size > 0) && (fmt->channels > 0));
		int x = fmt->freq;// * (fmt->size / 8) * fmt->channels;
		int i = atoi(argv[1]);
		engine->MoveTo(i*x);
	}
	if(argc >= 3)
	{
		// Convert seconds to samples
		WAVFORMAT *fmt = engine->GetFormat();
		assert((fmt->size > 0) && (fmt->channels > 0));
		int x = fmt->freq;// * (fmt->size / 8) * fmt->channels;
		int i = atoi(argv[2]);
		engine->SetSize(i*x);
	}

	WAVFORMAT *fmt = engine->GetFormat();
	assert((fmt->size > 0) && (fmt->channels > 0));

	int i = 0;

	switch(i)
	{
	case 0:
		// Render to file
		//output = new COutput(TEXT("wavfile"), engine);
		output = engine->CreateOutput("wavfile", fmt);
		output->Open("floopy.wav");
		break;
	case 1:
		// Output to speakers
		//output = new COutput(TEXT("waveout"), engine->getPlugin());
		output = engine->CreateOutput("waveout", fmt);
		break;
	case 2:
		// Output to svg
		output = engine->CreateOutput("svgfile", fmt);
		output->Open("floopy.svg");
		break;
	}

	// stdout?

//	printPath(engine, output);
	printf("%s\n", output->GetName());
	printPath(engine, 1);
	printf("Press enter to continue");
	getchar();
//	WAVFORMAT *fmt = engine->getPlugin()->GetFormat();
	
	printf("%d samples\n", engine->GetSize());

	process(engine, output);

	delete output;
	delete engine;

	printf("Press enter to exit");
	getchar();
}
