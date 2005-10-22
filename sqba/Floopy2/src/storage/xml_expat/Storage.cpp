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
	m_iLevel	= 0;
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

bool CStorage::Load(char *filename)
{
	Close();

	if(NULL == m_pEngine)
		return false;

	m_pFile = fopen(filename, "w");

	if(NULL != m_pFile)
	{
	}

	Close();

	return false;
}

bool CStorage::Save(char *filename)
{
	Close();

	m_iLevel = 0;

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
	writeTabs();
	fprintf(m_pFile, "<input source='%s' name='%s'>\n", 
			input->GetName(), input->GetDisplayName());

	m_iLevel++;

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

	m_iLevel--;

	writeTabs();
	fprintf(m_pFile, "</input>\n");
}

void CStorage::writeProperties(IFloopySoundInput *input)
{
	if(input->GetPropertyCount() > 0)
	{
		bool bStart = true;

		writeTabs();
		fprintf(m_pFile, "<properties>\n");

		m_iLevel++;

		for(int index=0; index<input->GetPropertyCount(); index++)
		{
			float value = 0.f;
			if(input->GetPropertyVal(index, &value))
			{
				char *name = input->GetPropertyName( index );

				writeTabs();
				fprintf(m_pFile,
					"<property name='%s' index='%d' value='%f'/>\n",
					name, index, value);
			}
		}

		m_iLevel--;

		writeTabs();
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
		m_iLevel--;
		writeTabs();
		fprintf(m_pFile, "</parameters>\n");
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

	do
	{
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
					writeTabs();
					fprintf(m_pFile, "<parameters>\n");
					m_iLevel++;
					root = true;
				}
				writeTabs();
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

void CStorage::writeTabs()
{
	if(m_iLevel > 0)
	{
		char *tabs = new char[m_iLevel+1];
		//memset(tabs, 0, m_iLevel+1);
		//strnset(tabs, '\t', m_iLevel);
		for(int i=0; i<m_iLevel; i++)
			tabs[i] = '\t';
		tabs[m_iLevel] = '\0';
		fprintf(m_pFile, "%s", tabs);
		delete tabs;
	}
}
