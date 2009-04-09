// Install dependencies
// apt-get install libxml++2.6-dev libxml++2.6-doc
// sudo apt-get install libgtkmm-2.4-dev


#include "../ifloopy.h"
#include "../platform.h"
//#include "storage.h"
#include "tinyxml/tinyxml.h"

#include <string>
#include <vector>
using namespace std;



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




IFloopySoundInput *load_input(IFloopySoundEngine *engine, TiXmlElement* pElement, IFloopySoundInput *parent);

void load_color(const char *str, UINT *r, UINT *g, UINT *b)
{
	char seps[] = ",";
	char color[12] = {0};
	strncpy(color, str, 12);
	char *token = strtok( color, seps );
	int i=0;
	while( token != NULL )
	{
		switch(i)
		{
		case 0:	*r = atoi(token);	break;
		case 1:	*g = atoi(token);	break;
		case 2:	*b = atoi(token);	break;
		}
		token = strtok( NULL, seps );
		i++;
	}
}
/*
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
bool load_timeline(TiXmlElement* pElement, IFloopySoundInput *input)
{
	if ( !pElement ) return false;

	const char *data = pElement->GetText ();
	if( !data ) return false;
/*
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
*/

	string str(data);
	vector<string> tokens = tokenize(str, ",");
	vector<string>::iterator it1, it2;
	for( it1=tokens.begin(); it1<tokens.end(); it1++ )
 	{
		vector<string> tokens2 = tokenize(*it1, ":");
		const char *time = tokens2[0].c_str();
		const char *param = tokens2[1].c_str();
		const char *value = tokens2.size()>2 ? tokens2[2].c_str() : "";
		printf("%s : %s (%s)\n", time, param, value);
 	}

	return false;
}

IFloopySoundInput *load_input(IFloopySoundEngine *engine, TiXmlElement* pParent, IFloopySoundInput *parent)
{
	if ( !pParent ) return NULL;

	IFloopySoundInput *input = NULL;

	TiXmlElement* pElement = pParent->FirstChildElement("input");
	if ( !pElement ) return NULL;

	// check for full path
	const char *plugin = pElement->Attribute("plugin");
	if( !plugin ) return NULL;
	bool bEngine = (0 == stricmp("engine", plugin));
	if( bEngine )
		input = engine;
	else
		input = engine->CreateInput(plugin);
	assert( input );
	if( !input ) return NULL;

	const char *source = pElement->Attribute("source");
	if(bEngine && source)
	{
		input->Open( source );
	}

	const char *name = pElement->Attribute("name");
	if( name )
		input->SetDisplayName(name, strlen(name));

	const char *color = pElement->Attribute("color");
	if( color )
	{
		UINT r, g, b;
		load_color(color, &r, &g, &b);
		input->SetColor(r, g, b);
	}

	TiXmlElement *pTimeline = pElement->FirstChildElement("timeline");
	if( pTimeline )
		load_timeline( pTimeline, engine );

	load_input( engine, pElement, input);

	return input;
}


#ifdef __cplusplus
extern "C" {
#endif
EXPORTED bool Load(IFloopySoundEngine *engine, char *filename)
{
//	CStorage *pStorage;
//	pStorage = new CStorage( engine );
//	bool bResult = pStorage->Load( filename );
//	return bResult;

	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	if( !loadOkay )
	{
		printf("Failed to load file \"%s\"\n", filename);
	return false;
	}
//	printf("\n%s:\n", filename);
//	dump_to_stdout( &doc ); // defined later in the tutorial
	load_input( engine, doc.RootElement(), NULL );
	return true;
}

EXPORTED bool Save(IFloopySoundEngine *engine, char *filename)
{
	// same as write_simple_doc1 but add each node
	// as early as possible into the tree.

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
