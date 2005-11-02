// BuzzMachineWrapper.cpp: implementation of the CVstWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "vstwrapper.h"

long Callback(AEffect *effect, long opcode,
			   long index, long value,
			   void *ptr, float opt)
{
	switch(opcode)
	{
	case audioMasterVersion:
		return 1;
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

	m_pAudioEffect = (AudioEffectX*)m_pPlugin->object;

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

	float *outputs[2] = { (float*)data, NULL };
	float **inputs = NULL;

	//int sampleFrames = size / fmt->channels / 4;
	int sampleFrames = size / 4;

	m_pPlugin->process(m_pPlugin, inputs, outputs, sampleFrames);

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
	if(NULL != m_pPlugin)
	{
		*value = m_pPlugin->getParameter(m_pPlugin, index);
		return true;
	}
	return false;
}

void CVstWrapper::SetParamVal(int index, float value)
{
	if(NULL != m_pPlugin)
		m_pPlugin->setParameter(m_pPlugin, index, value);

//	if(NULL != m_pAudioEffect)
//		m_pAudioEffect->processEvents(NULL);
}

char *CVstWrapper::GetParamName(int index)
{
	return NULL;
}

char *CVstWrapper::GetParamDesc(int index)
{
	return NULL;
}



