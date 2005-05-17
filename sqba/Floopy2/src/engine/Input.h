// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
#define AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "../ifloopy.h"
#include "timeline.h"

#define LOCAL_PARAM_COUNT	1
//#define _DEBUG_TIMER_

/**
 * \class CInput
 * \brief Input plug-in wrapper class.
 * \author Filip Pavlovic
 * \version
 * \date 02. May 2005.
 *
 * Input plug-in wrapper class.
 * Memorizes all parameter changes and the offsets
 * at which they occured. These parameters are then
 * set during execution.
 */
class CInput : public IFloopySoundInput
{
public:
	CInput(char *plugin);
	virtual ~CInput();
/*
	char *GetName()			{ return "inputw"; }
	char *GetDescription()	{ return "Floopy Input Plugin Wrapper"; }
	char *GetVersion()		{ return "0.1"; }
	char *GetAuthor()		{ return "sqba"; }
*/
	char *GetName()			{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	void MoveTo(int samples);
	void Reset();
	int Read(BYTE *data, int size);

	void Enable(BOOL bEnable);
	BOOL IsEnabled();

	int GetSize();

	int   GetParamCount();//					{ return m_plugin->GetParamCount(); }
	void  SetParam(int index, float value);//	{ m_plugin->SetParam(index, value); }
	float GetParam(int index);//				{ return m_plugin->GetParam(index); }
	BOOL GetParam(int index, float *value);
	char *GetParamName(int index);//			{ return m_plugin->GetParamName(index); }
	char *GetParamDesc(int index);//			{ return m_plugin->GetParamDesc(index); }
	int   GetParamIndex(char *name);//	{ return m_plugin->GetParamIndex(name); }

	BOOL SetSource(IFloopySoundInput *src)
	{
		return (m_plugin ? m_plugin->SetSource(src) : FALSE);
	}

	IFloopySoundInput *GetSource()			{ return m_plugin->GetSource(); }

	IFloopySoundInput *GetSource(int index)	{ return m_plugin->GetSource(index); }

	int GetNextOffset(int offset);

	int GetInputCount() { return (m_plugin ? m_plugin->GetInputCount() : 0); }

	//SOUNDFORMAT *GetFormat() { return m_plugin->GetFormat(); }
	//SOUNDFORMAT *GetFormat() { return IFloopySoundInput::GetFormat(); }

	void SetParamAt(int offset, int index, float value);
	void ResetParamAt(int offset, int index);
	void EnableAt(int offset, BOOL bEnable);

	void Close();

	int GetLastError();
	BOOL GetErrorDesc(char *str, int len);

	char *GetDisplayName() { return m_name; }
	void SetDisplayName(char *name, int len) { memcpy(m_name, name, (len<50?len:50)); }

private:
	void applyParamsAt(int offset);
	int samplesToBytes();
	int getEndOffset();

	int m_offset;
	HINSTANCE m_hinst;
	IFloopySoundInput *m_plugin;
	CTimeline m_timeline;

	BOOL m_bRecording;

	float m_fDebug;

	char m_name[50];

#ifdef _DEBUG_TIMER_
	BOOL	m_bDebugTimer;
	DWORD	m_dwSpeed;
	int		m_nFrameCount;
	int		m_nFrameSize;
#endif // _DEBUG_TIMER_
};

#endif // !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
