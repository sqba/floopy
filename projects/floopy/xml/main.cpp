
#include <string>
#include <vector>
#include "../ifloopy.h"
#include "../platform.h"
#include "tinyxml/tinyxml.h"


using namespace std;

char g_szPath[MAX_PATH] = {0};
IFloopySoundEngine *g_pEngine = NULL;


vector<string> tokenize(const string& str,const string& delimiters)
{
	vector<string> tokens;

	// skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
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

bool IsFilter(IFloopySoundInput *input)
{
	int type = input->GetType();
	return(type == (TYPE_FLOOPY_SOUND_FILTER | type));
}

bool IsMixer(IFloopySoundInput *input)
{
	int type = input->GetType();
	return(type == (TYPE_FLOOPY_SOUND_MIXER | type));
}

void get_library_path(LIB_HANDLE hModule, char *buff, int len)
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

void set_color(IFloopySoundInput *input, TiXmlElement* pElement)
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

void set_param(IFloopySoundInput *input, float offset, const char *param, float value)
{
	SOUNDFORMAT *fmt = input->GetFormat();
	float freq = (float)fmt->frequency;

	offset *= freq;

	if(0==stricmp(param, "ON"))
	{
		input->SetParamAt(offset, TIMELINE_PARAM_ENABLE, 0.f);
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

bool set_timeline(IFloopySoundInput *input, TiXmlElement* pElement)
{
	if( !pElement )
		return false;

	TiXmlElement *pTimeline = pElement->FirstChildElement("timeline");
	if( !pTimeline )
		return false;

	const char *data = pTimeline->GetText();
	if( !data )
		return false;

	string str(data);
	vector<string> tokens = tokenize(str, ",");
	vector<string>::iterator it1, it2;
	for( it1=tokens.begin(); it1<tokens.end(); it1++ )
 	{
		vector<string> tokens2 = tokenize(*it1, ":");
		const char *time = tokens2[0].c_str();
		const char *param = tokens2[1].c_str();
		const char *value = tokens2.size()>2 ? tokens2[2].c_str() : "";

//		printf("%s : %s (%s)\n", time, param, value);

		float offset = atof(time);
		float val = atof(value);
		set_param(input, offset, param, val);
 	}

	return true;
}

bool set_properties(IFloopySoundInput *input, TiXmlElement* pElement)
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

bool open_source(IFloopySoundInput *input, TiXmlElement* pElement)
{
	const char *source = pElement->Attribute("source");
	if( !source )
		return true;

	char path[MAX_PATH] = {0};
	strcpy(path, g_szPath);
	strcat(path, source);
	return input->Open( path );
}

bool set_name(IFloopySoundInput *input, TiXmlElement* pElement)
{
	const char *name = pElement->Attribute("name");
	if( !name )
		return false;
	input->SetDisplayName(name, strlen(name));
	return true;
}

IFloopySoundInput *create_input(IFloopySoundInput *parent, TiXmlElement* pElement)
{
	IFloopySoundInput *input = NULL;

	// check for full path
	const char *plugin = pElement->Attribute("plugin");
	if( !plugin )
		return NULL;

	bool bRoot = ((0 == stricmp("engine", plugin)) && (NULL == parent));

	input = bRoot ? g_pEngine : g_pEngine->CreateInput(plugin);

	assert( input );

	return input;
}

IFloopySoundInput *load_input(IFloopySoundInput *parent, TiXmlElement* pElement)
{
	if ( !pElement )
		return NULL;

	IFloopySoundInput *input = create_input(parent, pElement);

	if( !open_source(input, pElement) )
		return NULL;

	set_properties(input, pElement);
	set_name(input, pElement);
	set_color(input, pElement);
	set_timeline(input, pElement);

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

	return input;
}


#ifdef __cplusplus
extern "C" {
#endif
EXPORTED bool Load(IFloopySoundEngine *engine, char *filename)
{
	TiXmlDocument doc(filename);
	if( !doc.LoadFile() )
	{
		fprintf(stderr, "Failed to load file '%s'\n", filename);
		return false;
	}

	g_pEngine = engine;
	strcpy(g_szPath, filename);
	char *tmp = strrchr(g_szPath, PATH_SEP);
	if(tmp)
		*(tmp+1) = '\0';

	TiXmlElement* pElement = doc.FirstChildElement("input");
	if ( !pElement )
		return NULL;

	load_input( NULL, pElement );

	return true;
}

EXPORTED bool Save(IFloopySoundEngine *engine, char *filename)
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

EXPORTED char *GetExtension()
{
	return "XML";
}
#ifdef __cplusplus
}
#endif











/*
vector<string> tokenize(const string& str,const string& delimiters)
{
 vector<string> tokens;
 string::size_type delimPos = 0, tokenPos = 0, pos = 0;

 if(str.length()<1)  return tokens;
 while(1){
   delimPos = str.find_first_of(delimiters, pos);
   tokenPos = str.find_first_not_of(delimiters, pos);

   if(string::npos != delimPos){
     if(string::npos != tokenPos){
       if(tokenPos<delimPos){
         tokens.push_back(str.substr(pos,delimPos-pos));
       }else{
         tokens.push_back("");
       }
     }else{
       tokens.push_back("");
     }
     pos = delimPos+1;
   } else {
     if(string::npos != tokenPos){
       tokens.push_back(str.substr(pos));
     } else {
       tokens.push_back("");
     }
     break;
   }
 }
 return tokens;
}
*/


/*
IFloopySoundInput *load_input(IFloopySoundEngine *engine, TiXmlElement* pElement, IFloopySoundInput *parent);
int load_children(IFloopySoundEngine *engine, TiXmlElement* pElement, IFloopySoundInput *parent)
{
	if ( !pElement ) return false;

	int count = 0;
	TiXmlNode* child;
	for ( child = pElement->FirstChild(); child != 0; child = child->NextSibling())
	{
		TiXmlElement *input = child->FirstChildElement("input");
		if( load_input( engine, input, parent ) )
		{
			count++;
		}
	}

	return count;
}
*/








/*
const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent( unsigned int numIndents )
{
	static const char * pINDENT="                                      + ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt( unsigned int numIndents )
{
	static const char * pINDENT="                                        ";
	static const unsigned int LENGTH=strlen( pINDENT );
	unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
	if ( n > LENGTH ) n = LENGTH;

	return &pINDENT[ LENGTH-n ];
}

int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent)
{
	if ( !pElement ) return 0;

	TiXmlAttribute* pAttrib=pElement->FirstAttribute();
	int i=0;
	int ival;
	double dval;
	const char* pIndent=getIndent(indent);
	printf("\n");
	while (pAttrib)
	{
		printf( "%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    printf( " int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) printf( " d=%1.1f", dval);
		printf( "\n" );
		i++;
		pAttrib=pAttrib->Next();
	}
	return i;
}

void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 )
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
			case 0:  printf( " (No attributes)"); break;
			case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
			default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;

	case TiXmlNode::COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
	{
		dump_to_stdout( pChild, indent+1 );
	}
}
*/
