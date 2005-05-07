// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__1B41AC02_C337_4777_8CEC_C87828A934BE__INCLUDED_)
#define AFX_MIXER_H__1B41AC02_C337_4777_8CEC_C87828A934BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _DEBUG_TIMER_

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "../../ifloopy.h"

#define MAX_INPUTS	12

class CInput : public IFloopySoundInput
{
public:
	CInput();
	virtual ~CInput();

	char *GetName()			{ return "mixer"; }
	char *GetDescription()	{ return "Mixer component"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

#ifdef _DEBUG_TIMER_
	int   GetParamCount()			{ return 4; }
	void  SetParam(int index, float value);// { m_bDebugTimer = (value==1.f); }
	float GetParam(int index);//		{ return (float)m_bDebugTimer; }
	char *GetParamName(int index);//	{ return "timer"; }
	char *GetParamDesc(int index);//	{ return "Debug Timer"; }
#endif // _DEBUG_TIMER_

	void MoveTo(UINT samples);
	void Reset();
	void Close();
	UINT GetSize();
	//WAVFORMAT *GetFormat();

	UINT Read(BYTE *data, UINT size);

	int AddSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource(int index);
	void RemoveSource(IFloopySoundInput *src);
	int GetInputCount();

private:
	void MixBuffers(BYTE *buffers, int count, BYTE *output, UINT size);

#ifdef _DEBUG_TIMER_
	BOOL m_bDebugTimer;
	DWORD speed;
	int frames;
	int framesize;

	BYTE *buffers;
	int buffsize;

	UINT *lengths;
	int lensize;
#endif // _DEBUG_TIMER_

	IFloopySoundInput *inputs[MAX_INPUTS];
	int count;
};

#endif // !defined(AFX_MIXER_H__1B41AC02_C337_4777_8CEC_C87828A934BE__INCLUDED_)
