

/*
#include "../../ifloopy.h"
#include "storage.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) char *GetExtension()
{
	return "xml";
	//return "mix";
}

__declspec( dllexport ) bool Load(IFloopySoundEngine *engine, char *filename)
{
	CStorage storage(engine);
	return storage.Load(filename);
}

__declspec( dllexport ) bool Save(IFloopySoundEngine *engine, char *filename)
{
	CStorage storage(engine);
	return storage.Save(filename);
}

#ifdef __cplusplus
}
#endif
*/




#define MAX_PATH          260

#include "expat/xmlparse/xmlparse.h"
#include "../../ifloopy.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




struct tSessionInfo
{
	struct tTimeline
	{
		IFloopySoundInput *obj;
		char *data;
	} gObjects[1024];	// Temporary solution

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

	bool bInitialized;
};

bool loadXML(IFloopySoundEngine *engine, char *filename);
bool saveXML(IFloopySoundEngine *engine, char *filename);


void saveXML(tSessionInfo *si, FILE *fp, IFloopySoundInput *input, bool recursive);


#ifdef __cplusplus
extern "C" {
#endif
__declspec( dllexport ) bool Load(IFloopySoundEngine *engine, char *filename)
{
	return loadXML(engine, filename);
}

__declspec( dllexport ) bool Save(IFloopySoundEngine *engine, char *filename)
{
	return saveXML(engine, filename);
}

__declspec( dllexport ) char *GetExtension()
{
	return "XML";
}
#ifdef __cplusplus
}
#endif



void loadTimeline(tSessionInfo *si, IFloopySoundInput *input, char *data);
void loadTimelines(tSessionInfo *si);



bool IsFilter(IFloopySoundInput *input)
{
	int type = input->GetType();
	return (type == (TYPE_FLOOPY_SOUND_FILTER | type));
}



void loadColor(char *str, UINT *r, UINT *g, UINT *b)
{
	char seps[]   = ",";
	char *token = strtok( str, seps );
	int i=0;
	while( token != NULL )
	{
		switch(i)
		{
		case 0:
			*r = atoi(token);
			break;
		case 1:
			*g = atoi(token);
			break;
		case 2:
			*b = atoi(token);
			break;
		}
		token = strtok( NULL, seps );
		i++;
	}
}


