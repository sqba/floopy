// sndtch.h: interface for the CSoundTouch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNDTCH_H__E6C9AEA1_124C_4511_84A1_97E620527EC6__INCLUDED_)
#define AFX_SNDTCH_H__E6C9AEA1_124C_4511_84A1_97E620527EC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"
#include "SoundTouch/include/SoundTouch.h"

class CSoundTouch : public IFloopySoundFilter  
{
public:
	CSoundTouch();
	virtual ~CSoundTouch();

	char *GetName()				{ return "SoundTouch.SoundTouch"; }
	char *GetDescription()		{ return "SoundTouch library wrapper"; }
	char *GetVersion()			{ return "0.1"; }
	char *GetAuthor()			{ return "sqba"; }

	int Read(BYTE *data, int size);

	void MoveTo(int samples);
	void Reset();
	void Close();
	int GetSize();
	bool SetSource(IFloopySoundInput *src);

	bool ReadSourceIfDisabled()	{ return false; }

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
	void detectBPM();

private:
	SoundTouch m_SoundTouch;
	float m_fTempoDelta;
	float m_fPitch;
	float m_fRate;
	bool m_bQuickSeek;
	bool m_bUseAAFilter;
	float m_fNewBPM;
	float m_fOrigBPM;
};

#endif // !defined(AFX_SNDTCH_H__E6C9AEA1_124C_4511_84A1_97E620527EC6__INCLUDED_)
