// Input.h: interface for the CInput class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
#define AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../ifloopy.h"
#include "timeline.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

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
	CInput(UpdateCallback func);
	virtual ~CInput();

	char *GetName()			{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	BOOL Create(char *plugin);
	BOOL Create(IFloopySoundEngine *src);

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

	BOOL SetSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource()			{ return m_plugin->GetSource(); }

	IFloopySoundInput *GetSource(int index)	{ return m_plugin->GetSource(index); }

	int GetNextOffset(int offset);

	int GetInputCount() { return (m_plugin ? m_plugin->GetInputCount() : 0); }

	//SOUNDFORMAT *GetFormat() { return m_plugin->GetFormat(); }
	//SOUNDFORMAT *GetFormat() { return IFloopySoundInput::GetFormat(); }

	void SetParamAt(int offset, int index, float value);
	BOOL ResetParamAt(int offset, int index);
	void EnableAt(int offset, BOOL bEnable);

	void Close();

//	int GetLastError();
	BOOL GetLastError(char *str, int len);

	char *GetDisplayName() { return m_name; }
	void SetDisplayName(char *name, int len) { memcpy(m_name, name, (len<50?len:50)); }

	char *GetPath() { return _isEngine() ? m_source->GetPath() : m_szObjPath; }

	int GetLength();

	enumClassType GetType() { return _isEngine() ? TYPE_FLOOPY_ENGINE : TYPE_FLOOPY_SOUND_INPUT; }

private:
	inline BOOL _isEngine() { return (m_source->GetType() == TYPE_FLOOPY_ENGINE); }
	void applyParamsAt(int offset);
	int _getSamplesToBytes();
	int _getStartOffset();
	int _getEndOffset();
	void _recalcVariables();
//	BOOL _isEnabledAt(int offset);

//	int getSize();
//	int getRelativeSize()
//	int getEnd();

protected:
	int m_iCheck;	/** Some random number, used to check if the component was
						created by the same engine */

private:
	char m_name[50];
	char m_szLastError[100];
	char m_szObjPath[MAX_PATH];

	int m_nStartOffset;
	int m_nEndOffset;
	int m_nSamplesToBytes;

	UpdateCallback m_callback;

	int m_offset;
	HINSTANCE m_hinst;
	IFloopySoundInput *m_plugin;
	CTimeline m_timeline;

//	BOOL m_bRecording;

	float m_fDebug;

#ifdef _DEBUG_TIMER_
	BOOL	m_bDebugTimer;
	DWORD	m_dwSpeed;
	int		m_nFrameCount;
	int		m_nFrameSize;
#endif // _DEBUG_TIMER_
};

#endif // !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
