// vstwrapper.cpp: implementation of the CVstWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "vstwrapper.h"

long Callback(AEffect *effect, long opcode,
			   long index, long value,
			   void *ptr, float opt)
{
	switch(opcode)
	{
	case audioMasterAutomate:		// index, value, returns 0
		return 0;
	case audioMasterVersion:		// VST Version supported (for example 2200 for VST 2.2)
		return 2;
	case audioMasterCurrentId:		// Returns the unique id of a plug that's currently loading
		return 0;
	case audioMasterIdle:			// Call application idle routine (this will call
		return 0;					// effEditIdle for all open editors too) 
	case audioMasterPinConnected:	// Inquire if an input or output is beeing connected;
		return 0;					// index enumerates input or output counting from zero,
									// value is 0 for input and != 0 otherwise. note: the
									// return value is 0 for <true> such that older versions
									// will always return true.
	}
	return 0;
}


typedef AEffect* (*main_plugin)(audioMasterCallback audioMaster);



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVstWrapper::CVstWrapper()
{
	m_hinst		= NULL;
	m_pPlugin	= NULL;
	m_pAudioEffect = NULL;
	m_nBuffSize	= 0;

	m_pInputs[0] = NULL;
	m_pInputs[1] = NULL;

	m_pOutputs[0] = NULL;
	m_pOutputs[1] = NULL;

	// Popraviti error handling u engine-u kada
	// sledece vrednosti nisu postavljene!
	m_format.frequency		= 44100;
	m_format.format			= WAVE_FORMAT_PCM;
	m_format.channels		= 2;
	m_format.bitsPerSample	= 32;
	m_format.sampleDataType = SAMPLE_DATA_TYPE_FLOAT;
}

CVstWrapper::~CVstWrapper()
{
	if(NULL != m_hinst)
		FreeLibrary(m_hinst);

	if(NULL != m_pPlugin)
		delete m_pPlugin;

	if(NULL != m_pInputs[0])
		delete m_pInputs[0];

	if(NULL != m_pInputs[1])
		delete m_pInputs[1];

	if(NULL != m_pOutputs[0])
		delete m_pOutputs[0];

	if(NULL != m_pOutputs[1])
		delete m_pOutputs[1];
}

SOUNDFORMAT *CVstWrapper::GetFormat()
{
	return &m_format;
}

bool CVstWrapper::Open(char *filename)
{
	m_hinst = LoadLibraryA( filename );
	if(NULL == m_hinst)
	{
		//sprintf(m_szLastError, "File '%s' not found.\n\0", name);
		return false;
	}

	main_plugin func = (main_plugin)GetProcAddress(m_hinst, "main"); 
	if(NULL == func)
	{
		//sprintf(m_szLastError, "Function 'main' not found in library '%s'.\n\0", filename);
		return false;
	}

	m_pPlugin = func(Callback);
	if(NULL == m_pPlugin)
	{
		//sprintf(m_szLastError, "Plugin not created by function 'main'.\n\0");
		return false;
	}

	m_pAudioEffect = (AudioEffect*)m_pPlugin->object;

	m_pPlugin->setParameter(m_pPlugin, 0, 0.5);
	m_pPlugin->setParameter(m_pPlugin, 1, 0.5);
	m_pPlugin->setParameter(m_pPlugin, 2, 0.5);
	m_pPlugin->setParameter(m_pPlugin, 3, 0.5);

	return true;
}

int CVstWrapper::Read(BYTE *data, int size)
{
	if(NULL == m_pPlugin)
		return 0;

	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL == fmt)
		return 0;

	int sampleFrames = size / (fmt->bitsPerSample / 8);

	if(sampleFrames > m_nBuffSize)
	{
		if(NULL != m_pInputs[0])
			delete m_pInputs[0];

		if(NULL != m_pInputs[1])
			delete m_pInputs[1];

		if(NULL != m_pOutputs[0])
			delete m_pOutputs[0];

		if(NULL != m_pOutputs[1])
			delete m_pOutputs[1];

		m_nBuffSize = sampleFrames;

		m_pInputs[0]  = new float[sampleFrames];
		m_pInputs[1]  = new float[sampleFrames];
		m_pOutputs[0] = new float[sampleFrames];
		m_pOutputs[1] = new float[sampleFrames];
	}

	int len = IFloopySoundFilter::Read(data, size);

	float *inputs	= m_pInputs[0];
	float *in		= (float*)data;
	for(int i=0; i<sampleFrames; i++)
	{
		*inputs = *in;
		inputs++;
		in++;
	}

	try {
		m_pPlugin->processReplacing(m_pPlugin, m_pInputs, m_pOutputs, sampleFrames);
	} catch(...) {
		return 0;
	}

	float *outputs	= m_pOutputs[0];
	float *out		= (float*)data;
	for(i=0; i<sampleFrames; i++)
	{
		*out = *outputs;
		outputs++;
		out++;
	}

	return size;
}

void CVstWrapper::Reset()
{
	IFloopySoundFilter::Reset();
}

void CVstWrapper::MoveTo(int offset)
{
	IFloopySoundFilter::MoveTo(offset);
}





int CVstWrapper::GetParamCount()
{
	if(NULL != m_pPlugin)
		return m_pPlugin->numParams;
	return 0;
}

float CVstWrapper::GetParamMin(int index)
{
	return 0.f;
}

float CVstWrapper::GetParamMax(int index)
{
	return 1.f;
}

char *CVstWrapper::GetParamUnit(int index)
{
	return NULL;
}

float CVstWrapper::GetParamStep(int index)
{
	return 0.1f;
}

bool CVstWrapper::GetParamVal(int index, float *value)
{
	if(NULL!=m_pPlugin && index<GetParamCount())
	{
		*value = m_pPlugin->getParameter(m_pPlugin, index);
		return true;
	}
	return false;
}

void CVstWrapper::SetParamVal(int index, float value)
{
	if(NULL!=m_pPlugin && index<GetParamCount())
		m_pPlugin->setParameter(m_pPlugin, index, value);

//	if(NULL != m_pAudioEffect)
//		m_pAudioEffect->processEvents(NULL);
}

char *CVstWrapper::GetParamName(int index)
{
	static char name[9];
	memset(name, 0, 9);
	if(NULL!=m_pAudioEffect && index<GetParamCount())
	{
		try {
			AEffect *a = m_pAudioEffect->getAeffect();
			if(a->version > 0)
				m_pAudioEffect->getParameterName(index, name);
		} catch(...) {
			int d=1;
		}
		return name;
	}
	return NULL;
}

char *CVstWrapper::GetParamDesc(int index)
{
	return NULL;
}











	/*if(NULL != m_pAudioEffect)
	{
		VstEvents events[1];
		events[0].numEvents = 1;
		events[0].reserved	= 0;
		events[0].events[0]->type			= kVstAudioType;
		events[0].events[0]->byteSize		= 0;
		events[0].events[0]->deltaFrames	= 0;
		events[0].events[0]->flags			= 0;
		memset(events[0].events[0]->data, 0, 16);
		events[0].events[1]->type			= kVstAudioType;
		events[0].events[1]->byteSize		= 0;
		events[0].events[1]->deltaFrames	= 0;
		events[0].events[1]->flags			= 0;
		memset(events[0].events[1]->data, 0, 16);
		
		m_pAudioEffect->processEvents(events);
	}*/
