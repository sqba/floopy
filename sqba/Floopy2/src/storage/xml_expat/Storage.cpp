// Storage.cpp: implementation of the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "Storage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStorage::CStorage(IFloopySoundEngine *engine)
{
	m_pEngine	= engine;
	m_pFile		= NULL;
	m_iIndent	= 0;
}

CStorage::~CStorage()
{
	Close();
}

void CStorage::Close()
{
	if(NULL != m_pFile)
		fclose( m_pFile );
}

bool CStorage::Save(char *filename)
{
	Close();

	m_iIndent = 0;

	if(NULL == m_pEngine)
		return false;

	m_pFile = fopen(filename, "w");

	if(NULL != m_pFile)
	{
		writeInput(m_pEngine);
		Close();
		return true;
	}

	return false;
}

void CStorage::writeInput(IFloopySoundInput *input)
{
	writeIndentation();
	fprintf(m_pFile, "<input source='%s' name='%s'>\n", 
			input->GetName(), input->GetDisplayName());

	m_iIndent++;

	writeProperties(input);

	writeParameters( input );

	int type = input->GetType();
	if(type == TYPE_FLOOPY_SOUND_MIXER)
	{
		IFloopySoundMixer *mixer = (IFloopySoundMixer*)input;
		for(int i=0; i<mixer->GetInputCount(); i++)
		{
			writeInput(mixer->GetSource(i));
		}
	}
	else if( type == (TYPE_FLOOPY_SOUND_FILTER | type) )
	{
		IFloopySoundFilter *filter = (IFloopySoundFilter*)input;
		writeInput( filter->GetSource() );
	}

	m_iIndent--;

	writeIndentation();
	fprintf(m_pFile, "</input>\n");
}

void CStorage::writeProperties(IFloopySoundInput *input)
{
	if(input->GetPropertyCount() > 0)
	{
		bool bStart = true;

		writeIndentation();
		fprintf(m_pFile, "<properties>\n");

		m_iIndent++;

		for(int index=0; index<input->GetPropertyCount(); index++)
		{
			float value = 0.f;
			if(input->GetPropertyVal(index, &value))
			{
				char *name = input->GetPropertyName( index );

				writeIndentation();
				fprintf(m_pFile,
					"<property name='%s' index='%d' value='%f'/>\n",
					name, index, value);
			}
		}

		m_iIndent--;

		writeIndentation();
		fprintf(m_pFile, "</properties>\n");
	}
}

void CStorage::writeParameters(IFloopySoundInput *input)
{
	bool bRoot = false;

	writeParameter(input, TIMELINE_PARAM_ENABLE, bRoot);
	writeParameter(input, TIMELINE_PARAM_MOVETO, bRoot);

	for(int index=0; index<input->GetParamCount(); index++)
	{
		writeParameter(input, index, bRoot);
	}

	if(bRoot)
	{
		m_iIndent--;
		writeIndentation();
		fprintf(m_pFile, "</timeline>\n");
	}
}

void CStorage::writeParameter(IFloopySoundInput *input, int index, bool &root)
{
	SOUNDFORMAT *fmt = input->GetFormat();
	int freq		= fmt->frequency;
	int offset		= 0;
	bool bStart		= true;
	float value		= 0.f;
	char *tmp		= NULL;
	char *name		= input->GetParamName(index);

	do {
		float seconds = (float)offset / (float)freq;
		input->MoveTo(offset);
		if(input->GetParamAt(offset, index, &value))
		{
			if(!bStart)
				fprintf(m_pFile, ", "); // Separator
			else
			{
				if(!root)
				{
					writeIndentation();
					fprintf(m_pFile, "<timeline>\n");
					m_iIndent++;
					root = true;
				}
				writeIndentation();
				fprintf(m_pFile, "<param name='%s' index='%d'>", name, index);
				bStart = false;
			}
			
			switch(index)
			{
			case TIMELINE_PARAM_ENABLE:
				tmp = (value == PARAM_VALUE_ENABLED ? "ON" : "OFF");
				fprintf(m_pFile, "%.4f:%s", seconds, tmp);
				break;
			case TIMELINE_PARAM_MOVETO:
				if(value == 0.f)
					fprintf(m_pFile, "%.4f:RESET", seconds);
				else
					fprintf(m_pFile, "%.4f:MOVETO:%d", seconds, (int)value);
				break;
			default:
				fprintf(m_pFile, "%.4f:%.4f", seconds, value);
			}
		}
		offset = input->GetNextOffset(offset, index);
	} while (offset > 0);
	if(!bStart)
		fprintf(m_pFile, "</param>\n");
}

void CStorage::writeIndentation()
{
	for(int i=0; i<m_iIndent; i++)
		fprintf(m_pFile, "\t");
}











bool CStorage::Load(char *filename)
{
	Close();

	m_pFile = fopen(filename, "r");
	if(NULL == m_pFile)
	{
		fprintf(stderr, "File not found: %s\n", filename);
		return false;
	}

	tSessionInfo si;
	memset(&si, 0, sizeof(tSessionInfo));

	si.pEngine			= m_pEngine;
	si.pInput			= m_pEngine;
	si.buff[0]			= NULL;
	si.objects[0].obj	= m_pEngine;
	si.level			= 0;

	// Extract project path
	char *pathend = strrchr(filename, '\\');
	if(pathend)
		memcpy(si.pPath, filename, pathend - filename);


	char buf[BUFSIZ];
	si.parser = XML_ParserCreate(NULL);
	int done;

	XML_SetUserData(si.parser, &si);
	XML_SetElementHandler(si.parser, startElement, endElement);
	XML_SetCharacterDataHandler(si.parser, elementData);

	do {
		size_t len = fread(buf, 1, sizeof(buf), m_pFile);
		done = (len < sizeof(buf));
		if (!XML_Parse(si.parser, buf, len, done))
		{
			fprintf(stderr,
				"%s at line %d\n",
				XML_ErrorString(XML_GetErrorCode(si.parser)),
				XML_GetCurrentLineNumber(si.parser));
			XML_ParserFree(si.parser);
			fclose(m_pFile);
			return false;
		}
	} while( !done );

	loadTimelines(&si);

	XML_ParserFree(si.parser);
	fclose(m_pFile);

	return true;
}

