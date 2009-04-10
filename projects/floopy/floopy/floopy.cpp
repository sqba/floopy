// floopy.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "../util/engine_wrapper.h"
#include "args.h"


#define BUFFER_LENGTH	5120 //512


bool is_filter(IFloopySoundInput *input)
{
	int type = input->GetType();
	return (type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

void process(IFloopySoundInput *input, IFloopySoundOutput *output, int buffSize)
{
	int samples = input->GetSize();
	if(0 == samples)
		return;
	fprintf(stderr, "Reading %d samples...\n\n", samples);

	SOUNDFORMAT *fmt = input->GetFormat();
	assert((fmt->bitsPerSample > 0) && (fmt->channels > 0));
	int stb = (fmt->bitsPerSample / 8) * fmt->channels;

	clock_t start = clock();

	int offset = 0;
	int bufflen = buffSize * stb;
	//int bufflen = fmt->frequency * stb / 10;
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
		del = fprintf(stderr, "%d - %d%%", output->GetPosition(), percent);
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

int main(int argc, char* argv[])
{
	if(argc == 1)
	{
		fprintf(stderr, "Usage: %s -i [input] -s [start] -e [end] -o [output] -v [filename] -b [128]\n", argv[0]);
		fprintf(stderr, "\t-i - Input file\n");
		fprintf(stderr, "\t-s - Start (in seconds)\n");
		fprintf(stderr, "\t-e - End (in seconds)\n");
		fprintf(stderr, "\t-o - Output (file name or waveout)\n");
		fprintf(stderr, "\t-v - Document file (save to)\n");
		fprintf(stderr, "\t-b - Buffer size (in samples)\n");
		return 1;
	}

	float start = 0.f;
	float end = 0.f;
	int i = 0;
//	char *outfile = NULL;

	CEngineWrapper *engine = new CEngineWrapper("engine");
	IFloopySoundOutput *output = NULL;
	IFloopySoundInput *input = engine;

	const char *filename = GetArg(argc, argv, "i", "test.test");

	int buffSize = GetArg(argc, argv, "b", 128);

	fprintf(stderr, "Opening %s\n", filename);
	if(!engine->Open(filename))
	{
		fprintf(stderr, "%s: File not found!\n", filename);
		return 1;
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
		input = engine->CreateInput("std.playrgn");
		if(!input)
		{
			fprintf(stderr, "libstd.playrgn not found!\n");
			return 1;
		}

		((IFloopySoundFilter*)input)->SetSource( engine );

		if(input->GetParamIndex("startat", &i))
			input->SetParamVal(i, start*fmt->frequency);
		if(input->GetParamIndex("stopat", &i))
			input->SetParamVal(i, end*fmt->frequency);
	}

	filename = GetArg(argc, argv, "o", PCM_OUT);
	output   = engine->CreateOutput(filename, *fmt);

	if(output)
		fprintf(stdout, "%s", output->GetName());
    else
		fprintf(stderr, "%s", engine->GetLastErrorDesc());

//	fprintf(stderr, "\nPress enter to start...");
//	getchar();

	fprintf(stderr, "Start: %.3f seconds\n", start);
	fprintf(stderr, "End:   %.3f seconds\n",
		(end > 0.f ? end : (float)engine->GetSize()/(float)fmt->frequency));

	if(output)
	{
		process(input, output, buffSize);
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

//	fprintf(stderr, "\nPress enter to exit...");
//	getchar();

	return 0;
}
