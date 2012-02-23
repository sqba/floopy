// Wrapper.cpp: implementation of the CWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "Wrapper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWrapper::CWrapper()
{
	m_pHost = new CVSTHost();
	m_nEffect = 0;
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

CWrapper::~CWrapper()
{
	if(m_pHost != NULL)
		delete m_pHost;

	if(NULL != m_pInputs[0])
		delete m_pInputs[0];

	if(NULL != m_pInputs[1])
		delete m_pInputs[1];

	if(NULL != m_pOutputs[0])
		delete m_pOutputs[0];

	if(NULL != m_pOutputs[1])
		delete m_pOutputs[1];
}


SOUNDFORMAT *CWrapper::GetFormat()
{
	return &m_format;
}

bool CWrapper::Open(char *filename)
{
	if(m_pHost == NULL)
		return false;

	int nUniqueId = 1;
	m_nEffect = m_pHost->LoadPlugin(filename, nUniqueId);

	//m_pHost->EffSetSampleRate(m_nEffect, 44100);

	return false;
}

int CWrapper::Read(BYTE *data, int size)
{
	if(m_pHost == NULL)
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

	try
	{
		m_pHost->ProcessReplacing(m_pInputs, m_pOutputs, sampleFrames);
	}
	catch(...)
	{
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

void CWrapper::Reset()
{
	IFloopySoundFilter::Reset();
}

void CWrapper::MoveTo(int offset)
{
	IFloopySoundFilter::MoveTo(offset);
}





int CWrapper::GetParamCount()
{
	if(m_pHost == NULL)
		return 0;

	CEffect *a = m_pHost->GetAt(m_nEffect);
	if(a == NULL)
		return 0;

	return a->pEffect->numParams;
}

float CWrapper::GetParamMin(int index)
{
	return 0.f;
}

float CWrapper::GetParamMax(int index)
{
	return 1.f;
}

char *CWrapper::GetParamUnit(int index)
{
	return NULL;
}

float CWrapper::GetParamStep(int index)
{
	return 0.1f;
}

bool CWrapper::GetParamVal(int index, float *value)
{
	if(m_pHost!=NULL && index<GetParamCount()) {
		*value = m_pHost->EffGetParameter(m_nEffect, (long)index);
		return true;
	}
	return false;
}

void CWrapper::SetParamVal(int index, float value)
{
	if(m_pHost != NULL)
		m_pHost->EffSetParameter(m_nEffect, (long)index, value);
}

char *CWrapper::GetParamName(int index)
{
	if(m_pHost == NULL)
		return NULL;
	
	if(index >= GetParamCount())
		return NULL;

	static char name[9];
	memset(name, 0, 9);
	try
	{
		m_pHost->EffGetParamName(m_nEffect, index, name);
		return name;
	}
	catch(...)
	{
		return NULL;
	}
}

char *CWrapper::GetParamDesc(int index)
{
	return NULL;
}