void CStorage::startElement(void *userData, const char *name, const char **atts)
{
	tSessionInfo *si = (tSessionInfo*)userData;
	si->pElement = name;
	if(0 == strcmp(name, "input"))
	{
		loadInputNode(si, atts);
	}
	else if(0 == strcmp(name, "properties"))
	{
		loadPropertiesNode(si, atts);
	}
}

void CStorage::endElement(void *userData, const char *name)
{
	tSessionInfo *si = (tSessionInfo*)userData;

	if(si->pInput && (0 == strcmp(name, "input")))
	{
		si->pInput = si->buff[--si->level];
		si->iIndex++;
	}
	else if(0 == strcmp(si->pElement, "timeline"))
	{
		if(si->pBuff)
		{
			si->objects[si->iIndex].data = new char[si->iBuffEnd];
			memcpy(si->objects[si->iIndex].data, si->pBuff, si->iBuffEnd);
			si->iIndex++;
			//loadTimeline(pInput, pBuff);

			delete[] si->pBuff;
			si->pBuff = NULL;
			si->iBuffEnd = 0;
		}
	}
}

void CStorage::elementData(void *userData, const char *data, int len)
{
	tSessionInfo *si = (tSessionInfo*)userData;

	if(0 == strcmp(si->pElement, "timeline"))
	{
		if(!si->pBuff)
		{
			si->iBuffLen = len*2 + 1;
			si->pBuff = new char[si->iBuffLen];
			memset(si->pBuff, 0, si->iBuffLen);
		}

		if(si->iBuffEnd + len < si->iBuffLen)
		{
			strncat(si->pBuff, data, len);
		}
		else
		{
			char *tmp = si->pBuff;
			si->iBuffLen = si->iBuffEnd + len + 1;
			si->pBuff = new char[si->iBuffLen];
			memset(si->pBuff, 0, si->iBuffLen);
			memcpy(si->pBuff, tmp, si->iBuffEnd);
			strncat(si->pBuff, data, len);
			delete[] tmp;
		}
		si->iBuffEnd += len;
	}
}

void CStorage::loadInputNode(tSessionInfo *si, const char **atts)
{
	XML_Parser parser = si->parser;
	UINT r=256, g=256, b=256;

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
				if(0==strcmp(atts[i], "color"))
				{
					char *tmp = (char*)atts[i+1];
					loadColor(tmp, &r, &g, &b);
				}
			}
		}

		if(source && (0 != strcmp(source, "engine")))
		{
			char tmp[_MAX_PATH] = {0};
			if(strlen(si->pPath))
			{
				if(strchr(source, ':'))	// Absolute path!
					memcpy(tmp, source, strlen(source));
				else
				{
					memcpy(tmp, si->pPath, strlen(si->pPath));
					strcat(tmp, "\\");
					strcat(tmp, source);
				}
			}
			else
				memcpy(tmp, source, strlen(source));
			
			IFloopySoundInput *input = si->pEngine->CreateInput(tmp);
			if(input && si->pInput)
			{
				if( IsFilter(si->pInput) )
				{
					((IFloopySoundFilter*)si->pInput)->SetSource(input);
					si->pInput = input;
					if(desc)
						input->SetDisplayName(desc, strlen(desc));
					if(r<256 && g<256 && b<256)
						input->SetColor(r, g, b);
					r=g=b=256;
					si->objects[si->iIndex].obj = input;
					si->buff[++si->level] = si->pInput;
				}
			}
			else
				si->pInput = NULL;
		}
		else
		{
			if(desc)
				si->pEngine->SetDisplayName(desc, strlen(desc));
			if(r<256 && g<256 && b<256)
				si->pEngine->SetColor(r, g, b);
			r=g=b=256;
		}
	}
}

void CStorage::loadPropertiesNode(tSessionInfo *si, const char **atts)
{
	XML_Parser parser = si->parser;
	IFloopySoundInput *input = si->pInput;
	int n = XML_GetSpecifiedAttributeCount(parser);

	if(NULL!=input && n>=2)
	{
		for(int i=0; i<n-1; i+=2)
		{
			if(NULL != atts[i])
			{
				int index = 0;
				if( input->GetPropertyIndex((char*)atts[i], &index) )
				{
					float value = (float)atof(atts[i+1]);
					input->SetPropertyVal(index, value);
				}
			}
		}
	}
}

void CStorage::loadColor(char *str, UINT *r, UINT *g, UINT *b)
{
	char seps[] = ",";
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

bool CStorage::IsFilter(IFloopySoundInput *input)
{
	int type = input->GetType();
	return (type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

// Problem: MoveTo nece da radi kada izvor nije kompletno
// inicijalizovan, odnosno kada nije inicijalizovana
// SOUNDFORMAT struktura.
// Zbog toga, prvo se moraju ucitati svi objekti pa tek
// onda njihovi timeline-ovi.
void CStorage::loadTimelines(tSessionInfo *si)
{
	for(int i=0; i<si->iIndex; i++)
	{
		loadTimeline(si, si->objects[i].obj, si->objects[i].data);
	}
}


void CStorage::loadTimeline(tSessionInfo *si, IFloopySoundInput *input, char *data)
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
