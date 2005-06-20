// Mixer.h: interface for the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIXER_H__571408E7_BE83_4237_BA26_0C5306E32472__INCLUDED_)
#define AFX_MIXER_H__571408E7_BE83_4237_BA26_0C5306E32472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _DEBUG_TIMER_

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "../../ifloopy.h"

#define MAX_INPUTS	12

class CMixer : public IFloopySoundMixer
{
public:
	CMixer();
	virtual ~CMixer();

	char *GetName()			{ return "stdlib.mixer"; }
	char *GetDescription()	{ return "Mixer Component"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }

	void MoveTo(int samples);
	void Reset();
	void Close();
	int GetSize();
	//SOUNDFORMAT *GetFormat();

	int Read(BYTE *data, int size);

	BOOL SetSource(IFloopySoundInput *src) { return (AddSource(src) != -1); }
	int AddSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource(int index);
	void RemoveSource(IFloopySoundInput *src);
	int GetInputCount();

	BOOL ReadSourceIfDisabled()	{ return FALSE; }

#ifdef _DEBUG_TIMER_
	int   GetParamCount()			{ return 0; }
	void  SetParam(int index, float value);// { m_bDebugTimer = (value==1.f); }
	float GetParam(int index);//		{ return (float)m_bDebugTimer; }
	char *GetParamName(int index);//	{ return "timer"; }
	char *GetParamDesc(int index);//	{ return "Debug Timer"; }
#endif // _DEBUG_TIMER_

private:
	int sampleSize();
	//int samplesToBytes();
	void mixBuffers(BYTE *buffers, int count, BYTE *output, int size);

	int	*m_nLengths;
	int		m_nLengthsSize;

	BYTE	*m_pBuffers;
	int		m_nBuffSize;

	IFloopySoundInput *m_pInputs[MAX_INPUTS];
	int		m_nInputCount;

#ifdef _DEBUG_TIMER_
	BOOL	m_bDebugTimer;
	DWORD	m_dwSpeed;
	int		m_nFrameCount;
	int		m_nFrameSize;
#endif // _DEBUG_TIMER_
};

#endif // !defined(AFX_MIXER_H__571408E7_BE83_4237_BA26_0C5306E32472__INCLUDED_)
