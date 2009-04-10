
#include "xml.h"


CXml::CXml()
{
	m_szPath[0] = 0;
	m_pEngine = NULL;
}

CXml::~CXml()
{
}

std::vector<std::string> CXml::tokenize(const std::string& str,const std::string& delimiters)
{
	std::vector<std::string> tokens;

	// skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		// found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		// skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);

		// find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}

	return tokens;
}

bool CXml::IsFilter(IFloopySoundInput *input)
{
	int type = input->GetType();
	return(type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

bool CXml::IsMixer(IFloopySoundInput *input)
{
	int type = input->GetType();
	return(type == (TYPE_FLOOPY_SOUND_MIXER | type));
}

void CXml::get_library_path(LIB_HANDLE hModule, char *buff, int len)
{
#ifdef WIN32
	GetModuleFileName(hModule, buff, len);
#else
	// Linux specific

	// 1. executable
	readlink("/proc/self/exe", buff, len);

	// 2. dynamic library
	//DL_info info;
    //if (dladdr( &GetLibraryPath, &info ) == 0)
    //	strcpy(m_szPath, info.dli_fname);

    // 3.
    // g++ -o executable -Wl,-R -Wl,'$ORIGIN' executable.o libhe
#endif

	char *tmp = strrchr(buff, PATH_SEP);
	if(tmp)
		*(tmp+1) = '\0';
}

void CXml::set_color(IFloopySoundInput *input, TiXmlElement* pElement)
{
	const char *str = pElement->Attribute("color");
	if( !str )
		return;

	UINT r=0, g=0, b=0;
	char seps[] = ",";
	char color[12] = {0};
	strncpy(color, str, 12);
	char *token = strtok( color, seps );
	int i=0;
	while( token != NULL )
	{
		switch(i)
		{
		case 0:	r = atoi(token);	break;
		case 1:	g = atoi(token);	break;
		case 2:	b = atoi(token);	break;
		}
		token = strtok( NULL, seps );
		i++;
	}
	if(r<256 && g<256 && b<256)
		input->SetColor(r, g, b);
}

void CXml::set_param(IFloopySoundInput *input, float offset, const char *param, float value)
{
	SOUNDFORMAT *fmt = input->GetFormat();
	float freq = (float)fmt->frequency;

	assert( freq );

	offset *= freq;

//	printf("%f:%s:%f\n", offset, param, value);

	if(0==stricmp(param, "ON"))
	{
		input->EnableAt(offset, true);
	}
	else if(0==stricmp(param, "OFF"))
	{
		input->EnableAt(offset, false);
	}
	else if(0==stricmp(param, "RESET"))
	{
		input->SetParamAt(offset, TIMELINE_PARAM_MOVETO, 0.f);
	}
	else if(0==stricmp(param, "MOVETO"))
	{
		value *= freq;
		input->SetParamAt(offset, TIMELINE_PARAM_MOVETO, value);
	}
	else
	{
		int iParam = atoi(param);
		input->SetParamAt(offset, iParam, value);
	}
}

bool CXml::set_timeline(IFloopySoundInput *input, TiXmlElement* pElement)
{
	if( !pElement )
		return false;

	TiXmlElement *pTimeline = pElement->FirstChildElement("timeline");
	if( !pTimeline )
		return false;

	const char *data = pTimeline->GetText();
	if( !data )
		return false;

	std::string str(data);
	std::vector<std::string> tokens = tokenize(str, ",");
	std::vector<std::string>::iterator it1, it2;
	for( it1=tokens.begin(); it1<tokens.end(); it1++ )
 	{
		std::vector<std::string> tokens2 = tokenize(*it1, ":");
		const char *time = tokens2[0].c_str();
		const char *param = tokens2[1].c_str();
		const char *value = tokens2.size()>2 ? tokens2[2].c_str() : "";

		float offset = atof(time);
		float val = atof(value);

//		printf("%s(%f):%s:%s\n", time, offset, param, value);

		set_param(input, offset, param, val);
 	}

	return true;
}

bool CXml::set_properties(IFloopySoundInput *input, TiXmlElement* pElement)
{
	TiXmlElement *pProperties = pElement->FirstChildElement("properties");
	if( !pProperties )
		return false;

	TiXmlAttribute* pAttrib = pElement->FirstAttribute();
	while (pAttrib)
	{
		int index;
		const char *name = pAttrib->Name();
		if( input->GetPropertyIndex(name, &index) )
		{
			//float value = (float)atof( pAttrib->Value() );
			double value;
			if( pAttrib->QueryDoubleValue(&value) == TIXML_SUCCESS)
			{
				input->SetPropertyVal(index, value);
			}
		}
		pAttrib = pAttrib->Next();
	}

	return true;
}

bool CXml::open_source(IFloopySoundInput *input, TiXmlElement* pElement)
{
	const char *source = pElement->Attribute("source");
	if( !source )
		return true;

	char path[MAX_PATH] = {0};
	strcpy(path, m_szPath);
	strcat(path, source);
	return input->Open( path );
}

bool CXml::set_name(IFloopySoundInput *input, TiXmlElement* pElement)
{
	const char *name = pElement->Attribute("name");
	if( !name )
		return false;
	input->SetDisplayName(name, strlen(name));
	return true;
}

IFloopySoundInput *CXml::create_input(IFloopySoundInput *parent, TiXmlElement* pElement)
{
	IFloopySoundInput *input = NULL;

	// check for full path
	const char *plugin = pElement->Attribute("plugin");
	if( !plugin )
		return NULL;

	bool bRoot = ((0 == stricmp("engine", plugin)) && (NULL == parent));

	input = bRoot ? m_pEngine : m_pEngine->CreateInput(plugin);

	assert( input );

	return input;
}

IFloopySoundInput *CXml::load_input(IFloopySoundInput *parent, TiXmlElement* pElement)
{
	if ( !pElement )
		return NULL;

	IFloopySoundInput *input = create_input(parent, pElement);

	if( !open_source(input, pElement) )
		return NULL;

	if( IsMixer(input) )
	{
		TiXmlNode *pChild = NULL;
		while(pChild = pElement->IterateChildren("input", pChild))
		{
			TiXmlElement *pTmpElement = pChild->ToElement();
			if( pTmpElement )
			{
				IFloopySoundInput *src = load_input(input, pTmpElement);
				if(src);
					((IFloopySoundMixer*)input)->AddSource( src );
			}
		}
	}
	else if( IsFilter(input) )
	{
		TiXmlElement *pInput = pElement->FirstChildElement("input");
		if( pInput )
		{
			IFloopySoundInput *src = load_input(input, pInput);
			if( src );
				((IFloopySoundFilter*)input)->SetSource( src );
		}
	}

	set_properties(input, pElement);
	set_name(input, pElement);
	set_color(input, pElement);

	// Timeline has to be set at the end because of the frequency
	set_timeline(input, pElement);

	return input;
}


bool CXml::Load(IFloopySoundEngine *engine, const char *filename)
{
	TiXmlDocument doc(filename);
	if( !doc.LoadFile() )
	{
		fprintf(stderr, "Failed to load file '%s'\n", filename);
		return false;
	}

	m_pEngine = engine;
	strcpy(m_szPath, filename);
	char *tmp = strrchr(m_szPath, PATH_SEP);
	if(tmp)
		*(tmp+1) = '\0';

	TiXmlElement* pElement = doc.FirstChildElement("input");
	if ( !pElement )
		return NULL;

	load_input( NULL, pElement );

	return true;
}

bool CXml::Save(IFloopySoundEngine *engine, const char *filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement * element = new TiXmlElement( "Hello" );
	doc.LinkEndChild( element );

	TiXmlText * text = new TiXmlText( "World" );
	element->LinkEndChild( text );

	doc.SaveFile( filename );

	return false;
}

const char *CXml::GetExtension()
{
	return "XML";
}
