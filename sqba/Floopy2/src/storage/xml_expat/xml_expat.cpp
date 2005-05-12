// xml_expat.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include "expat/xmlparse/xmlparse.h"
#include "../../ifloopy.h"
#include <stdio.h>



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


void saveXML(FILE *fp, IFloopySoundInput *input, BOOL recursive);


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


struct tTimeline
{
	IFloopySoundInput *obj;
	char *data;
};
tTimeline gObjects[1024];	// Temporary solution
int gIndex = 0;


int gBuffLen = 0;
int gBuffEnd = 0;
char *gpbuff = NULL;

const char *gElement = NULL;

IFloopySoundInput *buff[20]; //temp
int level = 0;

XML_Parser parser = NULL;

IFloopyEngine *gEngine = NULL;
IFloopySoundInput *gInput = NULL;

void loadTimeline(IFloopySoundInput *input, char *data);
void loadTimelines();




void startElement(void *userData, const char *name, const char **atts)
{
	gElement = name;
	if(0 == strcmp(name, "input"))
	{
		int n = XML_GetSpecifiedAttributeCount(parser);
		if(n > 0)
		{
			char *source = (char*)atts[1];
			if(source && (0 != strcmp(source, "engine")))
			{
				IFloopySoundInput *input = gEngine->CreateInput(source);
				if(input)
				{
					gInput->SetSource(input);
					gInput = input;
					gObjects[gIndex].obj = input;
					buff[++level] = gInput;
				}
			}
		}
	}
}

void endElement(void *userData, const char *name)
{
	if(gInput && (0 == strcmp(name, "input")))
	{
		gInput = buff[--level];
		gIndex++;
	}
	else if(0 == strcmp(gElement, "timeline"))
	{
		if(gpbuff)
		{
			gObjects[gIndex].data = new char[gBuffEnd];
			memcpy(gObjects[gIndex].data, gpbuff, gBuffEnd);
			gIndex++;
			//loadTimeline(gInput, gpbuff);

			delete[] gpbuff;
			gpbuff = NULL;
			gBuffEnd = 0;
		}
	}
}

void elementData(void *userData, const char *data, int len)
{
	if(0 == strcmp(gElement, "timeline"))
	{
		if(!gpbuff)
		{
			gBuffLen = len*2 + 1;
			gpbuff = new char[gBuffLen];
			memset(gpbuff, 0, gBuffLen);
		}

		if(gBuffEnd + len < gBuffLen)
		{
			strncat(gpbuff, data, len);
		}
		else
		{
			char *tmp = gpbuff;
			gBuffLen = gBuffEnd + len + 1;
			gpbuff = new char[gBuffLen];
			memset(gpbuff, 0, gBuffLen);
			memcpy(gpbuff, tmp, gBuffEnd);
			strncat(gpbuff, data, len);
			delete[] tmp;
		}
		gBuffEnd += len;
	}
}


BOOL loadXML(IFloopyEngine *engine, char *filename)
{
	gEngine = engine;
	gInput = engine;
	buff[0] = gInput;
	gObjects[0].obj = engine;
	level = 0;

	FILE *fp = fopen(filename, "r");
	if(NULL == fp)
		return FALSE;
	char buf[BUFSIZ];
	parser = XML_ParserCreate(NULL);
	int done;
	int depth = 0;
	XML_SetUserData(parser, &depth);
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, elementData);
	do {
		size_t len = fread(buf, 1, sizeof(buf), fp);
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, len, done)) {
			fprintf(stderr,
				"%s at line %d\n",
				XML_ErrorString(XML_GetErrorCode(parser)),
				XML_GetCurrentLineNumber(parser));
			return FALSE;
		}
	} while (!done);
	loadTimelines();
	engine->Reset();
	XML_ParserFree(parser);
	fclose(fp);

//	printTree(stdout, engine, 0, FALSE, FALSE);
//	WAVFORMAT *fmt = engine->GetFormat();

	return TRUE;
}






