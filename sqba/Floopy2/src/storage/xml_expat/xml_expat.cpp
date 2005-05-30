// xml_expat.cpp : Defines the entry point for the DLL application.
//

#define MAX_PATH          260

#include "expat/xmlparse/xmlparse.h"
#include "../../ifloopy.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
*/



struct tTimeline
{
	IFloopySoundInput *obj;
	char *data;
};




struct tSessionInfo
{
	tTimeline gObjects[1024];	// Temporary solution
	int gIndex;


	int gBuffLen;
	int gBuffEnd;
	char *gpbuff;

	const char *gElement;

	char gPath[MAX_PATH];

	IFloopySoundInput *buff[20]; //temp
	int level;

	XML_Parser parser;
	IFloopySoundEngine *gEngine;
	IFloopySoundInput *gInput;
};

BOOL loadXML(IFloopySoundEngine *engine, char *filename);
BOOL saveXML(IFloopySoundEngine *engine, char *filename);


void saveXML(tSessionInfo *si, FILE *fp, IFloopySoundInput *input, BOOL recursive);


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) BOOL Load(IFloopySoundEngine *engine, char *filename)
{
	return loadXML(engine, filename);
}

__declspec( dllexport ) BOOL Save(IFloopySoundEngine *engine, char *filename)
{
	return saveXML(engine, filename);
}

#ifdef __cplusplus
}
#endif



void loadTimeline(tSessionInfo *si, IFloopySoundInput *input, char *data);
void loadTimelines(tSessionInfo *si);




void startElement(void *userData, const char *name, const char **atts)
{
	tSessionInfo *si = (tSessionInfo*)userData;
	XML_Parser parser = si->parser;
	si->gElement = name;
	if(0 == strcmp(name, "input"))
	{
		int n = XML_GetSpecifiedAttributeCount(parser);
		if(n > 0)
		{
			char *source = NULL;
			char *desc = NULL;
			
			for(int i=0; i<n-1; i+=2)
			{
				if(atts[i])
				{
					if(0==strcmp(atts[i], "source"))
						source = (char*)atts[i+1];
					if(0==strcmp(atts[i], "name"))
						desc = (char*)atts[i+1];
				}
			}

			if(source && (0 != strcmp(source, "engine")))
			{
				char tmp[MAX_PATH] = {0};
				if(strlen(si->gPath))
				{
					memcpy(tmp, si->gPath, strlen(si->gPath));
					strcat(tmp, "\\");
					strcat(tmp, source);
				}
				else
					memcpy(tmp, source, strlen(source));
				IFloopySoundInput *input = si->gEngine->CreateInput(tmp);
				if(input && si->gInput)
				{
					si->gInput->SetSource(input);
					si->gInput = input;
					if(desc)
						input->SetDisplayName(desc, strlen(desc));
					si->gObjects[si->gIndex].obj = input;
					si->buff[++si->level] = si->gInput;
				}
				else
					si->gInput = NULL;
			}
			else
			{
				if(desc)
					si->gEngine->SetDisplayName(desc, strlen(desc));
			}
		}
	}
}

void endElement(void *userData, const char *name)
{
	tSessionInfo *si = (tSessionInfo*)userData;

	if(si->gInput && (0 == strcmp(name, "input")))
	{
		si->gInput = si->buff[--si->level];
		si->gIndex++;
	}
	else if(0 == strcmp(si->gElement, "timeline"))
	{
		if(si->gpbuff)
		{
			si->gObjects[si->gIndex].data = new char[si->gBuffEnd];
			memcpy(si->gObjects[si->gIndex].data, si->gpbuff, si->gBuffEnd);
			si->gIndex++;
			//loadTimeline(gInput, gpbuff);

			delete[] si->gpbuff;
			si->gpbuff = NULL;
			si->gBuffEnd = 0;
		}
	}
}

void elementData(void *userData, const char *data, int len)
{
	tSessionInfo *si = (tSessionInfo*)userData;

	if(0 == strcmp(si->gElement, "timeline"))
	{
		if(!si->gpbuff)
		{
			si->gBuffLen = len*2 + 1;
			si->gpbuff = new char[si->gBuffLen];
			memset(si->gpbuff, 0, si->gBuffLen);
		}

		if(si->gBuffEnd + len < si->gBuffLen)
		{
			strncat(si->gpbuff, data, len);
		}
		else
		{
			char *tmp = si->gpbuff;
			si->gBuffLen = si->gBuffEnd + len + 1;
			si->gpbuff = new char[si->gBuffLen];
			memset(si->gpbuff, 0, si->gBuffLen);
			memcpy(si->gpbuff, tmp, si->gBuffEnd);
			strncat(si->gpbuff, data, len);
			delete[] tmp;
		}
		si->gBuffEnd += len;
	}
}


