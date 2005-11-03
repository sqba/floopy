// BuzzMachineWrapper.cpp: implementation of the CBuzzMachineWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "BuzzMachineWrapper.h"

typedef CMachineInterface* (*CreateMachine)();
typedef CMachineInfo const* (*GetInfo)();


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuzzMachineWrapper::CBuzzMachineWrapper()
{
	m_hinst			= NULL;
	m_pMachine		= NULL;
	m_pDataInput	= NULL;
	m_pCallbacks	= NULL;
	m_pMachineInfo	= NULL;

	// Popraviti error handling u engine-u kada
	// sledece vrednosti nisu postavljene!
	m_format.frequency		= 44100;
	m_format.format			= WAVE_FORMAT_PCM;
	m_format.channels		= 2;
	m_format.bitsPerSample	= 32;
	m_format.sampleDataType = SAMPLE_DATA_TYPE_FLOAT;

//	m_MasterInfo.BeatsPerMin = [16..500];
//	m_MasterInfo.TicksPerBeat = [1..32];
	m_MasterInfo.SamplesPerSec = 44100;
//	m_MasterInfo.SamplesPerTick = (int)((60 * SPS) / (BPM * TPB));
//	m_MasterInfo.PosInTick = [0..SamplesPerTick-1];
//	m_MasterInfo.TicksPerSec = (float)SPS / (float)SPT;
}

CBuzzMachineWrapper::~CBuzzMachineWrapper()
{
	if(NULL != m_hinst)
		FreeLibrary(m_hinst);

	if(NULL != m_pMachine)
		delete m_pMachine;
}

SOUNDFORMAT *CBuzzMachineWrapper::GetFormat()
{
	return &m_format;
}

bool CBuzzMachineWrapper::Open(char *filename)
{
	return create(filename);
}

int CBuzzMachineWrapper::Read(BYTE *data, int size)
{
	if(NULL == m_pMachine)
		return 0;

	SOUNDFORMAT *fmt = IFloopySoundFilter::GetFormat();
	if(NULL == fmt)
		return 0;

	/*float *in = (float*)data;
	int s = size/4;
	while(s--)
	{
		*in *= 32768.0;
		in++;
	}*/

	//m_pMachine->Work((float*)data, size/4, WM_READ);

	if( m_pMachine->Work((float*)data, size/4, WM_WRITE) )
	{
		float *in = (float*)data;
		int s = size/4;
		while(s--)
		{
			*in /= 32768.0;
			in++;
		}
		return size;
	}

	return 0;
}

void CBuzzMachineWrapper::Reset()
{
	if(NULL != m_pMachine)
	{
		/////////////////////////////////////////////
		/*BYTE *params = (BYTE*)m_pMachine->GlobalVals;
		params[0] = 9;
		params[1] = 40;
		params[2] = 0;
		params[3] = 0xC0;
		int *attributes = m_pMachine->AttrVals;
		attributes[0] = 500;
		attributes[1] = 500;*/
		/////////////////////////////////////////////

		m_pMachine->Tick();
		//m_pMachine->Init( m_pDataInput );
	}
}

void CBuzzMachineWrapper::MoveTo(int offset)
{
	if(NULL!=m_pMachine && offset==0)
	{
		Reset();
	}
}

bool CBuzzMachineWrapper::create(char *name)
{
	m_hinst = LoadLibraryA( name );
	if(NULL == m_hinst)
	{
		sprintf(m_szLastError, "File '%s' not found.\n\0", name);
		return false;
	}

	CreateMachine func = (CreateMachine)GetProcAddress(m_hinst, "CreateMachine"); 
	if(NULL == func)
	{
		sprintf(m_szLastError, "Function 'CreateMachine' not found in library '%s'.\n\0", name);
		return false;
	}

	m_pMachine = func();
	if(NULL == m_pMachine)
	{
		sprintf(m_szLastError, "Plugin not created by function 'CreateMachine'.\n\0");
		return false;
	}




	GetInfo getInfo = (GetInfo)GetProcAddress(m_hinst, "GetInfo"); 
	if(NULL == getInfo)
	{
		sprintf(m_szLastError, "Function 'GetInfo' not found in library '%s'.\n\0", name);
		return false;
	}

	m_pMachineInfo = getInfo();
	if(NULL == m_pMachineInfo)
	{
		sprintf(m_szLastError, "Plugin not created by function 'GetInfo'.\n\0");
		return false;
	}




	m_pMachine->pMasterInfo = &m_MasterInfo;

	m_pMachine->pCB = m_pCallbacks;
	m_pMachine->Init( m_pDataInput );



	/////////////////////////////////////////////
	/*BYTE *params = (BYTE*)m_pMachine->GlobalVals;
	params[0] = 9;
	params[1] = 40;
	params[2] = 0;
	params[3] = 0xC0;
	int *attributes = m_pMachine->AttrVals;
	attributes[0] = 500;
	attributes[1] = 500;*/
	m_pMachine->Init( m_pDataInput );
	m_pMachine->Tick();
	/////////////////////////////////////////////



	return true;
}