BOOL saveXML(IFloopyEngine *engine, char *filename)
{
	level = 0;
	FILE *fp = fopen(filename, "w");
	if(fp)
	{
		saveXML(fp, engine, TRUE);
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}



void saveXML(FILE *fp, IFloopySoundInput *input, BOOL recursive)
{
	if(NULL == input)
		return;

	input->Reset();

	char space[100] = {0};
	if(level < 100)
		memset(space, ' ', level);

	//fprintf(fp, "<%s source='%s'>\n", input->GetName(), comp->GetName());
	fprintf(fp, "%s<input source='%s'>\n", space, input->GetName());

	fprintf(fp, "%s<timeline>", space);

	WAVFORMAT *fmt = input->GetFormat();
	int freq = fmt->frequency;

	int offset=0;
	do {
		float seconds = (float)offset / (float)freq;
		input->MoveTo(offset);
		char *enabled = (input->IsEnabled() ? "ON" : "OFF");
		if(offset > 0)
			fprintf(fp, ", "); // Separator
		fprintf(fp, "%.3f:%s", seconds, enabled);
		if(input->GetParamCount() > 0)
		{
			for(int i=0; i<input->GetParamCount(); i++)
			{
				//char *paramName = input->GetParamName(i);
				//float paramVal = input->GetParam(i);
				//fprintf(fp, ", %.3f:%d:%.3f", seconds, i, paramVal);
				float paramVal = 0.f;
				if(input->GetParam(i, &paramVal))
				{
					char *paramName = input->GetParamName(i);
					fprintf(fp, ", %.3f:%d:%.3f", seconds, i, paramVal);
				}
			}
		}
		offset = input->GetNextOffset(offset);
	} while (offset > 0);

	fprintf(fp, "</timeline>\n");

	if(recursive)
	{
		level++;
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

	level--;

	//fprintf(fp, "</%s>\n", input->GetName());
	fprintf(fp, "%s</input>\n", space);
}


// Problem: MoveTo nece da radi kada izvor nije kompletno
// inicijalizovan, odnosno kada nije inicijalizovana
// WAVFORMAT struktura.
// Zbog toga, prvo se moraju ucitati svi objekti pa tek
// onda njihovi timeline-ovi.
void loadTimelines()
{
	for(int i=0; i<gIndex; i++)
	{
		loadTimeline(gObjects[i].obj, gObjects[i].data);
	}
}


void loadTimeline(IFloopySoundInput *input, char *data)
{
	if(!input)
		return;
	char seps[]   = ":,";
	char *token = strtok( data, seps );
	int i=0;
	int param=0;
	input->Reset();
	WAVFORMAT *fmt = input->GetFormat();
	int freq = fmt->frequency;
	int offset = 0;

	//BOOL bTest = TRUE;

	while( token != NULL )
	{
		switch(i)
		{
		case 0:
			//if(bTest)
				offset = atof(token) * (float)freq;
			//else
			//	input->MoveTo(atof(token) * (float)freq);
			i++;
			break;
		case 1:
			if(isalpha(*token))
			{
				//if(bTest)
					gEngine->EnableAt(input, offset, (0==strncmp(token, "ON", 2)));
				//else
				//	input->Enable(0==strncmp(token, "ON", 2));
				i=0;
			}
			else
			{
				param = atoi(token);
				i++;
			}
			break;
		case 2:
			if(param == -1)
				//if(bTest)
					gEngine->EnableAt(input, offset, (0==strncmp(token, "ON", 2)));
				//else
				//	input->Enable(0==strncmp(token, "ON", 2));
			else
				//if(bTest)
					gEngine->SetParamAt(input, offset, param, (float)atof(token));
				//else
				//	input->SetParam(param, (float)atof(token));
			i=0;
			break;
		}
		token = strtok( NULL, seps );
	}
	input->Reset();
}
