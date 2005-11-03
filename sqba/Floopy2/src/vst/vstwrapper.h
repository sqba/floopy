// BuzzMachineWrapper.h: interface for the CVstWrapper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VSTWRAPPER_H__E80D4824_07B0_4ACE_A378_D76117748BCE__INCLUDED_)
#define AFX_VSTWRAPPER_H__E80D4824_07B0_4ACE_A378_D76117748BCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"
#include "steinberg/audioeffectx.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>


//typedef	long (VSTCALLBACK *audioMasterCallback)(AEffect *effect, long opcode, long index,
//		long value, void *ptr, float opt);


class CVstWrapper : public IFloopySoundFilter
{
public:
	CVstWrapper();
	virtual ~CVstWrapper();

	char *GetName()			{ return "vst.vst"; }
	char *GetDescription()	{ return "VST plugin wrapper"; }
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

private:
	HINSTANCE		m_hinst;	/** Windows specific, handle to the DLL
									containing the plugin */

	//AudioEffectX	*m_pPlugin;
	AEffect		*m_pPlugin;
	AudioEffectX	*m_pAudioEffect;

	float *m_pOutput[2];
	float *m_pInput[2];
	int m_nBuffSize;

	char m_szLastError[100];
};

#endif // !defined(AFX_VSTWRAPPER_H__E80D4824_07B0_4ACE_A378_D76117748BCE__INCLUDED_)
