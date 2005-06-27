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

//#define LOCAL_PARAM_COUNT	1
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
class CInput : public IFloopySoundMixer
{
public:
	CInput(UpdateCallback func);
	virtual ~CInput();

	BOOL Create(char *plugin);
	BOOL Create(IFloopySoundEngine *src);

	char *GetName()			{ return m_fullName; }	//{ return m_plugin->GetName(); }
	char *GetDescription()	{ return m_plugin->GetDescription(); }
	char *GetVersion()		{ return m_plugin->GetVersion(); }
	char *GetAuthor()		{ return m_plugin->GetAuthor(); }

	int   GetParamCount();
	BOOL  GetParamVal(int index, float *value);
	void  SetParamVal(int index, float value);
	char *GetParamName(int index);
	char *GetParamDesc(int index);
	BOOL  GetParamIndex(char *name, int *index);

	BOOL GetColor(UINT *r, UINT *g, UINT *b);
	void SetColor(UINT r, UINT g, UINT b);

	void Enable(BOOL bEnable);
	BOOL IsEnabled();

	void MoveTo(int samples);
	void Reset();
	int  Read(BYTE *data, int size);

	int GetSize();
	int GetSourceSize();

	BOOL SetSource(IFloopySoundInput *src);
	IFloopySoundInput *GetSource();

	int GetNextOffset(int offset);

	void SetParamAt(int offset, int index, float value);
	BOOL ResetParamAt(int offset, int index);
	void EnableAt(int offset, BOOL bEnable);

	void Close();

//	int GetLastError();
	BOOL GetLastError(char *str, int len);

	char *GetDisplayName() { return m_name; }
	void SetDisplayName(char *name, int len)
	{
		memset(m_name, 0, 50);
		memcpy(m_name, name, (len<50?len:50));
	}

	char *GetPath() { return _isEngine() ? m_source->GetPath() : m_szObjPath; }

//	enumClassType GetType() { return _isEngine() ? TYPE_FLOOPY_ENGINE : TYPE_FLOOPY_SOUND_INPUT; }
	enumClassType GetType() { return (m_plugin ? m_plugin->GetType() : TYPE_FLOOPY); }

	BOOL MoveParam(int offset, int index, int newoffset);



	///////////////////////////////////////////
	// IFloopySoundMixer functions
	///////////////////////////////////////////
	IFloopySoundInput *GetSource(int index);
	int AddSource(IFloopySoundInput *src);
	void RemoveSource(IFloopySoundInput *src);
	int GetInputCount();
	///////////////////////////////////////////


private:
	inline BOOL _isEngine() { return (m_source->GetType() == TYPE_FLOOPY_SOUND_ENGINE); }
	void applyParamsAt(int offset);
	int _getSamplesToBytes();
	int _getStartOffset();
	int _getEndOffset();
	void _recalcVariables();
//	BOOL _isEnabledAt(int offset);
//	int _read(BYTE *data, int size);

//	int getSize();
//	int getRelativeSize()
//	int getEnd();
	UINT m_red, m_green, m_blue;

protected:
	int m_iCheck;	/** Some random number, used to check if the
						component was created by the same engine */

private:
	char m_name[50];
	char m_szLastError[100];
	char m_szObjPath[MAX_PATH];

	char m_libraryName[MAX_PATH];
	char m_pluginName[50];
	char m_fullName[MAX_PATH];

	UpdateCallback m_callback;

	int m_offset;
	HINSTANCE m_hinst;
	IFloopySoundInput *m_plugin;
	CTimeline m_timeline;

//	BOOL m_bRecording;

	float m_fDebug;

	/// Optimization variables
	int m_nStartOffset, m_nEndOffset, m_nSamplesToBytes;

#ifdef _DEBUG_TIMER_
	clock_t	_debugStartMeasuring();
	void	_debugStopMeasuring(clock_t start, int size);
	void	_debugFormatBytes(int bytes, char *str);
	void	_debugPrint();
	BOOL	m_bDebugTimer;
	clock_t	m_time;
	int		m_nFrameCount;
	int		m_nFrameSize;
#endif // _DEBUG_TIMER_
};

#endif // !defined(AFX_INPUT_H__0D3139FE_D3F2_4CAF_A696_AB92E4A51331__INCLUDED_)