void startElement(void *userData, const char *name, const char **atts)
{
	tSessionInfo *si = (tSessionInfo*)userData;
	XML_Parser parser = si->parser;
	si->gElement = name;
	UINT r=256, g=256, b=256;
	if(0 == strcmp(name, "input"))
	{
		int n = XML_GetSpecifiedAttributeCount(parser);
		if(n > 0)
		{
			char *source = NULL;
			char *desc = NULL;
			char *filename = NULL;
			
			for(int i=0; i<n-1; i+=2)
			{
				if(atts[i])
				{
					if(0==strcmp(atts[i], "source"))
						source = (char*)atts[i+1];
					if(0==strcmp(atts[i], "open"))
						filename = (char*)atts[i+1];
					if(0==strcmp(atts[i], "name"))
						desc = (char*)atts[i+1];
					if(0==strcmp(atts[i], "color"))
					{
						char *tmp = (char*)atts[i+1];
						loadColor(tmp, &r, &g, &b);
					}
				}
			}

			if(source && (0 != strcmp(source, "engine")))
			{
				char tmp[MAX_PATH] = {0};
				if(strlen(si->gPath))
				{
					if(strchr(source, ':'))	// Absolute path!
						memcpy(tmp, source, strlen(source));
					else
					{
						memcpy(tmp, si->gPath, strlen(si->gPath));
						strcat(tmp, "\\");
						strcat(tmp, source);
					}
				}
				else
					memcpy(tmp, source, strlen(source));
				
				IFloopySoundInput *input = si->gEngine->CreateInput(tmp);
				if(input && si->gInput)
				{
					if( IsFilter(si->gInput) )
					{
						((IFloopySoundFilter*)si->gInput)->SetSource(input);
						si->gInput = input;
						if(desc)
							input->SetDisplayName(desc, strlen(desc));
						if(r<256 && g<256 && b<256)
							input->SetColor(r, g, b);
						r=g=b=256;
						si->gObjects[si->gIndex].obj = input;
						si->buff[++si->level] = si->gInput;
					}
				}
				else
					si->gInput = NULL;
				
				if(input && filename)
				{
					char tmp[MAX_PATH] = {0};
					if(strchr(filename, ':'))	// Absolute path!
						memcpy(tmp, filename, strlen(filename));
					else
					{
						memcpy(tmp, si->gPath, strlen(si->gPath));
						strcat(tmp, "\\");
						strcat(tmp, filename);
					}
					input->Open( tmp );
				}
			}
			else
			{
				if(desc)
					si->gEngine->SetDisplayName(desc, strlen(desc));
				if(r<256 && g<256 && b<256)
					si->gEngine->SetColor(r, g, b);
				r=g=b=256;
			}
		}
	}
	else if(0 == strcmp(name, "properties"))
	{
		int n = XML_GetSpecifiedAttributeCount(parser);
		if(n > 0)
		{
			char *source = NULL;
			char *desc = NULL;

			IFloopySoundInput *input = si->gInput;
			
			if(NULL != input)
			{
				for(int i=0; i<n-1; i+=2)
				{
					if(atts[i])
					{
						int index = 0;
						char *name = (char*)atts[i];
						if( input->GetPropertyIndex(name, &index) )
						{
							float value = (float)atof(atts[i+1]);
							input->SetPropertyVal(index, value);
						}
					}
				}
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


bool loadXML(IFloopySoundEngine *engine, char *filename)
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
		return false;
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
			return false;
		}
	} while (!done);
	loadTimelines(&si);
//	engine->Reset();
	XML_ParserFree(si.parser);
	fclose(fp);

//	printTree(stdout, engine, 0, false, false);
//	SOUNDFORMAT *fmt = engine->GetFormat();

	return true;
}






bool saveXML(IFloopySoundEngine *engine, char *filename)
{
	tSessionInfo si;
	memset(&si, 0, sizeof(tSessionInfo));

	si.level = 0;

	si.bInitialized = false;

	FILE *fp = fopen(filename, "w");
	if(fp)
	{
		saveXML(&si, fp, engine, true);
		fclose(fp);
		return true;
	}
	return false;
}







void writeProperties(FILE *fp, IFloopySoundInput *input)
{
	if(input->GetPropertyCount() > 0)
	{
		bool bStart = true;

		fprintf(fp, "<properties ");

		for(int i=0; i<input->GetPropertyCount(); i++)
		{
			float propVal = 0.f;
			if(input->GetPropertyVal(i, &propVal))
			{
				char *propName = input->GetPropertyName(i);

				if(NULL == strchr(propName, ' '))
				{
					if(!bStart)
						fprintf(fp, " "); // Separator
					else
						bStart = false;
					
					fprintf(fp, "%s='%.4f'", propName, propVal);
				}
			}
		}

		fprintf(fp, "/>\n");
	}
}


bool writeParameters(FILE *fp, IFloopySoundInput *input, int offset)
{
	bool bStart = true;

	SOUNDFORMAT *fmt = input->GetFormat();
	float seconds = (float)offset / (float)fmt->frequency;

	float paramVal = 0.f;
	if(input->GetParamAt(offset, TIMELINE_PARAM_ENABLE, &paramVal))
	{
		char *enabled = (paramVal == PARAM_VALUE_ENABLED ? "ON" : "OFF");
		if(!bStart)
			fprintf(fp, ", "); // Separator
		else
			bStart = false;
		
		fprintf(fp, "%.4f:%s", seconds, enabled);
	}

	if(input->GetParamAt(offset, TIMELINE_PARAM_MOVETO, &paramVal))
	{
		if(!bStart)
			fprintf(fp, ", "); // Separator
		else
			bStart = false;
		
		if(paramVal==0.f)
			fprintf(fp, "%.4f:RESET", seconds);
		else
			fprintf(fp, "%.4f:MOVETO:%d", seconds, (int)paramVal);
	}

	if(input->GetParamCount() > 0)
	{
		for(int i=0; i<input->GetParamCount(); i++)
		{
			float paramVal = 0.f;
			if(input->GetParamAt(offset, i, &paramVal))
			{
				//char *paramName = input->GetParamName(i);
				if(!bStart)
					fprintf(fp, ", "); // Separator
				else
					bStart = false;
				
				fprintf(fp, "%.4f:%d:%.4f", seconds, i, paramVal);
			}
		}
	}

	return bStart;
}


void writeTimeline(FILE *fp, IFloopySoundInput *input)
{
	int offset=0;

	bool bStart = true;

	fprintf(fp, "<timeline>");
	do
	{
		if(!bStart)
			fprintf(fp, ", "); // Separator

		bStart = writeParameters(fp, input, offset);

		offset = input->GetNextOffset(offset);
	} while (offset > 0);
	fprintf(fp, "</timeline>\n");
}


void saveXML(tSessionInfo *si, FILE *fp, IFloopySoundInput *input, bool recursive)
{
	if(NULL == input)
		return;

	input->Reset();

	char space[100] = {0};
	if(si->level < 100)
		memset(space, ' ', si->level);

	bool bEngine = (input->GetType() == TYPE_FLOOPY_SOUND_ENGINE);

	//fprintf(fp, "<%s source='%s'>\n", input->GetName(), comp->GetName());
	if(bEngine && si->bInitialized)
	{
		char *path = input->GetPath();
		//if(0 == strnicmp(path, si->filename, strlen(si->gPath)))
		//	path += strlen(si->gPath);
		fprintf(fp, "%s<input source='%s' name='%s'", 
			space, path, input->GetDisplayName());
		recursive = false;
	}
	else
		fprintf(fp, "%s<input source='%s' name='%s' open='%s'", 
		space, input->GetName(), input->GetDisplayName(), input->GetPath());

	UINT r=0, g=0, b=0;
	if( input->GetColor(&r, &g, &b) )
		fprintf(fp, " color='%d,%d,%d'", r, g, b);

	fprintf(fp, ">\n");

	si->bInitialized = true;

	fprintf(fp, "%s ", space);
	writeProperties(fp, input);

	fprintf(fp, "%s ", space);
	writeTimeline(fp, input);

	if(recursive)
	{
		si->level++;
		//if(input->GetInputCount() > 1)
		if(input->GetType() == TYPE_FLOOPY_SOUND_MIXER)
		{
			IFloopySoundMixer *mixer = (IFloopySoundMixer*)input;
			for(int i=0; i<mixer->GetInputCount(); i++)
			{
				saveXML(si, fp, mixer->GetSource(i), true);
			}
		}
		else if( IsFilter(input) )
			saveXML(si, fp, ((IFloopySoundFilter*)input)->GetSource(), true);
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
				{
					input->EnableAt(offset, (0==strncmp(token, "ON", 2)));
					i=0;
				}
				else if(0==strncmp(token, "RESET", 5))
				{
					input->SetParamAt(offset, -2, 0.f);
					i=0;
				}
				else if(0==strncmp(token, "MOVETO", 6))
				{
					param = TIMELINE_PARAM_MOVETO;
					i++;
				}
			}
			else
			{
				param = atoi(token);
				i++;
			}
			break;
		case 2:
			if(param == -1)
				input->EnableAt(offset, (0==strncmp(token, "ON", 2)));
			else
				input->SetParamAt(offset, param, (float)atof(token));
			i=0;
			break;
		}
		token = strtok( NULL, seps );
	}
}


/////////////////////////
// Replace these:
//	& &amp;
//	< &lt;
//	> &gt;
//	" &quot;
//	' &apos;
/////////////////////////

