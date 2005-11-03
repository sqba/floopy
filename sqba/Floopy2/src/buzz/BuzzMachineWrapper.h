// BuzzMachineWrapper.h: interface for the CBuzzMachineWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUZZMACHINEWRAPPER_H__E80D4824_07B0_4ACE_A378_D76117748BCE__INCLUDED_)
#define AFX_BUZZMACHINEWRAPPER_H__E80D4824_07B0_4ACE_A378_D76117748BCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"
#include "MachineInterface.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

class CBuzzMachineWrapper : public IFloopySoundFilter
{
public:
	CBuzzMachineWrapper();
	virtual ~CBuzzMachineWrapper();

	char *GetName()			{ return "buzz.machine"; }
	char *GetDescription()	{ return "Buzz machine wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	bool ReadSourceIfDisabled()	{ return false; }

	SOUNDFORMAT *GetFormat();

	bool Open(char *filename);
	int Read(BYTE *data, int size);
	void Reset();
	void MoveTo(int offset);

	int   GetParamCount();
	float GetParamMin(int);
	float GetParamMax(int);
	char *GetParamUnit(int);
	char *GetParamName(int);
	char *GetParamDesc(int);
	float GetParamStep(int);
	void  SetParamVal(int, float);
	bool  GetParamVal(int, float*);

	int   GetPropertyCount();
	//bool  GetPropertyIndex(char *name, int *index);
	bool  GetPropertyVal(int index, float *value);
	void  SetPropertyVal(int index, float value);
	float GetPropertyMin(int index);
	float GetPropertyMax(int index);
	float GetPropertyStep(int index);
	char *GetPropertyName(int index);
	char *GetPropertyDesc(int index);
	//char *GetPropertyUnit(int index);

private:
	bool create(char *name);

private:
	HINSTANCE m_hinst;	/** Windows specific, handle to the DLL
							containing the plugin */

	CMasterInfo			m_MasterInfo;
	CMICallbacks		*m_pCallbacks;
	CMachineInterface	*m_pMachine;
	CMachineDataInput	*m_pDataInput;
	const CMachineInfo	*m_pMachineInfo;

	char m_szLastError[100];
};

#endif // !defined(AFX_BUZZMACHINEWRAPPER_H__E80D4824_07B0_4ACE_A378_D76117748BCE__INCLUDED_)