BOOL loadXML(IFloopySoundEngine *engine, char *filename)
{
	tSessionInfo si;
	memset(&si, 0, sizeof(tSessionInfo));

	//XML_Parser parser = NULL;

	si.gEngine = engine;
	si.gInput = engine;
	si.buff[0] = si.gInput;
	si.gObjects[0].obj = engine;
	si.level = 0;

	char *pathend = strrchr(filename, '\\');
	if(pathend)
	{
		memcpy(si.gPath, filename, pathend - filename);
	}

	FILE *fp = fopen(filename, "r");
	if(NULL == fp)
		return FALSE;
	char buf[BUFSIZ];
	si.parser = XML_ParserCreate(NULL);
	int done;
	//int depth = 0;
	//XML_SetUserData(parser, &depth);
	XML_SetUserData(si.parser, &si);
	XML_SetElementHandler(si.parser, startElement, endElement);
	XML_SetCharacterDataHandler(si.parser, elementData);
	do {
		size_t len = fread(buf, 1, sizeof(buf), fp);
		done = len < sizeof(buf);
		if (!XML_Parse(si.parser, buf, len, done)) {
			fprintf(stderr,
				"%s at line %d\n",
				XML_ErrorString(XML_GetErrorCode(si.parser)),
				XML_GetCurrentLineNumber(si.parser));
			return FALSE;
		}
	} while (!done);
	loadTimelines(&si);
//	engine->Reset();
	XML_ParserFree(si.parser);
	fclose(fp);

//	printTree(stdout, engine, 0, FALSE, FALSE);
//	SOUNDFORMAT *fmt = engine->GetFormat();

	return TRUE;
}






BOOL saveXML(IFloopySoundEngine *engine, char *filename)
{
	tSessionInfo si;
	memset(&si, 0, sizeof(tSessionInfo));

	si.level = 0;

	FILE *fp = fopen(filename, "w");
	if(fp)
	{
		saveXML(&si, fp, engine, TRUE);
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}



void saveXML(tSessionInfo *si, FILE *fp, IFloopySoundInput *input, BOOL recursive)
{
	if(NULL == input)
		return;

	input->Reset();

	char space[100] = {0};
	if(si->level < 100)
		memset(space, ' ', si->level);

	//fprintf(fp, "<%s source='%s'>\n", input->GetName(), comp->GetName());
	fprintf(fp, "%s<input source='%s'>\n", space, input->GetName());

	fprintf(fp, "%s <timeline>", space);

	SOUNDFORMAT *fmt = input->GetFormat();
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
		si->level++;
		if(input->GetInputCount() > 1)
		{
			for(int i=0; i<input->GetInputCount(); i++)
			{
				saveXML(si, fp, input->GetSource(i), TRUE);
			}
		}
		else
			saveXML(si, fp, input->GetSource(), TRUE);
	}

	si->level--;

	//fprintf(fp, "</%s>\n", input->GetName());
	fprintf(fp, "%s</input>\n", space);
}


// Problem: MoveTo nece da radi kada izvor nije kompletno
// inicijalizovan, odnosno kada nije inicijalizovana
// SOUNDFORMAT struktura.
// Zbog toga, prvo se moraju ucitati svi objekti pa tek
// onda njihovi timeline-ovi.
void loadTimelines(tSessionInfo *si)
{
	for(int i=0; i<si->gIndex; i++)
	{
		loadTimeline(si, si->gObjects[i].obj, si->gObjects[i].data);
	}
}


void loadTimeline(tSessionInfo *si, IFloopySoundInput *input, char *data)
{
	if(!input)
		return;
	char seps[]   = ":,";
	char *token = strtok( data, seps );
	int i=0;
	int param=0;
	SOUNDFORMAT *fmt = input->GetFormat();
	int freq = fmt->frequency;
	int offset = 0;

	while( token != NULL )
	{
		switch(i)
		{
		case 0:
			offset = atof(token) * (float)freq;
			i++;
			break;
		case 1:
			if(isalpha(*token))
			{
				if(token[0]=='o' || token[0]=='O')
					si->gEngine->EnableAt(input, offset, (0==strncmp(token, "ON", 2)));
				else if(0==strncmp(token, "RESET", 5))
					si->gEngine->SetParamAt(input, offset, -2, 0.f);
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
				si->gEngine->EnableAt(input, offset, (0==strncmp(token, "ON", 2)));
			else
				si->gEngine->SetParamAt(input, offset, param, (float)atof(token));
			i=0;
			break;
		}
		token = strtok( NULL, seps );
	}
}
