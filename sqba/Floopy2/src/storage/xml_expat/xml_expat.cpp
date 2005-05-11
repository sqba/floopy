// xml_expat.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include "expat/xmlparse/xmlparse.h"
#include "../../ifloopy.h"
#include <stdio.h>



/*
int length=0;
void printTree(FILE *fp, IFloopySoundInput *input, int level, BOOL bTree, BOOL bLast)
{
	int len=0;

	if(!input)
		return;

	input = input->GetComponent();

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
*/
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
tTimeline gObjects[100];
int gIndex = 0;


#define BUFF_LEN	1024
int gpos = 0;
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
					char *c = gInput->GetComponent()->GetName();
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
			gObjects[gIndex].data = new char[gpos];
			memcpy(gObjects[gIndex].data, gpbuff, gpos);
			gIndex++;
			//loadTimeline(gInput, gpbuff);

			delete[] gpbuff;
			gpbuff = NULL;
			gpos = 0;
		}
	}
}

void elementData(void *userData, const char *data, int len)
{
	if(0 == strcmp(gElement, "timeline"))
	{
		if(!gpbuff)
		{
			gpbuff = new char[BUFF_LEN];
			memset(gpbuff, 0, BUFF_LEN);
		}

		if(gpos + len < BUFF_LEN)
		{
			strncat(gpbuff, data, len);
		}
		else
		{
			int l = sizeof(gpbuff);
			char *tmp = new char[l];
			memcpy(tmp, gpbuff, l);

			delete[] gpbuff;
			gpbuff = new char[l + len];
			memcpy(gpbuff, tmp, l);

			strncat(gpbuff, data, len);

			delete tmp;
		}
		gpos += len;
	}
}


BOOL loadXML(IFloopyEngine *engine, char *filename)
{
	gEngine = engine;
	gInput = engine;
	buff[0] = gInput;
	gObjects[0].obj = engine;

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

	IFloopySoundInput *comp = input->GetComponent();

	//fprintf(fp, "<%s source='%s'>\n", input->GetName(), comp->GetName());
	fprintf(fp, "<input source='%s'>\n", comp->GetName());

	fprintf(fp, "<timeline>\n");

	int offset=0;
	do {
		input->MoveTo(offset);
		char *enabled = (input->IsEnabled() ? "ON" : "OFF");
		if(offset > 0)
			fprintf(fp, ", "); // Separator
		fprintf(fp, "%d:-1:%s", offset, enabled);
		if(input->GetParamCount() > 0)
		{
			for(int i=0; i<input->GetParamCount(); i++)
			{
				char *paramName = input->GetParamName(i);
				float paramVal = input->GetParam(i);
				fprintf(fp, ", %d:%d:%f", offset, i, paramVal);
			}
		}
		offset = input->GetNextOffset(offset);
	} while (offset > 0);

	fprintf(fp, "</timeline>\n");

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

	//fprintf(fp, "</%s>\n", input->GetName());
	fprintf(fp, "</input>\n");
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
	char *c = input->GetComponent()->GetName();
	char seps[]   = ":,";
	char *token = strtok( data, seps );
	int i=0;
	int param=0;
	input->Reset();
	while( token != NULL )
	{
		switch(i)
		{
		case 0:
			input->MoveTo(atoi(token));
			break;
		case 1:
			param = atoi(token);
			break;
		case 2:
			if(param == -1)
				input->Enable(0==strncmp(token, "ON", 2));
			else
				input->SetParam(param, (float)atof(token));
			break;
		}
		token = strtok( NULL, seps );
		if(i>=2)
			i=0;
		else
			i++;
	}
	input->Reset();
}

/*
void loadTimeline(IFloopySoundInput *input, char *data)
{
	if(!input)
		return;
	char *c = input->GetComponent()->GetName();
	char seps[]   = ",";
	char *token = strtok( data, seps );
	int i=0;
	int param=0;
	input->Reset();
	while( token != NULL )
	{
		char *s = strchr(token, ':');
		while(s)
		{
			switch(i)
			{
			case 0:
				input->MoveTo(atoi(token));
				break;
			case 1:
				input->Enable(0==strcmpi(token, "ON"));
				break;
			case 2:
				param = atoi(token);
				break;
			case 3:
				input->SetParam(param, (float)atof(token));
				break;
			}
			s = strchr(s, ':');
			i++;
		}
		token = strtok( NULL, seps );
	}
}
*/


/*
void saveXML(FILE *fp, IFloopySoundInput *input, BOOL recursive)
{
	if(NULL == input)
		return;

	input->Reset();

	IFloopySoundInput *comp = input->GetComponent();

	//fprintf(fp, "<%s source='%s'>\n", input->GetName(), comp->GetName());
	fprintf(fp, "<input source='%s'>\n", comp->GetName());

	fprintf(fp, "<timeline>\n");

	int offset=0;
	do {
		input->MoveTo(offset);
		char *enabled = (input->IsEnabled() ? "true" : "false");
		fprintf(fp, "<offset sample='%d'>\n", enabled);
		fprintf(fp, "<param enabled='%s'/>\n", enabled);
		if(input->GetParamCount() > 0)
		{
			for(int i=0; i<input->GetParamCount(); i++)
			{
				char *paramName = input->GetParamName(i);
				float paramVal = input->GetParam(i);
				fprintf(fp, "<param index='%d' value='%f'/>\n", i, paramVal);
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "</offset>\n");
		offset = input->GetNextOffset(offset);
	} while (offset > 0);

	fprintf(fp, "</timeline>\n");

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

	//fprintf(fp, "</%s>\n", input->GetName());
	fprintf(fp, "</input>\n");
}
*/