int CBuzzMachineWrapper::GetParamCount()
{
	if(m_pMachineInfo)
		return m_pMachineInfo->numGlobalParameters;
	return 0;
}

float CBuzzMachineWrapper::GetParamMin(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineParameter *param = m_pMachineInfo->Parameters[index];
		return (float)param->MinValue;
	}
	return 0.f;
}

float CBuzzMachineWrapper::GetParamMax(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineParameter *param = m_pMachineInfo->Parameters[index];
		return (float)param->MaxValue;
	}
	return 0.f;
}

char *CBuzzMachineWrapper::GetParamUnit(int index)
{
	return NULL;
}

float CBuzzMachineWrapper::GetParamStep(int index)
{
	return 1.f;
}

bool CBuzzMachineWrapper::GetParamVal(int index, float *value)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		BYTE *params = (BYTE*)m_pMachine->GlobalVals;
		int pos = 0;
		for(int i=0; i<index; i++)
		{
			int size = 1;
			const CMachineParameter *param = m_pMachineInfo->Parameters[i];
			switch(param->Type)
			{
			case pt_note:
				size = 1; //?
				break;
			case pt_switch:
				size = 1; //?
				break;
			case pt_byte:
				size = 1;
				break;
			case pt_word:
				size = 2;
				break;
			}
			pos += size;
		}
		*value = (float)*(params+pos);
		return true;
	}
	return false;
}

// Problem: kada se jednom promeni vrednost lednog kanala posebno,
// ta vrednost se upisuje u tajmlajn i vishe nije moguce postaviti
// vrednost za oba kanala zajedno (case 0).
void CBuzzMachineWrapper::SetParamVal(int index, float value)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		BYTE *params = (BYTE*)m_pMachine->GlobalVals;
		int pos = 0;
		for(int i=0; i<index; i++)
		{
			int size = 1;
			const CMachineParameter *param = m_pMachineInfo->Parameters[i];
			switch(param->Type)
			{
			case pt_note:
				size = 1; //?
				break;
			case pt_switch:
				size = 1; //?
				break;
			case pt_byte:
				size = 1;
				break;
			case pt_word:
				size = 2;
				break;
			}
			pos += size;
		}
		*(params+pos) = value;
		m_pMachine->Tick();
	}
}

char *CBuzzMachineWrapper::GetParamName(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineParameter *param = m_pMachineInfo->Parameters[index];
		return (char*)param->Name;
	}
	return NULL;
}

char *CBuzzMachineWrapper::GetParamDesc(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineParameter *param = m_pMachineInfo->Parameters[index];
		return (char*)param->Description;
	}
	return NULL;
}





int CBuzzMachineWrapper::GetPropertyCount()
{
	if(m_pMachineInfo)
		return m_pMachineInfo->numAttributes;
	return 0;
}

bool CBuzzMachineWrapper::GetPropertyVal(int index, float *value)
{
	if(m_pMachine && index<GetPropertyCount())
	{
		int *attributes = m_pMachine->AttrVals;
		*value = (float)attributes[index];
		return true;
	}
	return false;
}

void CBuzzMachineWrapper::SetPropertyVal(int index, float value)
{
	if(m_pMachine && index<GetPropertyCount())
	{
		int *attributes = m_pMachine->AttrVals;
		attributes[index] = (int)value;
	}
}

char *CBuzzMachineWrapper::GetPropertyName(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineAttribute *att = m_pMachineInfo->Attributes[index];
		return (char*)att->Name;
	}
	return NULL;
}

char *CBuzzMachineWrapper::GetPropertyDesc(int index)
{
	return NULL;
}

float CBuzzMachineWrapper::GetPropertyMin(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineAttribute *att = m_pMachineInfo->Attributes[index];
		return (float)att->MinValue;
	}
	return 0.f;
}

float CBuzzMachineWrapper::GetPropertyMax(int index)
{
	if(m_pMachineInfo && index<GetParamCount())
	{
		const CMachineAttribute *att = m_pMachineInfo->Attributes[index];
		return (float)att->MaxValue;
	}
	return 0.f;
}

float CBuzzMachineWrapper::GetPropertyStep(int index)
{
	return 0.1f;
}
