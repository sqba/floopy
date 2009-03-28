// Storage.cpp: implementation of the CStorage class.
//
//////////////////////////////////////////////////////////////////////

#include "storage.h"

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>

using namespace std;
using namespace xercesc;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStorage::CStorage(IFloopySoundEngine *engine)
{
	m_pEngine = engine;

	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Error during initialization! :\n"
			 << message << "\n";
		XMLString::release(&message);
	}
}

CStorage::~CStorage()
{

}

bool CStorage::Load(char *filename)
{
	bool bResult = false;
	XercesDOMParser* parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);    // optional

	ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
	parser->setErrorHandler(errHandler);

	try {
		parser->parse(filename);
		DOMDocument *doc = parser->getDocument();
		//bResult = true;
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
			 << message << "\n";
		XMLString::release(&message);
	}
	catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		cout << "Exception message is: \n"
			 << message << "\n";
		XMLString::release(&message);
	}
	catch (...) {
		cout << "Unexpected Exception \n" ;
	}

	delete parser;
	delete errHandler;
	
	return bResult;
}

bool CStorage::Save(char *filename)
{
	m_iIndent = 0;

	if(NULL == m_pEngine)
		return false;

	FILE *fp = fopen(filename, "w");

	if(NULL == fp)
		return false;

	writeInput(fp, m_pEngine);
	fclose( fp );
	return true;
}


// Writing

void CStorage::writeInput(FILE *fp, IFloopySoundInput *input)
{
	writeIndentation(fp);
	fprintf(fp, "<input source='%s' name='%s'>\n",
			input->GetName(), input->GetDisplayName());

	m_iIndent++;

	writeProperties(fp, input);

	writeParameters(fp, input);

	int type = input->GetType();
	if(type == TYPE_FLOOPY_SOUND_MIXER)
	{
		IFloopySoundMixer *mixer = (IFloopySoundMixer*)input;
		for(int i=0; i<mixer->GetInputCount(); i++)
		{
			writeInput(fp, mixer->GetSource(i));
		}
	}
	else if( type == (TYPE_FLOOPY_SOUND_FILTER | type) )
	{
		IFloopySoundFilter *filter = (IFloopySoundFilter*)input;
		writeInput(fp, filter->GetSource());
	}

	m_iIndent--;

	writeIndentation(fp);
	fprintf(fp, "</input>\n");
}

void CStorage::writeProperties(FILE *fp, IFloopySoundInput *input)
{
	if(input->GetPropertyCount() > 0)
	{
//		bool bStart = true;

		writeIndentation(fp);
		fprintf(fp, "<properties>\n");

		m_iIndent++;

		for(int index=0; index<input->GetPropertyCount(); index++)
		{
			float value = 0.f;
			if(input->GetPropertyVal(index, &value))
			{
				const char *name = input->GetPropertyName( index );

				writeIndentation(fp);
				fprintf(fp,
					"<property name='%s' index='%d' value='%f'/>\n",
					name, index, value);
			}
		}

		m_iIndent--;

		writeIndentation(fp);
		fprintf(fp, "</properties>\n");
	}
}

void CStorage::writeParameters(FILE *fp, IFloopySoundInput *input)
{
	bool bRoot = false;

	writeParameter(fp, input, TIMELINE_PARAM_ENABLE, bRoot);
	writeParameter(fp, input, TIMELINE_PARAM_MOVETO, bRoot);

	for(int index=0; index<input->GetParamCount(); index++)
	{
		writeParameter(fp, input, index, bRoot);
	}

	if(bRoot)
	{
		m_iIndent--;
		writeIndentation(fp);
		fprintf(fp, "</timeline>\n");
	}
}

void CStorage::writeParameter(FILE *fp, IFloopySoundInput *input, int index, bool &root)
{
	SOUNDFORMAT *fmt = input->GetFormat();
	int freq		= fmt->frequency;
	int offset		= 0;
	bool bStart		= true;
	float value		= 0.f;
	const char *tmp	= NULL;
	const char *name= input->GetParamName(index);

	do {
		float seconds = (float)offset / (float)freq;
		input->MoveTo(offset);
		if(input->GetParamAt(offset, index, &value))
		{
			if(!bStart)
				fprintf(fp, ", "); // Separator
			else
			{
				if(!root)
				{
					writeIndentation(fp);
					fprintf(fp, "<timeline>\n");
					m_iIndent++;
					root = true;
				}
				writeIndentation(fp);
				fprintf(fp, "<param name='%s' index='%d'>", name, index);
				bStart = false;
			}

			switch(index)
			{
			case TIMELINE_PARAM_ENABLE:
				tmp = (value == PARAM_VALUE_ENABLED ? "ON" : "OFF");
				fprintf(fp, "%.4f:%s", seconds, tmp);
				break;
			case TIMELINE_PARAM_MOVETO:
				if(value == 0.f)
					fprintf(fp, "%.4f:RESET", seconds);
				else
					fprintf(fp, "%.4f:MOVETO:%d", seconds, (int)value);
				break;
			default:
				fprintf(fp, "%.4f:%.4f", seconds, value);
			}
		}
		offset = input->GetNextOffset(offset, index);
	} while (offset > 0);
	if(!bStart)
		fprintf(fp, "</param>\n");
}

void CStorage::writeIndentation(FILE *fp)
{
	for(int i=0; i<m_iIndent; i++)
		fprintf(fp, "\t");
}


























/*
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>



#include <xercesc/util/PlatformUtils.hpp>
// Other include files, declarations, and non-Xerces-C++ initializations.

using namespace xercesc;

int main(int argc, char* argv[])
{
  try {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch) {
    // Do your failure processing here
    return 1;
  }

  // Do your actual work with Xerces-C++ here.

  XMLPlatformUtils::Terminate();

  // Other terminations and cleanup.
  return 0;
}
*/
/*
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <iostream>

using namespace std;
using namespace xercesc;
*/
/*
int main (int argc, char* args[]) {

	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Error during initialization! :\n"
			 << message << "\n";
		XMLString::release(&message);
		return 1;
	}

	XercesDOMParser* parser = new XercesDOMParser();
	parser->setValidationScheme(XercesDOMParser::Val_Always);
	parser->setDoNamespaces(true);    // optional

	ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
	parser->setErrorHandler(errHandler);

	char* xmlFile = "x1.xml";

	try {
		parser->parse(xmlFile);
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		cout << "Exception message is: \n"
			 << message << "\n";
		XMLString::release(&message);
		return -1;
	}
	catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		cout << "Exception message is: \n"
			 << message << "\n";
		XMLString::release(&message);
		return -1;
	}
	catch (...) {
		cout << "Unexpected Exception \n" ;
		return -1;
	}

	delete parser;
	delete errHandler;
	return 0;
}
*/

// http://xerces.apache.org/xerces-c/program-dom-3.html#UsingDOMAPI
// http://www.yolinux.com/TUTORIALS/XML-Xerces-C.html





