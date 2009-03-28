#include "storage.h"


#include <iostream>
//#include <gtkmm-2.4/gtkmm.h>
//#include <libxml++/libxml++.h>



CStorage::CStorage(IFloopySoundEngine *engine)
{
	m_pEngine = engine;
}

CStorage::~CStorage()
{
}




bool CStorage::Load(char *filename)
{
	bool bResult = false;
/*
	//Glib::ustring filepath = filename;
	std::string filepath = filename;

	try
	{
		xmlpp::DomParser parser;
		parser.set_validate();
		parser.set_substitute_entities(); //We just want the text to be resolved/unescaped automatically.
		parser.parse_file(filepath);
		if(parser)
		{
			//Walk the tree:
			//const xmlpp::Node* pNode = parser.get_document()->get_root_node(); //deleted by DomParser.
			//print_node(pNode);
		}
	}
	catch(const std::exception& ex)
	{
		std::cout << "Exception caught: " << ex.what() << std::endl;
	}
*/
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



